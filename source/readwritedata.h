#ifndef READWRITEDATA_H
#define READWRITEDATA_H

#include <QObject>
#include <QDir>
#include <QXmlStreamReader>

#include "patientdata.h"

class ReadWriteData : public QObject
{
    Q_OBJECT
public:
    explicit ReadWriteData(QObject *parent = 0);
    ~ReadWriteData();

    bool readPatientData();

signals:
    void finished();
    void errorOccured(QString err);
    void processingData(double,QString);
    void readingDataFinished(QString);

public slots:
    void process();

    void setDataObject(PatientData *patientData);
    void setDirectoryOct(QDir *dataDir);
    void setOctFile(QFile *dataFile);
    void setManualFilePath(QString mfPath);
    void addDirective(QString directive);
    void setShowMessage(bool show);
    void setDataSaveStrucure(QString structure);

private:
    void readOctSequence();
    void readOctFile();
    void readBinaryFile(QFile *dataFile, double *count, double *tasks);
    void readFundusImage(OCTDevice octDevice);

    QDir *octDir;
    QFile *octFile;
    QString manualFilePath;
    QList<QString> directives;
    PatientData *pData;
    bool showMessage;
    QString dataSaveStrucure;
};

#endif // READWRITEDATA_H
