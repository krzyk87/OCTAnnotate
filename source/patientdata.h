#ifndef PATIENTDATA_H
#define PATIENTDATA_H

#include <QString>
#include <QStringList>
#include <QDate>
#include <QList>
#include <QPoint>
#include <QImage>
#include <QMap>

// AmslerDist -----------------------------------------------------------------

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
    int getAge();
    void setAge(int newAge);

    // general exam


private:
    // patient's information
    QString lastName;
    QString firstName;
    QDate birthDate;
    int gender;  // 0 - female, 1 - male
    int age;

    // general exam

};

#endif // PATIENTDATA_H
