#include "batchprocessingdialog.h"
#include "ui_batchprocessingdialog.h"
#include "readwritedata.h"

#include <QTimer>
#include <QDebug>
#include <QDir>
#include <QMessageBox>

BatchProcessingDialog::BatchProcessingDialog(QList<QString> list, QDir mDir, QDir oDir, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BatchProcessingDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Przetwarzanie seryjne");

    ui->batchProgressBar->setVisible(false);

    folderList = list;
    folderListCount = folderList.count();
    ui->scansNumberLabel->setText(QString::number(folderListCount));
    currentFolder = 0;

    canceled = false;
    finished = false;

    sufix = "";
    examDir = mDir;
    octBaseDir = oDir;

    myTimer = new QTimer(this);
    connect(myTimer, SIGNAL(timeout()), this, SLOT(process()));
}

BatchProcessingDialog::~BatchProcessingDialog()
{
    myTimer->stop();
    delete ui;
}

void BatchProcessingDialog::on_cancelButton_clicked()
{
    canceled = true;
    this->close();
}

void BatchProcessingDialog::on_acceptButton_clicked()
{
    if (finished){
        this->close();
    } else {
        ui->acceptButton->setEnabled(false);
        if (ui->autoExplorerRButton->isChecked())
            sufix = ui->autoExplorerSufixLEdit->text();
        else
            sufix = ui->autoMatlabSufixLEdit->text();

        // setting progress bar
        tasksNumber = 3;
        if (ui->searchAutoSegmentationsCBox->isChecked())
            tasksNumber++;
        if (ui->copyAutoAsManualCBox->isChecked())
            tasksNumber++;
        // TODO: calculate error

        int sum = folderListCount*tasksNumber;

        ui->batchProgressBar->setMaximum(sum);
        ui->batchProgressBar->setValue(0);
        ui->batchProgressBar->setVisible(true);

        myTimer->start(0);
    }
}

void BatchProcessingDialog::process(){

    if (!canceled){
        currentFolder++;

        QString scanName = folderList[currentFolder-1];
//        QString scanName = QDir(folderPath).dirName();
        qDebug() << "Processing " << currentFolder << "/" << folderListCount << ": " << scanName;

        QDir manualDir = QDir(examDir.absolutePath() + "/mvri/");
        QDir autoDir = QDir(examDir.absolutePath() + "/iowa/");

        pData = PatientData();

        // check if binary
        QDir octDir = QDir(octBaseDir.absolutePath() + "/" + scanName);
        QFile octFile(octBaseDir.absolutePath() + "/" + scanName + ".oct");

        ReadWriteData *rwData = new ReadWriteData();
        rwData->setDataObject(&pData);
        if (octFile.exists()){
            pData.setIsBinary(true);
            rwData->setOctFile(&octFile);
        } else {
            pData.setIsBinary(false);
            rwData->setDirectoryOct(&octDir);
        }
        rwData->setDirectoryManual(&manualDir);
        rwData->setDirectoryAuto(&autoDir);
        rwData->setAutoFilePath(autoDir.absolutePath() + "/" + scanName + sufix);
//        rwData->setDataSaveStrucure(dataSaveStructure);
        rwData->addDirective("readPatientData");
        if (octFile.exists()){
            rwData->addDirective("readOctExamFile");
        } else {
            rwData->addDirective("readOctExamData");
        }

        if (ui->searchAutoSegmentationsCBox->isChecked()){
            rwData->addDirective("readAutoSegmentationData");
        }
        if (ui->copyAutoAsManualCBox->isChecked()){
            if (ui->copyAllRButton->isChecked()){
                rwData->addDirective("copyAutoAsManualAll");
            } else if (ui->copyPCVRButton->isChecked()){
                rwData->addDirective("copyAutoAsManualPCV");
            }
            rwData->addDirective("saveManualSegmentationData");
        }

        connect(rwData, SIGNAL(errorOccured(QString)), this, SLOT(on_errorOccured(QString)));
        connect(rwData, SIGNAL(processingData(double,QString)), this, SLOT(on_processingData(double,QString)));

        rwData->process();

        ui->batchProgressBar->setValue(currentFolder*tasksNumber);
    }

    if (currentFolder >= folderListCount){
        myTimer->stop();
        processingFinished();
    }
}

void BatchProcessingDialog::processingFinished(){
    finished = true;
    ui->acceptButton->setText("Zakończ");
    ui->acceptButton->setEnabled(true);
    ui->infoLabel->setText("Zakończono");
    ui->batchProgressBar->setVisible(false);
}

void BatchProcessingDialog::on_errorOccured(QString err){
    QMessageBox::critical(this, tr("Error"), err);
}

void BatchProcessingDialog::on_processingData(double count, QString message){
    if(!ui->batchProgressBar->isVisible())
        ui->batchProgressBar->setVisible(true);
    int procent = (currentFolder-1)*tasksNumber + count*tasksNumber/100;
    ui->batchProgressBar->setValue(procent);

    if (message != ""){
        message = "Skan " + QString::number(currentFolder) + "/" + QString::number(folderListCount) + ": " + message;
        ui->infoLabel->setText(message);
    }
}
