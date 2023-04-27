#include "patientdata.h"
#include <QPainter>
#include <qmath.h>
#include <math.h>
#include <QDebug>

// ----------------------------------------------------------------------------
// PatientData class ----------------------------------------------------------
// ----------------------------------------------------------------------------
PatientData::PatientData()
{
    this->lastName = "";
    this->firstName = "";
    this->birthDate = QDate(1900,1,1);
    this->gender = 0; // female
    this->age = 0;
}

// General Exam Data ----------------------------------------------------------


// Patient's Information -----------------------------------------------------
int PatientData::getGender(){
    return this->gender;
}

void PatientData::setGender(int newGender){
    this->gender = newGender;
}

int PatientData::getAge(){
    return this->age;
}

void PatientData::setAge(int newAge){
    this->age = newAge;
}

QDate PatientData::getBirthDate(){
    return this->birthDate;
}

void PatientData::setBirthDate(QDate newDate){
    this->birthDate = newDate;
}

QString PatientData::getFirstName(){
    return this->firstName;
}

void PatientData::setFirstName(QString newFirstName){
    this->firstName = newFirstName;
}

QString PatientData::getLastName(){
    return this->lastName;
}

void PatientData::setLastName(QString newLastName){
    this->lastName = newLastName;
}
