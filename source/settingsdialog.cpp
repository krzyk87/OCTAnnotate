#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QMessageBox>
#include <QDir>
#include <QTextStream>
#include <QFileDialog>
#include <QSettings>

const QString SettingsDialog::pathDatabaseDef = "../database/patients.db";
const QString SettingsDialog::pathOctDataDef = "../octData/";
const QString SettingsDialog::pathExamDataDef = "../examData/";
const QString SettingsDialog::dataSaveStructureDef = "xml";

const bool SettingsDialog::showETDRSGridDef = true;
const bool SettingsDialog::showCenterOnBscanDef = true;
const bool SettingsDialog::showBscanOnErrorPlotDef = true;
const bool SettingsDialog::blockPCVDef = true;

SettingsDialog::SettingsDialog(QString configFilePath, QWidget *parent) : QDialog(parent), ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    configFileName = configFilePath;
    readSettingsFromFile();

    if(!QDir(pathOctData).exists())
        QDir(pathOctData).mkpath(pathOctData);
    if(!QDir(pathExamData).exists())
        QDir(pathExamData).mkpath(pathOctData);

    ui->pathDatabaseLEdit->setText(pathDatabase);
    ui->pathOctDataLEdit->setText(pathOctData);
    ui->pathExamDataLEdit->setText(pathExamData);

    ui->showETDRSGridCBox->setChecked(showETDRSGrid);
    ui->showCenterOnBscanCBox->setChecked(showCenterOnBscan);
    ui->showBscanOnErrorPlotCBox->setChecked(showBscanOnErrorPlot);
    ui->blockPCVCBox->setChecked(blockPCV);
    if (dataSaveStructure == "xml")
        ui->dataSaveStructureCBox->setCurrentIndex(0);
    else if (dataSaveStructure == "json")
        ui->dataSaveStructureCBox->setCurrentIndex(1);
    else    // txt
        ui->dataSaveStructureCBox->setCurrentIndex(2);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_acceptButton_clicked()
{
    // check if data in form not empty
    bool ready = true;
    if (ui->pathDatabaseLEdit->text() == "")
        ready = false;
    if (ui->pathOctDataLEdit->text() == "")
        ready = false;
    if (ui->pathExamDataLEdit->text() == "")
        ready = false;

    if (!ready){
        QMessageBox msg;
        msg.setText("File paths can not be empty!");
        msg.setInformativeText("Please fill all of the fields.");
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setIcon(QMessageBox::Critical);
        msg.exec();
    } else {
        // save data
        this->pathOctData = ui->pathOctDataLEdit->text();
        this->pathExamData = ui->pathExamDataLEdit->text();
        this->pathDatabase = ui->pathDatabaseLEdit->text();

        this->showETDRSGrid = ui->showETDRSGridCBox->isChecked();
        this->showCenterOnBscan = ui->showCenterOnBscanCBox->isChecked();
        this->showBscanOnErrorPlot = ui->showBscanOnErrorPlotCBox->isChecked();
        this->blockPCV = ui->blockPCVCBox->isChecked();

        if (ui->dataSaveStructureCBox->currentIndex() == 0)
            this->dataSaveStructure = "xml";
        else if (ui->dataSaveStructureCBox->currentIndex() == 1)
            this->dataSaveStructure = "json";
        else
            this->dataSaveStructure = "txt";

        if(saveSettingsToFile())
            this->accept();
    }
}

void SettingsDialog::on_cancelButton_clicked()
{
    this->reject();
}

void SettingsDialog::readSettingsFromFile(){

    QSettings appSettings(configFileName,QSettings::IniFormat);

    pathDatabase = appSettings.value("Paths/path_database",pathDatabaseDef).toString();
    pathOctData = appSettings.value("Paths/path_OCT_data",pathOctDataDef).toString();
    pathExamData = appSettings.value("Paths/path_exam_data",pathExamDataDef).toString();

    showETDRSGrid = appSettings.value("Display/showETDRSGrid",showETDRSGridDef).toBool();
    showCenterOnBscan = appSettings.value("Display/showCenterOnBscan",showCenterOnBscanDef).toBool();
    showBscanOnErrorPlot = appSettings.value("Display/showBscanOnErrorPlot",showBscanOnErrorPlotDef).toBool();

    blockPCV = appSettings.value("blockPCV",blockPCVDef).toBool();
    dataSaveStructure = appSettings.value("dataSaveStructure",dataSaveStructureDef).toString();
}

bool SettingsDialog::saveSettingsToFile(){

    QSettings appSettings(configFileName,QSettings::IniFormat);

    appSettings.setValue("Paths/path_database",pathDatabase);
    appSettings.setValue("Paths/path_OCT_data",pathOctData);
    appSettings.setValue("Paths/path_exam_data",pathExamData);

    appSettings.setValue("Display/showETDRSGrid",showETDRSGrid);
    appSettings.setValue("Display/showCenterOnBscan",showCenterOnBscan);
    appSettings.setValue("Display/showBscanOnErrorPlot",showBscanOnErrorPlot);

    appSettings.setValue("blockPCV",blockPCV);
    appSettings.setValue("dataSaveStructure",dataSaveStructure);

    return true;
}

// get values -----------------------------------------------------------------------
QString SettingsDialog::getDatabasePath(){
    QString path = this->pathDatabase;

    if (QDir(path).isRelative())
        path = QDir::currentPath().append("/" + path);

    return path;
}

QString SettingsDialog::getPathOctData(){
    QString path = this->pathOctData;

    if (QDir(path).isRelative())
        path = QDir::currentPath().append("/" + path);
    return path;
}

QString SettingsDialog::getPathExamData(){
    QString path = this->pathExamData;

    if (QDir(path).isRelative())
        path = QDir::currentPath().append("/" + path);
    return path;
}

QString SettingsDialog::getDataSaveStructure(){
    return this->dataSaveStructure;
}

bool SettingsDialog::getShowETDRSGrid(){
    return this->showETDRSGrid;
}

bool SettingsDialog::getShowCenterOnBscan(){
    return this->showCenterOnBscan;
}

bool SettingsDialog::getShowBscanOnErrorPlot(){
    return this->showBscanOnErrorPlot;
}

bool SettingsDialog::getBlockPCV(){
    return this->blockPCV;
}


// reset -----------------------------------------------------------------------------
void SettingsDialog::on_resetPathOctDataButton_clicked()
{
    pathOctData = pathOctDataDef;
    ui->pathOctDataLEdit->setText(pathOctDataDef);
}

void SettingsDialog::on_resetPathExamDataButton_clicked()
{
    pathExamData = pathExamDataDef;
    ui->pathExamDataLEdit->setText(pathExamDataDef);
}

void SettingsDialog::on_resetPathDatabaseButton_clicked()
{
    pathDatabase = pathDatabaseDef;
    ui->pathDatabaseLEdit->setText(pathDatabaseDef);
}


// select path ----------------------------------------------------------------------
void SettingsDialog::on_selectPathDatabaseButton_clicked()
{
    pathDatabase = QFileDialog::getOpenFileName(this, tr("Open Database File"), pathDatabase, tr("SQLite database (*.db)"));
    if (pathDatabase != "")
        ui->pathDatabaseLEdit->setText(pathDatabase);
}

void SettingsDialog::on_selectPathOctDataButton_clicked()
{
    pathOctData = QFileDialog::getExistingDirectory(this, tr("Open Directory"), pathOctData, QFileDialog::ShowDirsOnly);
    if (pathOctData != "")
        ui->pathOctDataLEdit->setText(pathOctData);
}

void SettingsDialog::on_selectPathExamDataButton_clicked()
{
    pathExamData = QFileDialog::getExistingDirectory(this, tr("Open Directory"), pathExamData, QFileDialog::ShowDirsOnly);
    if (pathExamData != "")
        ui->pathExamDataLEdit->setText(pathExamData);
}


// other -----------------------------------------------------------------------------
void SettingsDialog::on_dataSaveStructureCBox_currentIndexChanged(int index)
{
    if (index == 0)
        dataSaveStructure = "xml";
    else if (index == 1)
        dataSaveStructure = "json";
    else
        dataSaveStructure = "txt";
}
