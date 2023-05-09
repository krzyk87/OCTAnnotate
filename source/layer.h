#ifndef LAYER_H
#define LAYER_H

#include <QString>
#include <QList>
#include <QVector3D>
#include <QColor>
#include <QLabel>
#include "enums.h"
#include "qradiobutton.h"

class Layer
{
public:
    Layer(int bscanWidth, int bscansNumber, int index);

    void resetPoints();
    void setPoint(int bscanNumber, int xPos, int zPos);
    QList<QVector3D> getPoints(int crossSection, int xMin, int xMax, bool isNormal);
    QVector<double> getPointsVector(int crossSection, bool isNormal, int imgHeight);

    void resetPointsAuto();
    void setPointAuto(int bscanNumber, int xPos, int zPos);
    int getPointAuto(int bscanNumber, int x);
    QList<QVector3D> getPointsAuto(int crossSection, int xMin, int xMax, bool isNormal);
    QVector<double> getPointsAutoVector(int crossSection, bool isNormal, int imgHeight);

    void assignColor();
    QColor getColor();
    void setDisplayObjects(QLabel *colorLabel, QRadioButton *radioButton);
    QRadioButton *getRadioButton();

private:
    LayerName layer;
    int index;
    QString name;
    int bscanWidth;
    int bscansNumber;

    QList<QList<int> > points;
    QList<QList<int> > pointsAuto;

    // display parameters
    QColor layerColor;
    QLabel *colorLabel;
    QRadioButton *layerRButton;

};

#endif // LAYER_H
