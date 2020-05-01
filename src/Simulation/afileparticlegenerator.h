#ifndef AFILEPARTICLEGENERATOR_H
#define AFILEPARTICLEGENERATOR_H

#include "aparticlegun.h"

#include <QString>
#include <QFile>
#include <QRegularExpression>
#include <QVector>
#include <QJsonObject>
#include <QDateTime>

#include <string>
#include <vector>

class AMaterialParticleCollection;
class QTextStream;
class AFilePGEngine;

enum class AFileMode {Standard = 0, G4ants = 1};

struct AParticleInFileStatRecord
{
    AParticleInFileStatRecord(const std::string & NameStd, double Energy);
    AParticleInFileStatRecord(const QString     & NameQt,  double Energy);
    AParticleInFileStatRecord() {}

    std::string NameStd;
    QString     NameQt;
    int         Entries = 0;
    double      Energy  = 0;
};

class AFileParticleGenerator : public AParticleGun
{
public:
    AFileParticleGenerator(const AMaterialParticleCollection & MpCollection);

    virtual         ~AFileParticleGenerator();

    bool            Init() override;               //has to be called before first use of GenerateEvent()
    void            ReleaseResources() override;
    bool            GenerateEvent(QVector<AParticleRecord*> & GeneratedParticles, int iEvent) override;

    void            RemoveParticle(int) override {} //cannot be used for this class
    bool            IsParticleInUse(int particleId, QString& SourceNames) const override; // not yet implemented, but is not critical

    void            writeToJson(QJsonObject& json) const override;
    bool            readFromJson(const QJsonObject& json) override;

    void            SetStartEvent(int startEvent) override;

    void            SetFileName(const QString &fileName);
    const QString   GetFileName() const {return FileName;}

    void            SetFileFormat(AFileMode mode);
    AFileMode       GetFileFormat() const {return Mode;}

    void            InvalidateFile();    //forces the file to be inspected again during next call of Init()
    bool            IsValidated() const;

    bool            generateG4File(int eventBegin, int eventEnd, const QString & FileName);

    void            setParticleMustBeDefined(bool flag);

public:
    int          RegisteredParticleCount  = -1; //saved - used in validity check for standard mode
    int          NumEventsInFile          = 0;  // is saved in config
    int          statNumEmptyEventsInFile = 0;
    int          statNumMultipleEvents    = 0;
    bool         bG4binary                = false;
    bool         bParticleMustBeDefined   = false; // set to true when sim is performed in ANTS2

    bool         bCollectExpandedStatistics = false;
    std::vector<AParticleInFileStatRecord> ParticleStat;

    const AMaterialParticleCollection & MpCollection;

private:
    AFilePGEngine * Engine = nullptr;

    QString FileName;
    AFileMode Mode = AFileMode::Standard;

    QDateTime FileLastModified;       //saved - used in validity check

private:
    void clearFileStat();
    bool isRequireInspection() const;
    bool testG4mode();
};

class AFilePGEngine
{
public:
    AFilePGEngine(AFileParticleGenerator * fpg) : FPG(fpg), FileName(fpg->GetFileName()) {}
    virtual ~AFilePGEngine(){}

    virtual bool doInit(bool bNeedInspect, bool bDetailedInspection, bool bParticleMustBeDefined) = 0;
    virtual bool doGenerateEvent(QVector<AParticleRecord*> & GeneratedParticles) = 0;
    virtual bool doSetStartEvent(int startEvent) = 0;
    virtual bool doGenerateG4File(int eventBegin, int eventEnd, const QString & FileName) = 0;

protected:
    AFileParticleGenerator * FPG = nullptr;
    QString FileName;

    const QRegularExpression rx = QRegularExpression("(\\ |\\,|\\:|\\t)");  // separators are: ' ' or ',' or ':' or '\t'
};

class AFilePGEngineStandard : public AFilePGEngine
{
public:
    AFilePGEngineStandard(AFileParticleGenerator * fpg) : AFilePGEngine(fpg) {}
    ~AFilePGEngineStandard();

    bool doInit(bool bNeedInspect, bool bDetailedInspection, bool bParticleMustBeDefined) override;
    bool doGenerateEvent(QVector<AParticleRecord*> & GeneratedParticles) override;
    bool doSetStartEvent(int startEvent) override;
    bool doGenerateG4File(int eventBegin, int eventEnd, const QString & FileName) override; // not in use! SimManager uses mainstream approach to generate events

private:
    QFile File;
    QTextStream * Stream = nullptr;
};

class AFilePGEngineG4antsTxt : public AFilePGEngine
{
public:
    AFilePGEngineG4antsTxt(AFileParticleGenerator * fpg) : AFilePGEngine(fpg) {}
    ~AFilePGEngineG4antsTxt();

    bool doInit(bool bNeedInspect, bool bDetailedInspection, bool bParticleMustBeDefined) override;
    bool doGenerateEvent(QVector<AParticleRecord*> & GeneratedParticles) override;
    bool doSetStartEvent(int startEvent) override;
    bool doGenerateG4File(int eventBegin, int eventEnd, const QString & FileName) override;

private:
    std::ifstream * inStream = nullptr;
};

class AFilePGEngineG4antsBin : public AFilePGEngine
{
public:
    AFilePGEngineG4antsBin(AFileParticleGenerator * fpg) : AFilePGEngine(fpg) {}
    ~AFilePGEngineG4antsBin();

    bool doInit(bool bNeedInspect, bool bDetailedInspection, bool bParticleMustBeDefined) override;
    bool doGenerateEvent(QVector<AParticleRecord*> & GeneratedParticles) override;
    bool doSetStartEvent(int startEvent) override;
    bool doGenerateG4File(int eventBegin, int eventEnd, const QString & FileName) override;

private:
    std::ifstream * inStream = nullptr;
};

#endif // AFILEPARTICLEGENERATOR_H
