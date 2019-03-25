#include "ageowin_si.h"
#include "mainwindow.h"
#include "simulationmanager.h"
#include "geometrywindowclass.h"
#include "reconstructionwindow.h"
#include "detectorclass.h"
#include "atrackrecords.h"

#include <QApplication>
#include <QThread>

#include "TGeoManager.h"
#include "TVirtualGeoTrack.h"
#include "TGeoTrack.h"

AGeoWin_SI::AGeoWin_SI(MainWindow *MW, ASimulationManager *SimManager)
 : MW(MW), SimManager(SimManager), Detector(MW->Detector)
{
  Description = "Access to the Geometry window of GUI";

  H["SaveImage"] = "Save image currently shown on the geometry window to an image file.\nTip: use .png extension";
}

double AGeoWin_SI::GetPhi()
{
    return MW->GeometryWindow->Phi;
}

double AGeoWin_SI::GetTheta()
{
  return MW->GeometryWindow->Theta;
}

void AGeoWin_SI::SetPhi(double phi)
{
  MW->GeometryWindow->Phi = phi;
}

void AGeoWin_SI::SetTheta(double theta)
{
  MW->GeometryWindow->Theta = theta;
}

void AGeoWin_SI::Rotate(double Theta, double Phi, int Steps, int msPause)
{
  if (Steps <= 0) return;
  double stepT = Theta/Steps;
  double stepP = Phi/Steps;
  double T0 = GetTheta();
  double P0 = GetPhi();

  QTime time;
  MW->GeometryWindow->fNeedZoom = false;
  MW->GeometryWindow->fRecallWindow = true;
  for (int i=0; i<Steps; i++)
    {
      qApp->processEvents();
      time.restart();
      MW->GeometryWindow->Theta = T0 + stepT*(i+1);
      MW->GeometryWindow->Phi   = P0 + stepP*(i+1);
      MW->GeometryWindow->PostDraw();

      int msPassed = time.elapsed();
      if (msPassed<msPause) QThread::msleep(msPause-msPassed);
    }
}

void AGeoWin_SI::SetZoom(int level)
{
  MW->GeometryWindow->ZoomLevel = level;
  MW->GeometryWindow->Zoom(true);
  MW->GeometryWindow->on_pbShowGeometry_clicked();
  MW->GeometryWindow->readRasterWindowProperties();
}

void AGeoWin_SI::UpdateView()
{
  MW->GeometryWindow->fRecallWindow = true;
  MW->GeometryWindow->PostDraw();
  MW->GeometryWindow->UpdateRootCanvas();
}

void AGeoWin_SI::SetParallel(bool on)
{
  MW->GeometryWindow->ModePerspective = !on;
}

void AGeoWin_SI::Show()
{
  MW->GeometryWindow->showNormal();
  MW->GeometryWindow->raise();
}

void AGeoWin_SI::Hide()
{
  MW->GeometryWindow->hide();
}

void AGeoWin_SI::BlockUpdates(bool on)
{
  MW->DoNotUpdateGeometry = on;
  MW->GeometryDrawDisabled = on;
}

int AGeoWin_SI::GetX()
{
  return MW->GeometryWindow->x();
}

int AGeoWin_SI::GetY()
{
  return MW->GeometryWindow->y();
}

int AGeoWin_SI::GetW()
{
  return MW->GeometryWindow->width();
}

int AGeoWin_SI::GetH()
{
    return MW->GeometryWindow->height();
}

QVariant AGeoWin_SI::GetWindowGeometry()
{
    QVariantList vl;
    vl << MW->GeometryWindow->x() << MW->GeometryWindow->y() << MW->GeometryWindow->width() << MW->GeometryWindow->height();
    return vl;
}

void AGeoWin_SI::SetWindowGeometry(QVariant xywh)
{
    if (xywh.type() != QVariant::List)
    {
        abort("Array [X Y Width Height] is expected");
        return;
    }
    QVariantList vl = xywh.toList();
    if (vl.size() != 4)
    {
        abort("Array [X Y Width Height] is expected");
        return;
    }

    int x = vl[0].toInt();
    int y = vl[1].toInt();
    int w = vl[2].toInt();
    int h = vl[3].toInt();
    //MW->GeometryWindow->setGeometry(x, y, w, h);
    MW->GeometryWindow->move(x, y);
    MW->GeometryWindow->resize(w, h);
}

void AGeoWin_SI::ShowGeometry()
{
  MW->GeometryWindow->readRasterWindowProperties();
  MW->GeometryWindow->ShowGeometry(false);
}

void AGeoWin_SI::ShowPMnumbers()
{
  MW->GeometryWindow->on_pbShowPMnumbers_clicked();
}

void AGeoWin_SI::ShowReconstructedPositions()
{
  //MW->Rwindow->ShowReconstructionPositionsIfWindowVisible();
  MW->Rwindow->ShowPositions(0, true);
}

void AGeoWin_SI::SetShowOnlyFirstEvents(bool fOn, int number)
{
  MW->Rwindow->SetShowFirst(fOn, number);
}

void AGeoWin_SI::ShowTruePositions()
{
  MW->Rwindow->DotActualPositions();
  MW->GeometryWindow->ShowGeometry(false, false);
}

void AGeoWin_SI::ShowTracks(int num, int OnlyColor)
{
  Detector->GeoManager->ClearTracks();
  if (SimManager->Tracks.isEmpty()) return;

  for (int iTr=0; iTr<SimManager->Tracks.size() && iTr<num; iTr++)
  {
      TrackHolderClass* th = SimManager->Tracks.at(iTr);
      TGeoTrack* track = new TGeoTrack(1, th->UserIndex);
      track->SetLineColor(th->Color);
      track->SetLineWidth(th->Width);
      track->SetLineStyle(th->Style);
      for (int iNode=0; iNode<th->Nodes.size(); iNode++)
        track->AddPoint(th->Nodes[iNode].R[0], th->Nodes[iNode].R[1], th->Nodes[iNode].R[2], th->Nodes[iNode].Time);

      if (track->GetNpoints()>1)
      {
          if (OnlyColor == -1  || OnlyColor == th->Color)
            Detector->GeoManager->AddTrack(track);
      }
      else delete track;
  }
  MW->GeometryWindow->DrawTracks();
}

void AGeoWin_SI::ShowSPS_position()
{
  MW->on_pbSingleSourceShow_clicked();
}

void AGeoWin_SI::ClearTracks()
{
  MW->GeometryWindow->on_pbClearTracks_clicked();
}

void AGeoWin_SI::ClearMarkers()
{
  MW->GeometryWindow->on_pbClearDots_clicked();
}

void AGeoWin_SI::SaveImage(QString fileName)
{
  MW->GeometryWindow->SaveAs(fileName);
}

void AGeoWin_SI::ShowTracksMovie(int num, int steps, int msDelay, double dTheta, double dPhi, double rotSteps, int color)
{
  int tracks = SimManager->Tracks.size();
  if (tracks == 0) return;
  if (num > tracks) num = tracks;

  int toDo;
  int thisNodes = 2;
  do //finished when all nodes of the longest track are shown
    {
      //this iteration will show track nodes < thisNode
      toDo = num; //every track when finished results in toDo--

      // cycle by indication step with interpolation between the nodes
      for (int iStep=0; iStep<steps; iStep++)
        {
          MW->Detector->GeoManager->ClearTracks();

          //cycle by tracks
          for (int iTr=0; iTr<num; iTr++)
            {
              TrackHolderClass* th = SimManager->Tracks.at(iTr);
              int ThisTrackNodes = th->Nodes.size();
              if (ThisTrackNodes <= thisNodes && iStep == steps-1) toDo--; //last node of this track

              TGeoTrack* track = new TGeoTrack(1, th->UserIndex);
              track->SetLineWidth(th->Width+1);
              if (color == -1) track->SetLineColor(15); // during tracking
              else track->SetLineColor(color); // during tracking
              int lim = std::min(thisNodes, th->Nodes.size());
              for (int iNode=0; iNode<lim; iNode++)
                {
                  double x, y, z;
                  if (iNode == thisNodes-1)
                    {
                      //here track is in interpolation mode
                      x = th->Nodes[iNode-1].R[0] + (th->Nodes[iNode].R[0]-th->Nodes[iNode-1].R[0])*iStep/(steps-1);
                      y = th->Nodes[iNode-1].R[1] + (th->Nodes[iNode].R[1]-th->Nodes[iNode-1].R[1])*iStep/(steps-1);
                      z = th->Nodes[iNode-1].R[2] + (th->Nodes[iNode].R[2]-th->Nodes[iNode-1].R[2])*iStep/(steps-1);
                      if (color == -1) track->SetLineColor(15); //black during tracking
                    }
                  else
                    {
                      x = th->Nodes[iNode].R[0];
                      y = th->Nodes[iNode].R[1];
                      z = th->Nodes[iNode].R[2];
                      if (color == -1) track->SetLineColor( th->Color );
                    }
                  track->AddPoint(x, y, z, th->Nodes[iNode].Time);
                }
              if (color == -1)
                if (ThisTrackNodes <= thisNodes && iStep == steps-1) track->SetLineColor( th->Color );

              //adding track to GeoManager
              if (track->GetNpoints()>1)
                Detector->GeoManager->AddTrack(track);
              else delete track;
            }
          MW->GeometryWindow->DrawTracks();
          Rotate(dTheta, dPhi, rotSteps);
          QThread::msleep(msDelay);
        }
      thisNodes++;
    }
  while (toDo>0);
}

void AGeoWin_SI::ShowEnergyVector()
{
  MW->Rwindow->UpdateSimVizData(0);
}
