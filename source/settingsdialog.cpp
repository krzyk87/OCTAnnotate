#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QMessageBox>
#include <QDir>
#include <QTextStream>
#include <QFileDialog>

const QString SettingsDialog::pathOctExamDef = "octData"; //QDir::currentPath().append("/octData");
const QString SettingsDialog::pathManualSegmDef = "examData/mvri"; //QDir::currentPath().append("/examData/mvri");
const QString SettingsDialog::pathAutoSegmDef = "examData/avri"; //QDir::currentPath().append("/examData/avri");
const QString SettingsDialog::openBskanDef = "m";
const QString SettingsDialog::dataSaveStructureDef = "xml";
const QString SettingsDialog::databasePathDef = "database/patients.db"; //QDir::currentPath().append("/database/patients.db");
const bool SettingsDialog::showETDRSGridDef = true;
const bool SettingsDialog::showCenterOnBscanDef = true;
const bool SettingsDialog::showBscanOnErrorPlotDef = true;
const bool SettingsDialog::blockPCVDef = true;

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent), ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    setDefaultValues();

    configFileName = QDir::currentPath().append("/config.txt");
    readSettingsFromFile();

    if(!QDir(pathOctExam).exists())
        QDir(pathOctExam).mkpath(pathOctExam);
    if(!QDir(pathManualSegm).exists())
        QDir(pathManualSegm).mkpath(pathManualSegm);
    if(!QDir(pathAutoSegm).exists())
        QDir(pathAutoSegm).mkpath(pathAutoSegm);

    ui->pathOctExamLEdit->setText(pathOctExam);
    ui->pathManualSegmLEdit->setText(pathManualSegm);
    ui->pathAutoSegmLEdit->setText(pathAutoSegm);
    ui->pathDatabaseLEdit->setText(databasePath);
    if (openBskan == "m")
        ui->openBskanMiddleRButton->setChecked(true);
    else  if (openBskan == "1")
        ui->openBskanFirstRButton->setChecked(true);
    ui->showETDRSGridCBox->setChecked(showETDRSGrid);
    ui->showCenterOnBscanCBox->setChecked(showCenterOnBscan);
    ui->showBscanOnErrorPlotCBox->setChecked(showBscanOnErrorPlot);
    ui->blockPCVCBox->setChecked(blockPCV);
    if (dataSaveStructure == "xml")
        ui->dataSaveStructureCBox->setCurrentIndex(0);
    else if (dataSaveStructure == "txt")
        ui->dataSaveStructureCBox->setCurrentIndex(1);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_acceptButton_clicked()
{
    // check if data in form not empty
    bool ready = true;
    if (ui->pathOctExamLEdit->text() == "")
        ready = false;
    if (ui->pathManualSegmLEdit->text() == "")
        ready = false;
    if (ui->pathAutoSegmLEdit->text() == "")
        ready = false;
    if (ui->pathDatabaseLEdit->text() == "")
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
        this->pathOctExam = ui->pathOctExamLEdit->text();
        this->pathManualSegm = ui->pathManualSegmLEdit->text();
        this->pathAutoSegm = ui->pathAutoSegmLEdit->text();
        this->databasePath = ui->pathDatabaseLEdit->text();

        if (ui->openBskanFirstRButton->isChecked())
            this->openBskan = "1";
        else if (ui->openBskanMiddleRButton->isChecked())
            this->openBskan = "m";

        this->showETDRSGrid = ui->showETDRSGridCBox->isChecked();
        this->showCenterOnBscan = ui->showCenterOnBscanCBox->isChecked();
        this->showBscanOnErrorPlot = ui->showBscanOnErrorPlotCBox->isChecked();
        this->blockPCV = ui->blockPCVCBox->isChecked();

        if (ui->dataSaveStructureCBox->currentIndex() == 0)
            this->dataSaveStructure = "xml";
        else if (ui->dataSaveStructureCBox->currentIndex() == 1)
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
    QString line;
    QFile configFile(configFileName);
    if (!configFile.open(QIODevice::ReadOnly)){
        QMessageBox::critical(this, tr("Error"), tr("Could not open config file"));
        return;
    }

    QTextStream configText(&configFile);
    do {
        line = configText.readLine();
        if (line.contains("=")){
            QStringList data = line.split("=");
            if (data.at(0) == "bscanNr"){
                openBskan = data.at(1);
            }
            if (data.at(0) == "pathAuto"){
                if (QDir(data.at(1)).exists())
                    pathAutoSegm = data.at(1);
            }
            if (data.at(0) == "pathManual"){
                if (QDir(data.at(1)).exists())
                    pathManualSegm = data.at(1);
            }
            if (data.at(0) == "pathOctExam"){
                if (QDir(data.at(1)).exists())
                    pathOctExam = data.at(1);
            }
            if (data.at(0) == "pathDatabase"){
                if (QFile(data.at(1)).exists())
                    databasePath = data.at(1);
            }
            if (data.at(0) == "showETDRSGrid"){
                if (data.at(1) == "1"){
                    showETDRSGrid = true;
                } else {
                    showETDRSGrid = false;
                }
            }
            if (data.at(0) == "showCenterOnBscan"){
                if (data.at(1) == "1"){
                    showCenterOnBscan = true;
                } else {
                    showCenterOnBscan = false;
                }
            }
            if (data.at(0) == "showBscanOnErrorPlot"){
                if (data.at(1) == "1"){
                    showBscanOnErrorPlot = true;
                } else {
                    showBscanOnErrorPlot = false;
                }
            }
            if (data.at(0) == "blockPCV"){
                if (data.at(1) == "1"){
                    blockPCV = true;
                } else {
                    blockPCV = false;
                }
            }
            if (data.at(0) == "dataSaveStructure"){
                dataSaveStructure = data.at(1);
            }
        }
    } while(!line.isNull());
    if (configFile.isOpen())
        configFile.close();
}

bool SettingsDialog::saveSettingsToFile(){

    QFile configFile(configFileName);
    if (!configFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)){
        QMessageBox::critical(this, tr("Error"), tr("Could not write to config file"));
        return false;
    } else {
        QTextStream stream(&configFile);
        stream << "bscanNr=" + openBskan << endl;
        stream << "pathOctExam=" + pathOctExam << endl;
        stream << "pathManual=" + pathManualSegm << endl;
        stream << "pathAuto=" + pathAutoSegm << endl;
        stream << "pathDatabase=" + databasePath << endl;
        if (showETDRSGrid)
            stream << "showETDRSGrid=1" << endl;
        else
            stream << "showETDRSGrid=0" << endl;
        if (blockPCV)
            stream << "blockPCV=1" << endl;
        else
            stream << "blockPCV=0" << endl;
        if (showCenterOnBscan)
            stream << "showCenterOnBscan=1" << endl;
        else
            stream << "showCenterOnBscan=0" << endl;
        if (showBscanOnErrorPlot)
            stream << "showBscanOnErrorPlot=1" << endl;
        else
            stream << "showBscanOnErrorPlot=0" << endl;
        stream << "dataSaveStructure=" + dataSaveStructure << endl;
    }
    if (configFile.isOpen())
        configFile.close();
    return true;
}

void SettingsDialog::setDefaultValues(){
    pathOctExam = pathOctExamDef;
    pathManualSegm = pathManualSegmDef;
    pathAutoSegm = pathAutoSegmDef;
    databasePath = databasePathDef;

    openBskan = openBskanDef;
    showETDRSGrid = showETDRSGridDef;
    showCenterOnBscan = showCenterOnBscanDef;
    showBscanOnErrorPlot = showBscanOnErrorPlotDef;
    blockPCV = blockPCVDef;

    dataSaveStructure = dataSaveStructureDef;
}

void SettingsDialog::on_resetPathOctExamButton_clicked()
{
    pathOctExam = pathOctExamDef;
    ui->pathOctExamLEdit->setText(pathOctExamDef);
}

void SettingsDialog::on_resetPathManualSegmButton_clicked()
{
    pathManualSegm = pathManualSegmDef;
    ui->pathManualSegmLEdit->setText(pathManualSegmDef);
}

void SettingsDialog::on_resetPathAutoSegmButton_clicked()
{
    pathAutoSegm = pathAutoSegmDef;
    ui->pathAutoSegmLEdit->setText(pathAutoSegmDef);
}

void SettingsDialog::on_resetPathDatabaseButton_clicked()
{
    databasePath = databasePathDef;
    ui->pathDatabaseLEdit->setText(databasePathDef);
}

QString SettingsDialog::getPathOctExam(){
    QString path = this->pathOctExam;

    if (QDir(path).isRelative())
        path = QDir::currentPath().append("/" + path);
    return path;
}

QString SettingsDialog::getPathManualSegm(){
    QString path = this->pathManualSegm;

    if (QDir(path).isRelative())
        path = QDir::currentPath().append("/" + path);
    return path;
}

QString SettingsDialog::getPathAutoSegm(){
    QString path = this->pathAutoSegm;

    if (QDir(path).isRelative())
        path = QDir::currentPath().append("/" + path);
    return path;
}

QString SettingsDialog::getOpenBskan(){
    return this->openBskan;
}

QString SettingsDialog::getDataSaveStructure(){
    return this->dataSaveStructure;
}

QString SettingsDialog::getDatabasePath(){
    QString path = this->databasePath;

//    QDir d = QDir(path);

//    if (d.makeAbsolute())
//        path = d.path();

    if (QDir(path).isRelative())
        path = QDir::currentPath().append("/" + path);

    return path;
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

void SettingsDialog::on_selectPathOctExamButton_clicked()
{
    pathOctExam = QFileDialog::getExistingDirectory(this, tr("Open Directory"), pathOctExam, QFileDialog::ShowDirsOnly);
    ui->pathOctExamLEdit->setText(pathOctExam);
}

void SettingsDialog::on_selectPathManualSegmButton_clicked()
{
    pathManualSegm = QFileDialog::getExistingDirectory(this, tr("Open Directory"), pathManualSegm, QFileDialog::ShowDirsOnly);
    ui->pathManualSegmLEdit->setText(pathManualSegm);
}

void SettingsDialog::on_selectPathAutoSegmButton_clicked()
{
    pathAutoSegm = QFileDialog::getExistingDirectory(this, tr("Open Directory"), pathAutoSegm, QFileDialog::ShowDirsOnly);
    ui->pathAutoSegmLEdit->setText(pathAutoSegm);
}

void SettingsDialog::on_selectPathDatabaseButton_clicked()
{
    databasePath = QFileDialog::getOpenFileName(this, tr("Open Database File"), databasePath, tr("SQLite database (*.db)"));
    ui->pathDatabaseLEdit->setText(databasePath);
}

void SettingsDialog::on_dataSaveStructureCBox_currentIndexChanged(int index)
{
    if (index == 0)
        dataSaveStructure = "xml";
    else if (index == 1)
        dataSaveStructure = "txt";
}
