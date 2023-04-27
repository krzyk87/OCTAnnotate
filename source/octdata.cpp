#include "octdata.h"
#include <QImage>

OCTData::OCTData(QObject *parent)
    : QObject{parent}
{

}

void OCTData::resetData(int width, int height, int depth)
{
    this->width = width;
    this->height = height;
    this->depth = depth;

    this->data.clear();

    QList<int> row;
    row.reserve(this->width);
    for (int x=0; x < this->width; x++){
        row.append(0);
    }
    QList< QList<int> > section;
    section.reserve(this->height);
    for (int y=0; y < this->height; y++){
        section.append(row);
    }
    this->data.reserve(this->depth);
    for (int s=0; s < this->depth; s++){
        this->data.append(section);
    }
}

void OCTData::setData(QImage bscan, int bscanNumber)
{
    // TODO: check if data size is correct
    int value = 0;
    for (int x=0; x < bscan.width(); x++){
        for (int y=0; y < bscan.height(); y++){
            value = QColor::fromRgb(bscan.pixel(x,y)).red();
            this->data[bscanNumber][y][x] = value;
        }
    }
}

void OCTData::setData(QList<QList<int> > bscan, int bscanNumber)
{
    // TODO: check if data size is correct
    int value = 0;
    for (int x=0; x < bscan.count(); x++){
        QList<int> column = bscan.at(x);

        for (int y=0; y < column.count(); y++){
            value = column.at(y);
            this->data[bscanNumber][y][x] = value;
        }
    }
}

QList<QList<QList<int> > > OCTData::getData()
{
    return this->data;
}

QImage OCTData::getDataImage(int bscanNumber)
{
    QImage image(this->width, this->height, QImage::Format_Indexed8);
    QRgb value;
    int pixel_val;

    for (int b=0; b < this->width; b++){
        for (int y=0; y < this->height; y++){
            pixel_val = this->data[bscanNumber][y][b];
            value = qRgb(pixel_val,pixel_val,pixel_val);
            image.setColor(pixel_val,value);
            image.setPixel(b,y,pixel_val);
        }
    }
    return image;
}

QImage OCTData::getNormalDataImage(int normalImageNumber)
{
    QImage normalImage(this->depth, this->height, QImage::Format_Indexed8);
    QRgb value;
    int pixel_val;

    for (int b=0; b < this->depth; b++){
        for (int y=0; y < this->height; y++){
            pixel_val = this->data[b][y][normalImageNumber];
            value = qRgb(pixel_val,pixel_val,pixel_val);
            normalImage.setColor(pixel_val,value);
            normalImage.setPixel(b,y,pixel_val);
        }
    }
    return normalImage;
}
