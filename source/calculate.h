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
    void averageErrorCalculated(double);
    void mseErrorCaluclated(double);

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
    void calculateError(PatientData *pData, bool etdrsOnly = false, bool multiscan = false);
    void calculateLayersDeviation(bool etdrsOnly = false);
    void orderVolumes(int folderNr);

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
    QList< RetinaLayers > error;

    QList<double> errorLayerPSNR;
    QList<double> errorLayerMSE; // list of layer errors
    QList<double> errorLayerRMSE;
    QList<double> errorLayerAvg;
    QList<double> errorLayerDev;
    QList<double> errorLayerProc;
    QList<double> errorLayerSum;
    QList<double> errorLayerSquareSum;
    QList<double> errorLayerSquareDiffSum;
    QList<double> errorLayerCount;
    QList<double> errorLayerCountProc;

    QList< QList<double> > errorFolderPSNR;
    QList< QList<double> > errorFolderMSE; // list of layer errors in folders
    QList< QList<double> > errorFolderRMSE;
    QList< QList<double> > errorFolderAvg;
    QList< QList<double> > errorFolderDev;
    QList< QList<double> > errorFolderProc;
    QList< QList<double> > errorFolderSum;
    QList< QList<double> > errorFolderSquareSum;
    QList< QList<double> > errorFolderSquareDiffSum;
    QList< QList<double> > errorFolderCount;
    QList< QList<double> > errorFolderCountProc;

    double errorCountAll;
    double errorCountProcAll;
    double sum;
    double squareSum;
    double squareDiffSum;

    double psnr;
    double mse;
    double rmse;
    double avg;
    double dev;
    double proc;

    QList<QPoint> centers;
    QList< QList<double> > volumes;
    QList<double> contactAreas;
    QList<double> retinaDepths;

    QList<QString> initials;
    QList<QString> vis;
    QList<QString> pathology;
    QList<int> age;
};

#endif // CALCULATE_H
