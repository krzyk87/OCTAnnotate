#include "calculate.h"

#include <QDebug>
#include <QColor>

Calculate::Calculate(QObject *parent) :
    QObject(parent)
{

}

void Calculate::setFolderList(QList<QString> list){
    this->folderList = list;
    this->folderCount = list.count();
    this->folderNumber = 0;
}

void Calculate::setupMatrixes(OCTDevice device){
    this->voxelDepth = 2;

    switch (device){
    case COPERNICUS:
        this->bscanWidth = 800;
        this->bscanHeight = 1010;
        this->bscansCount = 100;
        break;
    case AVANTI:
        this->bscanWidth = 385;
        this->bscanHeight = 640;
        this->bscansCount = 141;
        break;
    }
}

void Calculate::imageEnhancement(QImage *img, float contrast, int brightness){

    // contrast enhancement
    int max = 0;    // TODO: wykorzystac OpenCV
    for (int i=0; i<img->width(); i++){
        for (int j=0; j<img->height(); j++){
            int value = QColor::fromRgb(img->pixel(i,j)).red();
            if (value > max)
                max = value;
        }
    }
    if (max == 0)
        max = 1;
    for (int i=0; i<img->width(); i++){
        for (int j=0; j<img->height(); j++){
            int value = QColor::fromRgb(img->pixel(i,j)).red();
            value = value * 255 / max * contrast + brightness;
            value = qBound(0,value,255);
            img->setColor(value,qRgb(value,value,value));
            img->setPixel(i,j,value);
        }
    }
}

QImage Calculate::calculateFundus(QList< QList< QList<int> > > octData)
{
    int scansNumber = octData.count();
    int scanHeight = octData[0].count();
    int scanWidth = octData[0][0].count();

    int maxSum = 1;
    QRgb rgbValue;
    QList< QList<int> > fundusTemp;
    fundusTemp.reserve(scansNumber);
    QList<int> fundusColumn;
    int sum, fundusValue;

    QImage fundus(scanWidth,scansNumber,QImage::Format_Indexed8);

    for (int y=0; y < scansNumber; y++){
        fundusColumn.clear();
        fundusColumn.reserve(scanWidth);

        for (int x=0; x < scanWidth; x++){

            sum = 0;
            for (int z=0; z < scanHeight; z++){
                sum += octData[y][z][x];
            }
            if (sum > maxSum){
                maxSum = sum;
            }
            fundusColumn.append(sum);
        }
        fundusTemp.append(fundusColumn);
    }
    for (int y=0; y <scansNumber; y++){
        for (int x=0; x < scanWidth; x++){
            fundusValue = fundusTemp[y][x]*255 / (maxSum);
            fundusValue = qBound(0,(int)fundusValue,255);
            rgbValue = qRgb(fundusValue,fundusValue,fundusValue);
            fundus.setColor(fundusValue,rgbValue);
            fundus.setPixel(x,y,fundusValue);
        }
    }
    return fundus;
}

