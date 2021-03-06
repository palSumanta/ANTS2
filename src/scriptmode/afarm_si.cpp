#include "afarm_si.h"
#include "agridrunner.h"
#include "aremoteserverrecord.h"

#include <QJsonObject>
#include <QFile>

AFarm_si::AFarm_si(const QJsonObject & Config, AGridRunner & GridRunner) :
    AScriptInterface(), Config(Config), GridRunner(GridRunner)
{
    H["getServers"] = "Returns the list of all configured servers\nFormat: [ [NumThreads1, SpeedFactor1], [NumThreads2, SpeedFactor2], ... ])";
}

void AFarm_si::ForceStop()
{
    GridRunner.Abort();
}

void AFarm_si::setTimeout(double Timeout_ms)
{
    GridRunner.SetTimeout(Timeout_ms);
}

QVariantList AFarm_si::getServers()
{
    QVariantList res;

    QString err = GridRunner.CheckStatus();
    if (!err.isEmpty())
    {
        abort(err);
        return res;
    }

    for (const ARemoteServerRecord * r : GridRunner.ServerRecords)
    {
        QVariantList el;
        el << r->NumThreads_Allocated << r->SpeedFactor;
        res.push_back(el);
    }
    return res;
}

void AFarm_si::reconstruct()
{
    GridRunner.Reconstruct(&Config);
}

void AFarm_si::simulate()
{
    GridRunner.Simulate(&Config);
}

QVariantList AFarm_si::evaluateScript(QString Script, QVariantList Resources, QVariantList FileNames)
{
    QVariant res = GridRunner.EvaluateSript(Script, Config, Resources, FileNames);

    if (res.type() == QVariant::String)
    {
        //error is coded as a string
        abort(res.toString());
        return QVariantList();
    }
    else
        return res.toList();
}

void AFarm_si::uploadFile(int ServerIndex, QString FileName)
{
    QString err = GridRunner.UploadFile(ServerIndex, FileName);
    if (!err.isEmpty()) abort(err);
}

