#ifndef APARTICLEGUN_H
#define APARTICLEGUN_H

#include <QVector>
#include <QString>

class QJsonObject;
class AParticleRecord;

class AParticleGun
{
public:
    virtual ~AParticleGun(){}

    virtual bool Init() = 0;             //called before first use
    virtual void ReleaseResources() {}   //called after end of operation
    virtual void GenerateEvent(QVector<AParticleRecord*> & GeneratedParticles) = 0;

    virtual const QString CheckConfiguration() const = 0; //check consistency of the configuration - TODO: merge with Init()

    virtual void RemoveParticle(int particleId) = 0; //should NOT be used to remove one of particles in use! use onIsPareticleInUse first
    virtual bool IsParticleInUse(int particleId, QString& SourceNames) const = 0;

    virtual void writeToJson(QJsonObject &json) const = 0;
    virtual bool readFromJson(const QJsonObject &json) = 0;

    virtual void SetStartEvent(int) {} // for 'from file' generator

    const QString& GetErrorString() const {return ErrorString;}

protected:
    QString ErrorString;
};

#endif // APARTICLEGUN_H
