#include "newpatientdialog.h"
#include "ui_newpatientdialog.h"

#include <QMessageBox>

NewPatientDialog::NewPatientDialog(QWidget *parent, QSqlRecord record, int gender) :
    QDialog(parent),
    ui(new Ui::NewPatientDialog)
{
    ui->setupUi(this);
    if (record.isEmpty()){
        ui->odp_cavri_RButton->setChecked(true);
        ui->odp_undef_RButton->setChecked(true);
        ui->osp_cavri_RButton->setChecked(true);
        ui->osp_undef_RButton->setChecked(true);
    } else {
        ui->lastNameLEdit->setText(record.value("last_name").toString());
        ui->firstNameLEdit->setText(record.value("first_name").toString());
        ui->birthDateEdit->setDate(record.value("date_of_birth").toDate());
        ui->genderCBox->setCurrentIndex(gender-1);
        ui->fileNoLEdit->setText(record.value("file_no").toString());
        ui->notesTEdit->setText(record.value("notes").toString());

        QString pOD = record.value("od").toString();
        QString pOS = record.value("os").toString();
        if (pOD.length() > 0){
            if (pOD == "---"){
                ui->odp_cavri_RButton->setChecked(true);
                ui->odp_none_RButton->setChecked(true);
            } else if (pOD == "other"){
                ui->odp_cavri_RButton->setChecked(true);
                ui->odp_other_RButton->setChecked(true);
            } else {
                ui->odp_cavri_RButton->setChecked(true);
                if (pOD.contains("VMA")){
                    ui->odp_vma_CBox->setChecked(true);
                }
                if (pOD.contains("VMT")){
                    ui->odp_vmt_CBox->setChecked(true);
                }
                if (pOD.contains("ERM")){
                    ui->odp_erm_CBox->setChecked(true);
                }
                if (pOD.contains("MH")){
                    ui->odp_mh_CBox->setChecked(true);
                }
                if (pOD.contains("CNV")){
                    ui->odp_cnv_CBox->setChecked(true);
                }
            }
        } else {
            ui->odp_cavri_RButton->setChecked(true);
            ui->odp_undef_RButton->setChecked(true);
        }
        if (pOS.length() > 0){
            if (pOS == "---"){
                ui->osp_cavri_RButton->setChecked(true);
                ui->osp_none_RButton->setChecked(true);
            } else if (pOS == "other"){
                ui->osp_cavri_RButton->setChecked(true);
                ui->osp_other_RButton->setChecked(true);
            } else {
                ui->osp_cavri_RButton->setChecked(true);
                if (pOS.contains("VMA")){
                    ui->osp_vma_CBox->setChecked(true);
                }
                if (pOS.contains("VMT")){
                    ui->osp_vmt_CBox->setChecked(true);
                }
                if (pOS.contains("ERM")){
                    ui->osp_erm_CBox->setChecked(true);
                }
                if (pOS.contains("MH")){
                    ui->osp_mh_CBox->setChecked(true);
                }
                if (pOS.contains("CNV")){
                    ui->osp_cnv_CBox->setChecked(true);
                }
            }
        } else {
            ui->osp_cavri_RButton->setChecked(true);
            ui->osp_undef_RButton->setChecked(true);
        }
    }
}

NewPatientDialog::~NewPatientDialog()
{
    delete ui;
}

void NewPatientDialog::on_cancelButton_clicked()
{
    this->reject();
}

void NewPatientDialog::on_saveButton_clicked()
{
    QString lastName = ui->lastNameLEdit->text();//.toUpper();
    QString firstName = ui->firstNameLEdit->text();//.toUpper();
    int gender = ui->genderCBox->currentIndex();

    // check if all information given
    if ((lastName != "") && (firstName != "") && (gender < 2)){
        QMessageBox msg;
        msg.setText("Czy na pewno dodać nowego pacjenta?");
        msg.setInformativeText(lastName + " " + firstName + "\nData urodzenia: " + ui->birthDateEdit->date().toString());
        msg.setStandardButtons(QMessageBox::Cancel | QMessageBox::Save);
        msg.setIcon(QMessageBox::Question);
        int result = msg.exec();
        if (result == QMessageBox::Save)
            this->accept();
    } else {
        QMessageBox::critical(this, "Brak danych", "Nie podano wszystkich danych");
    }
}

QString NewPatientDialog::getLastName() const
{
    return ui->lastNameLEdit->text();//.toUpper();
}

QString NewPatientDialog::getFirstName() const
{
    return ui->firstNameLEdit->text();//.toUpper();
}

QDate NewPatientDialog::getBirthDate() const
{
    return ui->birthDateEdit->date();
}

int NewPatientDialog::getGender() const
{
    return ui->genderCBox->currentIndex()+1;
}

QString NewPatientDialog::getFileNo() const{
    return ui->fileNoLEdit->text();
}

QString NewPatientDialog::getNotes() const{
    return ui->notesTEdit->toPlainText();
}

QString NewPatientDialog::getPathologyOD() const{
    QString pathologyOD = "";

    if (ui->odp_none_RButton->isChecked()){
        pathologyOD = "---";
    } else if (ui->odp_cavri_RButton->isChecked()){
        if (ui->odp_vma_CBox->isChecked()){
            pathologyOD += "VMA";
        }
        if (ui->odp_vmt_CBox->isChecked()){
            if (pathologyOD.length() > 0)
                pathologyOD += ", ";
            pathologyOD += "VMT";
        }
        if (ui->odp_erm_CBox->isChecked()){
            if (pathologyOD.length() > 0)
                pathologyOD += ", ";
            pathologyOD += "ERM";
        }
        if (ui->odp_mh_CBox->isChecked()){
            if (pathologyOD.length() > 0)
                pathologyOD += ", ";
            pathologyOD += "MH";
        }
        if (ui->odp_cnv_CBox->isChecked()){
            if (pathologyOD.length() > 0)
                pathologyOD += ", ";
            pathologyOD += "CNV";
        }
    } else if (ui->odp_other_RButton->isChecked()){
        pathologyOD = "other";
    }

    return pathologyOD;
}

QString NewPatientDialog::getPathologyOS() const{
    QString pathologyOS = "";

    if (ui->osp_none_RButton->isChecked()){
        pathologyOS = "---";
    } else if (ui->osp_cavri_RButton->isChecked()){
        if (ui->osp_vma_CBox->isChecked()){
            pathologyOS += "VMA";
        }
        if (ui->osp_vmt_CBox->isChecked()){
            if (pathologyOS.length() > 0)
                pathologyOS += ", ";
            pathologyOS += "VMT";
        }
        if (ui->osp_erm_CBox->isChecked()){
            if (pathologyOS.length() > 0)
                pathologyOS += ", ";
            pathologyOS += "ERM";
        }
        if (ui->osp_mh_CBox->isChecked()){
            if (pathologyOS.length() > 0)
                pathologyOS += ", ";
            pathologyOS += "MH";
        }
        if (ui->osp_cnv_CBox->isChecked()){
            if (pathologyOS.length() > 0)
                pathologyOS += ", ";
            pathologyOS += "CNV";
        }
    } else if (ui->osp_other_RButton->isChecked()){
        pathologyOS = "other";
    }

    return pathologyOS;
}

void NewPatientDialog::on_odp_cavri_RButton_toggled(bool checked)
{
    if (checked){
        ui->odp_vma_CBox->setEnabled(true);
        ui->odp_vmt_CBox->setEnabled(true);
        ui->odp_erm_CBox->setEnabled(true);
        ui->odp_mh_CBox->setEnabled(true);
        ui->odp_cnv_CBox->setEnabled(true);
    } else {
        ui->odp_vma_CBox->setEnabled(false);
        ui->odp_vmt_CBox->setEnabled(false);
        ui->odp_erm_CBox->setEnabled(false);
        ui->odp_mh_CBox->setEnabled(false);
        ui->odp_cnv_CBox->setEnabled(false);
    }
}

void NewPatientDialog::on_osp_cavri_RButton_toggled(bool checked)
{
    if (checked){
        ui->osp_vma_CBox->setEnabled(true);
        ui->osp_vmt_CBox->setEnabled(true);
        ui->osp_erm_CBox->setEnabled(true);
        ui->osp_mh_CBox->setEnabled(true);
        ui->osp_cnv_CBox->setEnabled(true);
    } else {
        ui->osp_vma_CBox->setEnabled(false);
        ui->osp_vmt_CBox->setEnabled(false);
        ui->osp_erm_CBox->setEnabled(false);
        ui->osp_mh_CBox->setEnabled(false);
        ui->osp_cnv_CBox->setEnabled(false);
    }
}
