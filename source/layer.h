#ifndef LAYER_H
#define LAYER_H

#include <QString>
#include "enums.h"

class Layer
{
public:
    Layer(int bscanWidth, int bscansNumber, int index);

private:
    Layers layer;
    int index;
    QString name;
    int bscanWidth;
    int bscansNumber;
};

#endif // LAYER_H
