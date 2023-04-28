#include "layer.h"
#include "functions.h"

// ----------------------------------------------------------------------------
// Layer class ----------------------------------------------------------------
// ----------------------------------------------------------------------------

Layer::Layer(int bscanWidth, int bscansNumber, int index)
{
    this->index = index;
    this->layer = (LayerName)index;
    this->name = encodeLayer(this->layer);
    this->bscanWidth = bscanWidth;
    this->bscansNumber = bscansNumber;

    resetPoints();
}

void Layer::resetPoints(){

    this->points.clear();

    QList<int> pList;
    for (int i=0; i<this->bscanWidth; i++){
        pList.append(-1);
    }
    this->points.reserve(this->bscansNumber);
    for (int i=0; i < this->bscansNumber; i++){
        this->points.append(pList);
    }
}

void Layer::setPoint(int bscanNumber, int xPos, int zPos){
    this->points[bscanNumber][xPos] = zPos;
}
