#ifndef APARTICLESOURCEDIALOG_H
#define APARTICLESOURCEDIALOG_H

#include "particlesourcesclass.h"

#include <QDialog>

namespace Ui {
class AParticleSourceDialog;
}

class MainWindow;

class AParticleSourceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AParticleSourceDialog(MainWindow & MW, const AParticleSourceRecord * Rec);
    ~AParticleSourceDialog();

    AParticleSourceRecord* getResult(); //transfers ownership

private slots:
    void on_pbAccept_clicked();
    void on_pbReject_clicked();
    void on_pbGunTest_clicked();

    void on_cobGunSourceType_currentIndexChanged(int index);

    void on_pbGunAddNew_clicked();

    void on_pbGunRemove_clicked();

    void on_lwGunParticles_currentRowChanged(int currentRow);

    void on_cobUnits_activated(int index);

    void on_pbShowSource_toggled(bool checked);

    void on_cbLinkedParticle_toggled(bool checked);

    void on_pbUpdateRecord_clicked();

private:
    MainWindow& MW;

    AParticleSourceRecord* Rec;

    Ui::AParticleSourceDialog *ui;

private:
    void UpdateListWidget();
    void UpdateParticleInfo();
};

#endif // APARTICLESOURCEDIALOG_H
