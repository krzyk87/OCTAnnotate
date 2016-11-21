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
    void readGeneralExamData();
    void readFileManualSegmentation(QFile *dataFile);

signals:
    void finished();
    void errorOccured(QString err);
    void processingData(double,QString);
    void returnNewDirectory(QString);
    void readingDataFinished(QString);
    void savingDataFinished(QString);

public slots:
    void process();

    void setDataObject(PatientData *patientData);
    void setDirectoryOct(QDir *octDir);
    void setDirectoryManual(QDir *mDir);
    void setDirectoryAuto(QDir *aDir);
    void setManualFilePath(QString mfPath);
    void setAutoFilePath(QString afPath);
    void addDirective(QString directive);
    void setShowMessage(bool show);
    void setDataSaveStrucure(QString structure);

private:
    void readOctExamData();
    void readFileAutoSegmentation(QFile *dataFile);

    void saveGeneralExamData();
    void saveManualSegmentationData();
    void saveAutoSegmentationData();

    QList<int> parseXmlVoxelSize(QXmlStreamReader& xml, bool isAuto = 0);
    void parseXmlSurfaceLines(QXmlStreamReader& xml, bool isAuto = 0);
    void parseUndefinedRegion(QXmlStreamReader& xml, bool isAuto = 0);

    void copyAutoAsManual(QList<Layers> layersList);

    QDir *octDir;
    QDir *manualDir;
    QDir *autoDir;
    QString manualFilePath;
    QString autoFilePath;
    QList<QString> directives;
    PatientData *pData;
    bool showMessage;
    QString dataSaveStrucure;
};

#endif // READWRITEDATA_H
