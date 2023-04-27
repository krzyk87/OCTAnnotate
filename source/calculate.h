#ifndef CALCULATE_H
#define CALCULATE_H

#include <QtCore>
#include <QObject>
#include "patientdata.h"
#include "scan.h"

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
    void imageEnhancement(QImage *img, float contrast, int brightness);
    void setFolderList(QList<QString> list);
    void setupMatrixes(OCTDevice device);
    QImage calculateFundus(QList< QList< QList<int> > > octData);

private:
    QList<QString> folderList;
    int folderNumber;
    int folderCount;

    int voxelDepth;
    int bscanWidth;
    int bscanHeight;
    int bscansCount;

    PatientData patientData;

    struct Scan{
        QList<double> bscan;
    };

    QList<QString> initials;
    QList<int> age;
};

#endif // CALCULATE_H
