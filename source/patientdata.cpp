#include "patientdata.h"
#include <QPainter>
#include <qmath.h>
#include <math.h>
#include <QDebug>

DistType decodeDistType(QString type){
    DistType t = None;
    if (type == "pen"){
        t = Pen;
    } else if (type == "whiteSpot"){
        t = WhiteSpot;
    } else if (type == "blackSpot"){
        t = BlackSpot;
    } else if (type == "convex"){
        t = Convex;
    } else if (type == "concave"){
        t = Concave;
    } else if (type == "rightSpiral"){
        t = RightSpiral;
    } else if (type == "leftSpiral"){
        t = LeftSpiral;
    }
    return t;
}

QString encodeDistType(DistType type){
    QString s;

    switch (type) {
    case None:
        break;
    case Pen:
        s = "pen";
        break;
    case WhiteSpot:
        s = "whiteSpot";
        break;
    case BlackSpot:
        s = "blackSpot";
        break;
    case Convex:
        s = "convex";
        break;
    case Concave:
        s = "concave";
        break;
    case RightSpiral:
        s = "rightSpiral";
        break;
    case LeftSpiral:
        s = "leftSpiral";
        break;
    }

    return s;
}

Layers decodeLayer(QString layer){
    Layers l = NONE;
    if ((layer == "PCV") || (layer == "PSH")){
        l = PCV;
    } else if (layer == "ERM"){
        l = ERM;
    } else if (layer == "ILM"){
        l = ILM;
    } else if (layer == "GCL"){
        l = GCL;
    } else if (layer == "IPL"){
        l = IPL;
    } else if (layer == "INL"){
        l = INL;
    } else if (layer == "OPL"){
        l = OPL;
    } else if (layer == "ONL"){
        l = ONL;
    } else if (layer == "ELM"){
        l = ELM;
    } else if (layer == "MEZ"){
        l = MEZ;
    } else if (layer == "IOS"){
        l = IOS;
    } else if (layer == "RPE"){
        l = RPE;
    } else if (layer == "CHR"){
        l = CHR;
    }
    return l;
}

QString encodeLayer(Layers layer){
    QString s = "";

    switch (layer) {
    case PCV:
        s = "PCV";
        break;
    case ERM:
        s = "ERM";
        break;
    case ILM:
        s = "ILM";
        break;
    case GCL:
        s = "GCL";
        break;
    case IPL:
        s = "IPL";
        break;
    case INL:
        s = "INL";
        break;
    case OPL:
        s = "OPL";
        break;
    case ONL:
        s = "ONL";
        break;
    case ELM:
        s = "ELM";
        break;
    case MEZ:
        s = "MEZ";
        break;
    case IOS:
        s = "IOS";
        break;
    case RPE:
        s = "RPE";
        break;
    case CHR:
        s = "CHR";
        break;
    case NONE:
        s = "NONE";
    }
    return s;
}

QList<Layers> getAllLayers(){
    QList<Layers> list;
    list.append(PCV);
    list.append(ERM);
    list.append(ILM);
    list.append(GCL);
    list.append(IPL);
    list.append(INL);
    list.append(OPL);
    list.append(ONL);
    list.append(ELM);
    list.append(MEZ);
    list.append(IOS);
    list.append(RPE);
    list.append(CHR);
    return list;
}

QList<Layers> getLayersForError(){
    QList<Layers> list;
    list.append(PCV);
    list.append(ILM);
    list.append(GCL);
    list.append(INL);
    list.append(OPL);
    list.append(ONL);
    list.append(IOS);
    list.append(CHR);
    return list;
}

Pathologies decodePathology(QString pathology){
    Pathologies p = UNDEFINED;
    if (pathology == "NONE"){
        p = HEALTHY;
    } else if (pathology == "VMA"){
        p = VMA;
    } else if (pathology == "VMT"){
        p = VMT;
    } else if (pathology == "ERM"){
        p = ERMP;
    } else if (pathology == "IMH"){
        p = IMH;
    } else if (pathology == "UDF"){
        p = UNDEFINED;
    }
    return p;
}

QString encodePathology(Pathologies pathology){
    QString s = "";

    switch (pathology) {
    case HEALTHY:
        s = "NONE";
        break;
    case VMA:
        s = "VMA";
        break;
    case VMT:
        s = "VMT";
        break;
    case ERMP:
        s = "ERM";
        break;
    case IMH:
        s = "IMH";
        break;
    case UNDEFINED:
        s = "UDF";
        break;
    }
    return s;
}

bool isPointSet(QPoint p){
    bool isSet = false;
    if ((p.x() != -1) && (p.y() != -1)){
        isSet = true;
    }
    return isSet;
}


// ----------------------------------------------------------------------------
// AmslerDist class -----------------------------------------------------------
// ----------------------------------------------------------------------------
AmslerDist::AmslerDist(){
    this->id = -1;
}

AmslerDist::AmslerDist(DistType type, QList<QLine> lines, int id){
    this->setType(type);
    this->setLines(lines);
    this->setId(id);
}

AmslerDist::AmslerDist(DistType type, QPoint point, int width, int id){
    this->setType(type);
    this->setPoint(point);
    this->setWidth(width);
    this->setId(id);
}

// ----------------------------------------------------------------------------
void AmslerDist::setType(DistType newType){
    this->distType = newType;
}

DistType AmslerDist::getType(){
    return this->distType;
}

void AmslerDist::setLines(QList<QLine> newlines){
    this->lines = newlines;
}

QList<QLine> AmslerDist::getLines(){
    return this->lines;
}

void AmslerDist::setPoint(QPoint newPoint){
    this->point = newPoint;
}

QPoint AmslerDist::getPoint(){
    return this->point;
}

void AmslerDist::setWidth(int newWidth){
    this->width = newWidth;
}

int AmslerDist::getWidth(){
    return this->width;
}

void AmslerDist::setId(int id){
    this->id = id;
}

int AmslerDist::getId(){
    return this->id;
}


// ----------------------------------------------------------------------------
// Layer class ----------------------------------------------------------------
// ----------------------------------------------------------------------------
Layer::Layer(int bscanWidth, int index){
    this->index = index;
    this->layer = (Layers)index;
    this->name = encodeLayer(this->layer);
    this->bscanWidth = bscanWidth;

    this->points.clear();
    this->pointsAuto.clear();
    this->error.clear();
    this->errorTh.clear();

    QPoint p(-1,-1);
    this->points.reserve(bscanWidth);
    this->pointsAuto.reserve(bscanWidth);
    this->error.reserve(bscanWidth);
    this->errorTh.reserve(bscanWidth);
    for (int i=0; i<bscanWidth; i++){
        this->points.append(p);
        this->pointsAuto.append(p);
        this->error.append(0);
        this->errorTh.append(0);
    }

    this->anotated = false;
    this->errorAvg = 0;
    this->errorMSE = 0;
    this->errorDev = 0;
    this->errorSum = 0;
    this->errorSquareSum = 0;
    this->errorCount = 0;
}

void Layer::setPoint(QPoint p){
    if (p.y() == -1)
        this->points[p.x()] = QPoint(-1,-1);
    else
        this->points[p.x()] = p;
}

void Layer::setPoints(QList<QPoint> pointsList){
    foreach (QPoint p, pointsList) {
        if (p.y() == -1)
            this->points[p.x()] = QPoint(-1,-1);
        else
            this->points[p.x()] = p;
    }
}

QPoint Layer::getPoint(int x){
    return this->points[x];
}

QList<QPoint> Layer::getPoints(){
    QList<QPoint> list;
    QPoint p;
    for (int j=0; j < this->bscanWidth; j++){ //(x,y)
        p = this->points[j];
        if ((p.x() != -1) && (p.y() != -1)){
            list.append(p);
        }
    }
    return list;
}

QList<QPoint> Layer::getPoints(int xMin, int xMax){
    QList<QPoint> list;
    for (int i = xMin; i <= xMax; i++){
        list.append(this->points[i]);
    }
    return list;
}

void Layer::setAnnotation(bool value){
    this->anotated = value;
}

bool Layer::getAnnotation(){
    return this->anotated;
}

void Layer::countAnnotatedPixels(){
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

void Layer::smooth(){
    // find standing out points
    for (int j=0; j < (this->bscanWidth - 2); j++){
        QPoint p0 = this->points[j];
        QPoint p1 = this->points[j+1];
        QPoint p2 = this->points[j+2];
        if ((p0.x() != -1) && (p1.x() != -1) && (p2.x() != -1)){
            int diff02 = qAbs(p2.y() - p0.y());
            int diff01 = qAbs(p1.y() - p0.y());
            int diff12 = qAbs(p2.y() - p1.y());
            if ((diff01 > diff02) || (diff12 > diff02)){
                p1.setY((p0.y() + p2.y())/2);
                this->points[j+1] = p1;
            }
        }
    }

    // find holes of 1 or 2 pixels and refill them...
    int js=0;
    bool stop = false;
    while (!stop){
        if (js >= (this->bscanWidth-1)){
            stop = true;
            break;
        }
        // find first value (js)
        int x1 = this->points[js].x();
        int x2 = this->points[js+1].x();
        if ((x1 != -1) && (x2 == -1)){
            int je = js+1;
            bool stop2 = false;
            while (!stop2){
                if (je >= (this->bscanWidth-1)){
                    stop = true;
                    break;
                }
                // find second value (je)
                x1 = this->points[je].x();
                x2 = this->points[je+1].x();
                if ((x1 == -1) && (x2 != -1) && ((je - js) <= 3)){
                    // refill hole
                    QPoint p0 = this->points[js];
                    QPoint p1 = this->points[je+1];
                    float diff = (float)((float)(p1.y() - p0.y())/(float)(p1.x() - p0.x()));
                    int i=1;
                    for (int k=js+1; k <= je; k++){
                        this->points[k] = QPoint(k, (p0.y() + i*diff));
                        i++;
                    }
                    js = je;
                    stop2 = true;
                }
                je++;
            }
        }
        js++;
    }
}

void Layer::fillContactArea(QList<QPoint> pointsILM){
    // find end of annotation (on the right)
    int js=0;
    int je=0;
    bool stop = false;
    bool stop2 = false;
    while (!stop){
        if (js >= (this->bscanWidth-1)){
            stop = true;
            break;
        }
        // find first value (js)
        int x1 = this->points[js].x();
        int x2 = this->points[js+1].x();
        if ((x1 != -1) && (x2 == -1)){
            je = js+1;
            stop2 = false;
            while (!stop2){
                if (je >= (this->bscanWidth-1)){
                    stop = true;
                    break;
                }
                // find second value (je)
                x1 = this->points[je].x();
                x2 = this->points[je+1].x();
                if ((x1 == -1) && (x2 != -1)){
                    js = je;
                    stop2 = true;
                }
                je++;
            }
        }
        js++;
    }

    if ((js > 0) && (js < (this->bscanWidth-1)) && (je > js) && (je == this->bscanWidth-1)){
        js--;
        int diff = this->points[js].y() - pointsILM[js].y();
        if (qAbs(diff) <= 10){
            int sgn = 1;
            if (diff < 0)
                sgn = -1;
            for (int i=js+1; i < js+qAbs(diff); i++){
                if ((i > 0) && (i <= this->bscanWidth-1))
                    this->points[i] = QPoint(i,this->points[i-1].y() + sgn);
            }
            js += qAbs(diff);
            for (int i=js; i <= this->bscanWidth-1; i++){
                this->points[i] = pointsILM[i];
            }
        }
    }

    // find end of annotation (on the left)
    js=this->bscanWidth-1;
    je=this->bscanWidth-1;
    stop = false;
    stop2 = false;
    while (!stop){
        if (js <= 0){
            stop = true;
            break;
        }
        // find first value (js)
        int x1 = this->points[js].x();
        int x2 = this->points[js-1].x();
        if ((x1 != -1) && (x2 == -1)){
            je = js-1;
            stop2 = false;
            while (!stop2){
                if (je <= 0){
                    stop = true;
                    break;
                }
                // find second value (je)
                x1 = this->points[je].x();
                x2 = this->points[je-1].x();
                if ((x1 == -1) && (x2 != -1)){
                    js = je;
                    stop2 = true;
                }
                je--;
            }
        }
        js--;
    }

    if ((js > 0) && (js < (this->bscanWidth-1)) && (je < js) && (je == 0)){
        js++;
        int diff = this->points[js].y() - pointsILM[js].y();
        if (qAbs(diff) <= 10){
            int sgn = 1;
            if (diff < 0)
                sgn = -1;
            for (int i=js-1; i < js-qAbs(diff); i--){
                if ((i >= 0) && (i < this->bscanWidth-1))
                this->points[i] = QPoint(i,this->points[i+1].y() + sgn);
            }
            js -= qAbs(diff);
            for (int i=js; i >= 0; i--){
                this->points[i] = pointsILM[i];
            }
        }
    }
}

void Layer::fillFromILM(QList<QPoint> pointsILM){
    // find holes bigger than 1 pixel
    int js=0;
    bool stop = false;
    while (!stop){
        if (js >= (this->bscanWidth-1)){
            stop = true;
            break;
        }
        // find first value (js)
        int x1 = this->points[js].x();
        int x2 = this->points[js+1].x();
        if ((x1 != -1) && (x2 == -1)){
            int je = js+1;
            bool stop2 = false;
            while (!stop2){
                if (je >= (this->bscanWidth-1)){
                    stop = true;
                    break;
                }
                // find second value (je)
                x1 = this->points[je].x();
                x2 = this->points[je+1].x();
                if ((x1 == -1) && (x2 != -1)){
                    // refill hole
                    for (int k=js+1; k <= je; k++){
                        this->points[k] = pointsILM[k];
                    }
                    js = je;
                    stop2 = true;
                }
                je++;
            }
        }
        js++;
    }
}

void Layer::fillStraight(){
    // find holes bigger than 1 pixel
    int js=0;
    bool stop = false;
    while (!stop){
        if (js >= (this->bscanWidth-1)){
            stop = true;
            break;
        }
        // find first value (js)
        int x1 = this->points[js].x();
        int x2 = this->points[js+1].x();
        if ((x1 != -1) && (x2 == -1)){
            int je = js+1;
            bool stop2 = false;
            while (!stop2){
                if (je >= (this->bscanWidth-1)){
                    stop = true;
                    break;
                }
                // find second value (je)
                x1 = this->points[je].x();
                x2 = this->points[je+1].x();
                if ((x1 == -1) && (x2 != -1)){
                    // refill hole
                    QPoint p0 = this->points[js];
                    QPoint p1 = this->points[je+1];
                    float diff = (float)((float)(p1.y() - p0.y())/(float)(p1.x() - p0.x()));
                    int i=1;
                    for (int k=js+1; k <= je; k++){
                        this->points[k] = QPoint(k, (p0.y() + i*diff));
                        i++;
                    }
                    js = je;
                    stop2 = true;
                }
                je++;
            }
        }
        js++;
    }
}

void Layer::resetPoints(){
    QPoint p(-1,-1);

    this->points.clear();
    this->error.clear();
    this->errorTh.clear();

    this->points.reserve(this->bscanWidth);
    this->error.reserve(this->bscanWidth);
    this->errorTh.reserve(this->bscanWidth);
    for (int i=0; i<this->bscanWidth; i++){
        this->points.append(p);
        this->error.append(0);
        this->errorTh.append(0);
    }
    this->errorAvg = 0;
    this->errorMSE = 0;
    this->errorDev = 0;
    this->errorSum = 0;
    this->errorSquareSum = 0;
    this->errorCount = 0;
}

void Layer::setPointAuto(QPoint p){
    if ((p.x() > -1) && (p.x() < this->bscanWidth)){
        if (p.y() < 1)
            this->pointsAuto[p.x()] = QPoint(-1,-1);
        else
            this->pointsAuto[p.x()] = p;
    }
}

void Layer::setPointsAuto(QList<QPoint> pointsList){
    foreach (QPoint p, pointsList) {
        if ((p.x() > -1) && (p.x() < this->bscanWidth)){
            if (p.y() < 1)
                this->pointsAuto[p.x()] = QPoint(-1,-1);
            else
                this->pointsAuto[p.x()] = p;
        }
    }
}

QPoint Layer::getPointAuto(int x){
    return this->pointsAuto[x];
}

QList<QPoint> Layer::getPointsAuto(){
    QList<QPoint> list;
    QPoint p;
    for (int i=0; i < this->bscanWidth; i++){
        p = this->pointsAuto[i];
        if ((p.x() != -1) && (p.y() != -1)){
            list.append(p);
        }
    }
    return list;
}

QList<QPoint> Layer::getPointsAuto(int xMin, int xMax){
    QList<QPoint> list;
    for (int i = xMin; i <= xMax; i++){
        list.append(this->pointsAuto[i]);
    }
    return list;
}

void Layer::resetPointsAuto(){
    QPoint p(-1,-1);

    this->pointsAuto.clear();
    this->error.clear();
    this->errorTh.clear();

    this->pointsAuto.reserve(this->bscanWidth);
    this->error.reserve(this->bscanWidth);
    this->errorTh.reserve(this->bscanWidth);
    for (int i=0; i<this->bscanWidth; i++){
        this->pointsAuto.append(p);
        this->error.append(0);
        this->errorTh.append(0);
    }
    this->errorAvg = 0;
    this->errorMSE = 0;
    this->errorDev = 0;
    this->errorSum = 0;
    this->errorSquareSum = 0;
    this->errorCount = 0;
}

void Layer::computeError(){
    int errorAbs = 0;
    int errorAbsSum = 0;
    int errorAbsSquareSum = 0;
    double errorMSESquareSum = 0;
    int errorCount = 0;
    int errorCountAll = 0;

    for (int i=0; i < this->bscanWidth; i++){
        if ((this->points[i].x() != -1) && (this->pointsAuto[i].x() != -1)){
            this->error[i] = this->points[i].y() - this->pointsAuto[i].y();
            errorAbs = qAbs(this->error[i]);
            if (errorAbs > 5){
                this->errorTh[i] = errorAbs;
                errorAbsSum += errorAbs;
                errorAbsSquareSum += qPow(errorAbs,2);
                errorCount++;
            }
            errorCountAll++;
        }
    }

    if (errorCountAll != 0){
        this->errorAvg = (double)errorAbsSum/errorCountAll;
        this->errorDev = qSqrt(((double)errorAbsSquareSum / errorCountAll) - this->errorAvg*this->errorAvg);
        for (int i=0; i < this->bscanWidth; i++){
            if ((this->points[i].x() != -1) && (this->pointsAuto[i].x() != -1))
                errorMSESquareSum += qPow((double)this->errorTh[i],2);
        }
        this->errorMSE = (double)errorMSESquareSum / errorCountAll;
        double peakval = 255;
        this->errorPSNR = 10 * qLn(peakval * peakval / this->errorMSE) / qLn(10);
    } else {
        this->errorAvg = 0;
        this->errorDev = 0;
    }
    this->errorSum = errorAbsSum;
    this->errorSquareSum = errorAbsSquareSum;
    this->errorCount = errorCount;
    this->errorCountAll = errorCountAll;
}

int Layer::getErrorSum(){
    return this->errorSum;
}

int Layer::getErrorSquareSum(){
    return this->errorSquareSum;
}

QList<int> Layer::getErrorThVector(){
    return this->errorTh;
}

int Layer::getErrorCount(){
    return this->errorCount;
}

int Layer::getErrorCountAll(){
    return this->errorCountAll;
}

double Layer::getErrorAvg(){
    return this->errorAvg;
}

double Layer::getErrorMSE(){
    return this->errorMSE;
}

double Layer::getErrorDev(){
    return this->errorDev;
}

double Layer::getErrorPSNR(){
    return this->errorPSNR;
}

// ----------------------------------------------------------------------------
// PatientData class ----------------------------------------------------------
// ----------------------------------------------------------------------------
PatientData::PatientData()
{
    this->lastName = "";
    this->firstName = "";
    this->birthDate = QDate(1900,1,1);
    this->gender = 0; // female
    this->age = 0;

    this->visOP = "";
    this->visOL = "";
    this->snOP = "";
    this->snOL = "";
    this->anterior = "";
    this->posterior = "";
    this->otherDisorders = "";
	this->mcvOP = "";
	this->mcvOL = "";
	this->mchOP = "";
	this->mchOL = "";
    this->pathologyOP = UNDEFINED;
    this->pathologyOL = UNDEFINED;

    this->imageFileList = QStringList();

//    this->bscansNumber = 100;
//    this->bscansNumberAll = 100;
//    this->bscanWidth = 800;
//    this->bscanHeight = 1010;
//    this->voxelWidth = 8.0;
//    this->voxelHeight = 8.0;
//    this->voxelDepth = 2.0;
//    this->octDevice = COPERNICUS;

    this->bscansNumber = 141;
    this->bscansNumberAll = 144;
    this->bscanWidth = 385;
    this->bscanHeight = 640;
    this->voxelWidth = 7.0;
    this->voxelHeight = 7.0;
    this->voxelDepth = 2.0;
    this->octDevice = AVANTI;

    this->scanCenter = QPoint(-1,-1);
    this->eye = 1; // OD
    this->areaUnit = 0.0;
    this->depthCoeff = 0.0;
    this->examDate = QDate(2015,1,1);

    this->errorAvg = 0.0;
    this->errorDev = 0.0;
    this->errorMSE = 0.0;
    this->errorRMSE = 0.0;
    this->errorPSNR = 0.0;
    this->errorProc = 0.0;

    int layersCount = 13;
    layersErrorAvg.reserve(layersCount);
    layersErrorDev.reserve(layersCount);
    layersErrorMSE.reserve(layersCount);
    layersErrorRMSE.reserve(layersCount);
    layersErrorPSNR.reserve(layersCount);
    layersErrorProc.reserve(layersCount);
    for (int i=0; i < layersCount; i++){
        layersErrorAvg.append(0);
        layersErrorDev.append(0);
        layersErrorMSE.append(0);
        layersErrorRMSE.append(0);
        layersErrorPSNR.append(0);
        layersErrorProc.append(0);
    }
    this->manualAnnotations = false;
    this->autoAnnotations = false;

    this->contactAreaCF = 0.0;
    this->contactAreaIM = 0.0;
    this->contactAreaOM = 0.0;
    this->contactThreshold = 15.0;

    this->fundusImage = QImage();

    int count = 360;
    this->circProfileCF.reserve(count);
    this->circProfileIM.reserve(count);
    this->circProfileOM.reserve(count);
    for (int i=0; i < count; i++){
        this->circProfileCF.append(std::numeric_limits<double>::quiet_NaN());
        this->circProfileIM.append(0);
        this->circProfileOM.append(0);
    }

    int histBars = 8;
    this->virtualMapHistogram.reserve(histBars);
    for (int i=0; i < histBars; i++){
        this->virtualMapHistogram.append(0);
    }

    int gridAreas = 10;
    this->volumeGrid.reserve(gridAreas);
    for (int i=0; i < gridAreas; i++){
        this->volumeGrid.append(0);
    }

    this->retinaDepth = 0.0;
}

QImage PatientData::getNormalImage(int normalImageNumber){
    QImage normalImage(this->bscansNumber,this->bscanHeight,QImage::Format_Indexed8);
    QRgb value;
    int pixel_val;

    for (int b=0; b < this->bscansNumber; b++){
        for (int y=0; y < this->bscanHeight; y++){
            pixel_val = this->octdata[b][y][normalImageNumber];
            value = qRgb(pixel_val,pixel_val,pixel_val);
            normalImage.setColor(pixel_val,value);
            normalImage.setPixel(b,y,pixel_val);
        }
    }

    return normalImage;
}


// Fundus Image ---------------------------------------------------------------
QImage PatientData::getFundusImage(){
    return this->fundusImage;
}

void PatientData::setFundusImage(QImage newImage){
    this->fundusImage = newImage;
}


// Annotations ----------------------------------------------------------------
void PatientData::setPoint(int bscanNumber, Layers layer, QPoint p){
    this->Bscans[bscanNumber].layers[(int)layer].setPoint(p);
    this->manualAnnotations = true;
}

void PatientData::setLayerPoints(int bscanNumber, QList<QPoint> pointsList, Layers layer, bool isNormal){
    if (!isNormal){
        this->Bscans[bscanNumber].layers[(int)layer].setPoints(pointsList);
    } else {
        foreach (QPoint p, pointsList) {
            this->Bscans[p.x()].layers[(int)layer].setPoint(QPoint(bscanNumber,p.y()));
        }
    }
    this->manualAnnotations = true;
}

QPoint PatientData::getLayerPoint(int bscanNumber, Layers layer, int x){
    QPoint p(-1,-1);

    p = this->Bscans[bscanNumber].layers[(int)layer].getPoint(x);

    return p;
}

QList<QPoint> PatientData::getLayerPoints(int bscanNumber, Layers layer){
    QList<QPoint> list = this->Bscans[bscanNumber].layers[(int)layer].getPoints();

    return list;
}

QList<QPoint> PatientData::getLayerPoints(int bscanNumber, Layers layer, int xMin, int xMax, bool isNormal){
    QList<QPoint> list;
    if (!isNormal){
        list = this->Bscans[bscanNumber].layers[(int)layer].getPoints(xMin, xMax);
    } else {
        for (int scan=xMin; scan<=xMax; scan++){
            list.append(this->Bscans[scan].layers[(int)layer].getPoint(bscanNumber));
        }
    }

    return list;
}

void PatientData::setLayerAnnotation(int bscanNumber, Layers layer, bool value){

    this->Bscans[bscanNumber].layers[(int)layer].setAnnotation(value);
}

bool PatientData::getLayerAnnotation(int bscanNumber, Layers layer){
    bool res = this->Bscans[bscanNumber].layers[(int)layer].getAnnotation();

    return res;
}

int PatientData::getLayerAnnotatedCount(Layers layer){
    int result = 0;

    for (int i = 0; i < this->bscansNumber; i++){
        if (this->Bscans[i].layers[(int)layer].getAnnotation())
            result++;
    }

    return result;
}

void PatientData::countAnnotatedPixelsInLayer(int bscanNumber, Layers layer){

    this->Bscans[bscanNumber].layers[(int)layer].countAnnotatedPixels();

}

void PatientData::smoothLayer(Layers layer, int bscanNumber){
    if (bscanNumber == -1){
        for (int i=0; i < this->bscansNumber; i++){
            this->Bscans[i].layers[(int)layer].smooth();
        }
    } else {
        this->Bscans[bscanNumber].layers[(int)layer].smooth();
    }
}

void PatientData::fillContactArea(int bscanNumber){
    if (bscanNumber == -1){
        for (int i=0; i < this->bscansNumber; i++){
            QList<QPoint> pointsILM = this->Bscans[i].layers[(int)ILM].getPoints(0,this->bscanWidth-1);
            this->Bscans[i].layers[(int)PCV].fillContactArea(pointsILM);
        }
    } else {
        QList<QPoint> pointsILM = this->Bscans[bscanNumber].layers[(int)ILM].getPoints(0,this->bscanWidth-1);
        this->Bscans[bscanNumber].layers[(int)PCV].fillContactArea(pointsILM);
    }
}

void PatientData::fillLayerFromILM(Layers layer, int bscanNumber){
    if (bscanNumber == -1){
        for (int i=0; i < this->bscansNumber; i++){
            QList<QPoint> pointsILM = this->Bscans[i].layers[(int)ILM].getPoints(0,this->bscanWidth-1);
            this->Bscans[i].layers[(int)layer].fillFromILM(pointsILM);
        }
    } else {
        QList<QPoint> pointsILM = this->Bscans[bscanNumber].layers[(int)ILM].getPoints(0,this->bscanWidth-1);
        this->Bscans[bscanNumber].layers[(int)layer].fillFromILM(pointsILM);
    }
}

void PatientData::fillLayerStraight(Layers layer, int bscanNumber){
    if (bscanNumber == -1){
        for (int i=0; i < this->bscansNumber; i++){
            this->Bscans[i].layers[(int)layer].fillStraight();
        }
    } else {
        this->Bscans[bscanNumber].layers[(int)layer].fillStraight();
    }
}

void PatientData::resetManualAnnotations(){
    QList<Layers> layerList = getAllLayers();
    for (int i=0; i<this->bscansNumber; i++){
        foreach (Layers layer, layerList) {
            this->Bscans[i].layers[(int)layer].resetPoints();
            this->Bscans[i].layers[(int)layer].countAnnotatedPixels();
        }
    }

    this->manualAnnotations = false;

    this->resetVirtualMap();
    this->resetVirtualMapError();
}

bool PatientData::hasManualAnnotations(){
    return this->manualAnnotations;
}

// Auto annotations -----------------------------------------------------------
void PatientData::resetAutoAnnotations(){

    QList<Layers> layerList = getAllLayers();
    for (int i=0; i<this->bscansNumber; i++){
        foreach (Layers layer, layerList) {
            this->Bscans[i].layers[(int)layer].resetPointsAuto();
        }
    }

    this->autoAnnotations = false;

    // clear virtual map memory
    this->resetVirtualMap();
    this->resetVirtualMapAuto();
    this->resetVirtualMapError();
}

bool PatientData::hasAutoAnnotations(){
    return this->autoAnnotations;
}

void PatientData::setPointAuto(int bscanNumber, Layers layer, QPoint p){
    if ((bscanNumber >= 0) && (bscanNumber < this->bscansNumber)){
        this->Bscans[bscanNumber].layers[(int)layer].setPointAuto(p);
        this->autoAnnotations = true;
    }
}

void PatientData::setLayerPointsAuto(int bscanNumber, QList<QPoint> pointsList, Layers layer){
    this->Bscans[bscanNumber].layers[(int)layer].setPointsAuto(pointsList);
    this->autoAnnotations = true;
}

QPoint PatientData::getLayerPointAuto(int bscanNumber, Layers layer, int x){
    QPoint p(-1,-1);

    p = this->Bscans[bscanNumber].layers[(int)layer].getPointAuto(x);

    return p;
}

QList<QPoint> PatientData::getLayerPointsAuto(int bscanNumber, Layers layer){
    QList<QPoint> list = this->Bscans[bscanNumber].layers[(int)layer].getPointsAuto();

    return list;
}

QList<QPoint> PatientData::getLayerPointsAuto(int bscanNumber, Layers layer, int xMin, int xMax){
    QList<QPoint> list = this->Bscans[bscanNumber].layers[(int)layer].getPointsAuto(xMin, xMax);

    return list;
}

void PatientData::computeLayerError(Layers layer){

//    int layerErrorCount = 0;
//    int layerErrorCountAll = 0;
//    int layerErrorSum = 0;
//    int layerErrorSquareSum = 0;
//    double layerErrorMSESquareSum = 0;
//    double layerErrorAvg = 0;
//    double layerErrorMSE = 0;
//    double layerErrorDev = 0;
//    double layerErrorPSNR = 0;
//    QList<int> errorTh;
//    QList<QPoint> mPoints;
//    QList<QPoint> aPoints;

//    QList<double> ssList;
    for (int j=0; j < this->bscansNumber; j++){
        this->Bscans[j].layers[(int)layer].computeError();

//        layerErrorSum += this->Bscans[j].layers[(int)layer].getErrorSum();
//        layerErrorSquareSum += this->Bscans[j].layers[(int)layer].getErrorSquareSum();
//        layerErrorCount += this->Bscans[j].layers[(int)layer].getErrorCount();
//        layerErrorCountAll += this->Bscans[j].layers[(int)layer].getErrorCountAll();

        // TODO: zrobić qlist z layerErrorSquareSum i porównać z calc... (ss dla wszystkich b-skanow)
//        ssList.append(this->Bscans[j].layers[(int)layer].getErrorSquareSum());
    }

//    if (layerErrorCountAll != 0){
//        layerErrorAvg = (double)layerErrorSum/layerErrorCountAll;
//        layerErrorDev = qSqrt(((double)layerErrorSquareSum / (double)layerErrorCountAll) - layerErrorAvg*layerErrorAvg);
//        for (int j=0; j < this->bscansNumber; j++){
//            errorTh = this->Bscans[j].layers[(int)layer].getErrorThVector();
//            mPoints = this->Bscans[j].layers[(int)layer].getPoints(0, this->bscanWidth-1);
//            aPoints = this->Bscans[j].layers[(int)layer].getPointsAuto(0, this->bscanWidth-1);
//            for (int i=0; i < this->bscanWidth; i++){
//                //qDebug() << "bscan number: " << j << ", bscan pixel: " << i;
//                if ((mPoints[i].x() != -1) && (aPoints[i].x() != -1)){
//                    layerErrorMSESquareSum += qPow((double)errorTh[i],2.0);
//                    //max = qMax(max,errorTh[i]);
//                }
//            }
//        }
//        layerErrorMSE = (double)layerErrorMSESquareSum/layerErrorCountAll;
//        double peaksnr = this->bscanHeight;
//        layerErrorPSNR = 10 * qLn(peaksnr * peaksnr / layerErrorMSE) / qLn(10);
//    }

//    this->layersErrorAvg[(int)layer] = layerErrorAvg;
//    this->layersErrorDev[(int)layer] = layerErrorDev;
//    this->layersErrorMSE[(int)layer] = layerErrorMSE;
//   // this->layersErrorRMSE[(int)layer] = layerErrorRMSE;
//    this->layersErrorPSNR[(int)layer] = layerErrorPSNR;
//    this->layersErrorProc[(int)layer] = (double)layerErrorCount / (double)(this->bscansNumber*this->bscanWidth) * 100;
}

void PatientData::computeErrorAll(){
//    int errorCount = 0;
//    int errorSum = 0;
//    double errorSquareSum = 0;
//    QList<int> errorPoint;
//    QList<QPoint> mPoints;
//    QList<QPoint> aPoints;

//    QList<Layers> layersForError = getLayersForError();

//    int xc = this->scanCenter.x();
//    int yc = this->scanCenter.y();
//    if (xc == -1)
//        xc = this->bscanWidth / 2;
//    if (yc == -1)
//        yc = this->bscansNumber / 2;

//    double distance = 0.0;
//    double deltaX = (double)(this->voxelWidth / (this->bscanWidth - 1));
//    double deltaY = (double)(this->voxelHeight / (this->bscansNumber - 1));

//    foreach (Layers layer, layersForError) {
//        for (int j=0; j < this->bscansNumber; j++){
//            // get thresholded error
//            errorPoint = this->Bscans[j].layers[(int)layer].getErrorThVector();
//            mPoints = this->Bscans[j].layers[(int)layer].getPoints(0, this->bscanWidth-1);
//            aPoints = this->Bscans[j].layers[(int)layer].getPointsAuto(0, this->bscanWidth-1);
//            for (int i=0; i < this->bscanWidth; i++){
//                int x = i-xc;
//                int y = j-yc;
//                // check if point is in the ETDRS area & data exists
//                distance = calculateDistance(QPoint(xc,yc),QPoint(x,y),deltaX,deltaY);
//                if ((mPoints[i].x() != -1) && (aPoints[i].x() != -1) && (distance <= 3.0)){
//                    errorSquareSum += qPow((double)errorPoint[i],2.0);
//                    errorSum += errorPoint[i];
//                    errorCount++;
//                }
//            }
//        }
//    }

//    if (errorCount != 0){
//        errorAvg = (double)errorSum / (double)errorCount;
//        errorDev = qSqrt(((double)errorSquareSum / (double)errorCount) - errorAvg*errorAvg);
//        errorMSE = (double)errorSquareSum/(double)errorCount;
//        double peaksnr = this->bscanHeight;
//        errorPSNR = 10 * qLn(peaksnr * peaksnr / errorMSE) / qLn(10);
//    }
}

void PatientData::setErrorAll(double avg, double dev, double mse, double rmse, double psnr, double proc){
    this->errorAvg = avg;
    this->errorDev = dev;
    this->errorMSE = mse;
    this->errorRMSE = rmse;
    this->errorPSNR = psnr;
    this->errorProc = proc;
}

void PatientData::setErrorLayer(Layers layer, double avg, double dev, double mse, double rmse, double psnr, double proc){
    this->layersErrorAvg[(int)layer] = avg;
    this->layersErrorDev[(int)layer] = dev;
    this->layersErrorMSE[(int)layer] = mse;
    this->layersErrorRMSE[(int)layer] = rmse;
    this->layersErrorPSNR[(int)layer] = psnr;
    this->layersErrorProc[(int)layer] = proc;
}

double PatientData::getLayerErrorAvg(Layers layer, QString unit){
    double error = this->layersErrorAvg[(int)layer];

    // compute in [um] not [px]
    if (unit != ""){
        error = error * depthCoeff;
    }
    return error;
}

double PatientData::getLayerErrorMSE(Layers layer, QString unit){
    double error = this->layersErrorMSE[(int)layer];

    if (unit != ""){
        error = error * depthCoeff;
    }
    return error;
}

double PatientData::getLayerErrorDev(Layers layer, QString unit){
    double deviation = this->layersErrorDev[(int)layer];

    // compute in [um] not [px]
    if (unit != ""){
        deviation = deviation * depthCoeff;
    }
    return deviation;
}

double PatientData::getLayerErrorPSNR(Layers layer, QString unit){
    double psnr = this->layersErrorPSNR[(int)layer];

    if (unit != ""){
        psnr = psnr * depthCoeff;
    }
    return psnr;
}

double PatientData::getLayerErrorProc(Layers layer){
    double errorProc = this->layersErrorProc[(int)layer];
    return errorProc;
}

double PatientData::getLayerLineErrorAvg(Layers layer, int lineNumber, QString unit){
    double error = this->Bscans[lineNumber].layers[(int)layer].getErrorAvg();

    if (unit != ""){
        error = error * depthCoeff;
    }
    return error;
}

double PatientData::getLayerLineErrorDev(Layers layer, int lineNumber, QString unit){
    double deviation = this->Bscans[lineNumber].layers[(int)layer].getErrorDev();

    if (unit != ""){
        deviation = deviation * depthCoeff;
    }
    return deviation;
}

double PatientData::getErrorAvg(QString unit){
    double avg = this->errorAvg;
    if (unit != ""){
        avg = avg * depthCoeff;
    }
    return avg;
}

double PatientData::getErrorMSE(QString unit){
    double mse = this->errorMSE;
    if (unit != ""){
        mse = mse * depthCoeff;
    }
    return mse;
}

double PatientData::getErrorDev(QString unit){
    double dev = this->errorDev;
    if (unit != ""){
        dev = dev * depthCoeff;
    }
    return dev;
}

double PatientData::getErrorPSNR(QString unit){
    double psnr = this->errorPSNR;
    if (unit != ""){
        psnr = psnr * depthCoeff;
    }
    return psnr;
}

// Virtual Map ----------------------------------------------------------------
void PatientData::resetVirtualMap(){
    // clear virtual map memory
    this->virtualMap.clear();

    // create empty virtual map matrix
    QList<int> row;
    row.reserve(this->bscanWidth);
    for (int i=0; i < this->bscanWidth; i++){
        row.append(-1);
    }
    for (int j=0; j < this->bscansNumber; j++){
        this->virtualMap.insert(j, row);
    }
}

void PatientData::resetVirtualMapAuto(){
    // clear virtual map memory
    this->virtualMapAuto.clear();

    // create empty virtual map matrix
    QList<int> row;
    row.reserve(this->bscanWidth);
    for (int i=0; i < this->bscanWidth; i++){
        row.append(-1);
    }
    for (int j=0; j < this->bscansNumber; j++){
        this->virtualMapAuto.insert(j, row);
    }
}

void PatientData::resetVirtualMapError(){
    this->virtualMapError.clear();
    this->virtualMapLineErrorAvg.clear();
    this->virtualMapLineErrorDev.clear();

    // create empty virtual map matrix
    QList<int> row;
    QList<double> rowF;
    row.reserve(this->bscanWidth);
    rowF.reserve(this->bscanWidth);
    for (int i=0; i < this->bscanWidth; i++){
        row.append(0);
        rowF.append(0);
    }
    for (int j=0; j < this->bscansNumber; j++){
        this->virtualMapError.insert(j, row);
    }
    this->virtualMapLineErrorAvg = rowF;
    this->virtualMapLineErrorDev = rowF;

    this->virtualMapErrorAvg = 0;
    this->virtualMapErrorDev = 0;
}

void PatientData::computeVirtualMap(Layers layer1, Layers layer2){

    this->resetVirtualMap();

    // calculate distances
    for (int i=0; i < this->bscansNumber; i++){
        QList<int> row = this->virtualMap.value(i);

        for (int j=0; j < this->bscanWidth; j++){
            int layerTop = this->Bscans[i].layers[(int)layer1].getPoint(j).y();
            if (layer1 != layer2){
                int layerBottom = this->Bscans[i].layers[(int)layer2].getPoint(j).y();
                if ((layerTop != -1) && (layerBottom != -1)){
                    row[j] = layerBottom - layerTop; // reversed coordinate system
                    if (row[j] < 0)
                        row[j] = 0;
                }
            } else {
                if (layerTop != -1){
                    row[j] = this->bscanHeight - layerTop;
                }
            }
        }
        this->virtualMap.insert(i, row);
    }

    // calculate layers contact area
    calculateContactArea(1.0);
    calculateContactArea(3.0);
    calculateContactArea(6.0);

    // calculate circular profile curve
    calculateCircProfile(1.0);
    calculateCircProfile(3.0);
    calculateCircProfile(6.0);

    // calculate histogram values
    calculateVirtualMapHistogram();

    // calculate volume in ETDRS grid
    calculateVolumeGrid();

    // calculate retina depth
    calculateRetinaDepth();
}

void PatientData::computeVirtualMapAuto(Layers layer1, Layers layer2){

    // clear virtual map memory
    this->resetVirtualMapAuto();
    this->resetVirtualMapError();

    // calculate distances
    for (int i=0; i < this->bscansNumber; i++){
        QList<int> row = this->virtualMapAuto.value(i);

        for (int j=0; j < this->bscanWidth; j++){
            int layerTop = this->Bscans[i].layers[(int)layer1].getPointAuto(j).y();
            if (layer1 != layer2){
                int layerBottom = this->Bscans[i].layers[(int)layer2].getPointAuto(j).y();
                if ((layerTop != -1) && (layerBottom != -1)){
                    row[j] = layerBottom - layerTop;
                }
            } else {
                if (layerTop != -1){
                    row[j] = this->bscanHeight = layerTop;
                }
            }
        }
        this->virtualMapAuto.insert(i, row);
    }
}

int PatientData::getVirtualMapValue(int x, int y, QString unit){
    int value = this->virtualMap.value(y)[x];

    if (unit != ""){
        value = value * depthCoeff;
    }
    return value;
}

int PatientData::getVirtualMapAutoValue(int x, int y, QString unit){
    int value = this->virtualMapAuto.value(y)[x];

    if (unit != ""){
        value = value * depthCoeff;
    }
    return value;
}

void PatientData::computeVirtualMapError(){
    int sumNumber = 0;
    int error = 0;
    int errorSum = 0;
    int errorSquareSum = 0;

    int lineSumNumber;
    int lineErrorSum;
    int lineErrorSquareSum;

    for (int i=0; i < this->bscansNumber; i++){
        lineSumNumber = 0;
        lineErrorSum = 0;
        lineErrorSquareSum = 0;

        for (int j=0; j < this->bscanWidth; j++){
            if ((this->virtualMap.value(i)[j] != -1) && (this->virtualMapAuto.value(i)[j] != -1)){
                error = this->virtualMap.value(i)[j] - this->virtualMapAuto.value(i)[j];
                this->virtualMapError[i][j] = error;
                if (qAbs(error) > 5){
                    errorSum += qAbs(error);
                    errorSquareSum += (error * error);
                    sumNumber++;

                    lineErrorSum += qAbs(error);
                    lineErrorSquareSum += (error * error);
                    lineSumNumber++;
                }
                error = 0;
            }
        }

        if (lineSumNumber != 0){
            this->virtualMapLineErrorAvg[i] = (float)lineErrorSum/lineSumNumber;
            this->virtualMapLineErrorDev[i] = qSqrt(lineErrorSquareSum / lineSumNumber - this->virtualMapLineErrorAvg[i]*this->virtualMapLineErrorAvg[i]);
        } else {
            this->virtualMapLineErrorAvg[i] = 0;
            this->virtualMapLineErrorDev[i] = 0;
        }
    }

    if (sumNumber != 0){
        this->virtualMapErrorAvg = (float)errorSum/sumNumber;
        this->virtualMapErrorDev = (float)qSqrt((float)(errorSquareSum / sumNumber) - this->virtualMapErrorAvg*this->virtualMapErrorAvg);
    } else {
        this->virtualMapErrorAvg = 0;
        this->virtualMapErrorDev = 0;
    }
}

double PatientData::getVirtualMapErrorAvg(QString unit){
    double error = 0;

    error = this->virtualMapErrorAvg;

    if (unit != ""){
        error = error * depthCoeff;
    }

    return error;
}

double PatientData::getVirtualMapErrorDev(QString unit){
    double deviation = 0;

    deviation = this->virtualMapErrorDev;

    if (unit != ""){
        deviation = deviation * depthCoeff;
    }

    return deviation;
}

double PatientData::getVirtualMapLineErrorAvg(int lineNumber, QString unit){
    double error = this->virtualMapLineErrorAvg[lineNumber];

    if (unit != ""){
        error = error * depthCoeff;
    }

    return error;
}

double PatientData::getVirtualMapLineErrorDev(int lineNumber, QString unit){
    double error = this->virtualMapLineErrorDev[lineNumber];

    if (unit != ""){
        error = error * depthCoeff;
    }

    return error;
}

void PatientData::setScanCenter(QPoint p){
    this->scanCenter = p;
}

QPoint PatientData::getScanCenter(){
    return this->scanCenter;
}

void PatientData::calculateRatios(){
    this->areaUnit = (double)(this->voxelWidth * this->voxelHeight) / (double)((this->bscanWidth - 1) * (this->bscansNumber - 1));
    this->depthCoeff = this->voxelDepth * 1000 / (double)(this->bscanHeight - 1); // 1 px [um]
}

double PatientData::getAreaUnit(){
    return this->areaUnit;
}

double PatientData::getDepthCoeff(){
    return this->depthCoeff;
}

void PatientData::setContactThreshold(double threshold){
    this->contactThreshold = threshold;
}

void PatientData::calculateContactArea(double diameter){
    double area = 0.0;
    double vm1,vm2,vm3,vm4;
    double threshold = this->contactThreshold / (double)1000; // mm
    int count = 0;

    int deltaXp = (this->bscanWidth - 1) * diameter / 2 / this->voxelWidth;  // w [px]
    int deltaYp = (this->bscansNumber - 1) * diameter / 2 / this->voxelHeight;   // w [px]
    double deltaX = (double)(this->voxelWidth / (this->bscanWidth - 1));
    double deltaY = (double)(this->voxelHeight / (this->bscansNumber - 1));

    int xc = this->scanCenter.x();
    int yc = this->scanCenter.y();

    if (xc == -1)
        xc = this->bscanWidth / 2;
//    else
//        xc = xc * 2;
    if (yc == -1)
        yc = this->bscansNumber / 2;

    for (int x = (xc-deltaXp-1); x <= (xc+deltaXp); x++){
        for (int y = (yc-deltaYp-1); y <= (yc+deltaYp); y++){
            if ((x >= 0) && (x < this->bscanWidth - 1) && (y >= 0) && (y < this->bscansNumber - 1)){
                if (calculateDistance(scanCenter,QPoint(x,y),deltaX,deltaY) <= (diameter/2)){
                    count = 0;
                    // check 4 corners and sum up accordingly
                    vm1 = (double)this->virtualMap.value(y)[x];
                    vm2 = (double)this->virtualMap.value(y)[x+1];
                    vm3 = (double)this->virtualMap.value(y+1)[x];
                    vm4 = (double)this->virtualMap.value(y+1)[x+1];
                    // count corners above threshold
                    if ((vm1 != -1.0) && (vm1 * depthCoeff / 1000) <= threshold){
                        count++;
                    }
                    if ((vm2 != -1.0) && (vm2 * depthCoeff / 1000) <= threshold){
                        count++;
                    }
                    if ((vm3 != -1.0) && (vm3 * depthCoeff / 1000) <= threshold){
                        count++;
                    }
                    if ((vm4 != -1.0) && (vm4 * depthCoeff / 1000) <= threshold){
                        count++;
                    }
                    area += 0.25 * count;
                }
            }
        }
    }

    switch ((int)diameter) {
    case 1:
        this->contactAreaCF = area * this->areaUnit;
        break;
    case 3:
        this->contactAreaIM = area * this->areaUnit;
        break;
    case 6:
        this->contactAreaOM = area * this->areaUnit;
        break;
    default:
        break;
    }
}

double PatientData::getContactAreaCF(){
    return this->contactAreaCF;
}

double PatientData::getContactAreaIM(){
    return this->contactAreaIM;
}

double PatientData::getContactAreaOM(){
    return this->contactAreaOM;
}

double PatientData::calculateDistance(QPoint p1, QPoint p2, double dx, double dy){
    double deltaX = p1.x() - p2.x();
    double deltaY = p1.y() - p2.y();
    if (dx != 0){
        deltaX = deltaX * dx;
    }
    if (dy != 0){
        deltaY = deltaY * dy;
    }
    double dist = qSqrt(qPow(deltaX,2) + qPow(deltaY,2));
    return dist;
}

void PatientData::calculateCircProfile(double diameter){
    QList<double> profile;
    profile.reserve(360);

    double radius = diameter / 2.0;
    int x=0;
    int y=0;
    int xc = this->scanCenter.x();
    int yc = this->scanCenter.y();

    if (xc == -1)
        xc = this->bscanWidth / 2;
//    else
//        xc = xc * 2;
    if (yc == -1)
        yc = this->bscansNumber / 2;

    double alfaR = 0.0;
    double value;
    for (int alfa=0; alfa < 360; alfa++){
        if (this->eye == 1){    // right eye
            alfaR = 2 * 3.1415926535 * (double)(alfa+180) / 360.0;
        } else if (this->eye == 0){ // left eye
            alfaR = 2 * 3.1415926535 * (double)(-alfa) / 360.0;
        }
        x = (qCos(alfaR) * radius)*(this->bscanWidth-1)/this->voxelWidth + xc;
        y = (qSin(alfaR) * radius)*(this->bscansNumber-1)/this->voxelHeight + yc;

        x = qBound(0,x,this->bscanWidth-1);
        y = qBound(0,y,this->bscansNumber-1);

        value = this->virtualMap.value(y)[x];
        if (value == -1)
            value = std::numeric_limits<double>::quiet_NaN();
        profile.append(value);
    }

    switch ((int)diameter) {
    case 1:
        this->circProfileCF = profile;
        break;
    case 3:
        this->circProfileIM = profile;
        break;
    case 6:
        this->circProfileOM = profile;
        break;
    default:
        break;
    }
}

QList<double> PatientData::getCircProfileCF(){
    return this->circProfileCF;
}

QList<double> PatientData::getCircProfileIM(){
    return this->circProfileIM;
}

QList<double> PatientData::getCircProfileOM(){
    return this->circProfileOM;
}

void PatientData::calculateVirtualMapHistogram(){
    QList<double> hist;
    for (int i=0; i < 8; i++){
        hist.append(0);
    }

    for (int bs=0; bs < this->bscansNumber; bs++){
        for (int px=0; px < this->bscanWidth; px++){
            double p = this->virtualMap.value(bs)[px];
            if (p != -1){
                p = p * depthCoeff;
                if ((p >= 0) && (p <= 100)){
                    hist[0] = hist.at(0) + 1;
                } else if ((p > 100) && (p <= 200)){
                    hist[1] = hist.at(1) + 1;
                } else if ((p > 200) && (p <= 300)){
                    hist[2] = hist.at(2) + 1;
                } else if ((p > 300) && (p <= 400)){
                    hist[3] = hist.at(3) + 1;
                } else if ((p > 400) && (p <= 500)){
                    hist[4] = hist.at(4) + 1;
                } else if ((p > 500) && (p <= 600)){
                    hist[5] = hist.at(5) + 1;
                } else if ((p > 600) && (p <= 700)){
                    hist[6] = hist.at(6) + 1;
                } else if (p > 700){
                    hist[7] = hist.at(7) + 1;
                }
            }
        }
    }

    this->virtualMapHistogram = hist;
}

QList<double> PatientData::getVirtualMapHistogram(){
    return this->virtualMapHistogram;
}

void PatientData::calculateVolumeGrid(){
    QList<double> volumeValues;
    for (int i=0; i < 10; i++){
        volumeValues.append(0);
    }

    double radius1 = 0.5;
    double radius2 = 1.5;
    double radius3 = 3.0;

    int gridAreaRadiusX = (this->bscanWidth - 1) * radius3 / this->voxelWidth;  // w [px]
    int gridAreaRadiusY = (this->bscansNumber - 1) * radius3 / this->voxelHeight;   // w [px]
    double deltaX = (double)(this->voxelWidth / (this->bscanWidth - 1));
    double deltaY = (double)(this->voxelHeight / (this->bscansNumber - 1));

    int xc = this->scanCenter.x();
    int yc = this->scanCenter.y();

    if (xc == -1)
        xc = this->bscanWidth / 2;
    if (yc == -1)
        yc = this->bscansNumber / 2;

    double distance = 0.0;
    double angleR = 0.0;
    double PI = 3.1415926535;

    for (int x = (xc-gridAreaRadiusX-1); x <= (xc+gridAreaRadiusX); x++){
        for (int y = (yc-gridAreaRadiusY-1); y <= (yc+gridAreaRadiusY); y++){
            if ((x >= 0) && (x < this->bscanWidth - 1) && (y >= 0) && (y < this->bscansNumber - 1) && (this->virtualMap.value(y)[x] != -1)){
                distance = calculateDistance(QPoint(xc,yc),QPoint(x,y),deltaX,deltaY);
                angleR = qAtan2((y-yc)*deltaY, (x-xc)*deltaX) / PI * 180;
                if (angleR < 0)
                    angleR += 360.0;
                if (distance <= radius3){
                    if (distance <= radius1){    // area 1: CF
                        volumeValues[0] += (double)this->virtualMap.value(y)[x] * depthCoeff / 1000 * this->areaUnit;
                    } else if (distance <= radius2){     // area 2-5
                        if ((angleR >= 45.0) && (angleR < 135.0)){    // area 2: IM_bottom
                            volumeValues[1] += (double)this->virtualMap.value(y)[x] * depthCoeff / 1000 * this->areaUnit;
                        } else if ((angleR >= 135.0) && (angleR < 225.0)){    // area 3: IM_left
                            volumeValues[2] += (double)this->virtualMap.value(y)[x] * depthCoeff / 1000 * this->areaUnit;
                        } else if ((angleR >= 225.0) && (angleR < 315.0)){   // area 4: IM_top
                            volumeValues[3] += (double)this->virtualMap.value(y)[x] * depthCoeff / 1000 * this->areaUnit;
                        } else {    // area 5: IM_right
                            volumeValues[4] += (double)this->virtualMap.value(y)[x] * depthCoeff / 1000 * this->areaUnit;
                        }
                    } else {    // area 6-9
                        if ((angleR >= 45.0) && (angleR < 135.0)){    // area 6: OM_bottom
                            volumeValues[5] += (double)this->virtualMap.value(y)[x] * depthCoeff / 1000 * this->areaUnit;
                        } else if ((angleR >= 135.0) && (angleR < 225.0)){    // area 7: OM_left
                            volumeValues[6] += (double)this->virtualMap.value(y)[x] * depthCoeff / 1000 * this->areaUnit;
                        } else if ((angleR >= 225.0) && (angleR < 315.0)){    // area 8: OM_top
                            volumeValues[7] += (double)this->virtualMap.value(y)[x] * depthCoeff / 1000 * this->areaUnit;
                        } else {    // area 9: OM_right
                            volumeValues[8] += (double)this->virtualMap.value(y)[x] * depthCoeff / 1000 * this->areaUnit;
                        }
                    }
                }
            }
        }
    }
    for (int i=0; i<9; i++){
        volumeValues[9] += volumeValues[i];
    }
    this->volumeGrid = volumeValues;
}

QList<double> PatientData::getVolumeGrid(){
    return this->volumeGrid;
}

void PatientData::calculateRetinaDepth(){
    double depth = 0;
    int x, y;

    if ((this->scanCenter.x() != -1) && (this->scanCenter.y() != -1)){
        x = this->scanCenter.x();
        y = this->scanCenter.y();
    } else {
        x = this->bscanWidth / 2;
        y = this->bscansNumber / 2;
    }

    double ilm = this->Bscans[y].layers[(int)ILM].getPoint(x).y();
    double rpe = this->Bscans[y].layers[(int)CHR].getPoint(x).y();

    depth = qAbs(ilm - rpe) * depthCoeff;

    this->retinaDepth = depth;
}

double PatientData::getRetinaDepth(){
    return this->retinaDepth;
}

// OCT Exam Data --------------------------------------------------------------
void PatientData::resetBscansData(){

    // clear memory
    this->Bscans.clear();

    // create empty B-scan "b"
    Bscan b;
    QList<Layers> layerList = getAllLayers();
    foreach (Layers layer, layerList) {
        b.layers.append(Layer(this->bscanWidth, (int)layer));
    }
    // create list of empty B-scans
    this->Bscans.reserve(this->bscansNumber);
    for (int i=0; i < this->bscansNumber; i++){
        this->Bscans.append(b);
    }

    QList<int> row;
    row.reserve(this->bscanWidth);
    for (int x=0; x<this->bscanWidth; x++){
        row.append(0);
    }
    QList< QList<int> > section;
    section.reserve(this->bscanHeight);
    for (int y=0; y<this->bscanHeight; y++){
        section.append(row);
    }
    this->octdata.reserve(this->bscansNumber);
    for (int s=0; s<this->bscansNumber; s++){
        this->octdata.append(section);
    }

    this->calculateRatios();
    if (this->octDevice == AVANTI)
        this->scanCenter = QPoint(this->bscanWidth/2, this->bscansNumber/2);
    else
        this->scanCenter = QPoint(this->bscanWidth/2, this->bscansNumber/2);

    // clear virtual map memory
    this->resetVirtualMap();
    this->resetVirtualMapAuto();
    this->resetVirtualMapError();

    // clear image flattening memory
    this->resetFlatDifferences();

    this->manualAnnotations = false;
    this->autoAnnotations = false;
}

int PatientData::getBscanHeight(){
    return this->bscanHeight;
}

void PatientData::setBscanHeight(int height){
    this->bscanHeight = height;
}

int PatientData::getBscanWidth(){
    return this->bscanWidth;
}

void PatientData::setBscanWidth(int width){
    this->bscanWidth = width;
}

int PatientData::getBscansNumber(){
    return this->bscansNumber;
}

void PatientData::setBscansNumber(int number){
    this->bscansNumber = number;
}

int PatientData::getBscansNumberAll()
{
    return this->bscansNumberAll;
}

void PatientData::setBscansNumberAll(int number)
{
    this->bscansNumberAll = number;
}

double PatientData::getVoxelWidth(){
    return this->voxelWidth;
}

void PatientData::setVoxelWidth(float width){
    this->voxelWidth = width;
}

double PatientData::getVoxelHeight(){
    return this->voxelHeight;
}

void PatientData::setVoxelHeight(float height){
    this->voxelHeight = height;
}

double PatientData::getVoxelDepth(){
    return this->voxelDepth;
}

void PatientData::setVoxelDepth(float depth){
    this->voxelDepth = depth;
}

QStringList PatientData::getImageFileList(){
    return this->imageFileList;
}

void PatientData::setImageFileList(QStringList newList){
    this->imageFileList = newList;
}

void PatientData::setOCTdata(QImage bscan, int bscanNumber){
    int value = 0;
    for (int x=0; x < bscan.width(); x++){
        for (int y=0; y < bscan.height(); y++){
            value = QColor::fromRgb(bscan.pixel(x,y)).red();
            this->octdata[bscanNumber][y][x] = value;
        }
    }

}

void PatientData::resetFlatDifferences(){
    // clear memory
    this->flatDifferences.clear();
    this->flatDifferencesRPE.clear();

    // create empty matrix
    QList<int> row;
    row.reserve(this->bscanWidth);
    for (int i=0; i < this->bscanWidth; i++){
        row.append(-1);
    }
    for (int j=0; j < this->bscansNumber; j++){
        this->flatDifferences.insert(j, row);
        this->flatDifferencesRPE.insert(j,row);
    }
}

void PatientData::setFlatDifferences(int bscanNumber, QList<int> flatDiff){
    this->flatDifferences[bscanNumber] = flatDiff;
}

QList<int> PatientData::getFlatDifferences(int bscanNumber){
    return this->flatDifferences.at(bscanNumber);
}

QList<int> PatientData::getFlatDifferencesNormal(int bscanNormalNumber){
    QList<int> diffNormal;
    for (int i=0; i < this->bscansNumber; i++){
        diffNormal.append(getFlatDifference(i,bscanNormalNumber));
    }
    return diffNormal;
}

int PatientData::getFlatDifference(int bscanNumber, int bscanColumn){
    int diff = 0;

    diff = this->flatDifferences[bscanNumber][bscanColumn];

    return diff;
}

void PatientData::calculateFlatDifferencesRPE(int bscanNumber){
    // get RPE line
    QList<int> differences;
    QList<QPoint> rpeLine = this->Bscans[bscanNumber].layers[CHR].getPoints(0,this->bscanWidth-1);

    int bottom_val = 320+160;

    for (int c=0; c < this->bscanWidth; c++){
        if (rpeLine[c].y() != -1)
            differences.append(bottom_val - rpeLine[c].y());
        else
            differences.append(0);
    }

    this->flatDifferencesRPE[bscanNumber] = differences;
}

void PatientData::calculateFlatDifferencesNormalRPE(int bscanNormalNumber){
    int bottom_val = 320+160;

    for (int c=0; c < this->bscansNumber; c++){
        QPoint rpeValue = this->Bscans[c].layers[CHR].getPoint(bscanNormalNumber);
        if (rpeValue.y() != -1)
            this->flatDifferencesRPE[c][bscanNormalNumber] = bottom_val - rpeValue.y();
        else
            this->flatDifferencesRPE[c][bscanNormalNumber] = 0;
    }
}

QList<int> PatientData::getFlatDifferencesRPE(int bscanNumber){
    return this->flatDifferencesRPE.at(bscanNumber);
}

QList<int> PatientData::getFlatDifferencesNormalRPE(int bscanNormalNumber){
    QList<int> diffNormal;
    for (int i=0; i < this->bscansNumber; i++){
        diffNormal.append(getFlatDifferenceRPE(i, bscanNormalNumber));
    }
    return diffNormal;
}

int PatientData::getFlatDifferenceRPE(int bscanNumber, int bscanColumn){
    int diff = 0;
    diff = this->flatDifferencesRPE[bscanNumber][bscanColumn];
    return diff;
}

// General Exam Data ----------------------------------------------------------
QString PatientData::getSnOL(){
    return this->snOL;
}

void PatientData::setSnOL(QString newSnOL){
    this->snOL = newSnOL;
}

QString PatientData::getSnOP(){
    return this->snOP;
}

void PatientData::setSnOP(QString newSnOP){
    this->snOP = newSnOP;
}

QString PatientData::getVisOL(){
    return this->visOL;
}

void PatientData::setVisOL(QString newVisOL){
    this->visOL = newVisOL;
}

QString PatientData::getVisOP(){
    return this->visOP;
}

void PatientData::setVisOP(QString newVisOP){
    this->visOP = newVisOP;
}

QString PatientData::getMchOL(){
    return this->mchOL;
}

void PatientData::setMchOL(QString newMchOL){
    this->mchOL = newMchOL;
}

QString PatientData::getMchOP(){
    return this->mchOP;
}

void PatientData::setMchOP(QString newMchOP){
    this->mchOP = newMchOP;
}

QString PatientData::getMcvOL(){
    return this->mcvOL;
}

void PatientData::setMcvOL(QString newMcvOL){
    this->mcvOL = newMcvOL;
}

QString PatientData::getMcvOP(){
    return this->mcvOP;
}

void PatientData::setMcvOP(QString newMcvOP){
    this->mcvOP = newMcvOP;
}

Pathologies PatientData::getPathologyOP(){
    return this->pathologyOP;
}

void PatientData::setPathologyOP(Pathologies newPathologyOP){
    this->pathologyOP = newPathologyOP;
}

Pathologies PatientData::getPathologyOL(){
    return this->pathologyOL;
}

void PatientData::setPathologyOL(Pathologies newPathologyOL){
    this->pathologyOL = newPathologyOL;
}


// Amsler image ---------------------------------------------------------------
QList<AmslerDist> PatientData::getAmslerDistList(QString eye){
    QList<AmslerDist> dist;

    if (eye == "R")
        dist = this->amslerR.distortions;
    else if (eye == "L")
        dist = this->amslerL.distortions;

    QList<int> listToRemove;
    for (int i=0; i < dist.count(); i++) {  // find those to remove
        if (dist[i].getId() == -1){
            listToRemove.append(i);
        }
    }
    foreach (int i, listToRemove) { // remove
        dist.removeAt(i);
    }
    for (int i=0; i < dist.count(); i++) {  // update id's
        dist[i].setId(i);
    }
    if (eye == "R")
        this->amslerR.distortions = dist;
    else if (eye == "L")
        this->amslerL.distortions = dist;

    return dist;
}

AmslerDist PatientData::getAmslerDist(QString eye, int id){
    AmslerDist dist;

    if (eye == "R")
        dist = this->amslerR.distortions[id];
    else if (eye == "L")
        dist = this->amslerL.distortions[id];

    return dist;
}

void PatientData::setAmslerData(QString eye, DistType dataType, int id, QList<QLine> lines, QPoint point, int width){

    AmslerDist newDist;

    switch (dataType) {
    case None:
        break;
    case Pen:
    case WhiteSpot:
    case BlackSpot:
        newDist = AmslerDist(dataType, lines, id);
        break;
    case Convex:
    case Concave:
    case RightSpiral:
    case LeftSpiral:
        newDist = AmslerDist(dataType, point, width, id);
        break;
    // TODO: other types...
    }

    if (eye == "R"){
        while (this->amslerR.distortions.count() < (id + 1)){
            this->amslerR.distortions.append(AmslerDist());
        }
        this->amslerR.distortions[id] = newDist;
    } else if (eye == "L"){
        while (this->amslerL.distortions.count() < (id + 1)){
            this->amslerL.distortions.append(AmslerDist());
        }
        this->amslerL.distortions[id] = newDist;
    }
}

void PatientData::deleteAmslerData(QString eye, int id){
    QList<AmslerDist> list;

    if (eye == "R"){
        list = this->amslerR.distortions;
    } else if (eye == "L"){
        list = this->amslerL.distortions;
    }

    list.removeAt(id);
    for (int i=0; i < list.count(); i++){
        list[i].setId(i);
    }

    if (eye == "R")
        this->amslerR.distortions = list;
    else if (eye == "L")
        this->amslerL.distortions = list;
}

void PatientData::addAmslerDist(QString eye, DistType type, QList<QLine> lines, QPoint point, int width){
    AmslerDist newDist;

    switch (type) {
    case Pen:
    case WhiteSpot:
    case BlackSpot:
        newDist = AmslerDist(type, lines, -1);
        break;
    case Convex:
    case Concave:
    case RightSpiral:
    case LeftSpiral:
        newDist = AmslerDist(type, point, width, -1);
    case None:
        break;
    // TODO: other dist...
    }

    if (eye == "R"){
        newDist.setId(this->amslerR.distortions.count());
        this->amslerR.distortions.append(newDist);
    } else if (eye == "L"){
        newDist.setId(this->amslerL.distortions.count());
        this->amslerL.distortions.append(newDist);
    }
}

void PatientData::decodeAmslerData(QString code, QString data){
    QStringList c = code.split("_");
    QString eye = c.at(1);
    QString id = c.at(2);
    if (c.count() > 3){
        DistType distType = decodeDistType(c.at(3));

        QList<QLine> linesList;
        QPoint point;
        int width = 0;

        switch (distType) {
        case Pen:
        case WhiteSpot:
        case BlackSpot:
        {
            QStringList lines = data.split(";");
            for (int j=0; j < lines.count(); j++){
                QStringList points = lines.at(j).split(",");
                if (points.at(0) != ""){
                    linesList.append(QLine(points.at(0).toInt(), points.at(1).toInt(), points.at(2).toInt(), points.at(3).toInt()));
                }
            }
            this->setAmslerData(eye, distType, id.toInt(), linesList, point, width);
        }
            break;
        case Convex:
        case Concave:
        case RightSpiral:
        case LeftSpiral:
        {
            QStringList list = data.split(";");
            QStringList coords = list.at(0).split(",");
            point = QPoint(coords.at(0).toInt(), coords.at(1).toInt());
            width = list.at(1).toInt();
            this->setAmslerData(eye, distType, id.toInt(), linesList, point, width);
        }
            break;
        // TODO: other types...
        case None:
            break;
        }
    }
}

QString PatientData::encodeAmslerData(AmslerDist dist, QString eye){
    QString s, temp;

    s = "amsler_" + eye + "_" + QString::number(dist.getId()) + "_" + encodeDistType(dist.getType()) + "=";
    switch (dist.getType()) {
    case Pen:
    case WhiteSpot:
    case BlackSpot:
    {
        QList<QLine> lines = dist.getLines();
        foreach (QLine l, lines) {
            temp += QString::number(l.x1()) + "," + QString::number(l.y1()) + "," + QString::number(l.x2()) + "," + QString::number(l.y2()) + ";";
        }
    }
        break;
    case Convex:
    case Concave:
    case RightSpiral:
    case LeftSpiral:
    {
        QPoint p = dist.getPoint();
        temp += QString::number(p.x()) + "," + QString::number(p.y()) + ";";
        temp += QString::number(dist.getWidth()) + ";";
    }
        break;
    // TODO: other types...
    case None:
        break;
    }
    s += temp;

    return s;
}

QString PatientData::getAmslerComment(QString eye){
    if (eye == "R")
        return this->amslerR.comment;
    else if (eye == "L")
        return this->amslerL.comment;
    else
        return "";
}

void PatientData::setAmslerComment(QString comment, QString eye){
    if (eye == "R")
        this->amslerR.comment = comment;
    else if (eye == "L")
        this->amslerL.comment = comment;
}


// Patient's Information -----------------------------------------------------
int PatientData::getEye(){
    return this->eye;
}

void PatientData::setEye(int newEye){
    this->eye = newEye;
}

QDate PatientData::getExamDate(){
    return this->examDate;
}

void PatientData::setExamDate(QDate newExamDate){
    this->examDate = newExamDate;
}

QTime PatientData::getExamTime(){
    return this->examTime;
}

void PatientData::setExamTime(QTime newExamTime){
    this->examTime = newExamTime;
}

int PatientData::getGender(){
    return this->gender;
}

void PatientData::setGender(int newGender){
    this->gender = newGender;
}

int PatientData::getAge(){
    return this->age;
}

void PatientData::setAge(int newAge){
    this->age = newAge;
}

QDate PatientData::getBirthDate(){
    return this->birthDate;
}

void PatientData::setBirthDate(QDate newDate){
    this->birthDate = newDate;
}

QString PatientData::getFirstName(){
    return this->firstName;
}

void PatientData::setFirstName(QString newFirstName){
    this->firstName = newFirstName;
}

QString PatientData::getLastName(){
    return this->lastName;
}

void PatientData::setLastName(QString newLastName){
    this->lastName = newLastName;
}

OCTDevice PatientData::getOCTDevice(){
    return this->octDevice;
}

void PatientData::setOCTDevice(OCTDevice newOCTDevice){
    this->octDevice = newOCTDevice;
}
