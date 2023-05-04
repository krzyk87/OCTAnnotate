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
    resetPointsAuto();

    assignColor();
}

// -------------------------- reset points ----------------------------
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

void Layer::resetPointsAuto()
{
    this->pointsAuto.clear();

    QList<int> pList;
    for (int i=0; i<this->bscanWidth; i++){
        pList.append(-1);
    }
    this->pointsAuto.reserve(this->bscansNumber);
    for (int i=0; i < this->bscansNumber; i++){
        this->pointsAuto.append(pList);
    }
}

// ------------------------------- set points -----------------------------------
void Layer::setPoint(int bscanNumber, int xPos, int zPos){
    this->points[bscanNumber][xPos] = zPos;
}

// ------------------------------- get points --------------------------------
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

// ------------------------------- set points auto ---------------------------
void Layer::setPointAuto(int bscanNumber, int xPos, int zPos)
{
    if (zPos < 1)
        this->pointsAuto[bscanNumber][xPos] = -1;
    else
        this->pointsAuto[bscanNumber][xPos] = zPos;
}

// ------------------------------- get points auto ---------------------------
QVector<double> Layer::getPointsAutoVector(int crossSection, bool isNormal, int imgHeight)
{
    int xMax = this->bscanWidth-1;
    if (isNormal)
        xMax = this->bscansNumber-1;
    QVector<double> pointsVector(xMax+1);
    int zVal;
    for (int i=0; i<=xMax; i++){
        if (isNormal){
            zVal = this->pointsAuto[i][crossSection];
        } else {
            zVal = this->pointsAuto[crossSection][i];
        }
        if (zVal == -1){
            pointsVector[i] = std::numeric_limits<double>::quiet_NaN();
        } else {
            pointsVector[i] = imgHeight - zVal;// - flatDiff[i];
        }
    }
    return pointsVector;
}

// -----------------
void Layer::assignColor()
{
    switch (this->layer) {
    case PCV:
        this->layerColor = Qt::green;
        break;
    case IB_ERM:
        this->layerColor = Qt::white;
        break;
    case OB_ERM:
        this->layerColor = Qt::white;
        break;
    case ILM:
        this->layerColor = Qt::red;
        break;
    case RNFL_GCL:
        this->layerColor = Qt::blue;
        break;
    case GCL_IPL:
        this->layerColor = Qt::darkYellow;
        break;
    case IPL_INL:
        this->layerColor = Qt::blue;
        break;
    case INL_OPL:
        this->layerColor = Qt::magenta;
        break;
    case OPL_ONL:
        this->layerColor = Qt::yellow;
        break;
    case ELM:
        this->layerColor = Qt::yellow;
        break;
    case MEZ:
        this->layerColor = Qt::green;
        break;
    case IS_OS:
        this->layerColor = Qt::green;
        break;
    case IB_OPR:
        this->layerColor = Qt::blue;
        break;
    case IB_RPE:
        this->layerColor = Qt::red;
        break;
    case OB_RPE:
        this->layerColor = QColor(255,85,0);
        break;
    default:
        break;
    }
}

QColor Layer::getColor()
{
    return this->layerColor;
}

void Layer::setDisplayObjects(QLabel *colorLabel, QRadioButton *radioButton)
{
    this->colorLabel = colorLabel;
    QPalette palette;
    palette.setColor(QPalette::Window, this->layerColor);
    this->colorLabel->setAutoFillBackground(true);
    this->colorLabel->setPalette(palette);

    this->layerRButton = radioButton;
}

QRadioButton *Layer::getRadioButton()
{
    return this->layerRButton;
}
