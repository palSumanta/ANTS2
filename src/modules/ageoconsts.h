#ifndef AGEOCONSTS_H
#define AGEOCONSTS_H

#include <QVector>
#include <QString>
#include <QRegExp>
#include <ageoobject.h>

class QJsonObject;

class AGeoConsts final
{
public:
    static       AGeoConsts & getInstance();
    static const AGeoConsts & getConstInstance();

    const QVector<QString> & getNames()  const {return Names;}
    const QVector<double>  & getValues() const {return Values;}

    int  countConstants() const {return Names.size();}
    //QVector<QString> getGeoConstsInUse(const AGeoObject &obj) const;
    bool isGeoConstInUse(const QRegExp & nameRegExp, AGeoObject *obj) const;

    QString exportToJavaSript(AGeoObject *obj) const;
    QString formulaToJavaScript(QString &input) const;


    void writeToJson(QJsonObject & json) const;
    void readFromJson(const QJsonObject & json);

    bool evaluateFormula(QString str, double & returnValue) const;
    bool updateParameter(QString &errorStr, QString & str, double & returnValue, bool bForbidZero = true, bool bForbidNegative = true, bool bMakeHalf = true) const;

    QString getName(int index) const;
    bool rename(int index, const QString & newName);
    bool setNewValue(int index, double newValue);
    bool addNewConstant(const QString & name, double value);

    void remove(int index);

private:
    AGeoConsts(){}

    AGeoConsts(const AGeoConsts&) = delete;            // Copy ctor
    AGeoConsts(AGeoConsts&&) = delete;                 // Move ctor
    AGeoConsts& operator=(const AGeoConsts&) = delete; // Copy assign
    AGeoConsts& operator=(AGeoConsts&) = delete;       // Move assign

    QVector<QString> Names;
    QVector<double>  Values;
    QVector<QString> StrValues;
    //runtime
    QVector<QRegExp> RegExps;
    QVector<QString> Indexes;

    void update();
    void clearConstants();

public:
    static QVector<QString> getTFormulaReservedWords();
};

#endif // AGEOCONSTS_H

