#include "awebsocketserverdialog.h"
#include "ui_awebsocketserverdialog.h"
#include "mainwindow.h"
#include "globalsettingsclass.h"
#include "anetworkmodule.h"
#include "globalsettingswindowclass.h"

#include <QDebug>

AWebSocketServerDialog::AWebSocketServerDialog(MainWindow *MW) :
    QDialog(MW), MW(MW),
    ui(new Ui::AWebSocketServerDialog)
{
    ui->setupUi(this);
    setWindowTitle("ANTS2 servers");

    updateNetGui();
    QObject::connect(MW->GlobSet->NetModule, &ANetworkModule::StatusChanged, this, &AWebSocketServerDialog::updateNetGui);

    QObject::connect(MW->GlobSet->NetModule, &ANetworkModule::ReportTextToGUI, this, &AWebSocketServerDialog::addText);
}

#include "windownavigatorclass.h"
void AWebSocketServerDialog::updateNetGui()
{
  bool bW  = MW->GlobSet->NetModule->isWebSocketServerRunning();
  bool bJ  = MW->GlobSet->NetModule->isRootServerRunning();
  bool bbJ = false;
#ifdef USE_ROOT_HTML
       bbJ = true;
#endif

  ui->labWS->setText( bW ? "Running" : "Stopped" );
  ui->pbStartWS->setEnabled( !bW );
  ui->pbStopWS->setEnabled(   bW );
  ui->pteWS->setEnabled( bW );

  ui->labJSR->setText( bJ && bbJ ? "Running" : "Stopped" );
  ui->pbStartJSR->setEnabled( !bJ && bbJ );
  ui->pbStopJSR->setEnabled(   bJ && bbJ );

  if (bW)
  {
      MW->WindowNavigator->BusyOn();
      this->show();
  }
  else MW->WindowNavigator->BusyOff();
}

void AWebSocketServerDialog::addText(const QString text)
{
    ui->pteWS->appendPlainText(text);
}

AWebSocketServerDialog::~AWebSocketServerDialog()
{
    delete ui;
}

void AWebSocketServerDialog::on_pbStartWS_clicked()
{
    MW->GlobSet->NetModule->StartWebSocketServer(MW->GlobSet->DefaultWebSocketPort);
}

void AWebSocketServerDialog::on_pbStopWS_clicked()
{
    MW->GlobSet->NetModule->StopWebSocketServer();
}

void AWebSocketServerDialog::on_pbStartJSR_clicked()
{
    MW->GlobSet->NetModule->StartRootHttpServer(MW->GlobSet->DefaultRootServerPort, MW->GlobSet->ExternalJSROOT);  //does nothing if compilation flag is not set
}

void AWebSocketServerDialog::on_pbStopJSR_clicked()
{
    MW->GlobSet->NetModule->StopRootHttpServer();
}

void AWebSocketServerDialog::on_pbSettings_clicked()
{
    MW->GlobSetWindow->SetTab(5);
    MW->GlobSetWindow->show();
}
