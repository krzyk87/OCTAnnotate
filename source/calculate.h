#ifndef CALCULATE_H
#define CALCULATE_H

#include <QtCore>
#include <QObject>
#include "patientdata.h"

class Calculate : public QObject
{
    Q_OBJECT
public:
    explicit Calculate(QObject *parent = 0);

    QString examDataDir;
    QString octDirPath;
    QString alg;
    QString autoFilePath;

signals:
    void finished();
    void errorOccured(QString err);
    void processingData(double, QString);

public slots:
    void process();
    void processStatistics();
    void imageEnhancement(QImage *img, float contrast, int brightness);
    QList<int> calculateFlatteningDifferences(QImage *img);
    QList<int> calculateFlatteningDifferences(QList<QList<int> > img);
    QImage flattenImage(QImage *img, QList<int> flatDiff);

    void setFolderList(QList<QString> list);
    void setLayers(QList<Layers> list);
    void setupMatrixes(OCTDevice device);
    void readManual(QString folder);
    void readAuto(QString folder);
    void calculateLayersDeviation(bool etdrsOnly = false);

private:
    QList<QString> folderList;
    int folderNumber;
    int folderCount;

    int voxelDepth;
    int bscanWidth;
    int bscanHeight;
    int bscansCount;

    QList<Layers> allLayers;
    int layersCount;

    PatientData patientData;

    struct Scan{
        QList<double> bscan;
    };
    struct sLayer{
        QList<Scan> map;
    };
    struct RetinaLayers{
        QList<sLayer> layers;
    };
    QList< RetinaLayers > manualSeg; // list of folders
    QList< RetinaLayers > autoSeg;

//    double sum;
//    double squareSum;
//    double squareDiffSum;

    QList<QPoint> centers;

    QList<QString> initials;
    QList<int> age;
};

#endif // CALCULATE_H
