#ifndef ADRAWEXPLORERWIDGET_H
#define ADRAWEXPLORERWIDGET_H

#include "adrawobject.h"

#include <QTreeWidget>

class GraphWindowClass;
class QTreeWidgetItem;
class TObject;
class TH2;
class TAxis;
class TGaxis;

class ADrawExplorerWidget : public QTreeWidget
{
    Q_OBJECT
public:
    ADrawExplorerWidget(GraphWindowClass & GraphWindow, QVector<ADrawObject> & DrawObjects);

    void updateGui();

    TH2 * getObjectForCustomProjection() {return objForCustomProjection;}

    void activateCustomGuiForItem(int index);    
    void showObjectContextMenu(const QPoint &pos, int index);

private slots:
    void onContextMenuRequested(const QPoint & pos);
    void onItemDoubleClicked(QTreeWidgetItem *item, int column);

private:
    GraphWindowClass & GraphWindow;
    QVector<ADrawObject> & DrawObjects;

private:
    void addToDrawObjectsAndRegister(TObject * pointer, const QString & options);

    void rename(ADrawObject &obj);
    void toggleEnable(ADrawObject &obj);
    void remove(int index);
    void setAttributes(int index);
    void showPanel(ADrawObject &obj);
    void fitPanel(ADrawObject &obj);
    void scale(ADrawObject &obj);
    void scaleCDR(ADrawObject &obj);
    void shift(ADrawObject &obj);
    void drawIntegral(ADrawObject &obj);
    void fraction(ADrawObject &obj);
    void fwhm(int index);
    void linFit(int index);
    void expFit(int index);
    void interpolate(ADrawObject &obj);
    void median(ADrawObject &obj);
    void projection(ADrawObject &obj, bool bX);
    void customProjection(ADrawObject &obj);
    void splineFit(int index);
    void editAxis(ADrawObject &obj, int axisIndex);
    void addAxis(int axisIndex);
    void saveRoot(ADrawObject &obj);
    void saveAsTxt(ADrawObject &obj, bool fUseBinCenters);
    void extract(ADrawObject &obj);
    void editPave(ADrawObject &obj);
    void editTGaxis(ADrawObject &obj);

    bool canScale(ADrawObject &obj);
    void doScale(ADrawObject &obj, double sf);
    void copyAxisProperties(TGaxis & grAxis, TAxis  & axis);
    void copyAxisProperties(TAxis  & axis,   TGaxis & grAxis);
    const QString generateOptionForSecondaryAxis(int axisIndex, double u1, double u2);

private:
    TH2 * objForCustomProjection = nullptr;

signals:
    void requestShowLegendDialog();
};

#endif // ADRAWEXPLORERWIDGET_H
