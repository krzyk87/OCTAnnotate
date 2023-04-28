#ifndef LAYER_H
#define LAYER_H

#include <QString>
#include <QList>
#include "enums.h"

class Layer
{
public:
    Layer(int bscanWidth, int bscansNumber, int index);

    void setPoint(int bscanNumber, int xPos, int zPos);
    void resetPoints();

private:
    LayerName layer;
    int index;
    QString name;
    int bscanWidth;
    int bscansNumber;

    QList<QList<int> > points;
};

#endif // LAYER_H
