#include "layer.h"
#include "functions.h"

// ----------------------------------------------------------------------------
// Layer class ----------------------------------------------------------------
// ----------------------------------------------------------------------------

Layer::Layer(int bscanWidth, int bscansNumber, int index)
{
    this->index = index;
    this->layer = (Layers)index;
    this->name = encodeLayer(this->layer);
    this->bscanWidth = bscanWidth;
    this->bscansNumber = bscansNumber;

    resetPoints();

    this->anotated = false;
}

void Layer::setPoint(QPoint p){
    if (p.y() == -1)
        this->points[p.x()] = QPoint(-1,-1);
    else
        this->points[p.x()] = p;
}

void Layer::countAnnotatedPixels()
{
    int sum = 0;
    for (int i = 0; i < this->bscanWidth; i++){
        if ((this->points[i].x() != -1) && (this->points[i].y() != -1))
            sum++;
    }
    if (sum > 0)
        this->anotated = true;
    else
        this->anotated = false;
}


void Layer::resetPoints(){
    QPoint p(-1,-1);

    this->points.clear();

    this->points.reserve(this->bscanWidth);
    for (int i=0; i<this->bscanWidth; i++){
        this->points.append(p);
    }
}
