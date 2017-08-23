#ifndef ANEUTRONINTERACTIONELEMENT_H
#define ANEUTRONINTERACTIONELEMENT_H

#include <QString>
#include <QVector>

class QJsonObject;
class AMaterialParticleCollection;

// --- base class ---

class ANeutronInteractionElement  // basic class for capture and elastic scattering
{
public:
    ANeutronInteractionElement(QString IsotopeSymbol, int Mass, double MolarFraction) :
        Name(IsotopeSymbol), Mass(Mass), MolarFraction(MolarFraction) {}
    ANeutronInteractionElement() : Name("Undefined"), Mass(777), MolarFraction(0) {}

    QString Name;
    int Mass;
    double MolarFraction;

    QVector<double> Energy;
    QVector<double> CrossSection;

protected:
    void writeToJson(QJsonObject& json) const;  // not to be used directly!!!
    void readFromJson(const QJsonObject& json); // not to be used directly!!!
};

// --- elastic ---

class AElasticScatterElement  : public ANeutronInteractionElement
{
public:
    AElasticScatterElement(QString IsotopeSymbol, int Mass, double MolarFraction) :
        ANeutronInteractionElement(IsotopeSymbol, Mass, MolarFraction) {}
    AElasticScatterElement() :
        ANeutronInteractionElement() {}

    void writeToJson(QJsonObject& json) const;
    const QJsonObject writeToJson() const;
    void readFromJson(const QJsonObject& json);
};

// --- absorption ---

class AAbsorptionGeneratedParticle
{
public:
    int ParticleId;
    double Energy;    // in keV
    bool bOpositeDirectionWithPrevious;

    AAbsorptionGeneratedParticle(int ParticleId, double Energy, bool bOpositeDirectionWithPrevious) :
    ParticleId(ParticleId), Energy(Energy), bOpositeDirectionWithPrevious(bOpositeDirectionWithPrevious) {}
    AAbsorptionGeneratedParticle() : ParticleId(0), Energy(0), bOpositeDirectionWithPrevious(false) {}

    void writeToJson(QJsonObject& json, AMaterialParticleCollection *MpCollection) const;
    const QJsonObject writeToJson(AMaterialParticleCollection *MpCollection) const;
    void readFromJson(const QJsonObject& json, AMaterialParticleCollection *MpCollection);
};

class ACaptureReaction
{
public:
    double Branching;
    QVector<AAbsorptionGeneratedParticle> GeneratedParticles;

    ACaptureReaction(double Branching) : Branching(Branching) {}
    ACaptureReaction() : Branching(1.0) {}

    void writeToJson(QJsonObject& json, AMaterialParticleCollection *MpCollection) const;
    const QJsonObject writeToJson(AMaterialParticleCollection *MpCollection) const;
    void readFromJson(const QJsonObject& json, AMaterialParticleCollection *MpCollection);
};

class AAbsorptionElement : public ANeutronInteractionElement
{
public:
    QVector<ACaptureReaction> Reactions;

    AAbsorptionElement(QString IsotopeSymbol, int Mass, double MolarFraction) :
        ANeutronInteractionElement(IsotopeSymbol, Mass, MolarFraction) {Reactions.resize(1);}
    AAbsorptionElement() :
        ANeutronInteractionElement() {Reactions.resize(1);}

    void writeToJson(QJsonObject& json, AMaterialParticleCollection *MpCollection) const;
    const QJsonObject writeToJson(AMaterialParticleCollection *MpCollection) const;
    void readFromJson(QJsonObject& json, AMaterialParticleCollection *MpCollection);
};

#endif // ANEUTRONINTERACTIONELEMENT_H