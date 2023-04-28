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

QList<QVector3D> Layer::getPoints(int crossSection, int xMin, int xMax, bool isNormal)
{
    QList<QVector3D> list;
    for (int i = xMin; i <= xMax; i++){
        QVector3D v;
        if (isNormal){
            v.setX(crossSection);
            v.setY(i);
            v.setZ(this->points[i][crossSection]);
        } else {
            v.setX(i);
            v.setY(crossSection);
            v.setZ(this->points[crossSection][i]);
        }
        list.append(v);
    }
    return list;
}

QVector<double> Layer::getPointsVector(int crossSection, bool isNormal, int imgHeight)
{
    int xMax = this->bscanWidth-1;
    if (isNormal)
        xMax = this->bscansNumber-1;
    QVector<double> pointsVector(xMax+1);
    int zVal;
    for (int i=0; i<=xMax; i++){
        if (isNormal){
            zVal = this->points[i][crossSection];
        } else {
            zVal = this->points[crossSection][i];
        }
        if (zVal == -1){
            pointsVector[i] = std::numeric_limits<double>::quiet_NaN();
        } else {
            pointsVector[i] = imgHeight - zVal;// - flatDiff[i];
        }
    }
    return pointsVector;
}
