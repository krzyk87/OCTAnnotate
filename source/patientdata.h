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

enum DistType{
    None = -1,
    Pen = 0,
    WhiteSpot = 1,
    BlackSpot = 2,
    Convex = 3,
    Concave = 4,
    RightSpiral = 5,
    LeftSpiral = 6
};

DistType decodeDistType(QString type);
QString encodeDistType(DistType type);

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
QList<Layers> getLayersForError();

enum OCTDevice{
    COPERNICUS = 0,
    AVANTI = 1
};

enum Pathologies{
    HEALTHY = 0,
    VMA = 1,
    VMT = 2,
    ERMP = 3,
    IMH = 4,
    UNDEFINED = 5
};

Pathologies decodePathology(QString pathology);
QString encodePathology(Pathologies pathology);

bool isPointSet(QPoint p);
QList<QPoint> computeLinePoints(QPoint p0, QPoint p1);

// AmslerDist -----------------------------------------------------------------
class AmslerDist{
public:
    AmslerDist();
    AmslerDist(DistType type, QList<QLine> lines, int id);
    AmslerDist(DistType type, QPoint point, int width, int id);

    void setType(DistType newType);
    DistType getType();

    void setLines(QList<QLine> newlines);
    void addLine(QLine newline);
    QList<QLine> getLines();

    void setPoint(QPoint newPoint);
    QPoint getPoint();

    void setWidth(int newWidth);
    int getWidth();

    void setId(int id);
    int getId();

private:
    int id;
    DistType distType;
    QList<QLine> lines;
    QPoint point;
    int width;
};

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
    void fillContactArea(QList<QPoint> pointsILM);
    void fillFromILM(QList<QPoint> pointsILM);
    void fillStraight();
    void resetPoints();

    void setPointAuto(QPoint p);
    void setPointsAuto(QList<QPoint> pointsList);
    QPoint getPointAuto(int x);
    QList<QPoint> getPointsAuto();
    QList<QPoint> getPointsAuto(int xMin, int xMax);
    void resetPointsAuto();

    void computeError();
    int getErrorSum();
    int getErrorSquareSum();
    int getErrorCount();
    int getErrorCountAll();
    QList<int> getErrorThVector();

    double getErrorAvg();
    double getErrorMSE();
    double getErrorDev();
    double getErrorPSNR();

private:
    Layers layer;
    int index;
    QString name;
    int bscanWidth;

    QList<QPoint> points;
    QList<QPoint> pointsAuto;
    QList<int> error;
    QList<int> errorTh;

    int errorSum;
    int errorSquareSum;
    int errorCount;
    int errorCountAll;
    double errorAvg;
    double errorMSE;
    double errorDev;
    double errorPSNR;

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
    QString getVisOP();
    void setVisOP(QString newVisOP);
    QString getVisOL();
    void setVisOL(QString newVisOL);
    QString getSnOP();
    void setSnOP(QString newSnOP);
    QString getSnOL();
    void setSnOL(QString newSnOL);
    QString getMcvOP();
    void setMcvOP(QString newMcvOP);
    QString getMcvOL();
    void setMcvOL(QString newMcvOL);
    QString getMchOP();
    void setMchOP(QString newMchOP);
    QString getMchOL();
    void setMchOL(QString newMchOL);
    Pathologies getPathologyOP();
    void setPathologyOP(Pathologies newPathologyOP);
    Pathologies getPathologyOL();
    void setPathologyOL(Pathologies newPathologyOL);

    // amsler image
    QList<AmslerDist> getAmslerDistList(QString eye);
    AmslerDist getAmslerDist(QString eye, int id);
    void setAmslerData(QString eye, DistType dataType, int id, QList<QLine> lines, QPoint point, int width);
    void addAmslerDist(QString eye, DistType type, QList<QLine> line, QPoint point, int width);
    void deleteAmslerData(QString eye, int id);
    void decodeAmslerData(QString code, QString data);
    QString encodeAmslerData(AmslerDist dist, QString eye);
    QString getAmslerComment(QString eye);
    void setAmslerComment(QString comment, QString eye);

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
    void fillContactArea(int bscanNumber = -1);
    void fillLayerFromILM(Layers layer, int bscanNumber = -1);
    void fillLayerStraight(Layers layer, int bscanNumber = -1);
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
    void computeLayerError(Layers layer);
    void computeErrorAll();
    void setErrorAll(double avg, double dev, double mse, double rmse, double psnr, double proc);
    void setErrorLayer(Layers layer, double avg, double dev, double mse, double rmse, double psnr, double proc);
    //int getErrorValue(int x, int y, Layers layer = NONE, QString unit = "");
    double getLayerErrorAvg(Layers layer, QString unit = "");
    double getLayerErrorMSE(Layers layer, QString unit = "");
    double getLayerErrorDev(Layers layer, QString unit = "");
    double getLayerErrorPSNR(Layers layer, QString unit = "");
    double getLayerErrorProc(Layers layer);
    double getLayerLineErrorAvg(Layers layer, int lineNumber, QString unit = "");
    double getLayerLineErrorDev(Layers layer, int lineNumber, QString unit = "");
    double getErrorAvg(QString unit = "");
    double getErrorMSE(QString unit = "");
    double getErrorDev(QString unit = "");
    double getErrorPSNR(QString unit = "");

    // virtual map
    void resetVirtualMap();
    void resetVirtualMapAuto();
    void resetVirtualMapError();
    void resetVirtualMapContours();
    void computeVirtualMap(Layers layer1, Layers layer2);
    void computeVirtualMapAuto(Layers layer1, Layers layer2);
    void computeVirtualMapContours();
    int findContourPoint(double height);
    QVector<QVector<double> > findContourLine(int startPoint);
    double getVirtualMapValue(int x, int y, QString unit = "");
    double getVirtualMapAutoValue(int x, int y, QString unit = "");
    QVector<double> getContourTData(int contourID);
    QVector<double> getContourXData(int contourID);
    QVector<double> getContourYData(int contourID);
    void computeVirtualMapError();
    double getVirtualMapErrorAvg(QString unit = "");
    double getVirtualMapErrorDev(QString unit = "");
    double getVirtualMapLineErrorAvg(int lineNumber, QString unit = "");
    double getVirtualMapLineErrorDev(int lineNumber, QString unit = "");

    // virtual map statistics - contact area
    void setContactThreshold(double threshold);
    void calculateContactArea(double diameter);
    double getContactAreaCF();
    double getContactAreaIM();
    double getContactAreaOM();
    double calculateDistance(QPoint p1, QPoint p2, double dx = 0, double dy = 0);

    // virtual map statistics - circular profile curve
    void calculateCircProfile(double diameter);
    QList<double> getCircProfileCF();
    QList<double> getCircProfileIM();
    QList<double> getCircProfileOM();

    // virtual map statistics - volume
    void calculateVolumeGrid();
    QList<double> getVolumeGrid();

    // retina depth
    void resetRetinaThicknessMap();
    void computeRetinaThicknessMap();
    double getRetinaThicknessMapValue(int x, int y, QString unit = "");
    void calculateRetinaDepth();
    double getRetinaDepth();

    // retina volume
    void calculateRetinaVolume();
    double getRetinaVolume();
    void calculatePreretinalVolume();
    double getPreretinalVolume();
    void calculateFoveaPitVolume();
    double getFoveaPitVolume();
    void computeFoveaPitContour();
    QList<QPoint> getFoveaPitContour();
    QList<QPoint> getFoveaPitContour2();

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
    QString visOP;
    QString visOL;
    QString snOP;
    QString snOL;
    QString anterior;
    QString posterior;
    QString otherDisorders;
    QString mcvOP;
    QString mcvOL;
    QString mchOP;
    QString mchOL;
    Pathologies pathologyOP;
    Pathologies pathologyOL;

    // amsler image
    struct Amsler{
        QList<AmslerDist> distortions;
        QString comment;
    };
    Amsler amslerR;
    Amsler amslerL;

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
    QList<double> layersErrorAvg;
    QList<double> layersErrorDev;
    QList<double> layersErrorMSE;
    QList<double> layersErrorRMSE;
    QList<double> layersErrorPSNR;
    QList<double> layersErrorProc;
    double errorAvg;
    double errorDev;
    double errorMSE;
    double errorRMSE;
    double errorPSNR;
    double errorProc;

    bool manualAnnotations;
    bool autoAnnotations;

    // VMT virtual map
    QMap<int, QList<int> > virtualMap;
    QMap<int, QList<int> > virtualMapAuto;
    QMap<int, QList<int> > virtualMapError;
    QList<double> virtualMapLineErrorAvg;
    QList<double> virtualMapLineErrorDev;
    double virtualMapErrorAvg;
    double virtualMapErrorDev;
    struct Contour{
        double height;
        QVector<double> t;
        QVector<double> keys;
        QVector<double> values;
    };
    QList<Contour> contours;
    QVector<bool> checkList;

    // statistical data
    double contactAreaCF;
    double contactAreaIM;
    double contactAreaOM;
    double contactThreshold;

    QList<double> circProfileCF;
    QList<double> circProfileIM;
    QList<double> circProfileOM;

    QList<double> volumeGrid;

    // retina depth
    QMap<int, QList<int> > retinaThicknessMap;
    double retinaDepth;
    double retinaVolume;
    double preretinalVolume;
    double foveaPitVolume;
    QList<QPoint> foveaPitContour;
    QList<QPoint> foveaPitContour2;

    // fundus image
    QImage fundusImage;
};

#endif // PATIENTDATA_H
