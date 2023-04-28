#ifndef SCAN_H
#define SCAN_H

#include <QObject>
#include <QPoint>
#include <QDate>
#include <QTime>
#include <QImage>
#include "enums.h"
#include "layer.h"
#include "octdata.h"

class Scan : public QObject
{
    Q_OBJECT
public:
    explicit Scan(QObject *parent = nullptr);
    ~Scan();

    // general scan information
    int getEye();
    void setEye(int newEye);
    QDate getExamDate();
    void setExamDate(QDate newExamDate);
    QTime getExamTime();
    void setExamTime(QTime newExamTime);
    OCTDevice getOCTDevice();
    void setOCTDevice(OCTDevice newOCTDevice);
    bool getIsLoaded() const;
    void setIsLoaded(bool value);
    bool getIsBinary() const;
    void setIsBinary(bool value);

    // dimentions
    int getBscansNumber();
    void setBscansNumber(int number);
    int getBscansNumberAll();
    void setBscansNumberAll(int number);
    int getBscanWidth();
    void setBscanWidth(int width);
    int getBscanHeight();
    void setBscanHeight(int height);
    double getScanWidth();
    void setScanWidth(double width);
    double getScanHeight();
    void setScanHeight(double height);
    double getScanDepth();
    void setScanDepth(double depth);
    QPoint getScanCenter();
    void setScanCenter(QPoint p);

    // ratios
    void calculateRatios();
    double getAreaUnit();
    double getDepthCoeff();

    // oct data
    void resetBscansData();
    void setOCTdata(QImage bscan, int bscanNumber);
    void setOCTdata(QList<QList<int> > bscan, int bscanNumber);
    QList<QList<QList<int> > > getOCTdata();
    QImage getImage(int bscanNumber);
    QImage getNormalImage(int normalImageNumber);

    // fundus image
    void setFundusImage(QImage newImage);
    QImage getFundusImage();

    // layers
    void resetManualAnnotations();
    void setPoint(LayerName layer, int bscanNumber, int x, int z);
    QList<QVector3D> getLayerPoints(LayerName layer, int bscanNumber, int xMin, int xMax, bool isNormal);
    QVector<double> getLayerPointsVector(LayerName layer, int bscanNumber, bool isNormal);

    // auto annotations

signals:

private:
    // oct exam info
    int bscansNumber;
    int bscansNumberAll;
    int bscanWidth;
    int bscanHeight;
    double scanWidth;
    double scanHeight;
    double scanDepth;
    OCTDevice octDevice;
    QPoint scanCenter;
    int eye;
    QDate examDate;
    QTime examTime;

    // ratios
    double areaUnit;
    double voxelDepth;
    double voxelWidth;

    // oct data
    bool isLoaded;
    bool isBinary;
    OCTData *octData;

    // fundus
    QImage fundusImage;

    // annotations
    QList<Layer*> layers;
    bool manualAnnotations;
};

#endif // SCAN_H
