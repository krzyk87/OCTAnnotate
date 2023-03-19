#include "patientdata.h"
#include <QPainter>
#include <qmath.h>
#include <math.h>
#include <QDebug>

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

bool isPointSet(QPoint p){
    bool isSet = false;
    if ((p.x() != -1) && (p.y() != -1)){
        isSet = true;
    }
    return isSet;
}

QList<QPoint> computeLinePoints(QPoint p0, QPoint p1){
    QList<QPoint> list;

    int dx = qAbs(p1.x() - p0.x());
    int dy = qAbs(p1.y() - p0.y());

    int sx = 0;
    int sy = 0;

    if (p0.x() < p1.x())
        sx = 1;
    else
        sx = -1;
    if (p0.y() < p1.y())
        sy = 1;
    else
        sy = -1;
    int err = dx - dy;

    list.append(p0);

    int x = p0.x();
    int y = p0.y();

    while ((x != p1.x()) || (y != p1.y())){
        int e2 = 2 * err;
        if (e2 >= -dy){
            err = err - dy;
            x = x + sx;
        }
        if (e2 <= dx){
            err = err + dx;
            y = y + sy;
        }
        list.append(QPoint(x,y));
    }

    return list;
}


// ----------------------------------------------------------------------------
// AmslerDist class -----------------------------------------------------------
// ----------------------------------------------------------------------------

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

    QPoint p(-1,-1);
    this->points.reserve(bscanWidth);
    this->pointsAuto.reserve(bscanWidth);
    for (int i=0; i<bscanWidth; i++){
        this->points.append(p);
        this->pointsAuto.append(p);
    }

    this->anotated = false;
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

void Layer::resetPoints(){
    QPoint p(-1,-1);

    this->points.clear();

    this->points.reserve(this->bscanWidth);
    for (int i=0; i<this->bscanWidth; i++){
        this->points.append(p);
    }
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

    this->pointsAuto.reserve(this->bscanWidth);
    for (int i=0; i<this->bscanWidth; i++){
        this->pointsAuto.append(p);
    }
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

    this->imageFileList = QStringList();
    this->isLoaded = false;
    this->isBinary = false;

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

    this->manualAnnotations = false;
    this->autoAnnotations = false;

    this->fundusImage = QImage();
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

void PatientData::resetManualAnnotations(){
    QList<Layers> layerList = getAllLayers();
    for (int i=0; i<this->bscansNumber; i++){
        foreach (Layers layer, layerList) {
            this->Bscans[i].layers[(int)layer].resetPoints();
            this->Bscans[i].layers[(int)layer].countAnnotatedPixels();
        }
    }

    this->manualAnnotations = false;
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

// Virtual Map ----------------------------------------------------------------
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

    // reset octData
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

    // clear image flattening memory
    this->resetFlatDifferences();

    this->manualAnnotations = false;
    this->autoAnnotations = false;
}

bool PatientData::getIsLoaded() const
{
    return isLoaded;
}

void PatientData::setIsLoaded(bool value)
{
    isLoaded = value;
}

bool PatientData::getIsBinary() const
{
    return isBinary;
}

void PatientData::setIsBinary(bool value)
{
    isBinary = value;
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

void PatientData::setOCTdata(QList<QList<int> > bscan, int bscanNumber){
    int value = 0;
    for (int x=0; x < bscan.count(); x++){
        QList<int> column = bscan.at(x);

        for (int y=0; y < column.count(); y++){
            value = column.at(y);
            this->octdata[bscanNumber][y][x] = value;
        }
    }
}

QList<QList<int> > PatientData::getOCTdata(int bscanNumber)
{
    QList<QList<int> > img = this->octdata.at(bscanNumber);
    return img;
}

QImage PatientData::getImage(int bscanNumber)
{
    QImage image(this->bscanWidth,this->bscanHeight,QImage::Format_Indexed8);
    QRgb value;
    int pixel_val;

    for (int b=0; b < this->bscanWidth; b++){
        for (int y=0; y < this->bscanHeight; y++){
            pixel_val = this->octdata[bscanNumber][y][b];
            value = qRgb(pixel_val,pixel_val,pixel_val);
            image.setColor(pixel_val,value);
            image.setPixel(b,y,pixel_val);
        }
    }

    return image;
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


// Amsler image ---------------------------------------------------------------

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
