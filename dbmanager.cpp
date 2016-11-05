#include "dbmanager.h"

DbManager::DbManager(const QString& path)
{
    mydb = QSqlDatabase::addDatabase("QSQLITE");
    mydb.setDatabaseName(path);

    if (!mydb.open()){
        qDebug() << "Error: connection with database fail";
    } else {
        qDebug() << "Database: connection ok";
    }
}

DbManager::~DbManager(){
    if (mydb.isOpen()){
        mydb.close();
    }
}

bool DbManager::isOpen() const{
    return mydb.isOpen();
}

void DbManager::closeDatabase(){
    mydb.close();
}

QSqlDatabase DbManager::getDatabase(){
    return mydb;
}

bool DbManager::addNewPatient(QString last_name, QString first_name, QDate birth_date, int gender, QString pathologyOD, QString pathologyOS, QString file_no, QString notes){
    bool success = false;

    QSqlQuery query;
    query.prepare("INSERT INTO patients (last_name, first_name, date_of_birth, gender, od, os, file_no, notes) VALUES (:last_name, :first_name, :date_of_birth, :gender, :od, :os, :file_no, :notes)");
    query.bindValue(":last_name", last_name);
    query.bindValue(":first_name", first_name);
    query.bindValue(":date_of_birth", birth_date.toString("yyyy-MM-dd"));
    query.bindValue(":gender", gender);
    query.bindValue(":od", pathologyOD);
    query.bindValue(":os", pathologyOS);
    query.bindValue(":file_no", file_no);
    query.bindValue(":notes", notes);

    if (query.exec()){
        success = true;
        qDebug() << "addNewPatient query: " << query.lastQuery();
    } else {
        qDebug() << "addNewPatient error: " << query.lastError();
    }

    return success;
}

bool DbManager::editPatient(int id, QString last_name, QString first_name, QDate birth_date, int gender, QString pathologyOD, QString pathologyOS, QString file_no, QString notes){
    bool success = false;

    QSqlQuery query;
    query.prepare("UPDATE patients SET last_name = :last_name, first_name = :first_name, date_of_birth = :date_of_birth, gender = :gender, od = :od, os = :os, file_no = :file_no, notes = :notes WHERE id = :id");
    query.bindValue(":last_name", last_name);
    query.bindValue(":first_name", first_name);
    query.bindValue(":date_of_birth", birth_date.toString("yyyy-MM-dd"));
    query.bindValue(":gender", gender);
    query.bindValue(":od", pathologyOD);
    query.bindValue(":os", pathologyOS);
    query.bindValue(":file_no", file_no);
    query.bindValue(":notes", notes);
    query.bindValue(":id", id);

    if (query.exec()){
        success = true;
        qDebug() << "editPatient query: " << query.lastQuery();
    } else {
        qDebug() << "editPatient error: " << query.lastError();
    }

    return success;
}

bool DbManager::deletePatient(int patientID){
    bool success = false;

    QSqlQuery query;
    query.prepare("DELETE FROM patients WHERE id = :patientID");
    query.bindValue(":patientID", patientID);

    if (query.exec()){
        success = true;
        qDebug() << "deletePatient query: " << query.lastQuery() << ", id: " << QString::number(patientID);
        query.prepare("DELETE FROM scans WHERE patient_id = :patientID");
        query.bindValue(":patientID", patientID);
        if (query.exec()){
            qDebug() << "scans for the patient deleted";
        } else {
            qDebug() << "Error: deleting scans for the patient!";
        }
    } else {
        qDebug() << "deletePatient error: " << query.lastError();
    }

    return success;
}

int DbManager::getPatientID(QString last_name, QString first_name){
    int patientID = -1;

    QSqlQuery query;
    query.prepare("SELECT * FROM patients WHERE last_name = :last_name AND first_name = :first_name");
    query.bindValue(":last_name", last_name);
    query.bindValue(":first_name", first_name);

    if (query.exec()){
        // get patient id
        if (query.next()){
            patientID = query.value("id").toInt();
            qDebug() << "patient exists... ";
        } else {
            qDebug() << "patient does not exist...";
        }
    } else {
        qDebug() << "getPatientID query failed!";
    }

    return patientID;
}

int DbManager::getPatientAge(int patientID){
    int patientAge = 0;

    QSqlQuery query;
    query.prepare("SELECT age FROM patients WHERE id = :patient_id");
    query.bindValue(":patient_id", patientID);

    if (query.exec()){
        if (query.next()){
            patientAge = query.value("age").toInt();
            qDebug() << "patient age fetched...";
        } else {
            qDebug() << "no age for patient...";
        }
    } else {
        qDebug() << "Error while geting patient age!";
    }

    return patientAge;
}

int DbManager::getPatientGender(int patientID){
    int patientGender = 0;

    QSqlQuery query;
    query.prepare("SELECT gender FROM patients WHERE id = :patient_id");
    query.bindValue(":patient_id", patientID);

    if (query.exec()){
        if (query.next()){
            patientGender = query.value("gender").toInt();
            qDebug() << "patient gender fetched...";
        } else {
            qDebug() << "no gender for patient...";
        }
    } else {
        qDebug() << "Error while geting patient gender!";
    }

    return patientGender;
}

void DbManager::calculatePatientAge(int patientID, QDateTime examDate){
    int patientAge = 0;
    QDate patientBirthDate;

    QSqlQuery query;
    query.prepare("SELECT date_of_birth FROM patients WHERE id = :patient_id");
    query.bindValue(":patient_id", patientID);

    if (query.exec()){
        if (query.next()){
            patientBirthDate = query.value("date_of_birth").toDate();
            qDebug() << "patient birth date fetched...";
            int patientBirthYear = patientBirthDate.toString("yyyy").toInt();
            int scanYear = examDate.toString("yyyy").toInt();
            patientAge = scanYear - patientBirthYear;
            query.prepare("UPDATE patients SET age=:age WHERE id = :patient_id");
            query.bindValue(":age", patientAge);
            query.bindValue(":patient_id", patientID);
            if (query.exec()){
                qDebug() << "Age calculated.";
            } else {
                qDebug() << "Error while updating patient age!";
            }
        } else {
            qDebug() << "no birth date for patient...";
        }
    } else {
        qDebug() << "Error while geting patient age!";
    }
}

bool DbManager::addNewScan(int patientID, QString eye, QDateTime scan_date, QString device, QString scan_type, QString scan_direction, QString scan_dimensions, double scan_qi, QString scan_file_path, QString scan_folder_path){
    bool success = false;

    QSqlQuery query;
    query.prepare("INSERT INTO scans (patient_id, eye, date, device, series, scan_type, scan_direction, scan_dimensions, scan_qi, scan_file_path, scan_folder_path) VALUES (:patientID, :eye, :scan_date, :device, :series, :scan_type, :scan_direction, :scan_dimensions, :scan_qi, :scan_file_path, :scan_folder_path)");
    query.bindValue(":patientID", patientID);
    query.bindValue(":eye", eye);
    query.bindValue(":scan_date", scan_date.toString("yyyy-MM-dd hh:mm:ss.000"));
    query.bindValue(":device", device);
    query.bindValue(":series", "Seria 4");
    query.bindValue(":scan_type", scan_type);
    query.bindValue(":scan_direction", scan_direction);
    query.bindValue(":scan_dimensions", scan_dimensions);
    query.bindValue(":scan_qi", scan_qi);
    query.bindValue(":scan_file_path", scan_file_path);
    query.bindValue(":scan_folder_path", scan_folder_path);

    if (query.exec()){
        success = true;
        qDebug() << "new scan added!";
    } else {
        qDebug() << "addNewScan error: " << query.lastError();
    }

    return success;
}

int DbManager::getScanID(QString examDirName){
    int scanID = -1;

    QSqlQuery query;
    query.prepare("SELECT * FROM scans WHERE scan_folder_path = :examDirName");
    query.bindValue(":examDirName", examDirName);

    if (query.exec()){
        if (query.next()){
            scanID = query.value("id").toInt();
            qDebug() << "Scan exists!";
        } else {
            qDebug() << "Scan does not exist";
        }
    } else {
        qDebug() << "Error while searching for scan...";
    }

    return scanID;
}
