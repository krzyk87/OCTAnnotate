#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QtSql>

class DbManager
{
public:
    DbManager(const QString& path);
    ~DbManager();

    bool isOpen() const;
    void closeDatabase();
    QSqlDatabase getDatabase();

    bool addNewPatient(QString last_name, QString first_name, QDate birth_date, int gender, QString pathologyOD, QString pathologyOS, QString file_no, QString notes);
    bool editPatient(int id, QString last_name, QString first_name, QDate birth_date, int gender, QString pathologyOD, QString pathologyOS, QString file_no, QString notes);
    bool deletePatient(int patientID);
    int getPatientID(QString last_name, QString first_name);
    int getPatientAge(int patientID);
    int getPatientGender(int patientID);
    void calculatePatientAge(int patientID, QDateTime examDate);
    bool addNewScan(int patientID, QString eye, QDateTime scan_date, QString device, QString scan_type, QString scan_direction, QString scan_dimensions, double scan_qi, QString scan_name);
    int getScanID(QString examDirName);
    bool editScanHasAutoExplorer(int scanID, bool hasAutoExplorer);

signals:

public slots:

private:
    QSqlDatabase mydb;
};

#endif // DBMANAGER_H
