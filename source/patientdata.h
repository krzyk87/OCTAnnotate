#ifndef PATIENTDATA_H
#define PATIENTDATA_H

#include <QString>
#include <QStringList>
#include <QDate>
#include <QList>
#include <QPoint>
#include <QImage>
#include <QMap>

enum OCTDevice{
    COPERNICUS = 0,
    AVANTI = 1
};

// AmslerDist -----------------------------------------------------------------

// Layer ----------------------------------------------------------------------

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
    void setExamDate(QDate newExamDate);
    QDate getExamDate();
    void setExamTime(QTime newExamTime);
    QTime getExamTime();
    void setOCTdata(QImage bscan, int bscanNumber);
    void setOCTdata(QList<QList<int> > bscan, int bscanNumber);
    QList<QList<QList<int> > > getOCTdata();
    QList<QList<int> > getOCTdata(int bscanNumber);
    QImage getImage(int bscanNumber);
    QImage getNormalImage(int normalImageNumber);

    // ratios
    void calculateRatios();
    double getAreaUnit();
    double getDepthCoeff();

    // annotations

    // auto annotations

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
    int eye;
    QDate examDate;
    QTime examTime;
    QList< QList< QList<int> > > octdata;   // image intensitites
    bool isLoaded;
    bool isBinary;

    // ratios
    double areaUnit;
    double depthCoeff;

    // annotations

    // VMT virtual map

    // statistical data

    // retina depth

    // fundus image
    QImage fundusImage;
};

#endif // PATIENTDATA_H
