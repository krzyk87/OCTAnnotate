#ifndef LAYER_H
#define LAYER_H

#include <QString>
#include <QList>
#include <QPoint>
#include "enums.h"

class Layer
{
public:
    Layer(int bscanWidth, int bscansNumber, int index);

    void setPoint(QPoint p);
    void resetPoints();

private:
    Layers layer;
    int index;
    QString name;
    int bscanWidth;
    int bscansNumber;

    QList<QPoint> points;
};

#endif // LAYER_H
