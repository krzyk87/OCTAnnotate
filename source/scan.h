#ifndef SCAN_H
#define SCAN_H

#include <QObject>
#include <QPoint>
#include <QDate>
#include <QTime>
#include <QImage>
#include <QLabel>
#include "enums.h"
#include "layer.h"
#include "octdata.h"
#include "qradiobutton.h"

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
    QColor getLayerColor(LayerName layer);
    void setLayerDisplayObjects(LayerName layer, QLabel *cLabel, QRadioButton *rButton);
    QRadioButton *getLayerRButton(LayerName layer); // <-- not working

    // auto annotations
    void resetAutoAnnotations();
    void setPointAuto(LayerName layer, int bscanNumber, int x, int z);
    int getLayerPointAuto(LayerName layer, int bscanNumber, int x);
    QList<QVector3D> getLayerPointsAuto(LayerName layer, int bscanNumber, int xMin, int xMax, bool isNormal);
    QVector<double> getLayerPointsAutoVector(LayerName layer, int bscanNumber, bool isNormal);
    bool hasAutoAnnotations();

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
    bool autoAnnotations;
};

#endif // SCAN_H
