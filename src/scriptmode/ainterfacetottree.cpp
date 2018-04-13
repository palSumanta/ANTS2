#include "ainterfacetottree.h"
#include "aroottreerecord.h"
#include "tmpobjhubclass.h"

#include <QDebug>

#include "TFile.h"
//#include "TTree.h"
#include "TBranch.h"
#include "TLeaf.h"

AInterfaceToTTree::AInterfaceToTTree(TmpObjHubClass *TmpHub) :
    TmpHub(TmpHub)
{
    Description = "Interface to CERN ROOT Trees";
}

void AInterfaceToTTree::LoadTTree(const QString& TreeName, const QString& FileName, const QString& TreeNameInFile)
{
    if (!bGuiThread)
    {
        abort("Can load TTree only in main thread!");
        return;
    }

    ARootObjBase* r = TmpHub->Trees.getRecord(TreeName);
    if (r)
    {
        if (bAbortIfExists)
        {
            abort("Tree with name " + TreeName + " already exists!");
            return;
        }
        else DeleteTree(TreeName);
    }

    TFile *f = TFile::Open(FileName.toLocal8Bit().data(), "READ");
    if (!f)
    {
        abort("Cannot open file " + FileName);
        return;
    }
    TTree *t = 0;
    f->GetObject(TreeNameInFile.toLocal8Bit().data(), t);
    if (!t)
    {
        abort("Tree " + TreeNameInFile + " not found in file " + FileName);
        return;
    }
    t->Print();

    r = new ARootObjBase(t, TreeName, "TTree");
    TmpHub->Trees.append(TreeName, r);
}

void AInterfaceToTTree::CreateTree(const QString &TreeName, const QVariant HeadersOfBranches)
{
    if (!bGuiThread)
    {
        abort("Can load TTree only in main thread!");
        return;
    }

    const QVariantList headersVL = HeadersOfBranches.toList();
    if (headersVL.size() < 1)
    {
        abort("CreateTree() requires array of arrays as the second argument");
        return;
    }

    QVector<QPair<QString, QString>> h;
    for (int ibranch = 0; ibranch < headersVL.size(); ibranch++)
    {
        QVariantList th = headersVL.at(ibranch).toList();
        if (th.size() != 2)
        {
            abort("CreateTree() headers should be array of [Name,Type] values");
            return;
        }

        QString Bname = th.at(0).toString();
        QString Btype = th.at(1).toString();
        if (!ABranchBuffer::getAllTypes().contains(Btype))
        {
            abort("CreateTree() header contain unknown branch type: " + Btype );
            return;
        }

        h << QPair<QString, QString>(Bname, Btype);
    }

    ARootTreeRecord* rec = new ARootTreeRecord(0, TreeName);
    bool bOK = rec->createTree(TreeName, h);
    if (bOK)
    {
        bOK = TmpHub->Trees.append(TreeName, rec, bAbortIfExists);
        if (!bOK)
        {
            delete rec;
            abort("Tree " + TreeName+" already exists!");
        }
    }
    else abort("Failed to create tree: "+ TreeName);
}

void AInterfaceToTTree::FillTree_SingleEntry(const QString &TreeName, const QVariant Array)
{
    ARootTreeRecord* r = dynamic_cast<ARootTreeRecord*>(TmpHub->Trees.getRecord(TreeName));
    if (!r)
        abort("Tree "+TreeName+" not found!");
    else
    {
        const QVariantList vl = Array.toList();
        const bool bOK = r->fillSingle(vl);
        if (!bOK)
            abort("FillTree_SingleEntry() failed - check that array size = number of branches");
    }
}

const QString AInterfaceToTTree::PrintBranches(const QString& TreeName)
{
    ARootObjBase* r = TmpHub->Trees.getRecord(TreeName);
    if (!r)
    {
        abort("Tree " + TreeName + " not found!");
        return "";
    }
    TTree *t = static_cast<TTree*>(r->GetObject());

    QString s = "Thee ";
    s += TreeName;
    s += " has the following branches (-> data_type):<br>";
    for (int i=0; i<t->GetNbranches(); i++)
    {
        TObjArray* lb = t->GetListOfBranches();
        const TBranch* b = (const TBranch*)(lb->At(i));
        QString name = b->GetName();
        s += name;
        s += " -> ";
        QString type = b->GetClassName();
        if (type.isEmpty())
        {
            QString title = b->GetTitle();
            title.remove(name);
            title.remove("/");
            s += title;
        }
        else
        {
            type.replace("<", "(");
            type.replace(">", ")");
            s += type;
        }
        s += "<br>";
    }
    return s;
}

const QVariant AInterfaceToTTree::GetBranch(const QString& TreeName, const QString& BranchName)
{
    ARootObjBase* r = TmpHub->Trees.getRecord(TreeName);
    if (!r)
    {
        abort("Tree " + TreeName + " not found!");
        return "";
    }
    TTree *t = static_cast<TTree*>(r->GetObject());

    TBranch* branch = t->GetBranch(BranchName.toLocal8Bit().data());
    if (!branch)
    {
        abort("Tree " + TreeName + " does not have branch " + BranchName);
        return QVariant();
    }

    int numEntries = branch->GetEntries();
    qDebug() << "The branch contains:" << numEntries << "elements";

    QList< QVariant > varList;
    QString type = branch->GetClassName();
    qDebug() << "Element type:" << type;
    if (type == "vector<double>")
    {
        std::vector<double> *v = 0;
        t->SetBranchAddress(BranchName.toLocal8Bit().data(), &v, &branch);

        for (Int_t i = 0; i < numEntries; i++)
        {
            Long64_t tentry = t->LoadTree(i);
            branch->GetEntry(tentry);
            QList<QVariant> ll;
            for (UInt_t j = 0; j < v->size(); ++j)
                ll.append( (*v)[j] );
            QVariant r = ll;
            varList << r;
        }
    }
    else if (type == "vector<float>")
    {
        std::vector<float> *v = 0;
        t->SetBranchAddress(BranchName.toLocal8Bit().data(), &v, &branch);

        for (Int_t i = 0; i < numEntries; i++)
        {
            Long64_t tentry = t->LoadTree(i);
            branch->GetEntry(tentry);
            QList<QVariant> ll;
            for (UInt_t j = 0; j < v->size(); ++j)
                ll.append( (*v)[j] );
            QVariant r = ll;
            varList << r;
        }
    }
    else if (type == "vector<int>")
    {
        std::vector<int> *v = 0;
        t->SetBranchAddress(BranchName.toLocal8Bit().data(), &v, &branch);

        for (Int_t i = 0; i < numEntries; i++)
        {
            Long64_t tentry = t->LoadTree(i);
            branch->GetEntry(tentry);
            QList<QVariant> ll;
            for (UInt_t j = 0; j < v->size(); ++j)
                ll.append( (*v)[j] );
            QVariant r = ll;
            varList << r;
        }
    }
    else if (type == "")
    {
        //have to use another system
        QString title = branch->GetTitle();  //  can be, e.g., "blabla/D" or "signal[19]/F"

        if (title.contains("["))
        {
            qDebug() << "Array of data"<<title;
            QRegExp selector("\\[(.*)\\]");
            selector.indexIn(title);
            QStringList List = selector.capturedTexts();
            if (List.size()!=2)
            {
               abort("Cannot extract the length of the array");
               return QVariant();
            }
            else
            {
               QString s = List.at(1);
               bool fOK = false;
               int numInArray = s.toInt(&fOK);
               if (!fOK)
               {
                  abort("Cannot extract the length of the array");
                  return QVariant();
               }
               qDebug() << "in the array there are"<<numInArray<<"elements";

               //type dependent too!
               if (title.contains("/I"))
               {
                   qDebug() << "It is an array with ints";
                   int *array = new int[numInArray];
                   t->SetBranchAddress(BranchName.toLocal8Bit().data(), array, &branch);

                   for (Int_t i = 0; i < numEntries; i++)
                   {
                       Long64_t tentry = t->LoadTree(i);
                       branch->GetEntry(tentry);
                       QList<QVariant> ll;
                       for (int j = 0; j < numInArray; j++)
                           ll.append( array[j] );
                       QVariant r = ll;
                       varList << r;
                   }
                   delete [] array;
               }
               else if (title.contains("/D"))
               {
                   qDebug() << "It is an array with doubles";
                   double *array = new double[numInArray];
                   t->SetBranchAddress(BranchName.toLocal8Bit().data(), array, &branch);

                   for (Int_t i = 0; i < numEntries; i++)
                   {
                       Long64_t tentry = t->LoadTree(i);
                       branch->GetEntry(tentry);
                       QList<QVariant> ll;
                       for (int j = 0; j < numInArray; j++)
                           ll.append( array[j] );
                       QVariant r = ll;
                       varList << r;
                   }
                   delete [] array;
               }
               else if (title.contains("/F"))
               {
                   qDebug() << "It is an array with floats";
                   float *array = new float[numInArray];
                   t->SetBranchAddress(BranchName.toLocal8Bit().data(), array, &branch);

                   for (Int_t i = 0; i < numEntries; i++)
                   {
                       Long64_t tentry = t->LoadTree(i);
                       branch->GetEntry(tentry);
                       QList<QVariant> ll;
                       for (int j = 0; j < numInArray; j++)
                           ll.append( array[j] );
                       QVariant r = ll;
                       varList << r;
                   }
                   delete [] array;
               }
               else
               {
                   abort("Cannot extract the type of the array");
                   return QVariant();
               }
            }
        }
        else if (title.contains("/I"))
        {
            qDebug() << "Int data - scalar";
            int v = 0;
            t->SetBranchAddress(BranchName.toLocal8Bit().data(), &v, &branch);
            for (Int_t i = 0; i < numEntries; i++)
            {
                Long64_t tentry = t->LoadTree(i);
                branch->GetEntry(tentry);
                varList.append(v);
            }
        }
        else if (title.contains("/D"))
        {
            qDebug() << "Double data - scalar";
            double v = 0;
            t->SetBranchAddress(BranchName.toLocal8Bit().data(), &v, &branch);
            for (Int_t i = 0; i < numEntries; i++)
            {
                Long64_t tentry = t->LoadTree(i);
                branch->GetEntry(tentry);
                varList.append(v);
            }
        }
        else if (title.contains("/F"))
        {
            qDebug() << "Float data - scalar";
            float v = 0;
            t->SetBranchAddress(BranchName.toLocal8Bit().data(), &v, &branch);
            for (Int_t i = 0; i < numEntries; i++)
            {
                Long64_t tentry = t->LoadTree(i);
                branch->GetEntry(tentry);
                varList.append(v);
            }
        }
        else
        {
            abort("Unsupported data type of the branch - title is: "+title);
            return QVariant();
        }

    }
    else
    {
        abort("Tree branch type " + type + " is not supported");
        return QVariant();
    }

    t->ResetBranchAddresses();
    return varList;
}

const QVariantList assertBinsAndRanges(const QVariant& in)
{
    QVariantList out;
    bool bOK;

    QVariantList inVL = in.toList();
    if (inVL.size() == 3)
    {
        int bins = inVL.at(0).toInt(&bOK);
        if (!bOK) bins = 100;
        double from = inVL.at(1).toDouble(&bOK); if (!bOK) from = 0;
        double to   = inVL.at(2).toDouble(&bOK); if (!bOK) to   = 0;
        out << bins << from << to;
    }
    else out << 100 << 0.0 << 0.0;
    return out;
}

void AInterfaceToTTree::Draw(const QString &TreeName, const QString &what, const QString &cuts, const QString &options, const QVariant binsAndRanges)
{
    if (!bGuiThread)
    {
        abort("Threads cannot draw!");
        return;
    }

    ARootTreeRecord* r = dynamic_cast<ARootTreeRecord*>(TmpHub->Trees.getRecord(TreeName));
    if (!r)
        abort("Tree "+TreeName+" not found!");
    else
    {
        QVariantList vlIn = binsAndRanges.toList();
        QVariantList out;
        for (int i = 0; i < 3; i++)
        {
            QVariantList el = assertBinsAndRanges( i < vlIn.size() ? vlIn.at(i) : 0 );
            out.push_back( el );
        }

        r->externalLock();
        emit RequestTreeDraw((TTree*)r->GetObject(), what, cuts, options, out);
        //r->reconnectBranchAddresses();
        r->externalUnlock();
    }
}

bool AInterfaceToTTree::DeleteTree(const QString& TreeName)
{
    return TmpHub->Trees.remove(TreeName);
}

void AInterfaceToTTree::DeleteAllTrees()
{
    TmpHub->Trees.clear();
}