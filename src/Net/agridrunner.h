#ifndef AGRIDRUNNER_H
#define AGRIDRUNNER_H

#include <QObject>
#include <QVector>
#include <QHostAddress>
#include <QVariant>
#include <QString>

class EventsDataClass;
class APmHub;
class ASimulationManager;
class ARemoteServerRecord;
class AWebSocketSession;
class AWebSocketWorker_Base;
class QJsonObject;
struct AGridScriptResources;

class AGridRunner : public QObject
{
    Q_OBJECT
public:
    AGridRunner(EventsDataClass & EventsDataHub, const APmHub & PMs, ASimulationManager & simMan);
    ~AGridRunner();

    QString CheckStatus();
    QString Simulate(const QJsonObject* config);
    QString Reconstruct(const QJsonObject* config);
    QString RateServers(const QJsonObject* config);

    QVariant EvaluateSript(const QString & Script, const QJsonObject & config, const QVariantList & PerThreadResources, const QVariantList & PerThreadFiles);
    QString  UploadFile(int iServer, const QString & FileName);

    void Abort();

    void SetTimeout(int timeout);

    void writeConfig();
    void readConfig();
    void clearRecords();

    QVector<ARemoteServerRecord *> ServerRecords;

public slots:
    void onRequestTextLog(int index, const QString message);

private:
    EventsDataClass & EventsDataHub;
    const APmHub & PMs;
    ASimulationManager & SimMan;
    int TimeOut = 5000;

    bool bAbortRequested = false;

private:
    AWebSocketWorker_Base * startCheckStatusOfServer(int index, ARemoteServerRecord *serverRecord);
    AWebSocketWorker_Base * startSim(int index, ARemoteServerRecord *serverRecord, const QJsonObject* config);
    AWebSocketWorker_Base * startRec(int index, ARemoteServerRecord *server, const QJsonObject* config);

    AWebSocketWorker_Base * startScriptWorker(int index, ARemoteServerRecord * serverrecord, const QJsonObject & config, const QString & script, AGridScriptResources & data);
    AWebSocketWorker_Base * startUploadWorker(int index, ARemoteServerRecord * serverrecord, const QString & fileName);

    void startInNewThread(AWebSocketWorker_Base *worker);

    void waitForWorkersToFinish(QVector<AWebSocketWorker_Base *> &workers);
    void waitForWorkersToPauseOrFinish(QVector<AWebSocketWorker_Base *> &workers);

    void regularToCustomNodes(const QJsonObject & RegularScanOptions, QJsonArray & toArray);
    void populateNodeArrayFromSimMan(QJsonArray & toArray);

    void doAbort(QVector<AWebSocketWorker_Base *> &workers);

    void onStart();
    QString commonStart();

signals:
    void requestTextLog(int index, const QString message);
    void requestStatusLog(const QString message);
    void requestDelegateGuiUpdate();

    void notifySimulationFinished();
    void notifyReconstructionFinished();

};

class AWebSocketWorker_Base : public QObject
{
    Q_OBJECT
public:
    AWebSocketWorker_Base(int index, ARemoteServerRecord* rec, int timeOut,  const QJsonObject* config = nullptr);

    bool isRunning() const {return bRunning;}
    bool isPausedOrFinished() const {return bPaused || !bRunning;}
    bool isPaused() const {return bPaused;}
    void setStarted() {bRunning = true;}
    void setPaused(bool flag) {bPaused = flag;}

    void setExtraScript(const QString& script) {extraScript = script;}

    void RequestAbort();

    //ARemoteServerRecord* getRecord() {return rec;}
    ARemoteServerRecord * rec = nullptr;

public slots:
    virtual void run() = 0;

protected:
    int index;
    int TimeOut = 5000;

    bool bRunning       = false;
    bool bPaused        = false;
    bool bExternalAbort = false;


    const QJsonObject* config;
    AWebSocketSession* ants2socket = nullptr;

    QString extraScript; //e.g. script to modify config according to distribution of sim events

    AWebSocketSession* connectToServer(int port);
    bool               allocateAntsServer();
    AWebSocketSession* connectToAntsServer();
    bool               establishSession();

    bool               sendAnts2Config();
    bool               uploadFile(const QString & LocalFileName, const QString & RemoteFileName);
    bool               evaluateScript(const QString & Script, QVariant * Result = nullptr);

signals:
    void finished();
    void requestTextLog(int index, const QString message);
};

class AWebSocketWorker_Check : public AWebSocketWorker_Base
{
    Q_OBJECT
public:
    AWebSocketWorker_Check(int index, ARemoteServerRecord* rec, int timeOut);

public slots:
    virtual void run() override;

private:
    int  timerInterval = 250; //ms
    int  timeElapsed = 0;

private slots:
    void onTimer();

};

class AWebSocketWorker_Sim : public AWebSocketWorker_Base
{
    Q_OBJECT
public:
    AWebSocketWorker_Sim(int index, ARemoteServerRecord* rec, int timeOut, const QJsonObject* config);

public slots:
    virtual void run() override;

private:    
    void runSimulation();
};

class AWebSocketWorker_Rec : public AWebSocketWorker_Base
{
    Q_OBJECT
public:
    AWebSocketWorker_Rec(int index, ARemoteServerRecord* rec, int timeOut, const QJsonObject* config);

public slots:
    virtual void run() override;

private:
    void runReconstruction();
};

class AWorker_Script : public AWebSocketWorker_Base
{
    Q_OBJECT
public:
    AWorker_Script(int index, ARemoteServerRecord* rec, int timeOut, const QJsonObject* config, const QString & script, AGridScriptResources & data);

    const QString        & script;
    AGridScriptResources & data;

    bool       bSuccess = false;
    bool       bFailed  = false;

public slots:
    void run() override;

private:
    void runEvalScript();
};

struct AGridScriptResources
{
    QVariant Resource;
    QString  FileName;

    bool bDone = false;
    bool bAllocated = false;

    QVariant EvalResult;
};

class AWorker_Upload : public AWebSocketWorker_Base
{
    Q_OBJECT
public:
    AWorker_Upload(int index, ARemoteServerRecord* rec, int timeOut, const QString & fileName);

    const QString & FileName;
    bool bSuccess = false;

public slots:
    void run() override;

};

#endif // AGRIDRUNNER_H
