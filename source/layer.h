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
    void countAnnotatedPixels();
    void resetPoints();

private:
    Layers layer;
    int index;
    QString name;
    int bscanWidth;
    int bscansNumber;

    QList<QPoint> points;

    bool anotated;
};

#endif // LAYER_H
