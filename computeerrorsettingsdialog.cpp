#include "computeerrorsettingsdialog.h"
#include "ui_computeerrorsettingsdialog.h"

#include <QMessageBox>

const QString ComputeErrorSettingsDialog::examDataDirDef = "D:/Egnyte/Private/agis/Qt/Deployment/OCTAnnotate/examData/"; //currentDir.path().append("/examData/");
const QString ComputeErrorSettingsDialog::autoDirDef = "auto matlab/KKE 2016 study/None/chorzy_gradNew/";
const QString ComputeErrorSettingsDialog::algorithmDef = "_graphDenoiseNoneGradNew_cut0_trackILMandISOS";

ComputeErrorSettingsDialog::ComputeErrorSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ComputeErrorSettingsDialog)
{
    ui->setupUi(this);
    setDefaultValues();

    ui->examDataDirLEdit->setText(examDataDir);
    ui->autoDirLEdit->setText(autoDir);
    ui->algorithmLEdit->setText(algorithm);
}

ComputeErrorSettingsDialog::~ComputeErrorSettingsDialog()
{
    delete ui;
}

void ComputeErrorSettingsDialog::on_acceptButton_clicked()
{
    // check if data in form not empty
    bool ready = true;
    if (ui->examDataDirLEdit->text() == "")
        ready = false;
    if (ui->autoDirLEdit->text() == "")
        ready = false;
    if (ui->algorithmLEdit->text() == "")
        ready = false;
    if (!ready){
        QMessageBox msg;
        msg.setText("Ścieżki do plików nie mogą pozostać puste!");
        msg.setInformativeText("Proszę uzupełnić wszystkie pola.");
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setIcon(QMessageBox::Critical);
        msg.exec();
    } else {
        // save data
        this->examDataDir = ui->examDataDirLEdit->text();
        this->autoDir = ui->autoDirLEdit->text();
        this->algorithm = ui->algorithmLEdit->text();

        this->layers = fillLayerList();

        //if(saveSettingsToFile())
            this->accept();
    }
}

void ComputeErrorSettingsDialog::on_cancelButton_clicked()
{
    this->reject();
}


void ComputeErrorSettingsDialog::setDefaultValues(){
    examDataDir = examDataDirDef;
    autoDir = autoDirDef;
    algorithm = algorithmDef;
}


QString ComputeErrorSettingsDialog::getExamDataDir(){
    return this->examDataDir;
}

QString ComputeErrorSettingsDialog::getAutoDir(){
    return this->autoDir;
}

QString ComputeErrorSettingsDialog::getAlgorithm(){
    return this->algorithm;
}

QList<Layers> ComputeErrorSettingsDialog::getLayers(){
    return this->layers;
}

void ComputeErrorSettingsDialog::on_allLayersCBox_toggled(bool checked)
{
    if(checked){
        ui->pcvLayerCBox->setChecked(true);
        ui->ermLayerCBox->setChecked(true);
        ui->ilmLayerCBox->setChecked(true);
        ui->gclLayerCBox->setChecked(true);
        ui->iplLayerCBox->setChecked(true);
        ui->inlLayerCBox->setChecked(true);
        ui->oplLayerCBox->setChecked(true);
        ui->onlLayerCBox->setChecked(true);
        ui->elmLayerCBox->setChecked(true);
        ui->mezLayerCBox->setChecked(true);
        ui->iosLayerCBox->setChecked(true);
        ui->rpeLayerCBox->setChecked(true);
        ui->chrLayerCBox->setChecked(true);
    } else {
        ui->pcvLayerCBox->setChecked(false);
        ui->ermLayerCBox->setChecked(false);
        ui->ilmLayerCBox->setChecked(false);
        ui->gclLayerCBox->setChecked(false);
        ui->iplLayerCBox->setChecked(false);
        ui->inlLayerCBox->setChecked(false);
        ui->oplLayerCBox->setChecked(false);
        ui->onlLayerCBox->setChecked(false);
        ui->elmLayerCBox->setChecked(false);
        ui->mezLayerCBox->setChecked(false);
        ui->iosLayerCBox->setChecked(false);
        ui->rpeLayerCBox->setChecked(false);
        ui->chrLayerCBox->setChecked(false);
    }
}

QList<Layers> ComputeErrorSettingsDialog::fillLayerList(){
    QList<Layers> newList;

    if(ui->pcvLayerCBox->isChecked())
        newList.append(PCV);
    if(ui->ermLayerCBox->isChecked())
        newList.append(ERM);
    if(ui->ilmLayerCBox->isChecked())
        newList.append(ILM);
    if(ui->gclLayerCBox->isChecked())
        newList.append(GCL);
    if(ui->iplLayerCBox->isChecked())
        newList.append(IPL);
    if(ui->inlLayerCBox->isChecked())
        newList.append(INL);
    if(ui->oplLayerCBox->isChecked())
        newList.append(OPL);
    if(ui->onlLayerCBox->isChecked())
        newList.append(ONL);
    if(ui->elmLayerCBox->isChecked())
        newList.append(ELM);
    if(ui->mezLayerCBox->isChecked())
        newList.append(MEZ);
    if(ui->iosLayerCBox->isChecked())
        newList.append(IOS);
    if(ui->rpeLayerCBox->isChecked())
        newList.append(RPE);
    if(ui->chrLayerCBox->isChecked())
        newList.append(CHR);

    return newList;
}
