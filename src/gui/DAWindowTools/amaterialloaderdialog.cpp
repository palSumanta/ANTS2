#include "amaterialloaderdialog.h"
#include "ui_amaterialloaderdialog.h"
#include "amaterialparticlecolection.h"
#include "ajsontools.h"
#include "amessage.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QSet>

AMaterialLoaderDialog::AMaterialLoaderDialog(const QString & fileName, AMaterialParticleCollection & MpCollection, QWidget *parentWidget) :
    QDialog(parentWidget), MpCollection(MpCollection),
    ui(new Ui::AMaterialLoaderDialog)
{
    ui->setupUi(this);
    ui->pbDummt->setVisible(false);
    ui->labForceByNeutron->setVisible(false);

    DefinedMaterials = MpCollection.getListOfMaterialNames();
    ui->cobMaterial->addItems(DefinedMaterials);

    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->scrollArea->setWidgetResizable(true);

    QJsonObject json;
    bFileOK = LoadJsonFromFile(json, fileName);
    if (!bFileOK)
    {
        ui->labError->setText("Cannot open file: "+fileName);
        ui->labError->setVisible(true);
        return;
    }
    if (!json.contains("Material"))
    {
        bFileOK = false;
        ui->labError->setText("File format error: Json with material settings not found");
        ui->labError->setVisible(true);
        return;
    }
    ui->labError->setVisible(false);

    MaterialJson = json["Material"].toObject();
    parseJson(MaterialJson, "*MaterialName", NameInFile);
    ui->leMaterialNameInFile->setText(NameInFile);
    ui->leName->setText(NameInFile);

    const QVector<QString> UndefinedParticles = MpCollection.getUndefinedParticles(MaterialJson);
    for (const QString & str : UndefinedParticles)
    {
        AParticleRecordForMerge pr(str);
        NewParticles << pr;
    }
    generateParticleGui();
    updateParticleGui();

    if (DefinedMaterials.size() != 0)
    {
        int iBest = 0;
        int BestVal = -1;
        for (int i=0; i<DefinedMaterials.size(); i++)
        {
            const int match = getMatchValue(NameInFile, DefinedMaterials.at(i));
            if (match > BestVal)
            {
                iBest = i;
                BestVal = match;
            }
        }
        ui->cobMaterial->setCurrentIndex(iBest);
    }
    updateMaterialPropertiesGui();
}

AMaterialLoaderDialog::~AMaterialLoaderDialog()
{
    delete ui;
}

const QVector<QString> AMaterialLoaderDialog::getSuppressedParticles() const
{
    QVector<QString> SuppressedParticles;

    for (int i = 0; i < NewParticles.size(); i++)
        if (!NewParticles.at(i).isChecked())
            SuppressedParticles << NewParticles.at(i).ParticleName;

    return SuppressedParticles;
}

void AMaterialLoaderDialog::generateParticleGui()
{
    ui->cbToggleAllParticles->setVisible(NewParticles.size() > 1);

    QWidget * w = new QWidget();
    QVBoxLayout * lay = new QVBoxLayout(w);
    for (AParticleRecordForMerge & rec : NewParticles)
    {
        QCheckBox * CheckBox = new QCheckBox(rec.ParticleName);
        rec.connectCheckBox(CheckBox);
            connect(CheckBox, &QCheckBox::clicked, [this, &rec](bool checked)
            {
                rec.setChecked(checked);
                if (rec.ParticleName == "neutron") updateParticleGui();
                updateMaterialPropertiesGui();
                ui->cbToggleAllParticles->setChecked(false);
            });
        lay->addWidget(CheckBox);
    }
    lay->addStretch();
    ui->scrollArea->setWidget(w);
}

void AMaterialLoaderDialog::updateParticleGui()
{
    const QVector<QString> ParticlesForcedByNeutron = getForcedByNeutron();

    for (AParticleRecordForMerge & pr : NewParticles)
        pr.setForced( ParticlesForcedByNeutron.contains(pr.ParticleName) );

    ui->labForceByNeutron->setVisible( !ParticlesForcedByNeutron.isEmpty() );
}

bool AMaterialLoaderDialog::isNameAlreadyExists() const
{
    return DefinedMaterials.contains(ui->leName->text());
}

void AMaterialLoaderDialog::updateLoadEnabled()
{
    bool bLoadActive = true;
    if (ui->twMain->currentIndex() == 0 && isNameAlreadyExists()) bLoadActive = false;
    ui->pbLoad->setEnabled(bLoadActive);
}

void AMaterialLoaderDialog::on_pbDummt_clicked()
{
    //dummy
}

void AMaterialLoaderDialog::on_pbLoad_clicked()
{
    if (ui->twMain->currentIndex() == 0)
    {
        if (isNameAlreadyExists())
        {
            message("Provide a unique name!", this);
            return;
        }
        MaterialJson["*MaterialName"] = ui->leName->text();
    }
    else
    {
        message("Not implemented yet!", this);
        return;
    }
    accept();
}

void AMaterialLoaderDialog::on_pbCancel_clicked()
{
    reject();
}

void AMaterialLoaderDialog::on_leName_textChanged(const QString &)
{
    ui->labAlreadyExists->setVisible(isNameAlreadyExists());
    updateLoadEnabled();
}

void AMaterialLoaderDialog::on_twMain_currentChanged(int)
{
    updateLoadEnabled();
}

void AMaterialLoaderDialog::on_cbToggleAllParticles_clicked(bool checked)
{
    for (AParticleRecordForMerge & rec : NewParticles)
    {
        if (!checked) rec.setForced(false);
        rec.setChecked(checked);
    }

    updateParticleGui();
    updateMaterialPropertiesGui();
}

void AMaterialLoaderDialog::updateMaterialPropertiesGui()
{
    ui->lwProps->clear();

    int numMat = DefinedMaterials.size();
    ui->tabMerge->setEnabled(numMat > 0);
    if (numMat == 0) return;

    int iMat = ui->cobMaterial->currentIndex();
    if (iMat < 0 || iMat >= numMat)
    {
        ui->labError->setText("Corrupted material record");
        ui->labError->setVisible(true);
        return;
    }
    ui->labError->setVisible(false);

    const AMaterial * matTo = MpCollection[iMat];

    QJsonObject MaterialTo;
    matTo->writeToJson(MaterialTo, &MpCollection);

    qDebug() << "Merging" << NameInFile << " to " << matTo->name;
    QSet<QString> Ignore;
    Ignore << "*MaterialName" << "*Tags" << "Comments"
           << "TGeoP1" << "TGeoP2" << "TGeoP3"
           << "MatParticles";

    int iDifProps = 0;
    foreach(const QString & key, MaterialJson.keys())
    {
        qDebug() << "\nKey = " << key;
        if (Ignore.contains(key)) continue;

        QJsonValue valueFrom = MaterialJson.value(key);
        QJsonValue valueTo   = MaterialTo.value(key);
        if (valueFrom == valueTo) continue;

        iDifProps++;
        QListWidgetItem * item = new QListWidgetItem(ui->lwProps);

        QWidget * wid = new QWidget();
            QHBoxLayout * lay = new QHBoxLayout(wid);
                QCheckBox * cb = new QCheckBox(key);
                cb->setChecked(true);
            lay->addWidget(cb);
        item->setSizeHint(wid->sizeHint());
        ui->lwProps->setItemWidget(item, wid);
    }

    addInteractionItems(MaterialTo);


    ui->cbToggleAllProps->setVisible(iDifProps > 1);
    ui->labAllMatch->setVisible(iDifProps == 0);
    ui->labSelectProp->setVisible(iDifProps != 0);
    ui->lwProps->setVisible(iDifProps != 0);
}

int AMaterialLoaderDialog::addInteractionItems(QJsonObject & MaterialTo)
{
    qDebug() << "\nProcessing MaterialParticle records";

    QJsonArray ArrMpFrom = MaterialJson["MatParticles"].toArray();
    QJsonArray ArrMpTo   = MaterialTo  ["MatParticles"].toArray();

    for (int iFrom = 0; iFrom < ArrMpFrom.size(); iFrom++)
    {
        QJsonObject jsonFrom = ArrMpFrom[iFrom].toObject();
        QJsonObject jsonParticleFrom = jsonFrom["*Particle"].toObject();
        AParticle ParticleFrom;
        ParticleFrom.readFromJson(jsonParticleFrom);
        qDebug() << "\nParticle" << ParticleFrom.ParticleName;

        if (isSuppressedParticle(ParticleFrom.ParticleName))
        {
            qDebug() << "This particle will not be imported";
            continue;
        }

        bool bFound = false;
        QJsonObject jsonTo;
        QJsonObject jsonParticleTo;
        int iTo;
        for (iTo = 0; iTo < ArrMpTo.size(); iTo++)
        {
            jsonTo = ArrMpTo[iTo].toObject();
            jsonParticleTo = jsonTo["*Particle"].toObject();
            AParticle ParticleTo;
            ParticleTo.readFromJson(jsonParticleTo);

            if (ParticleFrom == ParticleTo)
            {
                bFound = true;
                break;
            }
        }

        if (!bFound)
        {
            qDebug() << "Interaction data in target material not found for this particle";
            continue;
        }

        qDebug() << "Interaction data exists in the target material";


        if (jsonFrom == jsonTo)
        {
            qDebug() << "Records are identical";
            continue;
        }

        qDebug() << "--- Records are different ---";


        /*
        QListWidgetItem * item = new QListWidgetItem(ui->lwProps);
        QWidget * wid = new QWidget();
            QHBoxLayout * lay = new QHBoxLayout(wid);
                QCheckBox * cb = new QCheckBox(key);
                cb->setChecked(true);
            lay->addWidget(cb);
        item->setSizeHint(wid->sizeHint());
        ui->lwProps->setItemWidget(item, wid);
        */
    }

}

bool AMaterialLoaderDialog::isSuppressedParticle(const QString & ParticleName) const
{
    for (const AParticleRecordForMerge & rec : NewParticles)
        if (rec.ParticleName == ParticleName)
            return !rec.isChecked();
    return false;
}

const QVector<QString> AMaterialLoaderDialog::getForcedByNeutron() const
{
    QVector<QString> VecParticles;

    AMaterialParticleCollection FakeCollection;
    AMaterial Mat;
    Mat.readFromJson(MaterialJson, &FakeCollection, getSuppressedParticles());

    QStringList DefParticles = FakeCollection.getListOfParticleNames();
    int iNeutron = DefParticles.indexOf("neutron");
    if (iNeutron == -1)
    {
        qDebug() << "Neutron is not found";
        return VecParticles;
    }

    const MatParticleStructure & neutron = Mat.MatParticle.at(iNeutron);

    for (const NeutralTerminatorStructure & term : neutron.Terminators)
        VecParticles << term.getSecondaryParticles(FakeCollection);

    return VecParticles;
}

int AMaterialLoaderDialog::getMatchValue(const QString & s1, const QString & s2) const
{
    int imatch = 0;
    for (int i = 0; (i < s1.size()) && (i < s2.size()); i++)
    {
        if (s1.at(i) == s2.at(i)) imatch++;
        else break;
    }
    return imatch;
}

void AMaterialLoaderDialog::on_cbToggleAllProps_toggled(bool checked)
{

}

void AMaterialLoaderDialog::on_cobMaterial_activated(int)
{
    updateMaterialPropertiesGui();
}

void AParticleRecordForMerge::connectCheckBox(QCheckBox * cb)
{
    CheckBox = cb;
    updateIndication();
}

void AParticleRecordForMerge::setChecked(bool flag)
{
    if (bForcedByNeutron) bChecked = true;
    else                  bChecked = flag;
    updateIndication();
}

void AParticleRecordForMerge::setForced(bool flag)
{
    bForcedByNeutron = flag;
    if (flag) bChecked = true;
    updateIndication();
}

void AParticleRecordForMerge::updateIndication()
{
    if (CheckBox)
    {
        CheckBox->setChecked(bChecked);
        CheckBox->setEnabled(!bForcedByNeutron);
    }
}
