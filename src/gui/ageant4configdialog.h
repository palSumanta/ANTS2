#ifndef AGEANT4CONFIGDIALOG_H
#define AGEANT4CONFIGDIALOG_H

#include <QDialog>

class AG4SimulationSettings;

namespace Ui {
class AGeant4ConfigDialog;
}

class AGeant4ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AGeant4ConfigDialog(AG4SimulationSettings & G4SimSet, QWidget *parent = 0);
    ~AGeant4ConfigDialog();

private slots:
    void on_pbAddCommand_clicked();

    void on_pbRemoveCommand_clicked();

    void on_pbAccept_clicked();

    void on_pbAddVolume_clicked();

    void on_pbRemoveVolume_clicked();

private:
    AG4SimulationSettings & G4SimSet;
    Ui::AGeant4ConfigDialog *ui;
};

#endif // AGEANT4CONFIGDIALOG_H
