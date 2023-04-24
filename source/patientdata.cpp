#include "patientdata.h"
#include <QPainter>
#include <qmath.h>
#include <math.h>
#include <QDebug>

// ----------------------------------------------------------------------------
// AmslerDist class -----------------------------------------------------------
// ----------------------------------------------------------------------------

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

    this->bscansNumber = 141;
    this->bscansNumberAll = 144;
    this->bscanWidth = 385;
    this->bscanHeight = 640;
    this->voxelWidth = 7.0;
    this->voxelHeight = 7.0;
    this->voxelDepth = 2.0;
    this->octDevice = AVANTI;

    this->eye = 1; // OD
    this->areaUnit = 0.0;
    this->depthCoeff = 0.0;
    this->examDate = QDate(2015,1,1);

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

// Auto annotations -----------------------------------------------------------

// Virtual Map ----------------------------------------------------------------
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
    // clear virtual map memory

    // clear image flattening memory
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

QList<QList<QList<int> > > PatientData::getOCTdata()
{
    return this->octdata;
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
