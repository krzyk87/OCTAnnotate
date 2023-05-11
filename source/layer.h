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

    // set points
    void resetPoints(bool isManual);
    void setPoint(bool isManual, int bscanNumber, int xPos, int zPos);

    // get points
    int getPoint(bool isManual, int bscanNumber, int x);
    QList<QVector3D> getPoints(bool isManual, int crossSection, int xMin, int xMax, bool isNormal);
    QVector<double> getPointsVector(bool isManual, int crossSection, bool isNormal, int imgHeight);

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
