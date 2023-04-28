#ifndef LAYER_H
#define LAYER_H

#include <QString>
#include <QList>
#include <QVector3D>
#include "enums.h"

class Layer
{
public:
    Layer(int bscanWidth, int bscansNumber, int index);

    void resetPoints();
    void setPoint(int bscanNumber, int xPos, int zPos);
    QList<QVector3D> getPoints(int crossSection, int xMin, int xMax, bool isNormal);
    QVector<double> getPointsVector(int crossSection, bool isNormal, int imgHeight);

private:
    LayerName layer;
    int index;
    QString name;
    int bscanWidth;
    int bscansNumber;

    QList<QList<int> > points;
};

#endif // LAYER_H
