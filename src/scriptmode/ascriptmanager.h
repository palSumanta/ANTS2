#ifndef ASCRIPTMANAGER_H
#define ASCRIPTMANAGER_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QVariant>

class AScriptInterface;
class TRandom2;
class ACore_SI;
class QElapsedTimer;

class AScriptManager : public QObject
{
  Q_OBJECT

public:
  AScriptManager(TRandom2 * RandGen);
  virtual ~AScriptManager();

  //configuration
  virtual void      RegisterInterfaceAsGlobal(AScriptInterface* interface) = 0;
  virtual void      RegisterCoreInterfaces(bool bCore = true, bool bMath = true) = 0;
  virtual void      RegisterInterface(AScriptInterface* interface, const QString& name) = 0;

  //run
  virtual int       FindSyntaxError(const QString & /*script*/ ) {return -1;} //returns line number of the first syntax error; -1 if no errors found
  virtual QString   Evaluate(const QString & Script) = 0;
  virtual QVariant  EvaluateScriptInScript(const QString & script) = 0;

  virtual bool      isUncaughtException() const {return false;}
  virtual int       getUncaughtExceptionLineNumber() const {return -1;}
  virtual QString   getUncaughtExceptionString() const {return "";}

  virtual void      collectGarbage(){}
  virtual void      abortEvaluation() = 0;

#ifdef GUI
  virtual void      hideMsgDialogs();
  virtual void      restoreMsgDialogs();
  void              deleteMsgDialogs();    // *!* obsolete?
#endif
  bool              isEngineRunning() const {return fEngineIsRunning;}
  bool              isEvalAborted() const {return fAborted;}

  const QString &   getLastError() const {return LastError;}
  int               getLastErrorLineNumber() const {return LastErrorLineNumber;}
  qint64            getElapsedTime();
  QString           getFunctionReturnType(const QString & UnitFunction);
  void              ifError_AbortAndReport();

public slots:
  virtual void      AbortEvaluation(QString message = "Aborted!");

public:
  QVector<AScriptInterface*> interfaces;  // registered interfaces (units)
  TRandom2 *        RandGen;              // math module uses it

  //pointers to starter dirs
  QString *         LibScripts  = nullptr;
  QString *         LastOpenDir = nullptr;
  QString *         ExamplesDir = nullptr;

  //for minimizer
  QString           MiniFunctionName;
  double            MiniBestResult = 1e30;
  int               MiniNumVariables = 0;

protected:
  bool              bOwnRandomGen = false;  //the main manager (GUI thread) does not own RandGen, but multithread managers do

  bool              fEngineIsRunning = false;
  bool              fAborted = false;

  QString           LastError;
  int               LastErrorLineNumber = -1;   //added to be used with #include processing errors in javascript
  bool              bShowAbortMessageInOutput = true; //can be false -> in multithread if abort is local to this thread

  QElapsedTimer *   timer = nullptr;
  qint64            timeOfStart;
  qint64            timerEvalTookMs = 0;

  QVector<int>      LineNumberMapper;
  bool              bScriptExpanded = false;

  bool              expandScript(const QString & OriginalScript, QString & ExpandedScript);  // can be splitted in functions, but wait till making it general for both JS and Python
  void              correctLineNumber(int & iLineNumber) const; // if needed, converts the error line number in the script with expanded #include(s) to the line number in the original script
  virtual void      updateBlockCommentStatus(const QString & /*Line*/, bool & /*bInsideBlockComments*/) const {return;}  // only needed for JavaScript, for Python block commenting is not possible

signals:
    void            onStart();
    void            onAbort();
    void            onFinish(QString eval);

    void            showMessage(QString message);
    void            showPlainTextMessage(QString message);
    void            clearText();
    void            requestHighlightErrorLine(int lineNumber);

    void            reportProgress(int percent);
};

#endif // ASCRIPTMANAGER_H
