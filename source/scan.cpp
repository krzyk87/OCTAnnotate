#include "scan.h"
#include "functions.h"

Scan::Scan(QObject *parent)
    : QObject{parent}
{
    this->isLoaded = false;
    this->isBinary = false;
    this->scanCenter = QPoint(-1,-1);
    this->bscansNumber = 141;
    this->bscansNumberAll = 144;
    this->bscanWidth = 385;
    this->bscanHeight = 640;
    this->scanWidth = 7.0;
    this->scanHeight = 7.0;
    this->scanDepth = 2.0;
    this->octDevice = OCTDevice::AVANTI;

    this->calculateRatios();
    this->octData = new OCTData();
    this->eye = 1; // OD
    this->examDate = QDate(2015, 1, 1);

    this->fundusImage = QImage();

    this->manualAnnotations = false;
}

Scan::~Scan(){

}

// general scan information -------------------------------------------------
int Scan::getEye(){
    return this->eye;
}

void Scan::setEye(int newEye){
    this->eye = newEye;
}

QDate Scan::getExamDate(){
    return this->examDate;
}

void Scan::setExamDate(QDate newExamDate){
    this->examDate = newExamDate;
}

QTime Scan::getExamTime(){
    return this->examTime;
}

void Scan::setExamTime(QTime newExamTime){
    this->examTime = newExamTime;
}

OCTDevice Scan::getOCTDevice()
{
    return this->octDevice;
}

void Scan::setOCTDevice(OCTDevice newOCTDevice)
{
    this->octDevice = newOCTDevice;
    // TODO: initialize new data
}

bool Scan::getIsLoaded() const
{
    return this->isLoaded;
}

void Scan::setIsLoaded(bool value)
{
    this->isLoaded = value;
}

bool Scan::getIsBinary() const
{
    return this->isBinary;
}

void Scan::setIsBinary(bool value)
{
    this->isBinary = value;
}

// dimentions ----------------------------------------------------------------------
int Scan::getBscansNumber()
{
    return this->bscansNumber;
}

void Scan::setBscansNumber(int number)
{
    this->bscansNumber = number;
}

int Scan::getBscansNumberAll()
{
    return this->bscansNumberAll;
}

void Scan::setBscansNumberAll(int number)
{
    this->bscansNumberAll = number;
}

int Scan::getBscanWidth(){
    return this->bscanWidth;
}

void Scan::setBscanWidth(int width){
    this->bscanWidth = width;
}

int Scan::getBscanHeight(){
    return this->bscanHeight;
}

void Scan::setBscanHeight(int height){
    this->bscanHeight = height;
}

double Scan::getScanWidth(){
    return this->scanWidth;
}

void Scan::setScanWidth(double width){
    this->scanWidth = width;
}

double Scan::getScanHeight(){
    return this->scanHeight;
}

void Scan::setScanHeight(double height){
    this->scanHeight = height;
}

double Scan::getScanDepth(){
    return this->scanDepth;
}

void Scan::setScanDepth(double depth){
    this->scanDepth = depth;
}

QPoint Scan::getScanCenter()
{
    return this->scanCenter;
}

void Scan::setScanCenter(QPoint p)
{
    this->scanCenter = p;
}

// Ratios ---------------------------------------------------------------------
void Scan::calculateRatios(){
    this->areaUnit = static_cast<double>(this->scanWidth * this->scanHeight) / static_cast<double>((this->bscanWidth - 1) * (this->bscansNumber - 1));
    this->voxelDepth = this->scanDepth * 1000 / static_cast<double>(this->bscanHeight - 1); // 1 px [um]
    this->voxelWidth = this->scanWidth * 1000 / static_cast<double>(this->bscanWidth - 1);
}

double Scan::getAreaUnit(){
    return this->areaUnit;
}

double Scan::getDepthCoeff(){
    return this->voxelDepth;
}

// OCT Exam Data --------------------------------------------------------------
void Scan::resetBscansData()
{
    // clear annotations memory
    this->layers.clear();
    QList<Layers> layerList = getAllLayers();
    foreach (Layers layer, layerList){
        layers.append(new Layer(this->bscanWidth, this->bscansNumber, static_cast<int>(layer)));
    }

    // reset octData
    this->octData->resetData(this->bscanWidth, this->bscanHeight, this->bscansNumber);

    this->calculateRatios();
    this->scanCenter = QPoint(this->bscanWidth/2, this->bscansNumber/2);

    // clear image flattening memory
}

void Scan::setOCTdata(QImage bscan, int bscanNumber){
    this->octData->setData(bscan, bscanNumber);
}

void Scan::setOCTdata(QList<QList<int> > bscan, int bscanNumber){
    this->octData->setData(bscan, bscanNumber);
}

QList<QList<QList<int> > > Scan::getOCTdata()
{
    return this->octData->getData();
}

QImage Scan::getImage(int bscanNumber)
{
    return this->octData->getDataImage(bscanNumber);
}

QImage Scan::getNormalImage(int normalImageNumber)
{
    return this->octData->getNormalDataImage(normalImageNumber);
}

// Fundus Image ---------------------------------------------------------------
QImage Scan::getFundusImage(){
    return this->fundusImage;
}

void Scan::setFundusImage(QImage newImage){
    this->fundusImage = newImage;
}

// Layers ----------------------------------------------------------------
void Scan::resetManualAnnotations(){
    QList<Layers> layerList = getAllLayers();
    for (int i=0; i<this->bscansNumber; i++){
        foreach (Layers layer, layerList) {
            this->layers[static_cast<int>(layer)]->resetPoints();
        }
    }

    this->manualAnnotations = false;
}

void Scan::setPoint(Layers layer, int bscanNumber, int x, int z)
{
    this->layers[static_cast<int>(layer)]->setPoint(bscanNumber, x, z);
    this->manualAnnotations = true;
}
