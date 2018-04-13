#ifndef AINTERFACETOTTREE_H
#define AINTERFACETOTTREE_H

#include "ascriptinterface.h"

#include <QObject>
#include <QVector>
#include <QString>
#include <QVariant>

#include "TString.h"
#include "TTree.h"

#include <vector>

class TmpObjHubClass;

class AInterfaceToTTree : public AScriptInterface
{
  Q_OBJECT

public:
   AInterfaceToTTree(TmpObjHubClass *TmpHub);
   ~AInterfaceToTTree() {}

public slots:
   void     SetAbortIfAlreadyExists(bool flag) {bAbortIfExists = flag;}

   void     LoadTTree(const QString &TreeName, const QString &FileName, const QString &TreeNameInFile);

   void     CreateTree(const QString &TreeName, const QVariant HeadersOfBranches);
   void     FillTree_SingleEntry(const QString &TreeName, const QVariant Array);

   const QString  PrintBranches(const QString &TreeName);
   const QVariant GetBranch(const QString &TreeName, const QString &BranchName);

   void     Draw(const QString& TreeName, const QString& what, const QString& cuts, const QString& options, const QVariant binsAndRanges = QVariantList());

   bool     DeleteTree(const QString &TreeName);
   void     DeleteAllTrees();

signals:
   void     RequestTreeDraw(TTree* tree, const QString& what, const QString& cond, const QString& how, const QVariantList& binsAndRanges);

private:
   TmpObjHubClass *TmpHub;

   bool           bAbortIfExists = false;
};

#endif // AINTERFACETOTTREE_H