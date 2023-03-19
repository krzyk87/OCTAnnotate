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
