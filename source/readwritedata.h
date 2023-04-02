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
    void addDirective(QString directive);
    void setShowMessage(bool show);
    void setDataSaveStrucure(QString structure);

private:
    void readOctExamData();
    void readOctExamFile();
    void readBinaryFile(QFile *dataFile, double *count, double *tasks);

    QDir *octDir;
    QFile *octFile;
    QList<QString> directives;
    PatientData *pData;
    bool showMessage;
    QString dataSaveStrucure;
};

#endif // READWRITEDATA_H