#ifndef PATIENTDATA_H
#define PATIENTDATA_H

#include <QString>
#include <QStringList>
#include <QDate>
#include <QList>
#include <QPoint>
#include <QImage>
#include <QMap>

//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>

enum Layers{
    NONE = -1,
    PCV = 0,
    ERM = 1,
    ILM = 2,
    GCL = 3,
    IPL = 4,
    INL = 5,
    OPL = 6,
    ONL = 7,
    ELM = 8,
    MEZ = 9,
    IOS = 10,
    RPE = 11,
    CHR = 12
};

Layers decodeLayer(QString layer);
QString encodeLayer(Layers layer);
QList<Layers> getAllLayers();

enum OCTDevice{
    COPERNICUS = 0,
    AVANTI = 1
};

bool isPointSet(QPoint p);
QList<QPoint> computeLinePoints(QPoint p0, QPoint p1);

// AmslerDist -----------------------------------------------------------------

// Layer ----------------------------------------------------------------------
class Layer{
public:
    Layer(int bscanWidth, int index);

    void setPoint(QPoint p);
    void setPoints(QList<QPoint> pointsList);
    QPoint getPoint(int x);
    QList<QPoint> getPoints();
    QList<QPoint> getPoints(int xMin, int xMax);
    void setAnnotation(bool value);
    bool getAnnotation();
    void countAnnotatedPixels();
    bool checkForGaps();
    void smooth();
    void resetPoints();

    void setPointAuto(QPoint p);
    void setPointsAuto(QList<QPoint> pointsList);
    QPoint getPointAuto(int x);
    QList<QPoint> getPointsAuto();
    QList<QPoint> getPointsAuto(int xMin, int xMax);
    void resetPointsAuto();

private:
    Layers layer;
    int index;
    QString name;
    int bscanWidth;

    QList<QPoint> points;
    QList<QPoint> pointsAuto;

    bool anotated;
};

// PatientData ----------------------------------------------------------------
class PatientData
{
public:
    PatientData();

    // patient's information
    QString getLastName();
    void setLastName(QString newLastName);
    QString getFirstName();
    void setFirstName(QString newFirstName);
    QDate getBirthDate();
    void setBirthDate(QDate newDate);
    int getGender();
    void setGender(int newGender);
    int getEye();
    void setEye(int newEye);
    int getAge();
    void setAge(int newAge);

    // general exam

    // amsler image

    // oct exam
    bool getIsLoaded() const;
    void setIsLoaded(bool value);
    bool getIsBinary() const;
    void setIsBinary(bool value);
    void resetBscansData();
    QStringList getImageFileList();
    void setImageFileList(QStringList newList);
    int getBscansNumber();
    void setBscansNumber(int number);
    int getBscansNumberAll();
    void setBscansNumberAll(int number);
    int getBscanWidth();
    void setBscanWidth(int width);
    int getBscanHeight();
    void setBscanHeight(int height);
    double getVoxelWidth();
    void setVoxelWidth(float width);
    double getVoxelHeight();
    void setVoxelHeight(float height);
    double getVoxelDepth();
    void setVoxelDepth(float depth);
    OCTDevice getOCTDevice();
    void setOCTDevice(OCTDevice newOCTDevice);
    void setScanCenter(QPoint p);
    QPoint getScanCenter();
    void setExamDate(QDate newExamDate);
    QDate getExamDate();
    void setExamTime(QTime newExamTime);
    QTime getExamTime();
    void setOCTdata(QImage bscan, int bscanNumber);
    void setOCTdata(QList<QList<int> > bscan, int bscanNumber);
    QList<QList<int> > getOCTdata(int bscanNumber);
    QImage getImage(int bscanNumber);
    QImage getNormalImage(int normalImageNumber);
    void resetFlatDifferences();
    void setFlatDifferences(int bscanNumber, QList<int> flatDiff);
    QList<int> getFlatDifferences(int bscanNumber);
    QList<int> getFlatDifferencesNormal(int bscanNormalNumber);
    int getFlatDifference(int bscanNumber, int bscanColumn);
    void calculateFlatDifferencesRPE(int bscanNumber);
    void calculateFlatDifferencesNormalRPE(int bscanNormalNumber);
    QList<int> getFlatDifferencesRPE(int bscanNumber);
    QList<int> getFlatDifferencesNormalRPE(int bscanNormalNumber);
    int getFlatDifferenceRPE(int bscanNumber, int bscanColumn);

    // ratios
    void calculateRatios();
    double getAreaUnit();
    double getDepthCoeff();

    // annotations
    void setPoint(int bscanNumber, Layers layer, QPoint p);
    void setLayerPoints(int bscanNumber, QList<QPoint> pointsList, Layers layer, bool isNormal=false);
    QPoint getLayerPoint(int bscanNumber, Layers layer, int x);
    QList<QPoint> getLayerPoints(int bscanNumber, Layers layer);
    QList<QPoint> getLayerPoints(int bscanNumber, Layers layer, int xMin, int xMax, bool isNormal=false);
    void setLayerAnnotation(int bscanNumber, Layers layer, bool value);
    bool getLayerAnnotation(int bscanNumber, Layers layer);
    int getLayerAnnotatedCount(Layers layer);
    void countAnnotatedPixelsInLayer(int bscanNumber, Layers layer);
    void smoothLayer(Layers layer, int bscanNumber = -1);
    void resetManualAnnotations();
    bool hasManualAnnotations();

    // auto annotations
    void resetAutoAnnotations();
    bool hasAutoAnnotations();
    void setPointAuto(int bscanNumber, Layers layer, QPoint p);
    void setLayerPointsAuto(int bscanNumber, QList<QPoint> pointsList, Layers layer);
    QPoint getLayerPointAuto(int bscanNumber, Layers layer, int x);
    QList<QPoint> getLayerPointsAuto(int bscanNumber, Layers layer);
    QList<QPoint> getLayerPointsAuto(int bscanNumber, Layers layer, int xMin, int xMax);

    // virtual map

    // virtual map statistics - contact area
    double calculateDistance(QPoint p1, QPoint p2, double dx = 0, double dy = 0);

    // virtual map statistics - circular profile curve

    // virtual map statistics - volume

    // retina depth

    // retina volume

    // fundus image
    QImage getFundusImage();
    void setFundusImage(QImage newImage);

private:
    // patient's information
    QString lastName;
    QString firstName;
    QDate birthDate;
    int gender;  // 0 - female, 1 - male
    int age;

    // general exam

    // amsler image

    // oct exam
    QStringList imageFileList;
    int bscansNumber;
    int bscansNumberAll;
    int bscanWidth;
    int bscanHeight;
    double voxelWidth;
    double voxelHeight;
    double voxelDepth;
    OCTDevice octDevice;
    QPoint scanCenter;
    int eye;
    QDate examDate;
    QTime examTime;
    QList< QList< QList<int> > > octdata;
    QList< QList<int> > flatDifferences;
    QList< QList<int> > flatDifferencesRPE;
    bool isLoaded;
    bool isBinary;

    // ratios
    double areaUnit;
    double depthCoeff;

    // annotations
    struct Bscan{
        QList<Layer> layers;
    };
    QList<Bscan> Bscans;

    bool manualAnnotations;
    bool autoAnnotations;

    // VMT virtual map
    QVector<bool> checkList;

    // statistical data

    // retina depth

    // fundus image
    QImage fundusImage;
};

#endif // PATIENTDATA_H
