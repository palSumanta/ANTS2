#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "scanfloodstructure.h"

#include <QMainWindow>
#include <QVector>
#include <QThread>
#include <QTimer>

// forward declarations
class AConfiguration;
struct AEnergyDepositionCell;
class GeoMarkerClass;
class AParticleOnStack;
class AMaterialParticleCollection;
class EventsDataClass;
class GeneralSimSettings;
class GeometryWindowClass;
class GraphWindowClass;
class RasterWindow;
class LRFwindow;
class ReconstructionWindow;
class SensorLRFs;
class ExamplesWindow;
class CheckUpWindowClass;
class DetectorAddOnsWindow;
class pms;
class ReconstructionManagerClass;
class MaterialInspectorWindow;
class OutputWindow;
class QComboBox;
class TH1D;
class TH1I;
class ParticleSourcesClass;
class WindowNavigatorClass;
class GeometryWindowAddOn;
class GlobalSettingsClass;
class GlobalSettingsWindowClass;
class GainEvaluatorWindowClass;
class TApplication;
class Viewer2DarrayObject;
class GenericScriptWindowClass;
class DetectorClass;
class ASimulatorRunner;
class ParticleSourceSimulator;
class TmpObjHubClass;
class ASlabListWidget;
class InterfaceToPMscript;
class InterfaceToNodesScript;
class QMessageBox;
class QListWidgetItem;
class QFile;
class ASimulationManager;
class AScriptWindow;
class ALrfWindow;
class ANetworkModule;
struct ParticleSourceStructure;

#ifdef ANTS_FANN
class NeuralNetworksWindow;
#endif


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(DetectorClass *Detector,
                        EventsDataClass *EventsDataHub,
                        TApplication *RootApp,
                        ASimulationManager *SimulationManager,
                        ReconstructionManagerClass *ReconstructionManager,
                        ANetworkModule *Net,
                        TmpObjHubClass *TmpHub,
                        GlobalSettingsClass *GlobSet);
    ~MainWindow();

    //All config
    AConfiguration* Config;

    //ROOT
    TApplication *RootApp;

    //ANTS2 windows
    GraphWindowClass *GraphWindow;
    GeometryWindowClass *GeometryWindow;   
    OutputWindow *Owindow;
    LRFwindow *lrfwindow;
    ReconstructionWindow *Rwindow;
    MaterialInspectorWindow *MIwindow;
    WindowNavigatorClass *WindowNavigator;
    ExamplesWindow* ELwindow;
    DetectorAddOnsWindow* DAwindow;
    CheckUpWindowClass* CheckUpWindow;
    GainEvaluatorWindowClass* GainWindow;
    GenericScriptWindowClass* GenScriptWindow;
    GlobalSettingsWindowClass* GlobSetWindow;
    AScriptWindow* ScriptWindow; //global script window
    ALrfWindow* newLrfWindow; //window of the new LRF module

#ifdef ANTS_FANN
    NeuralNetworksWindow* NNwindow;
#endif

    //ANTS2 modules    
    pms* PMs;    //alias
    ASimulationManager* SimulationManager; //alias
    ReconstructionManagerClass *ReconstructionManager;   //alias
    ParticleSourcesClass* ParticleSources; //alias - actually belongs to SimulationManager
    ANetworkModule* NetModule;

    //Data hub
    EventsDataClass *EventsDataHub;

    //Detector-related properties
    DetectorClass *Detector;
    AMaterialParticleCollection* MpCollection; //just the pointer, detector has the same
    ASlabListWidget* lw;

    //Temporary objects (more will migrate there later)
    TmpObjHubClass *TmpHub;

    //local data, just for GUI
    QVector<AEnergyDepositionCell*> EnergyVector;
    QVector<GeoMarkerClass*> GeoMarkers;
    QVector<AParticleOnStack*> ParticleStack;

    InterfaceToPMscript *PMscriptInterface;

    QVector<QVector3D*> CustomScanNodes;
    InterfaceToNodesScript *NodesScriptInterface;
    QString NodesScript;

    //global settings
    GlobalSettingsClass* GlobSet; //global settings

    //critical - updates
    void NumberOfPMsHaveChanged();

    //busy triggered on start of user action that can take long time and user should not be able
    //to change any settins
    void onBusyOn();
    void onBusyOff();

    void startRootUpdate();  // starts timer which in regular intervals process Root events
    void stopRootUpdate();   // stops timer which in regular intervals process Root events

    void ClearData(); //clear status, disconnect reconstruction

    //detector constructor
    void ReconstructDetector(bool fKeepData = false);

    void ListActiveParticles();

    void ShowGraphWindow();
    void UpdateMaterialListEdit();

    void UpdateTestWavelengthProperties(); //if material properties were updated, need to update indication in the Test tab

    void ShowTracks();
    void ShowGeometry(bool ActivateWindow = true, bool SAME = true, bool ColorUpdateAllowed = true);    

    void writeDetectorToJson(QJsonObject &json); //GDML is NOT here
    bool readDetectorFromJson(QJsonObject &json);
    void writeSimSettingsToJson(QJsonObject &json, bool fVerbose = false);  //true - save point and source settings
    bool readSimSettingsFromJson(QJsonObject &json);

    //save data to file - public due to batch mode usage
    int LoadSimulationDataFromTree(QString fileName, int maxEvents = -1);
    int LoadPMsignals(QString fileName);
    //
    void ExportDeposition(QFile &outputFile);
    void ImportDeposition(QFile &file);

    //configuration from outside
    void setShowTop(bool flag) {ShowTop = flag;}
    void setColorByMaterial(bool flag) {ColorByMaterial = flag;}
    void SetProgress(int val);

    //gains and ch per ph.el
    void SetMultipliersUsingGains(QVector<double> Gains);
    void SetMultipliersUsingChPhEl(QVector<double> ChPerPhEl);

    //config setting (updated bu the Examples window ELwindow)
    bool ShowExamplesOnStart;

    //public flags
    bool DoNotUpdateGeometry;  //if GUI is in bulk-update, we do not detector geometry be updated on each line
    bool GeometryDrawDisabled; //no drawing of th geometry or tracks
    bool fStartedFromGUI;          //flag indicating that an action was run from GUI, e.g. simulation

    bool isWavelengthResolved() const;
    double WaveFrom, WaveTo, WaveStep;
    int WaveNodes;

    QVector<QString> NoiseTypeDescriptions;

    TH1D *histSecScint;

    int ScriptWinX, ScriptWinY, ScriptWinW, ScriptWinH;
    void recallGeometryOfScriptWindow();
    void extractGeometryOfScriptWindow();

    void LoadSecScintTheta(QString fileName);

    int PMArrayType(int ul);
    void SetPMarrayType(int ul, int itype);

    void LoadDummyPMs(QString DFile);

    void ShowGeoMarkers(); //Show dots on ALREADY PREPARED geometry window!

    //handling of material COBs
    void AddMaterialToCOBs(QString s);

    void CheckPresenseOfSecScintillator();
    void DeleteLoadedEvents(bool KeepFileList = false);       
    void SavePreprocessingAddMulti(QString fileName);
    void LoadPreprocessingAddMulti(QString filename);

public slots:   
    void LoadEventsListContextMenu(const QPoint &pos);
    void LRF_ModuleReadySlot(bool ready);
    void on_pbSimulate_clicked();
    void on_pbTrackStack_clicked();
    void on_pbGenerateLight_clicked();
    void on_pbParticleSourcesSimulate_clicked();    
    void RefreshPhotSimOnTimer(int Progress, double msPerEv);
    void PMscriptSuccess();
    void NodesScriptSuccess();
    void onGDMLstatusChage(bool fGDMLactivated);
    void updateLoaded(int events, int progress);
    void on_pbSingleSourceShow_clicked();
    void on_pbClearAllStack_clicked();
    void on_pbRefreshStack_clicked();
    void ShowGeometrySlot();

private slots:
    void on_pbAddParticleToStack_clicked();
    void on_pbRemoveFromStack_clicked();
    void on_pbRefreshMaterials_clicked();
    void on_cbXbyYarray_stateChanged(int arg1);
    void on_cbRingsArray_stateChanged(int arg1);
    void on_pbRefreshOverrides_clicked();
    void on_pbOverride_clicked();
    void on_pbStartMaterialInspector_clicked();
    void on_pbAddparticleToActive_clicked();
    void on_cobParticleToInspect_currentIndexChanged(int index);
    void on_leParticleName_editingFinished();
    void on_pbRefreshParticles_clicked();
    void on_cbSecondAxis_toggled(bool checked);
    void on_cbThirdAxis_toggled(bool checked);
    void on_cobPMdeviceType_currentIndexChanged(int index);
    void on_cbUPM_toggled(bool checked);
    void on_cbLPM_toggled(bool checked);
    void on_pbRefreshPMArrayData_clicked();
    void on_pbShowPMsArrayRegularData_clicked();
    void on_pbRefreshPMproperties_clicked();
    void on_pbUpdatePMproperties_clicked();
    void on_sbPMtype_valueChanged(int arg1);
    void on_sbPMtypeForGroup_valueChanged(int arg1);
    void on_cbXbyYarray_clicked();
    void on_cbRingsArray_clicked();
    void on_cobUpperLowerPMs_currentIndexChanged(int index);
    void on_pbRemoveThisPMtype_clicked();
    void on_pbAddNewPMtype_clicked();
    void on_cbAreaSensitive_toggled(bool checked);
    void on_cbAngularSensitive_toggled(bool checked);
    void on_cbTimeResolved_toggled(bool checked);
    void on_pbSavePMtype_clicked();
    void on_pbLoadPMtype_clicked();
    void on_pbPMtypeShowCurrent_clicked();
    void on_cbWaveResolved_toggled(bool checked);
    void on_ledWaveFrom_editingFinished();
    void on_ledWaveTo_editingFinished();
    void on_ledWaveStep_editingFinished();
    void on_pbUpdateTestWavelengthProperties_clicked();
    void on_pbTestShowPrimary_clicked();
    void on_pbTestShowSecondary_clicked();
    void on_cobMaterialForWaveTests_currentIndexChanged(int index);
    void on_pbTestGeneratorPrimary_clicked();
    void on_pbTestGeneratorSecondary_clicked();
    void on_pbTestShowRefrIndex_clicked();
    void on_pbTestShowAbs_clicked();
    void on_pbShowThisMatInfo_clicked();
    void on_sbWaveIndexPointSource_valueChanged(int arg1);
    void on_pbShowPDE_clicked();
    void on_pbLoadPDE_clicked();
    void on_pbDeletePDE_clicked();
    void on_pbShowPDEbinned_clicked();
    void on_pbPMtypeLoadAngular_clicked();
    void on_pbPMtypeShowAngular_clicked();
    void on_pbPMtypeDeleteAngular_clicked();
    void on_pbPMtypeShowEffectiveAngular_clicked();
    void on_sbCosBins_valueChanged(int arg1);
    void on_cobGunSourceType_currentIndexChanged(int index);
    void on_pbGunShowSource_clicked();
    void on_pbGunTest_clicked();
    void on_pbGunRefreshparticles_clicked();
    void on_pbGunAddNew_clicked();
    void on_pbGunRemove_clicked();
    void on_cobGunParticle_activated(int index);
    void on_ledGunEnergy_editingFinished();
    void on_ledGunParticleWeight_editingFinished();
    void on_pbGunLoadSpectrum_clicked();
    void on_pbGunShowSpectrum_clicked();
    void on_pbGunDeleteSpectrum_clicked();
    void on_ledGunAverageNumPartperEvent_editingFinished();
    void on_ledMediumRefrIndex_editingFinished();
    void on_pbPMtypeLoadArea_clicked();
    void on_pbPMtypeShowArea_clicked();
    void on_pbPMtypeDeleteArea_clicked();
    void on_cobPMarrayRegularity_currentIndexChanged(int index);
    void on_sbIndPMnumber_valueChanged(int arg1);
    void on_pbIndPMshowInfo_clicked();
    void on_tabWidget_currentChanged(int index);
    void on_pbUpdateToFullCustom_clicked();
    void on_pbUpdateToFixedZ_clicked();
    void on_pbIndPmRemove_clicked();
    void on_pbIndShowType_clicked();
    void on_ledIndEffectiveDE_editingFinished();
    void on_pbIndRestoreEffectiveDE_clicked();
    void on_pbIndShowDE_clicked();
    void on_pbIndRestoreDE_clicked();
    void on_pbIndLoadDE_clicked();
    void on_pbIndShowDEbinned_clicked();
    void on_pbAddPM_clicked();
    void on_pbIndLoadAngular_clicked();
    void on_pbIndRestoreAngular_clicked();
    void on_pbIndShowAngular_clicked();
    void on_pbIndShowEffectiveAngular_clicked();
    void on_ledIndMediumRefrIndex_editingFinished();
    void on_pbIndLoadArea_clicked();
    void on_pbIndRestoreArea_clicked();
    void on_pbIndShowArea_clicked();
    void on_cbGunAllowMultipleEvents_toggled(bool checked);
    void LoadPMsignalsRequested();
    void on_pbDeleteLoadedEvents_clicked();
    void on_pbPreprocessingLoad_clicked();
    void on_pbPreprocessingSave_clicked();
    void on_sbPreprocessigPMnumber_valueChanged(int arg1);
    void on_ledPreprocessingAdd_editingFinished();
    void on_pbReloadExpData_clicked();
    void on_cobPMshape_currentIndexChanged(int index);
    void on_pbElGainLoadDistr_clicked();
    void on_pbElGainShowDistr_clicked();
    void on_pbElTestGenerator_clicked();
    void on_sbElPMnumber_valueChanged(int arg1);
    void on_pbElUpdateIndication_clicked();
    void on_pbElCopyGainData_clicked();
    void on_cbEnableSPePHS_toggled(bool checked);
    void on_cbEnableADC_toggled(bool checked);
    void on_pbScanDistrLoad_clicked();
    void on_pbScanDistrShow_clicked();
    void on_pbScanDistrDelete_clicked();
    void on_pbUpdateScanFloodTabWidget_clicked();
    void on_pbInitializeScanFloodNoise_clicked();
    void on_tabwidScanFlood_cellChanged(int row, int column);
    void on_actionWindow_navigator_triggered();
    void on_actionGeometry_triggered();
    void on_actionReconstructor_triggered();
    void on_actionOutput_triggered();
    void on_actionLRF_triggered();
    void on_actionGraph_triggered();
    void on_actionReset_position_of_windows_triggered();
    void on_actionSave_position_and_stratus_of_all_windows_triggered();
    void on_actionLoad_positions_and_status_of_all_windows_triggered();
    void on_actionMaterial_inspector_window_triggered();
    void on_twSingleScan_currentChanged(int index);
    void on_pbExportDeposition_clicked();
    void on_pbImportDeposition_clicked();
    void on_pbTestsInterpolation_clicked();
    void on_cbEnableElNoise_toggled(bool checked);
    void on_actionExamples_triggered();
    void on_cobSecScintillationGenType_currentIndexChanged(int index);
    void on_pbSecScintShowProfile_clicked();
    void on_pbSecScintLoadProfile_clicked();
    void on_pbSecScintDeleteProfile_clicked();
    void on_lwMaterials_currentRowChanged(int currentRow);
    void on_lwMaterials_doubleClicked(const QModelIndex &index);
    void on_lwParticles_currentRowChanged(int currentRow);
    void on_pbReconstruction_clicked();
    void on_pbConfigureAddOns_clicked();
    void LoadSimTreeRequested();
    void on_pbRemoveSource_clicked();
    void on_pbAddSource_clicked();
    void on_pbUpdateSources_clicked();
    void on_pbUpdateSourcesIndication_clicked();

protected:
    void closeEvent(QCloseEvent *);    
    bool event(QEvent *event);

private:
    Ui::MainWindow *ui;
    QTimer *RootUpdateTimer; //root update timer
    QMessageBox *msBox; //box to be used to confirm discard or save sim data on data clear; 0 if not activated

    //flags
    bool TriggerForbidden;
    bool BulkUpdate;

    TH1I* histScan;

public:
    bool ShutDown; //when exiting ANTS2 by closing the main window    

    bool fSimDataNotSaved;

    void createScriptWindow();

    void SimGeneralConfigToJson(QJsonObject &jsonMaster);                              //Save to JSON general options of simulation
    void SimPointSourcesConfigToJson(QJsonObject &jsonMaster, bool fVerbose = false);  //Save to JSON config for PointSources simulation
    void SimParticleSourcesConfigToJson(QJsonObject &json);     //Save to JSON config for ParticleSources simulation

    void updatePMArrayDataIndication();    

    void writeLoadExpDataConfigToJson(QJsonObject &json);
    bool readLoadExpDataConfigFromJson(QJsonObject &json);
    void clearGeoMarkers(int All_Rec_True = 0);
    void clearCustomScanNodes();
    void setFontSizeAllWindows(int size);
    void writeExtraGuiToJson(QJsonObject &json);
    void readExtraGuiFromJson(QJsonObject &json);
    void SaveSimulationDataTree();
    void SaveSimulationDataAsText();

    void setFloodZposition(double Z);
    void UpdateCustomScanNodesIndication();
    void CalculateIndividualQEPDE(); //Public for use in scripting
    void clearEnergyVector();
private:
    bool startupDetector();  //on ANTS start load/create detector
    void PointSource_UpdateTabWidget();
    void PointSource_InitTabWidget();
    void PointSource_ReadTabWidget();
    void CheckSetMaterial(const QString name, QComboBox* cob, QVector<QString>* vec);
    void ToggleUpperLowerPMs();
    void PopulatePMarray(int ul, double z, int istart);
    void AddDefaultPMtype();
    void CorrectWaveTo();
    void RefreshAngularButtons();
    void RefreshAreaButtons();
    void ShowPMcount();
    void initOverridesAfterLoad();  //after detector is loaded, show first non-empty optical override
    //void SavePhElToSignalData(QString fileName);
    //void LoadPhElToSignalData(QString fileName);
    void LoadScanPhotonDistribution(QString fileName);

    int LoadAreaResponse(QString fileName, QVector<QVector<double> >* tmp, double* xStep, double* yStep);       ///see MainWindowDiskIO.cpp
    int LoadSPePHSfile(QString fileName, QVector<double>* SPePHS_x, QVector<double>* SPePHS);                   ///see MainWindowDiskIO.cpp    
    QStringList LoadedEventFiles, LoadedTreeFiles;

    void ShowSource(int isource, bool clear = true);

    QString CheckerScript;

    QVector<ScanFloodStructure> ScanFloodNoise;
    bool NoiseTableLocked;
    double ScanFloodNoiseProbability;

    QString PreprocessingFileName;

    int PreviousNumberPMs;
    bool ShowTop;
    bool ColorByMaterial;
    bool fConfigGuiLocked;

    bool populateTable; //for SimLoadConfig - compatability check

    bool fStopLoadRequested;

    void clearPreprocessingData();
    void updateCOBsWithPMtypeNames();
    void ViewChangeRelFactors(QString options);    
    void RemoveParticle(int Id);

private slots:
    void timerTimeout(); //timer-based update of Root events

    void on_cbIndividualParticle_clicked(bool checked);
    void on_ledLinkingProbability_editingFinished();
    void on_pbShowCheckUpWindow_clicked();
    void on_lePreprocessingMultiply_editingFinished();
    void on_extractPedestals_clicked();   
    void on_sbLoadedEnergyChannelNumber_editingFinished();
    void on_pbPositionScript_clicked();
    void on_cobPMtypes_currentIndexChanged(int index);
    void on_pbShowMaterialInPMtypes_clicked();
    void on_cobPMtypeInArrays_currentIndexChanged(int index);
    void on_pbLoadPMcenters_clicked();
    void on_pbSavePMcenters_clicked();
    void on_pbSetPMtype_clicked();    
    void on_pbViewChangeRelQEfactors_clicked();
    void on_pbLoadRelQEfactors_clicked();
    void on_pbViewChangeRelELfactors_clicked();
    void on_pbLoadRelELfactors_clicked();
    void on_pbRandomScaleELaverages_clicked();
    void on_pbSetELaveragesToUnity_clicked();
    void on_pbShowRelGains_clicked();
    void on_actionSave_configuration_triggered();
    void on_actionLoad_configuration_triggered();
    void on_actionQuicksave_triggered();
    void on_actionQuickload_triggered();
    void on_pbRemoveParticle_clicked();
    void on_lwGunParticles_currentRowChanged(int currentRow);
    void on_pbSaveParticleSource_clicked();
    void on_pbLoadParticleSource_clicked();
    void on_pbSaveResults_clicked();
    void on_pbRemoveMaterial_clicked();
    void on_lwLoadedEventsFiles_itemChanged(QListWidgetItem *item);
    void on_pobTest_clicked();
    void on_pbCheckDerivatives_clicked();    
    void on_actionGain_evaluation_triggered();
    void on_leiParticleLinkedTo_editingFinished();
    void on_cbLRFs_toggled(bool checked);  
    void on_pbClearAdd_clicked();
    void on_pbClearMulti_clicked();
    void on_actionCredits_triggered();
    void on_actionVersion_triggered();
    void on_actionLicence_triggered();
    void on_pnShowHideAdvanced_toggled(bool checked);
    void on_pbYellow_clicked();
    void on_pbReconstruction_2_clicked();
    void on_cobXYtype_activated(int index);
    void on_cobTOP_activated(int index);
    void on_actionNew_detector_triggered();
    void on_pbSurfaceWLS_Show_clicked();
    void on_pbSurfaceWLS_Load_clicked();
    void on_pbSurfaceWLS_ShowSpec_clicked();
    void on_pbSurfaceWLS_LoadSpec_clicked();
    void on_pbReloadTreeData_clicked();
    void on_pbRenameSource_clicked();
    void on_pbStopLoad_clicked();
    void on_pbConfigureNumberOfThreads_clicked();
    void on_cobFixedDirOrCone_currentIndexChanged(int index);
    void on_cbLinkingOpposite_clicked(bool checked);
    void on_pbShowComptonAngles_clicked();
    void on_pbShowComptonEnergies_clicked();
    void on_pbCheckRandomGen_clicked();

private slots:
    void on_cbPointSourceBuildTracks_toggled(bool checked);
    void on_cbGunPhotonTracks_toggled(bool checked);
    void on_cbBuilPhotonTrackstester_toggled(bool checked);

    /************************* Simulation *************************/
public:
    void startSimulation(QJsonObject &json);
private:   
    ParticleSourceSimulator *setupParticleTestSimulation(GeneralSimSettings &simSettings);
signals:
    void StopRequested();
private slots:
    void simulationFinished();
    /**************************************************************/

    void on_pbGDML_clicked();
    void on_pbLoadNodes_clicked();
    void on_pbShowNodes_clicked();
    void on_pbRunNodeScript_clicked();
    void on_cobPMdeviceType_activated(const QString &arg1);
    void on_cobMatPointSource_activated(int index);
    void on_pbShowColorCoding_pressed();
    void on_pbShowColorCoding_released();
    void on_ledSourceActivity_textChanged(const QString &arg1);
    void on_actionOpen_settings_triggered();
    void on_actionSave_Load_windows_status_on_Exit_Init_toggled(bool arg1);
    void on_pbShowEnergyDeposition_clicked();
    void on_pbUpdateElectronics_clicked();
    void on_ledSimplisticAbs_editingFinished();
    void on_ledSimplisticSpecular_editingFinished();
    void on_ledSimplisticScatter_editingFinished();
    void on_pbOverlay_clicked();
    void on_pbScalePDE_clicked();
    void on_pbLoadManifestFile_clicked();
    void on_pbDeleteManifestFile_clicked();
    void on_pbManifestFileHelp_clicked();
    void on_pbLoadAppendFiles_clicked();
    void on_sbLoadASCIIpositionXchannel_valueChanged(int arg1);
    void on_pbPDEFromWavelength_clicked();
    void on_cobLoadDataType_customContextMenuRequested(const QPoint &pos);
    void on_pbManuscriptExtractNames_clicked();
    void on_pbShowDetailedLog_clicked();
    void on_pbCSMtestmany_clicked();
    void on_pbST_showTracks_clicked();
    void on_pbST_AngleCos_clicked();
    void on_lwOverrides_itemClicked(QListWidgetItem *item);
    void on_pbST_uniform_clicked();
    void on_pbST_RvsAngle_clicked();
    void on_pbTestFit_clicked();
    void on_pbST_ReflectionVsParameter_clicked();
    void on_pbST_VsParameterHelp_clicked();
    void on_cobST_ShowWhatRef_activated(int index);
    void on_pbSF_SvsL_clicked();
    void on_pbSF_help_clicked();
    void on_pbTestFit_simplistic_clicked();
    void on_actionGlobal_script_triggered();
    void on_pbLockGui_clicked();
    void on_pbUnlockGui_clicked();
    void on_actionNewLRFModule_triggered();
    void on_cobPMampGainModel_currentIndexChanged(int index);
    void on_pbAddCellMCcrosstalk_clicked();
    void on_pbShowMCcrosstalk_clicked();
    void on_pbLoadMCcrosstalk_clicked();
    void on_tabMCcrosstalk_cellChanged(int row, int column);
    void on_cbEnableMCcrosstalk_toggled(bool checked);
    void on_pbRemoveCellMCcrosstalk_clicked();
    void on_pbMCnormalize_clicked();

    void on_leSourceLimitMaterial_textChanged(const QString &arg1);

    void on_leLimitNodesObject_textChanged(const QString &arg1);

    void on_cbLimitNodesOutsideObject_toggled(bool checked);

    void on_bpResults_clicked();

    void on_pobTest_2_clicked();

    void on_bpResults_2_clicked();

    void on_actionScript_window_triggered();

public slots:
    void on_cobSF_chi2Vs_activated(int index);
    void on_pbRebuildDetector_clicked();

    void onRequestDetectorGuiUpdate();     // called to update GUI related to Detector
    void onRequestSimulationGuiUpdate();   // called to update GUI related to simulations
    void onRequestUpdateGuiForClearData(); // called to clear data indication after EventsDataHub.clear() is triggered
    void UpdatePreprocessingSettingsIndication();  // called when preprocessing settings were modified in EventsDataHub
    void onGlobalScriptStarted();
    void onGlobalScriptFinished();
    void on_pbUpdatePreprocessingSettings_clicked(); //updates preprocessing settings in Config
    void on_pbUpdateSimConfig_clicked();   // updates simulation-related properies in Config from GUI
    void selectFirstActiveParticleSource(); //trigger after load new config to select the first source with non-zero activity

private:
    void initDetectorSandwich();
    void SourceUpdateThisParticleIndication();
    void onGuiEnableStatus(bool fLocked);
    void clearParticleSourcesIndication();   
    void updateOneParticleSourcesIndication(ParticleSourceStructure *ps);

    //new sandwich
public slots:
    void UpdateSandwichGui();
    void OnWarningMessage(QString text);
    void OnDetectorColorSchemeChanged(int scheme, int matId);    
    void OnSlabDoubleClicked(QString SlabName);
    void onNewConfigLoaded();

signals:
    void RequestStopLoad();
};

#endif // MAINWINDOW_H
