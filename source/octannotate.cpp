#include "octannotate.h"
#include "ui_octannotate.h"
#include "newpatientdialog.h"
#include "autoasmanualdialog.h"
#include "settingsdialog.h"
#include "computeerrorsettingsdialog.h"
#include "batchprocessingdialog.h"
#include "qmath.h"
#include "calculate.h"
#include "readwritedata.h"
#include "dbmanager.h"
#include "infodialog.h"

#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QMouseEvent>
#include <QPainter>
#include <QColorDialog>
#include <QClipboard>
#include <QXmlStreamReader>
#include <QDebug>

const QString OCTAnnotate::settingsFilePath = QDir::currentPath().append("/config.ini");

OCTAnnotate::OCTAnnotate(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::OCTAnnotate)
{
    ui->setupUi(this);
    QFont font;
    font.setPixelSize(11);
    this->setFont(font);
    quit = false;
    ui->actionEditAnnotations->setChecked(true);
    ui->actionSaveGeneralExam->setEnabled(false);
    ui->actionSaveOCTExam->setEnabled(false);

    // configurations
    SettingsDialog *settingsDialog = new SettingsDialog(settingsFilePath);
    loadConfigurations(settingsDialog);

    // setup patients database
    patientsDB = new DbManager(databasePath);
    if (!patientsDB->isOpen()){
        qDebug() << "Database not opened!";
        QMessageBox::critical(this,"Database connection error","Could not connect to the patients database. Please set the correct path to the database in the Settings menu and restart the application.");
    } else {
        qDebug() << "Database opened!";
        initializeModelPatients();
        initializeModelScans();
        ui->scanListGroupCBox->setCurrentIndex(0);
        ui->fundusDBLabel->setScaledContents(true);
        ui->bscanHLabel->setScaledContents(true);
        ui->bscanVLabel->setScaledContents(true);
    }  

    patientData = PatientData();

    currentImageNumber = 0;
    currentNormalImageNumber = 0;
    currentImageLayersNumber = 0;
    generalDataModified = false;
    octDataModified = false;
    settingScanCenter = false;

    QRegExp rx("[0-9][\\.,][0-9]{1,2}");
    QRegExpValidator *rxv = new QRegExpValidator(rx);
    ui->visOPLEdit->setValidator(rxv);
    ui->visOLLEdit->setValidator(rxv);
    ui->snOPLEdit->setValidator(rxv);
    ui->snOLLEdit->setValidator(rxv);
    rx = QRegExp("[0-9]{1,3}");
    rxv = new QRegExpValidator(rx);
    ui->currImageNumberLEdit->setValidator(rxv);
    ui->currImageNumberLayersLEdit->setValidator(rxv);
    ui->verticalLayout_6->setAlignment(ui->fundusImageLabel, Qt::AlignHCenter);

    contrast = 1.5;
    brightness = 0;
    ui->contrastSlider->setValue(contrast*10);

    ui->fundusImageLabel->setScaledContents(true);
    ui->fundusImageLabel->installEventFilter(this);
    ui->virtualMapImageCPlot->installEventFilter(this);
    ui->virtualMapAutoImageCPlot->installEventFilter(this);
    ui->virtualMapCircProfileCPlot->installEventFilter(this);
    ui->virtualMapHistCPlot->installEventFilter(this);
    ui->ETDRSgridCPlot->installEventFilter(this);
    ui->imageLayersCPlot->installEventFilter(this);
    ui->layerCPlot->installEventFilter(this);
    ui->bScanHCPlot->installEventFilter(this);
    ui->bScanVCPlot->installEventFilter(this);
    ui->errorVirtualMapCPlot->installEventFilter(this);

    myPenColor = Qt::red;
    myPenWidth = 2;
    isControlPressed = false;
    drawing = false;
    erasing = false;
    fundusAnnotate = false;
    prevPoint = QPoint(-1,-1);
    prevPointN = QPoint(-1,-1);

    ui->pcvLayerRButton->setChecked(true);
    selectedLayer = PCV;
    pcvColor = Qt::green;
    ermColor = Qt::white;
    ilmColor = Qt::red; // darkRed
    gclColor = Qt::blue; // cyan
    iplColor = Qt::darkYellow;
    inlColor = Qt::blue;
    oplColor = Qt::magenta;
    onlColor = Qt::yellow; // darkGreen
    elmColor = Qt::yellow;
    mezColor = Qt::green; // darkCyan
    iosColor = Qt::green; // darkMagenta
    rpeColor = Qt::red;
    chrColor = QColor(255,85,0); // darkBlue
    myPalette.setColor(QPalette::Background, pcvColor);
    ui->pcvColorLabel->setAutoFillBackground(true);
    ui->pcvColorLabel->setPalette(myPalette);
    myPalette.setColor(QPalette::Background, ermColor);
    ui->ermColorLabel->setAutoFillBackground(true);
    ui->ermColorLabel->setPalette(myPalette);
    myPalette.setColor(QPalette::Background, ilmColor);
    ui->ilmColorLabel->setAutoFillBackground(true);
    ui->ilmColorLabel->setPalette(myPalette);
    myPalette.setColor(QPalette::Background, gclColor);
    ui->gclColorLabel->setAutoFillBackground(true);
    ui->gclColorLabel->setPalette(myPalette);
    myPalette.setColor(QPalette::Background, iplColor);
    ui->iplColorLabel->setAutoFillBackground(true);
    ui->iplColorLabel->setPalette(myPalette);
    myPalette.setColor(QPalette::Background, inlColor);
    ui->inlColorLabel->setAutoFillBackground(true);
    ui->inlColorLabel->setPalette(myPalette);
    myPalette.setColor(QPalette::Background, oplColor);
    ui->oplColorLabel->setAutoFillBackground(true);
    ui->oplColorLabel->setPalette(myPalette);
    myPalette.setColor(QPalette::Background, onlColor);
    ui->onlColorLabel->setAutoFillBackground(true);
    ui->onlColorLabel->setPalette(myPalette);
    myPalette.setColor(QPalette::Background, elmColor);
    ui->elmColorLabel->setAutoFillBackground(true);
    ui->elmColorLabel->setPalette(myPalette);
    myPalette.setColor(QPalette::Background, mezColor);
    ui->mezColorLabel->setAutoFillBackground(true);
    ui->mezColorLabel->setPalette(myPalette);
    myPalette.setColor(QPalette::Background, iosColor);
    ui->iosColorLabel->setAutoFillBackground(true);
    ui->iosColorLabel->setPalette(myPalette);
    myPalette.setColor(QPalette::Background, rpeColor);
    ui->rpeColorLabel->setAutoFillBackground(true);
    ui->rpeColorLabel->setPalette(myPalette);
    myPalette.setColor(QPalette::Background, chrColor);
    ui->chrColorLabel->setAutoFillBackground(true);
    ui->chrColorLabel->setPalette(myPalette);

    // TODO: fix virtual map width for height ratio
    QSizePolicy policy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    policy.setHeightForWidth(true);
    ui->virtualMapImageCPlot->setSizePolicy(policy);
    ui->virtualMapAutoImageCPlot->setSizePolicy(policy);

    setupImageLayersPlot();
    setupBScanPlots();
    setupVirtualMapPlot(ui->virtualMapImageCPlot);
    setupVirtualMapPlot(ui->virtualMapAutoImageCPlot);
    //setupVirtualMapPlot(ui->errorVirtualMapCPlot);
    setupCircProfilePlot();
    setupHistPlot();
    setupVolumeGridPlot();

    scaleFactor = 0;
    scales.append(1.0);
    scales.append(1.5);
    scales.append(2.0);
    scales.append(2.5);
    scales.append(3.0);
    scales.append(3.5);
    scales.append(4.0);
    scales.append(4.5);
    scales.append(5.0);
    bscanRange = QCPRange(0,640);

    // TODO: inne nazwy granic warstw (takie jak wyświetlane, a nie takie jak w kodzie)
    QList<Layers> layers = getAllLayers();
    foreach (Layers layer, layers) {
        ui->layer1CBox->addItem(encodeLayer(layer));
        ui->layer2CBox->addItem(encodeLayer(layer));
        ui->layerErrorCBox->addItem(encodeLayer(layer));
    }
    ui->layer1CBox->setCurrentIndex(0);
    ui->layer2CBox->setCurrentIndex(2);
    ui->layerErrorCBox->setCurrentIndex(2);

    thresholds.append(0);
    thresholds.append(15);
    thresholds.append(50);
    foreach (double value, thresholds) {
        ui->contactThresholdCBox->addItem(QString::number(value) + " [um]");
    }
    ui->contactThresholdCBox->setCurrentIndex(1);

    ui->tabWidget->removeTab(2);
    appVersion = "v1.6";
    this->setWindowTitle("OCTAnnotate " + appVersion);
    progressBar = new QProgressBar();
    progressBar->setMaximumWidth(250);
    progressBar->setMaximum(100);
    ui->statusBar->addPermanentWidget(progressBar);
    progressBar->setVisible(false);
}

OCTAnnotate::~OCTAnnotate()
{
    patientsDB->closeDatabase();
    delete ui;
}

// initialization ---------------------------------------------------------------------------------
void OCTAnnotate::loadConfigurations(SettingsDialog *sDialog){

    databasePath = sDialog->getDatabasePath();
    octDir = QDir(sDialog->getPathOctData());
    examDir = QDir(sDialog->getPathExamData());
    manualDir = QDir(sDialog->getPathExamData().append("mvri/"));
    autoDir = QDir(sDialog->getPathExamData().append("avri/"));
//    manualDir = QDir(settingsDialog->getPathManualSegm());
//    autoDir = QDir(settingsDialog->getPathAutoSegm());
    tmpDir = examDir;

    dataSaveStructure = sDialog->getDataSaveStructure();
    blockPCV = sDialog->getBlockPCV();

    on_actionShowETDRSGrid_toggled(sDialog->getShowETDRSGrid());
    on_actionShowCenterOnBscan_toggled(sDialog->getShowCenterOnBscan());
//    showETDRSGrid = sDialog->getShowETDRSGrid();
//    showCenterOnBscan = sDialog->getShowCenterOnBscan();
    showBscanOnErrorPlot = sDialog->getShowBscanOnErrorPlot();
    displayImageLayersPlot(currentImageLayersNumber,selectedErrorLayer);

    currentDir = QDir::current();
}

void OCTAnnotate::on_actionSettings_triggered()
{
    SettingsDialog *settingsDialog = new SettingsDialog(settingsFilePath);
    if(settingsDialog->exec() == QDialog::Accepted){
        loadConfigurations(settingsDialog);
//        databasePath = settingsDialog->getDatabasePath();
//        examDir = settingsDialog->getPathOctExam();
//        manualDir = settingsDialog->getPathManualSegm();
//        autoDir = settingsDialog->getPathAutoSegm();

//        dataSaveStructure = settingsDialog->getDataSaveStructure();
//        blockPCV = settingsDialog->getBlockPCV();
//        showBscanOnErrorPlot = settingsDialog->getShowBscanOnErrorPlot();
    }
}

void OCTAnnotate::initializeModelPatients(){
    modelPatients = new QSqlRelationalTableModel(this, patientsDB->getDatabase());
    modelPatients->setTable("patients");
    modelPatients->setRelation(5, QSqlRelation("gender","id","name"));

    modelPatients->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    modelPatients->setHeaderData(1, Qt::Horizontal, QObject::tr("Last name"));
    modelPatients->setHeaderData(2, Qt::Horizontal, QObject::tr("First name"));
    modelPatients->setHeaderData(3, Qt::Horizontal, QObject::tr("Birth date"));
    modelPatients->setHeaderData(4, Qt::Horizontal, QObject::tr("Age"));
    modelPatients->setHeaderData(5, Qt::Horizontal, QObject::tr("Gender"));
    modelPatients->setHeaderData(6, Qt::Horizontal, QObject::tr("OD"));
    modelPatients->setHeaderData(7, Qt::Horizontal, QObject::tr("OS"));
    modelPatients->setHeaderData(8, Qt::Horizontal, QObject::tr("File no"));
    modelPatients->setHeaderData(9, Qt::Horizontal, QObject::tr("Notes"));

    modelPatients->select();

    ui->patientsListTableView->setModel(modelPatients);
    ui->patientsListTableView->setSortingEnabled(true);
    ui->patientsListTableView->setColumnHidden(0, true);
    ui->patientsListTableView->setColumnHidden(8, true);
    ui->patientsListTableView->resizeColumnsToContents();
    ui->patientsListTableView->horizontalHeader()->setStretchLastSection(true);
    ui->patientsListTableView->setItemDelegate(new QSqlRelationalDelegate(ui->patientsListTableView));
    ui->patientsListTableView->sortByColumn(1,Qt::AscendingOrder);
    ui->patientsListTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->patientsListTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void OCTAnnotate::initializeModelScans(){
    modelScans = new QSqlRelationalTableModel(this, patientsDB->getDatabase());
    modelScans->setTable("scans");

    modelScans->setHeaderData(0, Qt::Horizontal, QObject::tr("Scan ID"));
    modelScans->setHeaderData(1, Qt::Horizontal, QObject::tr("Patient ID"));
    modelScans->setHeaderData(2, Qt::Horizontal, QObject::tr("Eye"));
    modelScans->setHeaderData(3, Qt::Horizontal, QObject::tr("Scan date"));
    modelScans->setHeaderData(4, Qt::Horizontal, QObject::tr("Device"));
    modelScans->setHeaderData(5, Qt::Horizontal, QObject::tr("Series"));
    modelScans->setHeaderData(6, Qt::Horizontal, QObject::tr("Scan type"));
    modelScans->setHeaderData(7, Qt::Horizontal, QObject::tr("Direction"));
    modelScans->setHeaderData(8, Qt::Horizontal, QObject::tr("Dimensions"));
    modelScans->setHeaderData(9, Qt::Horizontal, QObject::tr("QI"));
    modelScans->setHeaderData(10, Qt::Horizontal, QObject::tr("Scan Name"));
    modelScans->setHeaderData(11, Qt::Horizontal, QObject::tr("Notes"));
    modelScans->setHeaderData(12, Qt::Horizontal, QObject::tr("Default"));
    modelScans->setHeaderData(13, Qt::Horizontal, QObject::tr("AutoExplorer"));

    modelScans->select();

    ui->scansListTableView->setModel(modelScans);
    ui->scansListTableView->setSortingEnabled(true);
    ui->scansListTableView->setColumnHidden(0, true);
    ui->scansListTableView->setColumnHidden(1, true);
    ui->scansListTableView->setColumnHidden(4, true);
    ui->scansListTableView->setColumnHidden(6, true);
    ui->scansListTableView->setColumnHidden(7, true);
    ui->scansListTableView->setColumnHidden(8, true);
    ui->scansListTableView->setColumnHidden(9, true);
    ui->scansListTableView->resizeColumnsToContents();
//    ui->scansListTableView->horizontalHeader()->setStretchLastSection(true);
    ui->scansListTableView->horizontalHeader()->moveSection(10,0);
    ui->scansListTableView->sortByColumn(10,Qt::AscendingOrder);
    ui->scansListTableView->setEditTriggers(QAbstractItemView::SelectedClicked); // SelectedClicked, EditKeyPressed, AnyKeyPressed
    ui->scansListTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void OCTAnnotate::on_actionClose_triggered()
{
    QMessageBox msgBox;
    QPushButton *saveButton = msgBox.addButton(" Zapisz i zamknij ", QMessageBox::YesRole);
    QPushButton *cancelButton = msgBox.addButton(" Anuluj ", QMessageBox::RejectRole);
    msgBox.addButton(" Wyjdź bez zapisywania ", QMessageBox::NoRole);

    quit = true;

    // ask if save before quit
    if (generalDataModified){
        msgBox.setText("Informacje o badaniu okulistycznym zostały zmienione. Czy chcesz zapisać zmiany przed wyjściem?");
        msgBox.exec();
        if (msgBox.clickedButton() == saveButton){  // save data before quit
            on_actionSaveGeneralExam_triggered();
            delay(3);
        } else if (msgBox.clickedButton() == cancelButton) {   // quit without saving
            quit = false;
        }
    }
    if (octDataModified){
        msgBox.setText("Anotacje badania OCT zostały zmienione. Czy zapisać zmiany przed wyjściem?");
        msgBox.exec();
        if (msgBox.clickedButton() == saveButton){
            on_actionSaveOCTExam_triggered();
            delay(3);
        } else if (msgBox.clickedButton() == cancelButton) {
            quit = false;
        }
    }
    if (quit)
        qApp->quit();
}

void OCTAnnotate::closeEvent(QCloseEvent *cEvent){
    on_actionClose_triggered();
    if (!quit)
        cEvent->ignore();
}

void OCTAnnotate::on_actionSetScanCenter_toggled(bool checked)
{
    if (checked){
        settingScanCenter = true;
        ui->statusBar->showMessage("Ustawianie środka skanu: Proszę kliknąć na obrazie B-skan...");
    } else {
        settingScanCenter = false;
    }
}

void OCTAnnotate::on_actionShowCenterOnBscan_toggled(bool checked)
{
    showCenterOnBscan = checked;
    if (!patientData.getImageFileList().isEmpty())
        loadImage(currentImageNumber);
}

void OCTAnnotate::delay( int secondsToWait )
{
    QTime dieTime = QTime::currentTime().addSecs( secondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

void OCTAnnotate::on_actionInfo_triggered()
{
    InfoDialog *myInfo = new InfoDialog();
    myInfo->show();
}

// select OCT exam --------------------------------------------------------------------------------
void OCTAnnotate::on_actionLoadOCTSequence_triggered(QString scanFolderPath)
{
    QMessageBox msgBox;
    QPushButton *saveButton = msgBox.addButton(" Zapisz i wczytaj nowe badanie ", QMessageBox::YesRole);
    QPushButton *cancelButton = msgBox.addButton(" Anuluj ", QMessageBox::RejectRole);
    msgBox.addButton(" Wczytaj nowe badanie bez zapisywania ", QMessageBox::NoRole);
    bool selectNew = true;

    if (generalDataModified){
        msgBox.setText("Informacje o badaniu okulistycznym zostały zmienione. Czy chcesz zapisać zmiany przed otwarciem nowego badania?");
        msgBox.exec();
        if (msgBox.clickedButton() == saveButton){  // save data before quit
            on_actionSaveGeneralExam_triggered();
        } else if (msgBox.clickedButton() == cancelButton) {   // quit without saving
            selectNew = false;
        }
    }
    if (octDataModified){
        msgBox.setText("Anotacje badania OCT zostały zmienione. Czy zapisać zmiany przed wczytaniem nowego badania?");
        msgBox.exec();
        if (msgBox.clickedButton() == saveButton){
            on_actionSaveOCTExam_triggered();
        } else if (msgBox.clickedButton() == cancelButton) {
            selectNew = false;
        }
    }

    if (selectNew){
        QString dirName = scanFolderPath;
        if (dirName == "")
            dirName = QFileDialog::getExistingDirectory(this, tr("Open Directory"), octDir.path(), QFileDialog::ShowDirsOnly);

        qDebug() << "Opening scan: " << dirName;

        if (!dirName.isEmpty()){
            QDir scanDir = QDir(dirName);

            patientData = PatientData();

            ui->statusBar->showMessage("Trwa odczyt danych badania OCT...");
            progressBar->setMaximum(100);
            progressBar->setVisible(true);
            progressBar->setValue(0);

            ReadWriteData *rwData = new ReadWriteData();
            rwData->setDataObject(&patientData);
            rwData->setDirectoryOct(&scanDir);
            rwData->setDirectoryManual(&manualDir);
            rwData->setDirectoryAuto(&autoDir);
            rwData->setDataSaveStrucure(dataSaveStructure);
            rwData->addDirective("readPatientData");
            rwData->addDirective("readGeneralExamData");
            rwData->addDirective("readOctExamData");

            QThread *thread = new QThread;
            rwData->moveToThread(thread);
            connect(thread, SIGNAL(started()), rwData, SLOT(process()));
            connect(rwData, SIGNAL(errorOccured(QString)), this, SLOT(on_errorOccured(QString)));
            connect(rwData, SIGNAL(processingData(double,QString)), this, SLOT(on_processingData(double,QString)));
            //connect(rwData, SIGNAL(returnNewDirectory(QDir)), this, SLOT(on_returnNewDirectory(QDir)));
            connect(rwData, SIGNAL(readingDataFinished(QString)), this, SLOT(on_readingDataFinished(QString)));
            connect(rwData, SIGNAL(finished()), thread, SLOT(quit()));
            connect(rwData, SIGNAL(finished()), rwData, SLOT(deleteLater()));
            connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
            thread->start();
        }

        ui->fundusDBLabel->setPixmap(QPixmap());
        ui->bscanHLabel->setPixmap(QPixmap());
        ui->bscanVLabel->setPixmap(QPixmap());
    }
}

void OCTAnnotate::on_actionLoadOCTFile_triggered()
{
    QMessageBox msgBox;
    QPushButton *saveButton = msgBox.addButton(" Zapisz i wczytaj nowe badanie ", QMessageBox::YesRole);
    QPushButton *cancelButton = msgBox.addButton(" Anuluj ", QMessageBox::RejectRole);
    msgBox.addButton(" Wczytaj nowe badanie bez zapisywania ", QMessageBox::NoRole);
    bool selectNew = true;

    if (generalDataModified){
        msgBox.setText("Informacje o badaniu okulistycznym zostały zmienione. Czy chcesz zapisać zmiany przed otwarciem nowego badania?");
        msgBox.exec();
        if (msgBox.clickedButton() == saveButton){  // save data before quit
            on_actionSaveGeneralExam_triggered();
        } else if (msgBox.clickedButton() == cancelButton) {   // quit without saving
            selectNew = false;
        }
    }
    if (octDataModified){
        msgBox.setText("Anotacje badania OCT zostały zmienione. Czy zapisać zmiany przed wczytaniem nowego badania?");
        msgBox.exec();
        if (msgBox.clickedButton() == saveButton){
            on_actionSaveOCTExam_triggered();
        } else if (msgBox.clickedButton() == cancelButton) {
            selectNew = false;
        }
    }

    if (selectNew){
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open OCT file"), octDir.absolutePath(), tr("Avanti RTvue raw OCT data file (*.OCT)"));

        qDebug() << "Opening scan: " << fileName;

        if (!fileName.isEmpty()){
            octFile.setFileName(fileName);

            patientData = PatientData();

            ui->statusBar->showMessage("Trwa odczyt danych badania OCT...");
            progressBar->setMaximum(100);
            progressBar->setVisible(true);
            progressBar->setValue(0);

            ReadWriteData *rwData = new ReadWriteData();
            rwData->setDataObject(&patientData);
            rwData->setOctFile(&octFile);
            rwData->setDirectoryManual(&manualDir);
            rwData->setDirectoryAuto(&autoDir);
            rwData->setDataSaveStrucure(dataSaveStructure);
            rwData->addDirective("readPatientData");
            rwData->addDirective("readGeneralExamData");
            rwData->addDirective("readOctExamFile");

            QThread *thread = new QThread;
            rwData->moveToThread(thread);
            connect(thread, SIGNAL(started()), rwData, SLOT(process()));
            connect(rwData, SIGNAL(errorOccured(QString)), this, SLOT(on_errorOccured(QString)));
            connect(rwData, SIGNAL(processingData(double,QString)), this, SLOT(on_processingData(double,QString)));
            //connect(rwData, SIGNAL(returnNewDirectory(QDir)), this, SLOT(on_returnNewDirectory(QDir)));
            connect(rwData, SIGNAL(readingDataFinished(QString)), this, SLOT(on_readingDataFinished(QString)));
            connect(rwData, SIGNAL(finished()), thread, SLOT(quit()));
            connect(rwData, SIGNAL(finished()), rwData, SLOT(deleteLater()));
            connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
            thread->start();
        }

        ui->fundusDBLabel->setPixmap(QPixmap());
        ui->bscanHLabel->setPixmap(QPixmap());
        ui->bscanVLabel->setPixmap(QPixmap());
    }
}

// load image -------------------------------------------------------------------------------------
void OCTAnnotate::loadImage(int imageNumber){
    QFileInfo imageFileInfo(patientData.getImageFileList().at(imageNumber));

    // load image
    QImage image(patientData.getImageFileList().at(imageNumber));
    if (!image.isNull()){
        orgImageSize = image.size();

        currentImageNumber = imageNumber;
        // enable / disable buttons
        if (currentImageNumber >= (patientData.getBscansNumber() - 1))
            ui->nextImageButton->setEnabled(false);
        else
            ui->nextImageButton->setEnabled(true);
        if (currentImageNumber <= 0)
            ui->prevImageButton->setEnabled(false);
        else
            ui->prevImageButton->setEnabled(true);

        // contrast enhancement and flattening
        Calculate *calc = new Calculate();
        QList<int> flatDiff;
        for (int col=0; col < patientData.getBscanWidth(); col++){
            flatDiff.append(0);
        }
        if (flattenImage){
            if (patientData.hasManualAnnotations())
                flatDiff = patientData.getFlatDifferencesRPE(currentImageNumber);
            else
                flatDiff = patientData.getFlatDifferences(currentImageNumber);
            image = calc->flattenImage(&image,flatDiff);
        }
        calc->imageEnhancement(&image, contrast, brightness);

        // rescale image
        scaleFactor = qBound(0,scaleFactor,scales.count()-1);
        int imageHeight = patientData.getBscanHeight() / scales[scaleFactor];
        double dy = qBound(0, patientData.getBscanHeight() - (int)bscanRange.upper, patientData.getBscanHeight()-imageHeight);
        QImage newImage = image.copy(0, dy, patientData.getBscanWidth(), imageHeight);

        // display image
        ui->bScanHCPlot->yAxis->setRange(bscanRange);
        ui->bScanHCPlot->axisRect()->setBackground(QPixmap::fromImage(newImage),true,Qt::IgnoreAspectRatio);
        // display annotations
        displayAnnotations(flatDiff);

        // display image name
        ui->imageNumberLabel->setText(imageFileInfo.fileName());
        ui->currImageNumberLEdit->setText(QString::number(currentImageNumber));
    } else {
        qDebug() << "Image is null!";
    }
}

void OCTAnnotate::loadNormalImage(int normalImageNumber){
    QImage normalImage = patientData.getNormalImage(normalImageNumber);

    currentNormalImageNumber = normalImageNumber;

    // enable / disable buttons
    if (currentNormalImageNumber >= (patientData.getBscanWidth() - 1))
        ui->nextNormalImageButton->setEnabled(false);
    else
        ui->nextNormalImageButton->setEnabled(true);
    if (currentNormalImageNumber <= 0)
        ui->prevNormalImageButton->setEnabled(false);
    else
        ui->prevNormalImageButton->setEnabled(true);

    // contrast enhancement
    Calculate *calc = new Calculate();
    QList<int> flatDiffNormal;
    for (int col=0; col < patientData.getBscansNumber(); col++){
        flatDiffNormal.append(0);
    }
    if (flattenImage){
        if (patientData.hasManualAnnotations())
            flatDiffNormal = patientData.getFlatDifferencesNormalRPE(currentNormalImageNumber);
        else
            flatDiffNormal = patientData.getFlatDifferencesNormal(currentNormalImageNumber);
        normalImage = calc->flattenImage(&normalImage,flatDiffNormal);
    }
    calc->imageEnhancement(&normalImage, contrast, brightness);

    // scale
    scaleFactor = qBound(0,scaleFactor,scales.count()-1);
    int imageHeight = patientData.getBscanHeight() / scales[scaleFactor];
    double dy = qBound(0, patientData.getBscanHeight() - (int)bscanRange.upper, patientData.getBscanHeight()-imageHeight);
    QImage newImage = normalImage.copy(0, dy, patientData.getBscansNumber(), imageHeight);

    // display image
    ui->bScanVCPlot->yAxis->setRange(bscanRange);
    ui->bScanVCPlot->axisRect()->setBackground(QPixmap::fromImage(newImage),true,Qt::IgnoreAspectRatio);
    ui->currNormalImageNumberLEdit->setText(QString::number(currentNormalImageNumber));

    // display annotations
    displayNormalAnnotations(flatDiffNormal);
}

void OCTAnnotate::on_nextImageButton_clicked()
{
    if (!patientData.getImageFileList().isEmpty()){
        if (currentImageNumber < (patientData.getBscansNumber() - 1)){
            loadImage(currentImageNumber + 1);
            loadNormalImage(currentNormalImageNumber);
            fundusAnnotate = true;
            currentImageLayersNumber = currentImageNumber;
        }
    }
}

void OCTAnnotate::on_prevImageButton_clicked(){
    if (!patientData.getImageFileList().isEmpty()){
        if (currentImageNumber > 0){
            loadImage(currentImageNumber - 1);
            loadNormalImage(currentNormalImageNumber);
            fundusAnnotate = true;
            currentImageLayersNumber = currentImageNumber;
        }
    }
}

void OCTAnnotate::on_nextNormalImageButton_clicked()
{
    if (!patientData.getImageFileList().isEmpty()){
        if (currentNormalImageNumber < (patientData.getBscanWidth() - 1)){
            loadNormalImage(currentNormalImageNumber + 1);
            loadImage(currentImageNumber);
            fundusAnnotate = true;
        }
    }
}

void OCTAnnotate::on_prevNormalImageButton_clicked()
{
    if (!patientData.getImageFileList().isEmpty()){
        if (currentNormalImageNumber > 0){
            loadNormalImage(currentNormalImageNumber - 1);
            loadImage(currentImageNumber);
            fundusAnnotate = true;
        }
    }
}

void OCTAnnotate::on_currImageNumberLEdit_returnPressed()
{
    if (!patientData.getImageFileList().isEmpty()){
        int value = ui->currImageNumberLEdit->text().toInt();
        if (value < 0)
            value = 0;
        if (value >= patientData.getBscansNumber())
            value = patientData.getBscansNumber()-1;

        loadImage(value);
        fundusAnnotate = true;
        currentImageLayersNumber = currentImageNumber;
        ui->currImageNumberLEdit->clearFocus();
    }
}

void OCTAnnotate::on_currNormalImageNumberLEdit_returnPressed()
{
    if (!patientData.getImageFileList().isEmpty()){
        int value = ui->currNormalImageNumberLEdit->text().toInt();
        if (value < 0)
            value = 0;
        if (value >= patientData.getBscanWidth())
            value = patientData.getBscanWidth()-1;

        loadNormalImage(value);
        fundusAnnotate = true;
        ui->currNormalImageNumberLEdit->clearFocus();
    }
}

void OCTAnnotate::on_nextImageLayersButton_clicked()
{
    if (!patientData.getImageFileList().isEmpty()){
        if (currentImageLayersNumber < (patientData.getBscansNumber() - 1)){
            currentImageLayersNumber++;
            displayImageLayersPlot(currentImageLayersNumber,selectedErrorLayer);
            displayVirtualMap(ui->virtualMapAutoImageCPlot, true);
            displayVirtualMap(ui->virtualMapImageCPlot);
            loadImage(currentImageLayersNumber);

            if (currentImageLayersNumber >= (patientData.getBscansNumber() - 1))
                ui->nextImageLayersButton->setEnabled(false);
            ui->prevImageLayersButton->setEnabled(true);

            // display error for selected B-scan image
                // pcv layer
            ui->pcvLineErrorAvgLEdit->setText(QString::number(patientData.getLayerLineErrorAvg(PCV,currentImageLayersNumber)) + " px");
            ui->pcvLineErrorDevLEdit->setText(QString::number(patientData.getLayerLineErrorDev(PCV,currentImageLayersNumber)) + " px");
            ui->pcvLineErrorAvgLEdit_um->setText(QString::number(patientData.getLayerLineErrorAvg(PCV,currentImageLayersNumber,"um")) + " um");
            ui->pcvLineErrorDevLEdit_um->setText(QString::number(patientData.getLayerLineErrorDev(PCV,currentImageLayersNumber,"um")) + " um");
                // ilm layer
            ui->ilmLineErrorAvgLEdit->setText(QString::number(patientData.getLayerLineErrorAvg(ILM,currentImageLayersNumber)) + " px");
            ui->ilmLineErrorDevLEdit->setText(QString::number(patientData.getLayerLineErrorDev(ILM,currentImageLayersNumber)) + " px");
            ui->ilmLineErrorAvgLEdit_um->setText(QString::number(patientData.getLayerLineErrorAvg(ILM,currentImageLayersNumber,"um")) + " um");
            ui->ilmLineErrorDevLEdit_um->setText(QString::number(patientData.getLayerLineErrorDev(ILM,currentImageLayersNumber,"um")) + " um");
        }
    }
}

void OCTAnnotate::on_prevImageLayersButton_clicked()
{
    if (!patientData.getImageFileList().isEmpty()){
        if (currentImageLayersNumber > 0){
            currentImageLayersNumber--;
            displayImageLayersPlot(currentImageLayersNumber,selectedErrorLayer);
            displayVirtualMap(ui->virtualMapAutoImageCPlot, true);
            displayVirtualMap(ui->virtualMapImageCPlot);
            loadImage(currentImageLayersNumber);

            if (currentImageLayersNumber <= 0)
                ui->prevImageLayersButton->setEnabled(false);
            ui->nextImageLayersButton->setEnabled(true);

            // display error for selected B-scan image
                // pcv layer
            ui->pcvLineErrorAvgLEdit->setText(QString::number(patientData.getLayerLineErrorAvg(PCV,currentImageLayersNumber)) + " px");
            ui->pcvLineErrorDevLEdit->setText(QString::number(patientData.getLayerLineErrorDev(PCV,currentImageLayersNumber)) + " px");
            ui->pcvLineErrorAvgLEdit_um->setText(QString::number(patientData.getLayerLineErrorAvg(PCV,currentImageLayersNumber,"um")) + " um");
            ui->pcvLineErrorDevLEdit_um->setText(QString::number(patientData.getLayerLineErrorDev(PCV,currentImageLayersNumber,"um")) + " um");
                // ilm layer
            ui->ilmLineErrorAvgLEdit->setText(QString::number(patientData.getLayerLineErrorAvg(ILM,currentImageLayersNumber)) + " px");
            ui->ilmLineErrorDevLEdit->setText(QString::number(patientData.getLayerLineErrorDev(ILM,currentImageLayersNumber)) + " px");
            ui->ilmLineErrorAvgLEdit_um->setText(QString::number(patientData.getLayerLineErrorAvg(ILM,currentImageLayersNumber,"um")) + " um");
            ui->ilmLineErrorDevLEdit_um->setText(QString::number(patientData.getLayerLineErrorDev(ILM,currentImageLayersNumber,"um")) + " um");
        }
    }
}

void OCTAnnotate::on_currImageNumberLayersLEdit_returnPressed()
{
    if (!patientData.getImageFileList().isEmpty()){
        int value = ui->currImageNumberLayersLEdit->text().toInt();
        if (value < 0)
            value = 0;
        if (value >= patientData.getBscansNumber())
            value = patientData.getBscansNumber()-1;

        currentImageLayersNumber = value;
        displayImageLayersPlot(currentImageLayersNumber,selectedErrorLayer);
        loadImage(currentImageLayersNumber);
        fundusAnnotate = true;
        displayVirtualMap(ui->virtualMapImageCPlot);
        displayVirtualMap(ui->virtualMapAutoImageCPlot, true);

        if (currentImageLayersNumber <= 0)
            ui->prevImageLayersButton->setEnabled(false);
        else
            ui->prevImageLayersButton->setEnabled(true);
        if (currentImageLayersNumber >= (patientData.getBscansNumber()-1))
            ui->nextImageLayersButton->setEnabled(false);
        else
            ui->nextImageLayersButton->setEnabled(true);

        ui->currImageNumberLayersLEdit->clearFocus();
    }
}

void OCTAnnotate::on_actionImageFlattening_toggled(bool state)
{
    flattenImage = state;
    if (!patientData.getImageFileList().isEmpty()){
        loadImage(currentImageNumber);
        loadNormalImage(currentNormalImageNumber);
        displayImageLayersPlot(currentImageLayersNumber,ILM);
    }
}


// adjust contrast and brightness -----------------------------------------------------------------
void OCTAnnotate::on_contrastSlider_valueChanged(int value)
{
    contrast = (float)value / 10.0;
    if (!patientData.getImageFileList().isEmpty()){
        loadImage(currentImageNumber);
        loadNormalImage(currentNormalImageNumber);
    }
}

void OCTAnnotate::on_brightnessSlider_valueChanged(int value)
{
    brightness = value;
    if (!patientData.getImageFileList().isEmpty()){
        loadImage(currentImageNumber);
        loadNormalImage(currentNormalImageNumber);
    }
}

void OCTAnnotate::on_contrastResetButton_clicked()
{
    ui->contrastSlider->setValue(10);
}

void OCTAnnotate::on_brightnessResetButton_clicked()
{
    ui->brightnessSlider->setValue(0);
}


// display / calculate data for each tab ----------------------------------------------------------
void OCTAnnotate::on_tabWidget_currentChanged()
{
    QWidget *currWidget = ui->tabWidget->currentWidget();

    if (currWidget == ui->tabOCTExam){    // OCT Exam Tab
        if (!patientData.getImageFileList().isEmpty()){
            loadImage(currentImageNumber);
            loadNormalImage(currentNormalImageNumber);
            ui->fundusImageLabel->setPixmap(QPixmap::fromImage(patientData.getFundusImage())); // fundus
            fundusAnnotate = true;
            int pcvAnnotatedCount = patientData.getLayerAnnotatedCount(PCV);
            int ermAnnotatedCount = patientData.getLayerAnnotatedCount(ERM);
            int ilmAnnotatedCount = patientData.getLayerAnnotatedCount(ILM);
            int gclAnnotatedCount = patientData.getLayerAnnotatedCount(GCL);
            int iplAnnotatedCount = patientData.getLayerAnnotatedCount(IPL);
            int inlAnnotatedCount = patientData.getLayerAnnotatedCount(INL);
            int oplAnnotatedCount = patientData.getLayerAnnotatedCount(OPL);
            int onlAnnotatedCount = patientData.getLayerAnnotatedCount(ONL);
            int elmAnnotatedCount = patientData.getLayerAnnotatedCount(ELM);
            int mezAnnotatedCount = patientData.getLayerAnnotatedCount(MEZ);
            int iosAnnotatedCount = patientData.getLayerAnnotatedCount(IOS);
            int rpeAnnotatedCount = patientData.getLayerAnnotatedCount(RPE);
            int chrAnnotatedCount = patientData.getLayerAnnotatedCount(CHR);
            QString BscansNumber = QString::number(patientData.getBscansNumber());
            ui->pcvAnnotCountLabel->setText("(" + QString::number(pcvAnnotatedCount) + " / " + BscansNumber + ")");
            ui->ermAnnotCountLabel->setText("(" + QString::number(ermAnnotatedCount) + " / " + BscansNumber + ")");
            ui->ilmAnnotCountLabel->setText("(" + QString::number(ilmAnnotatedCount) + " / " + BscansNumber + ")");
            ui->gclAnnotCountLabel->setText("(" + QString::number(gclAnnotatedCount) + " / " + BscansNumber + ")");
            ui->iplAnnotCountLabel->setText("(" + QString::number(iplAnnotatedCount) + " / " + BscansNumber + ")");
            ui->inlAnnotCountLabel->setText("(" + QString::number(inlAnnotatedCount) + " / " + BscansNumber + ")");
            ui->oplAnnotCountLabel->setText("(" + QString::number(oplAnnotatedCount) + " / " + BscansNumber + ")");
            ui->onlAnnotCountLabel->setText("(" + QString::number(onlAnnotatedCount) + " / " + BscansNumber + ")");
            ui->elmAnnotCountLabel->setText("(" + QString::number(elmAnnotatedCount) + " / " + BscansNumber + ")");
            ui->mezAnnotCountLabel->setText("(" + QString::number(mezAnnotatedCount) + " / " + BscansNumber + ")");
            ui->iosAnnotCountLabel->setText("(" + QString::number(iosAnnotatedCount) + " / " + BscansNumber + ")");
            ui->rpeAnnotCountLabel->setText("(" + QString::number(rpeAnnotatedCount) + " / " + BscansNumber + ")");
            ui->chrAnnotCountLabel->setText("(" + QString::number(chrAnnotatedCount) + " / " + BscansNumber + ")");
        }
    } else if (currWidget == ui->tabVirtualMap){
        if (!patientData.getImageFileList().isEmpty()){
            //on_computeVirtualMapButton_clicked();
            displayVirtualMap(ui->virtualMapImageCPlot);
            displayCircProfile();
            displayHistogram();
            displayVolumes();

            double caCF = patientData.getContactAreaCF();
            double caIM = patientData.getContactAreaIM();
            double caOM = patientData.getContactAreaOM();
            ui->contactAreaCFLEdit->setText(QString::number(caCF));
            ui->contactAreaIMLEdit->setText(QString::number(caIM));
            ui->contactAreaOMLEdit->setText(QString::number(caOM));
            double max1 = 3.1415926535 * 1.0 /2 * 1.0 /2;
            double max2 = 3.1415926535 * 3.0 /2 * 3.0 /2;
            double max3 = 3.1415926535 * 6.0 /2 * 6.0 /2;
            ui->contactAreaProcCFLabel->setText(QString::number(caCF / max1 * 100,3,0) + " %");
            ui->contactAreaProcIMLabel->setText(QString::number(caIM / max2 * 100,3,0) + " %");
            ui->contactAreaProcOMLabel->setText(QString::number(caOM / max3 * 100,3,0) + " %");

            ui->retinaDepthLEdit->setText(QString::number(patientData.getRetinaDepth()));
        }
    } else if (currWidget == ui->tabErrorAnalysis){
        if (!patientData.getImageFileList().isEmpty()){
            //on_computeVirtualMapButton_clicked();
            displayVirtualMap(ui->virtualMapAutoImageCPlot, true);
            displayImageLayersPlot(currentImageLayersNumber,selectedErrorLayer);

            ui->layerErrorAvgLEdit->setText(QString::number(patientData.getLayerErrorMSE(selectedErrorLayer)) + " px");
            ui->layerErrorDevLEdit->setText(QString::number(patientData.getLayerErrorDev(selectedErrorLayer)) + " px");
            ui->layerErrorAvgLEdit_um->setText(QString::number(patientData.getLayerErrorMSE(selectedErrorLayer, "um")) + " um");
            ui->layerErrorDevLEdit_um->setText(QString::number(patientData.getLayerErrorDev(selectedErrorLayer, "um")) + " um");
            ui->layerErrorProcLEdit->setText(QString::number(patientData.getLayerErrorProc(selectedErrorLayer)) + " %");

            ui->virtualMapErrorAvgLEdit->setText(QString::number(patientData.getVirtualMapErrorAvg()) + " px");
            ui->virtualMapErrorDevLEdit->setText(QString::number(patientData.getVirtualMapErrorDev()) + " px");
            ui->virtualMapErrorAvgLEdit_um->setText(QString::number(patientData.getVirtualMapErrorAvg("um")) + " um");
            ui->virtualMapErrorDevLEdit_um->setText(QString::number(patientData.getVirtualMapErrorDev("um")) + " um");

            // display error for selected B-scan image
                // pcv layer
            ui->pcvLineErrorAvgLEdit->setText(QString::number(patientData.getLayerLineErrorAvg(PCV,currentImageLayersNumber)) + " px");
            ui->pcvLineErrorDevLEdit->setText(QString::number(patientData.getLayerLineErrorDev(PCV,currentImageLayersNumber)) + " px");
            ui->pcvLineErrorAvgLEdit_um->setText(QString::number(patientData.getLayerLineErrorAvg(PCV,currentImageLayersNumber,"um")) + " um");
            ui->pcvLineErrorDevLEdit_um->setText(QString::number(patientData.getLayerLineErrorDev(PCV,currentImageLayersNumber,"um")) + " um");
                // ilm layer
            ui->ilmLineErrorAvgLEdit->setText(QString::number(patientData.getLayerLineErrorAvg(ILM,currentImageLayersNumber)) + " px");
            ui->ilmLineErrorDevLEdit->setText(QString::number(patientData.getLayerLineErrorDev(ILM,currentImageLayersNumber)) + " px");
            ui->ilmLineErrorAvgLEdit_um->setText(QString::number(patientData.getLayerLineErrorAvg(ILM,currentImageLayersNumber,"um")) + " um");
            ui->ilmLineErrorDevLEdit_um->setText(QString::number(patientData.getLayerLineErrorDev(ILM,currentImageLayersNumber,"um")) + " um");

            ui->layerErrorMSEAllLEdit->setText(QString::number(patientData.getErrorMSE()));
            ui->layerErrorAvgAllLEdit->setText(QString::number(patientData.getErrorAvg()));
        }
    } else if (currWidget == ui->tabRetinaAnalysis){
        if (!patientData.getImageFileList().isEmpty()){

        }
    }
}


// save data --------------------------------------------------------------------------------------
void OCTAnnotate::on_actionSaveGeneralExam_triggered()
{
    // read the form
    patientData.setVisOP(ui->visOPLEdit->text());
    patientData.setVisOL(ui->visOLLEdit->text());
    patientData.setSnOP(ui->snOPLEdit->text());
    patientData.setSnOL(ui->snOLLEdit->text());

    patientData.setMcvOP(ui->mcVOPLEdit->text());
    patientData.setMcvOL(ui->mcVOLLEdit->text());
    patientData.setMchOP(ui->mcHOPLEdit->text());
    patientData.setMchOL(ui->mcHOLLEdit->text());

    ReadWriteData *rwData = new ReadWriteData();
    rwData->setDataObject(&patientData);
    rwData->setDirectoryManual(&manualDir);
    rwData->addDirective("saveGeneralExamData");

    QThread *thread = new QThread;
    rwData->moveToThread(thread);
    connect(thread, SIGNAL(started()), rwData, SLOT(process()));
    connect(rwData, SIGNAL(errorOccured(QString)), this, SLOT(on_errorOccured(QString)));
    connect(rwData, SIGNAL(processingData(double,QString)), this, SLOT(on_processingData(double,QString)));
    connect(rwData, SIGNAL(savingDataFinished(QString)), this, SLOT(on_savingDataFinished(QString)));
    connect(rwData, SIGNAL(finished()), thread, SLOT(quit()));
    connect(rwData, SIGNAL(finished()), rwData, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}

void OCTAnnotate::on_actionSaveOCTExam_triggered()
{
    ReadWriteData *rwData = new ReadWriteData();
    rwData->setDataObject(&patientData);
    rwData->setDirectoryManual(&manualDir);
    rwData->setDirectoryOct(&octDir);
    rwData->addDirective("saveManualSegmentationData");
    rwData->setDataSaveStrucure(dataSaveStructure);

    QThread *thread = new QThread;
    rwData->moveToThread(thread);
    connect(thread, SIGNAL(started()), rwData, SLOT(process()));
    connect(rwData, SIGNAL(errorOccured(QString)), this, SLOT(on_errorOccured(QString)));
    connect(rwData, SIGNAL(processingData(double,QString)), this, SLOT(on_processingData(double,QString)));
    connect(rwData, SIGNAL(savingDataFinished(QString)), this, SLOT(on_savingDataFinished(QString)));
    connect(rwData, SIGNAL(finished()), thread, SLOT(quit()));
    connect(rwData, SIGNAL(finished()), rwData, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}


// draw / erase bscan layers ----------------------------------------------------------------------
void OCTAnnotate::on_actionEditAnnotations_toggled(bool state)
{
    editAnnotations = state;
}

bool OCTAnnotate::eventFilter(QObject *target, QEvent *event){
    if (!patientData.getImageFileList().isEmpty()){
        QPoint currPoint;

        if (target == ui->bScanHCPlot) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            currPoint.setX(ui->bScanHCPlot->xAxis->pixelToCoord(mouseEvent->pos().x()));
            currPoint.setY(patientData.getBscanHeight() - ui->bScanHCPlot->yAxis->pixelToCoord(mouseEvent->pos().y()));

            if ((event->type() == QEvent::MouseButtonPress) && (editAnnotations)){
                if (mouseEvent->button() == Qt::LeftButton){    // start drawing

                    if (settingScanCenter){
                        // set center of the scan where clicked
                        patientData.setScanCenter(QPoint(currPoint.x(),currentImageNumber));
                        setScanCenter();
                    } else {
                        if (isControlPressed && isPointSet(prevPoint)){
                            drawLayer(currPoint, prevPoint);
                        } else {
                            drawing = isLayerActive(selectedLayer);
                        }
                        prevPoint = currPoint;
                        lastPoint = currPoint;
                    }
                } else if (mouseEvent->button() == Qt::RightButton){    // start erasing
                    lastPoint = currPoint;
                    prevPoint = currPoint;
                    erasing = isLayerActive(selectedLayer);
                }
            } else if (event->type() == QEvent::MouseMove){
                if ((mouseEvent->buttons() & Qt::LeftButton) && drawing){   // continue drawing
                    drawLayer(currPoint);
                    lastPoint = currPoint;
                } else if ((mouseEvent->buttons() & Qt::RightButton) && erasing){   // continue erasing
                    eraseLayer(currPoint);
                    lastPoint = currPoint;
                }
            } else if (event->type() == QEvent::MouseButtonRelease){    // end drawing
                if ((mouseEvent->button() == Qt::LeftButton) && drawing){
                    if (isControlPressed){
                        drawLayer(currPoint, prevPoint); // TODO: czy to potrzebne?
                    } else {
                        drawLayer(currPoint);
                    }
                    lastPoint = currPoint;
                    prevPoint = currPoint;
                    drawing = false;
                    // count pixels in layers to find if layer is annotated
                    patientData.countAnnotatedPixelsInLayer(currentImageNumber, selectedLayer);
                    int count = patientData.getLayerAnnotatedCount(selectedLayer);
                    QString text = "(" + QString::number(count) + " / " + QString::number(patientData.getBscansNumber()) + ")";
                    switch (selectedLayer) {
                    case PCV:
                        ui->pcvAnnotCountLabel->setText(text);
                        break;
                    case ERM:
                        ui->ermAnnotCountLabel->setText(text);
                        break;
                    case ILM:
                        ui->ilmAnnotCountLabel->setText(text);
                        break;
                    case GCL:
                        ui->gclAnnotCountLabel->setText(text);
                        break;
                    case IPL:
                        ui->iplAnnotCountLabel->setText(text);
                        break;
                    case INL:
                        ui->inlAnnotCountLabel->setText(text);
                        break;
                    case OPL:
                        ui->oplAnnotCountLabel->setText(text);
                        break;
                    case ONL:
                        ui->onlAnnotCountLabel->setText(text);
                        break;
                    case ELM:
                        ui->elmAnnotCountLabel->setText(text);
                        break;
                    case MEZ:
                        ui->mezAnnotCountLabel->setText(text);
                        break;
                    case IOS:
                        ui->iosAnnotCountLabel->setText(text);
                        break;
                    case RPE:
                        ui->rpeAnnotCountLabel->setText(text);
                        break;
                    case CHR:
                        ui->chrAnnotCountLabel->setText(text);
                        break;
                    default:
                        break;
                    }
                }
                if ((mouseEvent->button() == Qt::RightButton) && erasing){  // end erasing
                    eraseLayer(currPoint);
                    lastPoint = currPoint;
                    prevPoint = QPoint(-1,-1);
                    erasing = false;
                    // count pixels in layers to find if layer is annotated
                    patientData.countAnnotatedPixelsInLayer(currentImageNumber, selectedLayer);
                    int count = patientData.getLayerAnnotatedCount(selectedLayer);
                    QString text = "(" + QString::number(count) + " / " + QString::number(patientData.getBscansNumber()) + ")";
                    switch (selectedLayer) {
                    case PCV:
                        ui->pcvAnnotCountLabel->setText(text);
                        break;
                    case ERM:
                        ui->ermAnnotCountLabel->setText(text);
                        break;
                    case ILM:
                        ui->ilmAnnotCountLabel->setText(text);
                        break;
                    case GCL:
                        ui->gclAnnotCountLabel->setText(text);
                        break;
                    case IPL:
                        ui->iplAnnotCountLabel->setText(text);
                        break;
                    case INL:
                        ui->inlAnnotCountLabel->setText(text);
                        break;
                    case OPL:
                        ui->oplAnnotCountLabel->setText(text);
                        break;
                    case ONL:
                        ui->onlAnnotCountLabel->setText(text);
                        break;
                    case ELM:
                        ui->elmAnnotCountLabel->setText(text);
                        break;
                    case MEZ:
                        ui->mezAnnotCountLabel->setText(text);
                        break;
                    case IOS:
                        ui->iosAnnotCountLabel->setText(text);
                        break;
                    case RPE:
                        ui->rpeAnnotCountLabel->setText(text);
                        break;
                    case CHR:
                        ui->chrAnnotCountLabel->setText(text);
                        break;
                    default:
                        break;
                    }
                }
            } else if (event->type() == QEvent::Wheel){
                QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
                QPoint numSteps = wheelEvent->angleDelta() / 8 / 15;

                changeImageRange(numSteps.y());
            }
        } else if (target == ui->bScanVCPlot){
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            currPoint.setX(ui->bScanVCPlot->xAxis->pixelToCoord(mouseEvent->pos().x()));
            currPoint.setY(patientData.getBscanHeight() - ui->bScanVCPlot->yAxis->pixelToCoord(mouseEvent->pos().y()));

            if ((event->type() == QEvent::MouseButtonPress) && (editAnnotations)){
                if (mouseEvent->button() == Qt::LeftButton){    // start drawing

                    if (settingScanCenter){
                        patientData.setScanCenter(QPoint(currentNormalImageNumber,currPoint.x()));
                        setScanCenter();
                    } else {
                        if (isControlPressed && isPointSet(prevPointN)){
                            drawLayerNormal(currPoint, prevPointN);
                        } else {
                            drawing = isLayerActive(selectedLayer);
                        }
                        prevPointN = currPoint;
                        lastPointN = currPoint;
                    }
                } else if (mouseEvent->button() == Qt::RightButton){    // start erasing
                    lastPointN = currPoint;
                    prevPointN = currPoint;
                    erasing = isLayerActive(selectedLayer);
                }
            } else if (event->type() == QEvent::MouseMove){
                if ((mouseEvent->buttons() & Qt::LeftButton) && drawing){   // continue drawing
                    drawLayerNormal(currPoint);
                    lastPointN = currPoint;
                } else if ((mouseEvent->buttons() & Qt::RightButton) && erasing){
                    eraseLayerNormal(currPoint);
                    lastPointN = currPoint;
                }
            } else if (event->type() == QEvent::MouseButtonRelease){
                if ((mouseEvent->button() == Qt::LeftButton) && drawing){   // end drawing
                    if (isControlPressed){
                        drawLayerNormal(currPoint, prevPointN);
                    } else {
                        drawLayerNormal(currPoint);
                    }
                    lastPointN = currPoint;
                    prevPointN = currPoint;
                    drawing = false;
                }
                if ((mouseEvent->button() == Qt::RightButton) && erasing){  // end erasing
                    eraseLayerNormal(currPoint);
                    lastPointN = currPoint;
                    prevPointN = QPoint(-1,-1);
                    erasing = false;
                }
            } else if (event->type() == QEvent::Wheel){
                QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
                QPoint numSteps = wheelEvent->angleDelta() / 8 / 15;

                changeImageRange(numSteps.y());
            }

        } else if (target == ui->fundusImageLabel){
            if (event->type() == QEvent::MouseButtonPress){
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

                currentImageNumber = mouseEvent->pos().y() * patientData.getBscansNumber() / ui->fundusImageLabel->height();
                currentNormalImageNumber = mouseEvent->pos().x() * patientData.getBscanWidth() / ui->fundusImageLabel->width();

                loadImage(currentImageNumber);
                loadNormalImage(currentNormalImageNumber);

                fundusAnnotate = true;
                displayVirtualMap(ui->virtualMapImageCPlot);
                displayVirtualMap(ui->virtualMapAutoImageCPlot, true);
                currentImageLayersNumber = currentImageNumber;
                displayImageLayersPlot(currentImageLayersNumber,selectedErrorLayer);
            }
        } else if (target == ui->virtualMapImageCPlot){
            if (event->type() == QEvent::MouseButtonPress){
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
                if (mouseEvent->button() == Qt::LeftButton){
                    int BscanNumber = -(ui->virtualMapImageCPlot->yAxis2->pixelToCoord(mouseEvent->pos().y()));
                    if ((BscanNumber >= 0) && (BscanNumber < patientData.getBscansNumber())){
                        currentImageLayersNumber = BscanNumber;
                        loadImage(BscanNumber);
                        displayVirtualMap(ui->virtualMapImageCPlot);
                        displayVirtualMap(ui->virtualMapAutoImageCPlot, true);
                        displayImageLayersPlot(currentImageLayersNumber,selectedErrorLayer);
                    }
                }
                if (mouseEvent->button() == Qt::RightButton){   // copy to clipboard
                    QClipboard *clipboard = QApplication::clipboard();
                    clipboard->setPixmap(ui->virtualMapImageCPlot->toPixmap());
                    QMessageBox::information(this, "Skopiowano", "Obraz został skopiowany do schowka");
                }
            } else if (event->type() == QEvent::MouseMove){
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
                int bscanNumber = -(ui->virtualMapImageCPlot->yAxis2->pixelToCoord(mouseEvent->pos().y()));
                int bscanColumn = ui->virtualMapImageCPlot->xAxis2->pixelToCoord(mouseEvent->pos().x());
                if ((bscanNumber > 0) && (bscanNumber < patientData.getBscansNumber()) && (bscanColumn > 0) && (bscanColumn < patientData.getBscanWidth())){
                    int value = patientData.getVirtualMapValue(bscanColumn,bscanNumber,"um");
                    ui->virtualMapValueHoverLabel->setText("(" + QString::number(bscanColumn) + ", " + QString::number(bscanNumber) + ") " + QString::number(value) + " [um]");
                } else {
                    ui->virtualMapValueHoverLabel->setText("");
                }
            }
        } else if (target == ui->virtualMapAutoImageCPlot){
            if (event->type() == QEvent::MouseButtonPress){
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
                if (mouseEvent->button() == Qt::LeftButton){
                    int BscanNumber = -(ui->virtualMapAutoImageCPlot->yAxis2->pixelToCoord(mouseEvent->pos().y()));
                    if ((BscanNumber >=0) && (BscanNumber < patientData.getBscansNumber())){
                        currentImageLayersNumber = BscanNumber;
                        loadImage(BscanNumber);
                        displayImageLayersPlot(currentImageLayersNumber,selectedErrorLayer);
                        displayVirtualMap(ui->virtualMapImageCPlot);
                        displayVirtualMap(ui->virtualMapAutoImageCPlot, true);
                    }
                }
                if (mouseEvent->button() == Qt::RightButton){   // copy to clipboard
                    QClipboard *clipboard = QApplication::clipboard();
                    clipboard->setPixmap(ui->virtualMapAutoImageCPlot->toPixmap());
                    QMessageBox::information(this, "Skopiowano", "Obraz został skopiowany do schowka");
                }

            }
        } else if (target == ui->imageLayersCPlot){
            if (event->type() == QEvent::MouseButtonPress){
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
                if (mouseEvent->button() == Qt::LeftButton){
                    int lineNumber = (ui->imageLayersCPlot->xAxis->pixelToCoord(mouseEvent->pos().x()));
                    if ((lineNumber >= 0) && (lineNumber < patientData.getBscanWidth())){

                        ui->imageLayersCPlot->clearItems();

                        // display vertical line
                        QCPItemLine *line = new QCPItemLine(ui->imageLayersCPlot);
                        ui->imageLayersCPlot->addItem(line);
                        line->setPen(QPen(Qt::black));
                        line->start->setCoords(lineNumber,0);
                        line->end->setCoords(lineNumber,patientData.getBscanHeight());

                        // display layers values
                        QCPItemText *text = new QCPItemText(ui->imageLayersCPlot);
                        ui->imageLayersCPlot->addItem(text);
                        text->setPen(QPen(Qt::blue));
                        text->setBrush(QBrush(QColor(250,254,237)));
                        text->setTextAlignment(Qt::AlignLeft);
                        text->position->setCoords(100,patientData.getBscanHeight()-70);
                        int layer_value = 0, layer_value_auto = 0, v = 0; //ilm = 0, ilm_auto = 0
                        Layers layer = (Layers)ui->layerErrorCBox->currentIndex();
                        v = patientData.getLayerPoint(currentImageLayersNumber,layer,lineNumber).y();
                        if (v != -1)
                            layer_value = patientData.getBscanHeight() - v;
                        v = patientData.getLayerPointAuto(currentImageLayersNumber,layer,lineNumber).y();
                        if (v != -1)
                            layer_value_auto = patientData.getBscanHeight() - v;
//                        v = patientData.getLayerPoint(currentImageLayersNumber,ILM,lineNumber).y();
//                        if (v != -1)
//                            ilm = patientData.getBscanHeight() - v;
//                        v = patientData.getLayerPointAuto(currentImageLayersNumber,ILM,lineNumber).y();
//                        if (v != -1)
//                            ilm_auto = patientData.getBscanHeight() - v;
                        QString str = "  x:\t\t" + QString::number(lineNumber) + "\t\n";
                        str += "  Layer:\t\t\t" + QString::number(layer_value) + "\t\n";
                        str += "  Layer auto:\t" + QString::number(layer_value_auto) + "\t\n";
//                        str += "  ILM:\t\t\t" + QString::number(ilm) + "\t\n";
//                        str += "  ILM auto:\t" + QString::number(ilm_auto);
                        text->setText(str);
                        // TODO: other layers?....
                    }
                }
                if (mouseEvent->button() == Qt::RightButton){   // copy to clipboard
                    QClipboard *clipboard = QApplication::clipboard();
                    clipboard->setPixmap(ui->imageLayersCPlot->toPixmap());
                    QMessageBox::information(this, "Skopiowano", "Obraz został skopiowany do schowka");
                }
            }
        } else if (target == ui->layerCPlot){
            if (event->type() == QEvent::MouseButtonPress){
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
                if (mouseEvent->button() == Qt::RightButton){
                    QClipboard *clipboard = QApplication::clipboard();
                    clipboard->setPixmap(ui->layerCPlot->toPixmap());
                    QMessageBox::information(this, "Skopiowano", "Obraz został skopiowany do schowka");
                }
            }
        } else if (target == ui->virtualMapCircProfileCPlot){
            if (event->type() == QEvent::MouseButtonPress){
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
                if (mouseEvent->button() == Qt::RightButton){
                    QClipboard *clipboard = QApplication::clipboard();
                    clipboard->setPixmap(ui->virtualMapCircProfileCPlot->toPixmap());
                    QMessageBox::information(this, "Skopiowano", "Obraz został skopiowany do schowka");
                }
            }
        } else if (target == ui->virtualMapHistCPlot){
            if (event->type() == QEvent::MouseButtonPress){
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
                if (mouseEvent->button() == Qt::RightButton){
                    QClipboard *clipboard = QApplication::clipboard();
                    clipboard->setPixmap(ui->virtualMapHistCPlot->toPixmap());
                    QMessageBox::information(this, "Skopiowano", "Obraz został skopiowany do schowka");
                }
            }
        } else if (target == ui->ETDRSgridCPlot){
            if (event->type() == QEvent::MouseButtonPress){
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
                if (mouseEvent->button() == Qt::RightButton){
                    QClipboard *clipboard = QApplication::clipboard();
                    clipboard->setPixmap(ui->ETDRSgridCPlot->toPixmap());
                    QString ETDRSgridCPlotPath = octDir.absolutePath();
//                    ETDRSgridCPlotPath.chop(10);
//                    ETDRSgridCPlotPath += ".pdf";
                    ui->ETDRSgridCPlot->savePdf(ETDRSgridCPlotPath + "ETDRS.pdf", true, 400, 400);
                    QMessageBox::information(this, "Skopiowano", "Obraz został skopiowany do schowka");
                }
            }
        } else if (target == ui->errorVirtualMapCPlot){
            if (event->type() == QEvent::MouseButtonPress){
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
                if (mouseEvent->button() == Qt::RightButton){
                    QClipboard *clipboard = QApplication::clipboard();
                    clipboard->setPixmap(ui->errorVirtualMapCPlot->toPixmap());
                    QMessageBox::information(this, "Skopiowano", "Obraz został skopiowany do schowka");
                }
            }
        }
    }
    return false;
}

void OCTAnnotate::keyPressEvent(QKeyEvent *keyEvent){
    if (keyEvent->key() == Qt::Key_Up){
        on_prevImageButton_clicked();
    } else if (keyEvent->key() == Qt::Key_Down){
        on_nextImageButton_clicked();
    } else if (keyEvent->key() == Qt::Key_Right){
        //int index = ui->tabWidget->currentIndex() + 1;
        //ui->tabWidget->setCurrentIndex(index);
        on_nextImageButton_clicked();
    } else if (keyEvent->key() == Qt::Key_Left){
        //int index = ui->tabWidget->currentIndex() - 1;
        //ui->tabWidget->setCurrentIndex(index);
        on_prevImageButton_clicked();
    }
    if (keyEvent->modifiers().testFlag(Qt::ControlModifier)){
        isControlPressed = true;
    }
}

void OCTAnnotate::keyReleaseEvent(QKeyEvent *keyEvent){
    if (keyEvent->key() == Qt::Key_Control){
        isControlPressed = false;
    }
}

void OCTAnnotate::paintEvent(QPaintEvent *){
    if (fundusAnnotate){ // display annotation of fundus image (current B-skan line)
        if (!patientData.getImageFileList().isEmpty()){
            QPixmap fundusPixMap = QPixmap::fromImage(patientData.getFundusImage());
            QPainter painter(&fundusPixMap);

            // draw line with current scan
            myPenColor = Qt::green;
            painter.setPen(QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            QPoint startPoint = QPoint(0,currentImageNumber);
            QPoint endPoint = QPoint(fundusPixMap.width(), currentImageNumber);
            painter.drawLine(startPoint,endPoint);
            myPenColor = Qt::blue;
            painter.setPen(QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            startPoint = QPoint(currentNormalImageNumber,0);
            endPoint = QPoint(currentNormalImageNumber,fundusPixMap.height());
            painter.drawLine(startPoint,endPoint);

            // mark scan center
            QPoint center = patientData.getScanCenter();
            painter.setPen(QPen(Qt::red));
            int lengthy = 0.0625 * patientData.getBscansNumber();
            int lengthx = 0.0625 * patientData.getBscanWidth();

            if ((center.x() != -1) && (center.y() != -1)){
                startPoint = QPoint(center.x() - lengthx, center.y());
                endPoint = QPoint(center.x() + lengthx, center.y());
                painter.drawLine(startPoint, endPoint);
                painter.setPen(QPen(Qt::red, 4));
                startPoint = QPoint(center.x(), center.y() - lengthy);
                endPoint = QPoint(center.x(), center.y() + lengthy);
                painter.drawLine(startPoint, endPoint);
            } else {
                startPoint = QPoint(fundusPixMap.width()/2 - lengthx, patientData.getBscansNumber()/2);
                endPoint = QPoint(fundusPixMap.width()/2 + lengthx, patientData.getBscansNumber()/2);
                painter.drawLine(startPoint, endPoint);
                painter.setPen(QPen(Qt::red, 4));
                startPoint = QPoint(fundusPixMap.width()/2, patientData.getBscansNumber()/2 - lengthy);
                endPoint = QPoint(fundusPixMap.width()/2, patientData.getBscansNumber()/2 + lengthy);
                painter.drawLine(startPoint, endPoint);
            }

            ui->fundusImageLabel->setPixmap(fundusPixMap);
            fundusAnnotate = false;
        }
    }
}

void OCTAnnotate::displayAnnotations(QList<int> flatDiff){
    if (!patientData.getImageFileList().isEmpty()){

        QList<Layers> dispLayers = getLayersToDisplay();
        QList<Layers> allLayers = getAllLayers();
        int height = patientData.getBscanHeight();

        foreach (Layers layer, dispLayers) {
            int graphID = allLayers.indexOf(layer);
            QList<QPoint> pointsList = patientData.getLayerPoints(currentImageNumber,layer,0,patientData.getBscanWidth()-1);
            int pointsCount = pointsList.count();
            QVector<double> x(pointsCount), y(pointsCount);
            for (int i=0; i<pointsCount; i++){
                x[i] = i;
                int yVal = pointsList.at(i).y();
                if (yVal == -1){
                    y[i] = std::numeric_limits<double>::quiet_NaN();
                } else{
                    y[i] = height - yVal - flatDiff[i];
                }
            }
            ui->bScanHCPlot->graph(graphID)->setData(x,y);
        }

        // show scan center
        ui->bScanHCPlot->clearItems();
        QPoint center = patientData.getScanCenter();
        if (showCenterOnBscan && (currentImageNumber == center.y())){
            QCPItemLine *centerLine = new QCPItemLine(ui->bScanHCPlot);
            ui->bScanHCPlot->addItem(centerLine);
            centerLine->setPen(QPen(Qt::red));
            centerLine->start->setCoords(center.x(),0);
            centerLine->end->setCoords(center.x(),patientData.getBscanHeight());
        }

        // show line where current normal image
//        QCPItemLine *bScanVLine = new QCPItemLine(ui->bScanHCPlot);
//        ui->bScanHCPlot->addItem(bScanVLine);
//        bScanVLine->setPen(QPen(Qt::blue));
//        bScanVLine->start->setCoords(currentNormalImageNumber,0);
//        bScanVLine->end->setCoords(currentNormalImageNumber,patientData.getBscanHeight());

        ui->bScanHCPlot->replot();
    }
}

void OCTAnnotate::displayNormalAnnotations(QList<int> flatDiffNormal){
    if (!patientData.getImageFileList().isEmpty()){
        QList<Layers> dispLayers = getLayersToDisplay();
        QList<Layers> allLayers = getAllLayers();
        int height = patientData.getBscanHeight();

        // set data to graphs
        foreach (Layers layer, dispLayers){
            int graphID = allLayers.indexOf(layer);
            QList<QPoint> pointsList = patientData.getLayerPoints(currentNormalImageNumber,layer,0,patientData.getBscansNumber()-1,true);
            int pointsCount = pointsList.count();
            QVector<double> x(pointsCount), y(pointsCount);
            for (int i=0; i<pointsCount; i++){
                x[i] = i;
                int yVal = pointsList.at(i).y();
                if (yVal == -1){
                    y[i] = std::numeric_limits<double>::quiet_NaN();
                } else{
                    y[i] = height - yVal - flatDiffNormal[i];
                }
            }
            ui->bScanVCPlot->graph(graphID)->setData(x,y);
        }

        // show scan center
        ui->bScanVCPlot->clearItems();
        QPoint center = patientData.getScanCenter();
        if (showCenterOnBscan && (currentNormalImageNumber == center.x())){
            QCPItemLine *centerLine = new QCPItemLine(ui->bScanVCPlot);
            ui->bScanVCPlot->addItem(centerLine);
            centerLine->setPen(QPen(Qt::red));
            centerLine->start->setCoords(center.y(),0);
            centerLine->end->setCoords(center.y(),patientData.getBscanHeight());
        }

        // show line where current image
        QCPItemLine *bScanHLine = new QCPItemLine(ui->bScanVCPlot);
        ui->bScanVCPlot->addItem(bScanHLine);
        bScanHLine->setPen(QPen(Qt::green));
        bScanHLine->start->setCoords(currentImageNumber,0);
        bScanHLine->end->setCoords(currentImageNumber,patientData.getBscanHeight());

        ui->bScanVCPlot->replot();
    }
}

void OCTAnnotate::drawLayer(QPoint endPoint, QPoint prevPoint){

    int graphID = getAllLayers().indexOf(selectedLayer);
    myPenColor = getLayerColor(selectedLayer);

    if (flattenImage){
        if (patientData.hasManualAnnotations()){
            int diff = patientData.getFlatDifferenceRPE(currentImageNumber,endPoint.x());
            endPoint.setY(endPoint.y() - diff);
            if (isPointSet(prevPoint)){
                diff = patientData.getFlatDifferenceRPE(currentImageNumber,prevPoint.x());
                prevPoint.setY(prevPoint.y() - diff);
            } else {
                diff = patientData.getFlatDifferenceRPE(currentImageNumber,lastPoint.x());
                lastPoint.setY(lastPoint.y() - diff);
            }
        } else {
            int diff = patientData.getFlatDifference(currentImageNumber,endPoint.x());
            endPoint.setY(endPoint.y() - diff);
            if (isPointSet(prevPoint)){
                diff = patientData.getFlatDifference(currentImageNumber,prevPoint.x());
                prevPoint.setY(prevPoint.y() - diff);
            } else {
                diff = patientData.getFlatDifference(currentImageNumber,lastPoint.x());
                lastPoint.setY(lastPoint.y() - diff);
            }
        }
    }

    QPoint startPoint;
    if (isPointSet(prevPoint))
        startPoint = prevPoint;
    else
        startPoint = lastPoint;

    int min = qMin(startPoint.x(),endPoint.x());
    int max = qMax(startPoint.x(),endPoint.x());

    if ((min >= 0) && (max < patientData.getBscanWidth())){
        // erase old pixels
        ui->bScanHCPlot->graph(graphID)->removeData(min,max);

        if ((selectedLayer == PCV) && blockPCV){    // check if PCV under ILM
            QPoint spILM = patientData.getLayerPoint(currentImageNumber,ILM,startPoint.x());
            QPoint epILM = patientData.getLayerPoint(currentImageNumber,ILM,endPoint.x());
            if ((spILM.x() != -1) && (epILM.x() != -1)){
                if (startPoint.y() > spILM.y())
                    startPoint = spILM;
                if (endPoint.y() > epILM.y())
                    endPoint = epILM;
            }
        }
        if (isPointSet(prevPoint)){
            saveLayer(endPoint, "draw", prevPoint);
        } else{
            saveLayer(endPoint, "draw");
        }

        // if changed layer is RPE -> recalculate image flattening differences
        if (selectedLayer == CHR){
            patientData.calculateFlatDifferencesRPE(currentImageNumber);
        }

        // draw new pixels
        QList<QPoint> pointsList = patientData.getLayerPoints(currentImageNumber, selectedLayer, min, max);
        int pointsCount = pointsList.count();
        int height = patientData.getBscanHeight();
        QVector<double> x(pointsCount), y(pointsCount);
        for (int i=0; i<pointsCount; i++){
            x[i] = pointsList.at(i).x();
            y[i] = height - pointsList.at(i).y();
            if (flattenImage){
                int diff = 0;
                if (patientData.hasManualAnnotations())
                    diff = patientData.getFlatDifferenceRPE(currentImageNumber,x[i]);
                else
                    diff = patientData.getFlatDifference(currentImageNumber,x[i]);
                y[i] -= diff;
            }
        }
        ui->bScanHCPlot->graph(graphID)->addData(x,y);
        ui->bScanHCPlot->replot();
    }
}

void OCTAnnotate::drawLayerNormal(QPoint endPoint, QPoint prevPoint){
    int graphID = getAllLayers().indexOf(selectedLayer);
    myPenColor = getLayerColor(selectedLayer);

    if (flattenImage){
        if (patientData.hasManualAnnotations()){
            int diff = patientData.getFlatDifferenceRPE(endPoint.x(),currentNormalImageNumber);
            endPoint.setY(endPoint.y() - diff);
            if (isPointSet(prevPoint)){
                diff = patientData.getFlatDifferenceRPE(prevPoint.x(),currentNormalImageNumber);
                prevPoint.setY(prevPoint.y() - diff);
            } else {
                diff = patientData.getFlatDifferenceRPE(lastPointN.x(),currentNormalImageNumber);
                lastPointN.setY(lastPointN.y() - diff);
            }
        } else {
            int diff = patientData.getFlatDifference(endPoint.x(),currentNormalImageNumber);
            endPoint.setY(endPoint.y() - diff);
            if (isPointSet(prevPoint)){
                diff = patientData.getFlatDifference(prevPoint.x(),currentNormalImageNumber);
                prevPoint.setY(prevPoint.y() - diff);
            } else {
                diff = patientData.getFlatDifference(lastPointN.x(),currentNormalImageNumber);
                lastPointN.setY(lastPointN.y() - diff);
            }
        }
    }

    QPoint startPoint;
    if (isPointSet(prevPoint))
        startPoint = prevPoint;
    else
        startPoint = lastPointN;

    int min = qMin(startPoint.x(),endPoint.x());
    int max = qMax(startPoint.x(),endPoint.x());

    if ((min >= 0) && (max < patientData.getBscansNumber())){
        // erase old pixels
        ui->bScanVCPlot->graph(graphID)->removeData(min,max);

        if ((selectedLayer == PCV) && blockPCV){
            QPoint spILM = patientData.getLayerPoint(startPoint.x(),ILM,currentNormalImageNumber);
            QPoint epILM = patientData.getLayerPoint(endPoint.x(),ILM,currentNormalImageNumber);
            if ((spILM.x() != -1) && (epILM.x() != -1)){
                if (startPoint.y() > spILM.y())
                    startPoint = spILM;
                if (endPoint.y() > epILM.y())
                    endPoint = epILM;
            }
        }

        if (isPointSet(prevPoint)){
            saveLayerNormal(endPoint, "draw", prevPoint);
        } else{
            saveLayerNormal(endPoint, "draw");
        }

        // if changed layer is RPE -> recalculate image flattening differences
        if (selectedLayer == CHR){
            patientData.calculateFlatDifferencesNormalRPE(currentNormalImageNumber);
        }

        // draw new pixels
        QList<QPoint> pointsList = patientData.getLayerPoints(currentNormalImageNumber, selectedLayer, min, max, true);
        int pointsCount = pointsList.count();
        int height = patientData.getBscanHeight();
        QVector<double> x(pointsCount), y(pointsCount);
        for (int i=0; i<pointsCount; i++){
            x[i] = min+i+1;
            y[i] = height - pointsList.at(i).y();
            if (flattenImage){
                int diff = 0;
                if (patientData.hasManualAnnotations())
                    diff = patientData.getFlatDifferenceRPE(x[i],currentNormalImageNumber);
                else
                    diff = patientData.getFlatDifference(x[i],currentNormalImageNumber);
                y[i] -= diff;
            }
        }
        ui->bScanVCPlot->graph(graphID)->addData(x,y);
        ui->bScanVCPlot->replot();
    }
}

void OCTAnnotate::eraseLayer(QPoint endPoint){
    QPoint startPoint = lastPoint;
    int graphID = getAllLayers().indexOf(selectedLayer);

    int min = qMin(startPoint.x(),endPoint.x());
    int max = qMax(startPoint.x(),endPoint.x());

    if ((min >= 0) && (max < patientData.getBscanWidth())){

        saveLayer(endPoint, "erase");

        ui->bScanHCPlot->graph(graphID)->removeData(min,max);
        int pointsCount = max - min + 1;
        QVector<double> x(pointsCount), y(pointsCount);
        for (int i=0; i<pointsCount; i++){
            x[i] = min+i;
            y[i] = std::numeric_limits<double>::quiet_NaN();
        }
        ui->bScanHCPlot->graph(graphID)->addData(x,y);
        ui->bScanHCPlot->replot();
    }
}

void OCTAnnotate::eraseLayerNormal(QPoint endPoint){
    QPoint startPoint = lastPointN;
    int graphID = getAllLayers().indexOf(selectedLayer);

    int min = qMin(startPoint.x(),endPoint.x());
    int max = qMax(startPoint.x(),endPoint.x());

    if ((min > 0) && (max < patientData.getBscansNumber())){
        saveLayerNormal(endPoint, "erase");

        ui->bScanVCPlot->graph(graphID)->removeData(min,max);
        int pointsCount = max - min + 1;
        QVector<double> x(pointsCount), y(pointsCount);
        for (int i=0; i<pointsCount; i++){
            x[i] = min+i;
            y[i] = std::numeric_limits<double>::quiet_NaN();
        }
        ui->bScanVCPlot->graph(graphID)->addData(x,y);
        ui->bScanVCPlot->replot();
    }
}

void OCTAnnotate::saveLayer(QPoint endPoint, QString action, QPoint prevPoint){

    QPoint startPoint;
    if (isPointSet(prevPoint))
        startPoint = prevPoint;
    else
        startPoint = lastPoint;

    int min = qMin(startPoint.x(),endPoint.x());
    int max = qMax(startPoint.x(),endPoint.x());

    if ((min >= 0) && (max < patientData.getBscanWidth())){
        QList<QPoint> pointsList;

        if (action == "erase"){
            for (int i = min; i <= max; i++){
                pointsList.append(QPoint(i,-1));
            }
        } else {
            pointsList = computeLinePoints(startPoint, endPoint);
        }

        patientData.setLayerPoints(currentImageNumber, pointsList, selectedLayer);
        octDataModified = true;
    }
}

void OCTAnnotate::saveLayerNormal(QPoint endPoint, QString action, QPoint prevPoint){
    QPoint startPoint;
    if (isPointSet(prevPoint))
        startPoint = prevPoint;
    else
        startPoint = lastPointN;

    int min = qMin(startPoint.x(),endPoint.x());
    int max = qMax(startPoint.x(),endPoint.x());

    if ((min >= 0) && (max < patientData.getBscansNumber())){
        QList<QPoint> pointsList;

        if (action == "erase"){
            for (int i = min; i <= max; i++){
                pointsList.append(QPoint(i,-1));
            }
        } else {
            pointsList = computeLinePoints(startPoint, endPoint);
        }

        patientData.setLayerPoints(currentNormalImageNumber, pointsList, selectedLayer, true);
        octDataModified = true;
    }
}

QList<QPoint> OCTAnnotate::computeLinePoints(QPoint p0, QPoint p1){
    QList<QPoint> list;

    int dx = qAbs(p1.x() - p0.x());
    int dy = qAbs(p1.y() - p0.y());

    int sx = 0;
    int sy = 0;

    if (p0.x() < p1.x())
        sx = 1;
    else
        sx = -1;
    if (p0.y() < p1.y())
        sy = 1;
    else
        sy = -1;
    int err = dx - dy;

    list.append(p0);

    int x = p0.x();
    int y = p0.y();

    while ((x != p1.x()) || (y != p1.y())){
        int e2 = 2 * err;
        if (e2 >= -dy){
            err = err - dy;
            x = x + sx;
        }
        if (e2 <= dx){
            err = err + dx;
            y = y + sy;
        }
        list.append(QPoint(x,y));
    }

    return list;
}


// aditional settings -----------------------------------------------------------------------------
void OCTAnnotate::setScanCenter(){
    ui->actionSetScanCenter->setChecked(false);

    ui->virtualMapImageCPlot->clearPlottables();
    ui->virtualMapAutoImageCPlot->clearPlottables();
    resetImageLayersLabels();

    fundusAnnotate = true;
    displayVirtualMap(ui->virtualMapImageCPlot);
    displayVirtualMap(ui->virtualMapAutoImageCPlot, true);

    patientData.calculateContactArea(1.0);
    patientData.calculateContactArea(3.0);
    patientData.calculateContactArea(6.0);
    ui->contactAreaCFLEdit->setText(QString::number(patientData.getContactAreaCF()));
    ui->contactAreaIMLEdit->setText(QString::number(patientData.getContactAreaIM()));
    ui->contactAreaOMLEdit->setText(QString::number(patientData.getContactAreaOM()));
    double max1 = 3.1415926535 * 1.0 /2 * 1.0 /2;
    double max2 = 3.1415926535 * 3.0 /2 * 3.0 /2;
    double max3 = 3.1415926535 * 6.0 /2 * 6.0 /2;
    ui->contactAreaProcCFLabel->setText(QString::number(patientData.getContactAreaCF() / max1 * 100,3,0) + " %");
    ui->contactAreaProcIMLabel->setText(QString::number(patientData.getContactAreaIM() / max2 * 100,3,0) + " %");
    ui->contactAreaProcOMLabel->setText(QString::number(patientData.getContactAreaOM() / max3 * 100,3,0) + " %");

    patientData.calculateCircProfile(1.0);
    patientData.calculateCircProfile(3.0);
    patientData.calculateCircProfile(6.0);
    displayCircProfile();

    settingScanCenter = false;
    octDataModified = true;
    loadImage(currentImageNumber);
    loadNormalImage(currentNormalImageNumber);
    ui->statusBar->showMessage("Środek skanu został ustawiony",5000);
}

void OCTAnnotate::on_actionChangeLayerColor_triggered()
{
    QColor oldColor, newColor;
    switch (selectedLayer) {
    case PCV:
        oldColor = pcvColor;
        break;
    case ERM:
        oldColor = ermColor;
        break;
    case ILM:
        oldColor = ilmColor;
        break;
    case GCL:
        oldColor = gclColor;
        break;
    case IPL:
        oldColor = iplColor;
        break;
    case INL:
        oldColor = inlColor;
        break;
    case OPL:
        oldColor = oplColor;
        break;
    case ONL:
        oldColor = onlColor;
        break;
    case ELM:
        oldColor = elmColor;
        break;
    case MEZ:
        oldColor = mezColor;
        break;
    case IOS:
        oldColor = iosColor;
        break;
    case RPE:
        oldColor = rpeColor;
        break;
    case CHR:
        oldColor = chrColor;
        break;
    default:
        break;
    }
    newColor = QColorDialog::getColor(oldColor);
    if (newColor.isValid()){
        myPalette.setColor(QPalette::Background, newColor);
        switch (selectedLayer) {
        case PCV:
            pcvColor = newColor;
            ui->pcvColorLabel->setPalette(myPalette);
            on_pcvLayerCBox_stateChanged(true);
            break;
        case ERM:
            ermColor = newColor;
            ui->ermColorLabel->setPalette(myPalette);
            on_ermLayerCBox_stateChanged(true);
            break;
        case ILM:
            ilmColor = newColor;
            ui->ilmColorLabel->setPalette(myPalette);
            on_ilmLayerCBox_stateChanged(true);
            break;
        case GCL:
            gclColor = newColor;
            ui->gclColorLabel->setPalette(myPalette);
            on_gclLayerCBox_stateChanged(true);
            break;
        case IPL:
            iplColor = newColor;
            ui->iplColorLabel->setPalette(myPalette);
            on_iplLayerCBox_stateChanged(true);
            break;
        case INL:
            inlColor = newColor;
            ui->inlColorLabel->setPalette(myPalette);
            on_inlLayerCBox_stateChanged(true);
            break;
        case OPL:
            oplColor = newColor;
            ui->oplColorLabel->setPalette(myPalette);
            on_oplLayerCBox_stateChanged(true);
            break;
        case ONL:
            onlColor = newColor;
            ui->onlColorLabel->setPalette(myPalette);
            on_onlLayerCBox_stateChanged(true);
            break;
        case ELM:
            elmColor = newColor;
            ui->elmColorLabel->setPalette(myPalette);
            on_elmLayerCBox_stateChanged(true);
            break;
        case MEZ:
            mezColor = newColor;
            ui->mezColorLabel->setPalette(myPalette);
            on_mezLayerCBox_stateChanged(true);
            break;
        case IOS:
            iosColor = newColor;
            ui->iosColorLabel->setPalette(myPalette);
            on_iosLayerCBox_stateChanged(true);
            break;
        case RPE:
            rpeColor = newColor;
            ui->rpeColorLabel->setPalette(myPalette);
            on_rpeLayerCBox_stateChanged(true);
            break;
        case CHR:
            chrColor = newColor;
            ui->chrColorLabel->setPalette(myPalette);
            on_chrLayerCBox_stateChanged(true);
            break;
        default:
            break;
        }

        int graphID = getAllLayers().indexOf(selectedLayer);
        ui->bScanHCPlot->graph(graphID)->setPen(QPen(newColor));
        ui->bScanHCPlot->replot();
    }
}

QColor OCTAnnotate::getLayerColor(Layers layer){
    QColor color;

    switch (layer) {
    case PCV:
        color = pcvColor;
        break;
    case ERM:
        color = ermColor;
        break;
    case ILM:
        color = ilmColor;
        break;
    case GCL:
        color = gclColor;
        break;
    case IPL:
        color = iplColor;
        break;
    case INL:
        color = inlColor;
        break;
    case OPL:
        color = oplColor;
        break;
    case ONL:
        color = onlColor;
        break;
    case ELM:
        color = elmColor;
        break;
    case MEZ:
        color = mezColor;
        break;
    case IOS:
        color = iosColor;
        break;
    case RPE:
        color = rpeColor;
        break;
    case CHR:
        color = chrColor;
        break;
    default:
        break;
    }

    return color;
}

QList<Layers> OCTAnnotate::getLayersToDisplay(){
    QList<Layers> layers;
    if (ui->pcvLayerCBox->isChecked()){
        layers.append(PCV);
    }
    if (ui->ermLayerCBox->isChecked()){
        layers.append(ERM);
    }
    if (ui->ilmLayerCBox->isChecked()){
        layers.append(ILM);
    }
    if (ui->gclLayerCBox->isChecked()){
        layers.append(GCL);
    }
    if (ui->iplLayerCBox->isChecked()){
        layers.append(IPL);
    }
    if (ui->inlLayerCBox->isChecked()){
        layers.append(INL);
    }
    if (ui->oplLayerCBox->isChecked()){
        layers.append(OPL);
    }
    if (ui->onlLayerCBox->isChecked()){
        layers.append(ONL);
    }
    if (ui->elmLayerCBox->isChecked()){
        layers.append(ELM);
    }
    if (ui->mezLayerCBox->isChecked()){
        layers.append(MEZ);
    }
    if (ui->iosLayerCBox->isChecked()){
        layers.append(IOS);
    }
    if (ui->rpeLayerCBox->isChecked()){
        layers.append(RPE);
    }
    if (ui->chrLayerCBox->isChecked()){
        layers.append(CHR);
    }
    return layers;
}

bool OCTAnnotate::isLayerActive(Layers layer){
    bool isActive = false;
    switch (layer) {
        case PCV:
            if (ui->pcvLayerCBox->isChecked())
                isActive = true;
            break;
        case ERM:
            if (ui->ermLayerCBox->isChecked())
                isActive = true;
            break;
        case ILM:
            if (ui->ilmLayerCBox->isChecked())
                isActive = true;
            break;
        case GCL:
            if (ui->gclLayerCBox->isChecked())
                isActive = true;
            break;
        case IPL:
            if (ui->iplLayerCBox->isChecked())
                isActive = true;
            break;
        case INL:
            if (ui->inlLayerCBox->isChecked())
                isActive = true;
            break;
        case OPL:
            if (ui->oplLayerCBox->isChecked())
                isActive = true;
            break;
        case ONL:
            if (ui->onlLayerCBox->isChecked())
                isActive = true;
            break;
        case ELM:
            if (ui->elmLayerCBox->isChecked())
                isActive = true;
            break;
        case MEZ:
            if (ui->mezLayerCBox->isChecked())
                isActive = true;
            break;
        case IOS:
            if (ui->iosLayerCBox->isChecked())
                isActive = true;
            break;
        case RPE:
            if (ui->rpeLayerCBox->isChecked())
                isActive = true;
            break;
        case CHR:
            if (ui->chrLayerCBox->isChecked())
                isActive = true;
            break;
        default:
            break;
        }
    return isActive;
}


// layers display settings ------------------------------------------------------------------------
void OCTAnnotate::on_allLayersCBox_stateChanged(int state)
{
    if (state){
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

void OCTAnnotate::on_pcvLayerCBox_stateChanged(int state){
    if (state){ // pcv visible
        ui->bScanHCPlot->graph(getAllLayers().indexOf(PCV))->setVisible(true);
        ui->bScanHCPlot->replot();
        ui->bScanVCPlot->graph(getAllLayers().indexOf(PCV))->setVisible(true);
        ui->bScanVCPlot->replot();
        ui->pcvLayerRButton->setEnabled(true);
    } else { // pcv invisible
        ui->bScanHCPlot->graph(getAllLayers().indexOf(PCV))->setVisible(false);
        ui->bScanHCPlot->replot();
        ui->bScanVCPlot->graph(getAllLayers().indexOf(PCV))->setVisible(false);
        ui->bScanVCPlot->replot();
        ui->pcvLayerRButton->setEnabled(false);
    }
}

void OCTAnnotate::on_ermLayerCBox_stateChanged(int state){
    if (state){
        ui->bScanHCPlot->graph(getAllLayers().indexOf(ERM))->setVisible(true);
        ui->bScanHCPlot->replot();
        ui->bScanVCPlot->graph(getAllLayers().indexOf(ERM))->setVisible(true);
        ui->bScanVCPlot->replot();
        ui->ermLayerRButton->setEnabled(true);
    } else {
        ui->bScanHCPlot->graph(getAllLayers().indexOf(ERM))->setVisible(false);
        ui->bScanHCPlot->replot();
        ui->bScanVCPlot->graph(getAllLayers().indexOf(ERM))->setVisible(false);
        ui->bScanVCPlot->replot();
        ui->ermLayerRButton->setEnabled(false);
    }
}

void OCTAnnotate::on_ilmLayerCBox_stateChanged(int state) {
    if (state){ // ilm visible
        ui->bScanHCPlot->graph(getAllLayers().indexOf(ILM))->setVisible(true);
        ui->bScanHCPlot->replot();
        ui->bScanVCPlot->graph(getAllLayers().indexOf(ILM))->setVisible(true);
        ui->bScanVCPlot->replot();
        ui->ilmLayerRButton->setEnabled(true);
    } else { // ilm invisible
        ui->bScanHCPlot->graph(getAllLayers().indexOf(ILM))->setVisible(false);
        ui->bScanHCPlot->replot();
        ui->bScanVCPlot->graph(getAllLayers().indexOf(ILM))->setVisible(false);
        ui->bScanVCPlot->replot();
        ui->ilmLayerRButton->setEnabled(false);
    }
}

void OCTAnnotate::on_gclLayerCBox_stateChanged(int state){
    if (state){ // gcl visible
        ui->bScanHCPlot->graph(getAllLayers().indexOf(GCL))->setVisible(true);
        ui->bScanHCPlot->replot();
        ui->bScanVCPlot->graph(getAllLayers().indexOf(GCL))->setVisible(true);
        ui->bScanVCPlot->replot();
        ui->gclLayerRButton->setEnabled(true);
    } else { // gcl invisible
        ui->bScanHCPlot->graph(getAllLayers().indexOf(GCL))->setVisible(false);
        ui->bScanHCPlot->replot();
        ui->bScanVCPlot->graph(getAllLayers().indexOf(GCL))->setVisible(false);
        ui->bScanVCPlot->replot();
        ui->gclLayerRButton->setEnabled(false);
    }
}

void OCTAnnotate::on_iplLayerCBox_stateChanged(int state){
    if (state){ // ipl visible
        ui->bScanHCPlot->graph(getAllLayers().indexOf(IPL))->setVisible(true);
        ui->bScanHCPlot->replot();
        ui->bScanVCPlot->graph(getAllLayers().indexOf(IPL))->setVisible(true);
        ui->bScanVCPlot->replot();
        ui->iplLayerRButton->setEnabled(true);
    } else { // ipl invisible
        ui->bScanHCPlot->graph(getAllLayers().indexOf(IPL))->setVisible(false);
        ui->bScanHCPlot->replot();
        ui->bScanVCPlot->graph(getAllLayers().indexOf(IPL))->setVisible(false);
        ui->bScanVCPlot->replot();
        ui->iplLayerRButton->setEnabled(false);
    }
}

void OCTAnnotate::on_inlLayerCBox_stateChanged(int state){
    if (state){ // inl visible
        ui->bScanHCPlot->graph(getAllLayers().indexOf(INL))->setVisible(true);
        ui->bScanHCPlot->replot();
        ui->bScanVCPlot->graph(getAllLayers().indexOf(INL))->setVisible(true);
        ui->bScanVCPlot->replot();
        ui->inlLayerRButton->setEnabled(true);
    } else { // inl invisible
        ui->bScanHCPlot->graph(getAllLayers().indexOf(INL))->setVisible(false);
        ui->bScanHCPlot->replot();
        ui->bScanVCPlot->graph(getAllLayers().indexOf(INL))->setVisible(false);
        ui->bScanVCPlot->replot();
        ui->inlLayerRButton->setEnabled(false);
    }
}

void OCTAnnotate::on_oplLayerCBox_stateChanged(int state){
    if (state){ // opl visible
        ui->bScanHCPlot->graph(getAllLayers().indexOf(OPL))->setVisible(true);
        ui->bScanHCPlot->replot();
        ui->bScanVCPlot->graph(getAllLayers().indexOf(OPL))->setVisible(true);
        ui->bScanVCPlot->replot();
        ui->oplLayerRButton->setEnabled(true);
    } else { // opl invisible
        ui->bScanHCPlot->graph(getAllLayers().indexOf(OPL))->setVisible(false);
        ui->bScanHCPlot->replot();
        ui->bScanVCPlot->graph(getAllLayers().indexOf(OPL))->setVisible(false);
        ui->bScanVCPlot->replot();
        ui->oplLayerRButton->setEnabled(false);
    }
}

void OCTAnnotate::on_onlLayerCBox_stateChanged(int state){
    if (state){ // onl visible
        ui->bScanHCPlot->graph(getAllLayers().indexOf(ONL))->setVisible(true);
        ui->bScanHCPlot->replot();
        ui->bScanVCPlot->graph(getAllLayers().indexOf(ONL))->setVisible(true);
        ui->bScanVCPlot->replot();
        ui->onlLayerRButton->setEnabled(true);
    } else { // onl invisible
        ui->bScanHCPlot->graph(getAllLayers().indexOf(ONL))->setVisible(false);
        ui->bScanHCPlot->replot();
        ui->bScanVCPlot->graph(getAllLayers().indexOf(ONL))->setVisible(false);
        ui->bScanVCPlot->replot();
        ui->onlLayerRButton->setEnabled(false);
    }
}

void OCTAnnotate::on_elmLayerCBox_stateChanged(int state){
    if (state){ // elm visible
        ui->bScanHCPlot->graph(getAllLayers().indexOf(ELM))->setVisible(true);
        ui->bScanHCPlot->replot();
        ui->bScanVCPlot->graph(getAllLayers().indexOf(ELM))->setVisible(true);
        ui->bScanVCPlot->replot();
        ui->elmLayerRButton->setEnabled(true);
    } else { // elm invisible
        ui->bScanHCPlot->graph(getAllLayers().indexOf(ELM))->setVisible(false);
        ui->bScanHCPlot->replot();
        ui->bScanVCPlot->graph(getAllLayers().indexOf(ELM))->setVisible(false);
        ui->bScanVCPlot->replot();
        ui->elmLayerRButton->setEnabled(false);
    }
}

void OCTAnnotate::on_mezLayerCBox_stateChanged(int state){
    if (state){ // mez visible
        ui->bScanHCPlot->graph(getAllLayers().indexOf(MEZ))->setVisible(true);
        ui->bScanHCPlot->replot();
        ui->bScanVCPlot->graph(getAllLayers().indexOf(MEZ))->setVisible(true);
        ui->bScanVCPlot->replot();
        ui->mezLayerRButton->setEnabled(true);
    } else { // mez invisible
        ui->bScanHCPlot->graph(getAllLayers().indexOf(MEZ))->setVisible(false);
        ui->bScanHCPlot->replot();
        ui->bScanVCPlot->graph(getAllLayers().indexOf(MEZ))->setVisible(false);
        ui->bScanVCPlot->replot();
        ui->mezLayerRButton->setEnabled(false);
    }
}

void OCTAnnotate::on_iosLayerCBox_stateChanged(int state){
    if (state){ // ios visible
        ui->bScanHCPlot->graph(getAllLayers().indexOf(IOS))->setVisible(true);
        ui->bScanHCPlot->replot();
        ui->bScanVCPlot->graph(getAllLayers().indexOf(IOS))->setVisible(true);
        ui->bScanVCPlot->replot();
        ui->iosLayerRButton->setEnabled(true);
    } else { // ios invisible
        ui->bScanHCPlot->graph(getAllLayers().indexOf(IOS))->setVisible(false);
        ui->bScanHCPlot->replot();
        ui->bScanVCPlot->graph(getAllLayers().indexOf(IOS))->setVisible(false);
        ui->bScanVCPlot->replot();
        ui->iosLayerRButton->setEnabled(false);
    }
}

void OCTAnnotate::on_rpeLayerCBox_stateChanged(int state){
    if (state){ // rpe visible
        ui->bScanHCPlot->graph(getAllLayers().indexOf(RPE))->setVisible(true);
        ui->bScanHCPlot->replot();
        ui->bScanVCPlot->graph(getAllLayers().indexOf(RPE))->setVisible(true);
        ui->bScanVCPlot->replot();
        ui->rpeLayerRButton->setEnabled(true);
    } else { // rpe invisible
        ui->bScanHCPlot->graph(getAllLayers().indexOf(RPE))->setVisible(false);
        ui->bScanHCPlot->replot();
        ui->bScanVCPlot->graph(getAllLayers().indexOf(RPE))->setVisible(false);
        ui->bScanVCPlot->replot();
        ui->rpeLayerRButton->setEnabled(false);
    }
}

void OCTAnnotate::on_chrLayerCBox_stateChanged(int state){
    if (state){ // chr visible
        ui->bScanHCPlot->graph(getAllLayers().indexOf(CHR))->setVisible(true);
        ui->bScanHCPlot->replot();
        ui->bScanVCPlot->graph(getAllLayers().indexOf(CHR))->setVisible(true);
        ui->bScanVCPlot->replot();
        ui->chrLayerRButton->setEnabled(true);
    } else { // chr invisible
        ui->bScanHCPlot->graph(getAllLayers().indexOf(CHR))->setVisible(false);
        ui->bScanHCPlot->replot();
        ui->bScanVCPlot->graph(getAllLayers().indexOf(CHR))->setVisible(false);
        ui->bScanVCPlot->replot();
        ui->chrLayerRButton->setEnabled(false);
    }
}

void OCTAnnotate::on_pcvLayerRButton_clicked()
{
    if (ui->pcvLayerRButton->isChecked())
        selectedLayer = PCV;
}

void OCTAnnotate::on_ermLayerRButton_clicked()
{
    if (ui->ermLayerRButton->isChecked())
        selectedLayer = ERM;
}

void OCTAnnotate::on_ilmLayerRButton_clicked()
{
    if (ui->ilmLayerRButton->isChecked())
        selectedLayer = ILM;
}

void OCTAnnotate::on_gclLayerRButton_clicked()
{
    if (ui->gclLayerRButton->isChecked())
        selectedLayer = GCL;
}

void OCTAnnotate::on_iplLayerRButton_clicked()
{
    if (ui->iplLayerRButton->isChecked())
        selectedLayer = IPL;
}

void OCTAnnotate::on_inlLayerRButton_clicked()
{
    if (ui->inlLayerRButton->isChecked())
        selectedLayer = INL;
}

void OCTAnnotate::on_oplLayerRButton_clicked()
{
    if (ui->oplLayerRButton->isChecked())
        selectedLayer = OPL;
}

void OCTAnnotate::on_onlLayerRButton_clicked()
{
    if (ui->onlLayerRButton->isChecked())
        selectedLayer = ONL;
}

void OCTAnnotate::on_elmLayerRButton_clicked()
{
    if (ui->elmLayerRButton->isChecked())
        selectedLayer = ELM;
}

void OCTAnnotate::on_mezLayerRButton_clicked()
{
    if (ui->mezLayerRButton->isChecked())
        selectedLayer = MEZ;
}

void OCTAnnotate::on_iosLayerRButton_clicked()
{
    if (ui->iosLayerRButton->isChecked())
        selectedLayer = IOS;
}

void OCTAnnotate::on_rpeLayerRButton_clicked()
{
    if (ui->rpeLayerRButton->isChecked())
        selectedLayer = RPE;
}

void OCTAnnotate::on_chrLayerRButton_clicked()
{
    if (ui->chrLayerRButton->isChecked())
        selectedLayer = CHR;
}


// bscan scale / zoom -----------------------------------------------------------------------------
void OCTAnnotate::on_zoomInButton_clicked()
{
    if (!patientData.getImageFileList().isEmpty()){
        scaleFactor++;
        rescaleImage();
    }
}

void OCTAnnotate::on_zoomOutButton_clicked()
{
    if (!patientData.getImageFileList().isEmpty()){
        scaleFactor--;
        rescaleImage();
    }
}

void OCTAnnotate::rescaleImage(){
    scaleFactor = qBound(0,scaleFactor,scales.count()-1);

    int imageHeight = patientData.getBscanHeight() / scales[scaleFactor];
    double y2 = (int)((double)(patientData.getBscanHeight() - imageHeight) / 2.0);
    double y1 = (double)patientData.getBscanHeight() - y2;
    double offset = 0;//bscanRange.lower;
    bscanRange = QCPRange(y1-offset,y2-offset);
    ui->bScanHCPlot->yAxis->setRange(bscanRange);
    ui->bScanVCPlot->yAxis->setRange(bscanRange);

    QImage image(patientData.getImageFileList().at(currentImageNumber));
    QImage normalImage = patientData.getNormalImage(currentNormalImageNumber);

    Calculate *calc = new Calculate();
    calc->imageEnhancement(&image, contrast, brightness);
    calc->imageEnhancement(&normalImage, contrast, brightness);

    if (flattenImage){
        QList<int> flatDiff, flatDiffNormal;
        if (patientData.hasManualAnnotations()){
            flatDiff = patientData.getFlatDifferencesRPE(currentImageNumber);
            flatDiffNormal = patientData.getFlatDifferencesNormalRPE(currentNormalImageNumber);
        } else {
            flatDiff = patientData.getFlatDifferences(currentImageNumber);
            flatDiffNormal = patientData.getFlatDifferencesNormal(currentNormalImageNumber);
        }
        image = calc->flattenImage(&image, flatDiff);
        normalImage = calc->flattenImage(&normalImage, flatDiffNormal);
    }

    QImage newImage = image.copy(0,bscanRange.lower,patientData.getBscanWidth(),imageHeight);
    ui->bScanHCPlot->axisRect()->setBackground(QPixmap::fromImage(newImage),true,Qt::IgnoreAspectRatio);
    ui->bScanHCPlot->replot();

    QImage newNormalImage = normalImage.copy(0,bscanRange.lower,patientData.getBscansNumber(),imageHeight);
    ui->bScanVCPlot->axisRect()->setBackground(QPixmap::fromImage(newNormalImage),true,Qt::IgnoreAspectRatio);
    ui->bScanVCPlot->replot();

    if (scaleFactor <= 0){
        ui->zoomOutButton->setEnabled(false);
    } else {
        ui->zoomOutButton->setEnabled(true);
    }
    if (scaleFactor >= scales.count()-1){
        ui->zoomInButton->setEnabled(false);
    } else {
        ui->zoomInButton->setEnabled(true);
    }
}

void OCTAnnotate::changeImageRange(int dir){
    int imageHeight = patientData.getBscanHeight() / scales[scaleFactor];
    QCPRange plotRange = ui->bScanHCPlot->yAxis->range();
    QCPRange newRange;
    bool rescale = false;

    if ((dir > 0) && (plotRange.upper < patientData.getBscanHeight())){
        double upper = qBound(0.0, plotRange.upper + 20, (double)patientData.getBscanHeight());
        newRange = QCPRange(upper - imageHeight, upper);
        rescale = true;
    } else if ((dir < 0) && (plotRange.lower > 0)){
        double lower = qBound(0.0, plotRange.lower - 20, (double)patientData.getBscanHeight());
        newRange = QCPRange(lower, lower + imageHeight);
        rescale = true;
    }

    if (rescale){
        ui->bScanHCPlot->yAxis->setRange(newRange);
        ui->bScanVCPlot->yAxis->setRange(newRange);
        bscanRange = newRange;

        Calculate *calc = new Calculate();
        QImage image(patientData.getImageFileList().at(currentImageNumber));
        QImage normalImage = patientData.getNormalImage(currentNormalImageNumber);

        calc->imageEnhancement(&image, contrast, brightness);
        calc->imageEnhancement(&normalImage, contrast, brightness);

        if (flattenImage){
            QList<int> flatDiff, flatDiffNormal;
            if (patientData.hasManualAnnotations()){
                flatDiff = patientData.getFlatDifferencesRPE(currentImageNumber);
                flatDiffNormal = patientData.getFlatDifferencesNormalRPE(currentNormalImageNumber);
            } else {
                flatDiff = patientData.getFlatDifferences(currentImageNumber);
                flatDiffNormal = patientData.getFlatDifferencesNormal(currentNormalImageNumber);
            }
            image = calc->flattenImage(&image, flatDiff);
            normalImage = calc->flattenImage(&normalImage, flatDiffNormal);
        }

        double dy = qBound(0, patientData.getBscanHeight() - (int)newRange.upper, patientData.getBscanHeight()-imageHeight);
        QImage newImage = image.copy(0, dy, patientData.getBscanWidth(), imageHeight);
        ui->bScanHCPlot->axisRect()->setBackground(QPixmap::fromImage(newImage),true,Qt::IgnoreAspectRatio);
        ui->bScanHCPlot->replot();

        QImage newNormalImage = normalImage.copy(0,dy,patientData.getBscansNumber(),imageHeight);
        ui->bScanVCPlot->axisRect()->setBackground(QPixmap::fromImage(newNormalImage),true,Qt::IgnoreAspectRatio);
        ui->bScanVCPlot->replot();
    }
}

void OCTAnnotate::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep()/2)));
}


// general data edit ------------------------------------------------------------------------------
void OCTAnnotate::on_visOPLEdit_textEdited()
{
    QRegExp rx(",");
    ui->visOPLEdit->setText(ui->visOPLEdit->text().replace(rx, "."));
    generalDataModified = true;
}

void OCTAnnotate::on_visOLLEdit_textEdited()
{
    QRegExp rx(",");
    ui->visOLLEdit->setText(ui->visOLLEdit->text().replace(rx, "."));
    generalDataModified = true;
}

void OCTAnnotate::on_snOPLEdit_textEdited()
{
    QRegExp rx(",");
    ui->snOPLEdit->setText(ui->snOPLEdit->text().replace(rx, "."));
    generalDataModified = true;
}

void OCTAnnotate::on_snOLLEdit_textEdited()
{
    QRegExp rx(",");
    ui->snOLLEdit->setText(ui->snOLLEdit->text().replace(rx, "."));
    generalDataModified = true;
}

// m-charts edit ----------------------------------------------------------------------------------
void OCTAnnotate::on_mcVOPLEdit_textChanged()
{
    generalDataModified = true;
}

void OCTAnnotate::on_mcHOPLEdit_textChanged()
{
    generalDataModified = true;
}

void OCTAnnotate::on_mcVOLLEdit_textChanged()
{
    generalDataModified = true;
}

void OCTAnnotate::on_mcHOLLEdit_textChanged()
{
    generalDataModified = true;
}


// virtual map and layers plot --------------------------------------------------------------------
void OCTAnnotate::setupVirtualMapPlot(QCustomPlot *customPlot){
    customPlot->clearPlottables();

    // axis
    customPlot->xAxis->setLabel("Horizontal distance from scan center [mm]");
    customPlot->yAxis->setLabel("Vertical distance from scan center [mm]");
    //customPlot->xAxis->setLabel("Odległość od środka skanu w kierunku poziomym");
    //customPlot->yAxis->setLabel("Odległość od środka skanu w kierunku pionowym");
    customPlot->xAxis->setLabelFont(QFont("Arial",16));
    customPlot->yAxis->setLabelFont(QFont("Arial",16));
    QPoint center = patientData.getScanCenter();
    int scanWidth = (int)patientData.getVoxelWidth();
    if (center.x() != -1){
        int m = patientData.getBscanWidth();
        double x1 = -(double)(scanWidth * (center.x() - (double)m/2))/m - scanWidth/2;
        double x2 = -(double)(scanWidth * (center.x() - (double)m/2))/m + scanWidth/2;
        int n = patientData.getBscansNumber();
        double y1 = (double)(scanWidth * (center.y() - (double)n/2))/n - scanWidth/2;
        double y2 = (double)(scanWidth * (center.y() - (double)n/2))/n + scanWidth/2;
        customPlot->xAxis->setRange(x1,x2);
        customPlot->yAxis->setRange(y1,y2);
    } else {
        customPlot->xAxis->setRange(-4,4);
        customPlot->yAxis->setRange(-4,4);
    }
    customPlot->xAxis->setLabelPadding(3);
    customPlot->yAxis->setLabelPadding(3);
    customPlot->xAxis->setTickLabelFont(QFont("Arial",12));
    customPlot->yAxis->setTickLabelFont(QFont("Arial",12));

    customPlot->xAxis2->setLabel("B-scan pixel");
    customPlot->yAxis2->setLabel("B-scan number");
    //customPlot->xAxis2->setLabel("Piksel obrazu B-skan");
    //customPlot->yAxis2->setLabel("Numer obazu B-skan");
    customPlot->xAxis2->setLabelFont(QFont("Arial",16));
    customPlot->yAxis2->setLabelFont(QFont("Arial",16));
    if (!patientData.getImageFileList().isEmpty()){
        customPlot->xAxis2->setRange(0,patientData.getBscanWidth());
        customPlot->yAxis2->setRange(-patientData.getBscansNumber(),0);
    } else {
        customPlot->xAxis2->setRange(0,800);
        customPlot->yAxis2->setRange(-100,0);
    }
    customPlot->yAxis2->setAutoTickStep(false);
    customPlot->yAxis2->setTickStep(10);
    customPlot->xAxis2->setVisible(true);
    customPlot->yAxis2->setVisible(true);
    customPlot->xAxis2->setLabelPadding(3);
    customPlot->yAxis2->setLabelPadding(3);
    customPlot->xAxis2->setTickLabelFont(QFont("Arial",12));
    customPlot->yAxis2->setTickLabelFont(QFont("Arial",12));
}

void OCTAnnotate::setupImageLayersPlot(){
    ui->imageLayersCPlot->clearGraphs();

    // configure axis
    ui->imageLayersCPlot->xAxis->setLabel("B-scan pixel (horizontal direction)");
    ui->imageLayersCPlot->yAxis->setLabel("B-scan pixel (vertical direction)");
    if (!patientData.getImageFileList().isEmpty()){
        ui->imageLayersCPlot->xAxis->setRange(0,patientData.getBscanWidth());
        ui->imageLayersCPlot->yAxis->setRange(0,patientData.getBscanHeight());
    } else {
        ui->imageLayersCPlot->xAxis->setRange(0,800);
        ui->imageLayersCPlot->yAxis->setRange(0,1010);
    }
    ui->imageLayersCPlot->xAxis2->setLabel("Horizontal distance from center [mm]");
    ui->imageLayersCPlot->yAxis2->setLabel("Scan width [mm]");

    int scanWidth = patientData.getVoxelWidth();
    QPoint center = patientData.getScanCenter();
    if ((center.x() != -1) && (center.y() != -1)){
        int m = patientData.getBscanWidth();
        double x1 = -(double)(scanWidth * (center.x() - (double)m/2))/m - scanWidth/2;
        double x2 = -(double)(scanWidth * (center.x() - (double)m/2))/m + scanWidth/2;
        ui->imageLayersCPlot->xAxis2->setRange(x1,x2);
    } else {
        ui->imageLayersCPlot->xAxis2->setRange(-scanWidth/2,scanWidth/2);
    }
    ui->imageLayersCPlot->yAxis2->setRange(0,patientData.getVoxelDepth());
    ui->imageLayersCPlot->xAxis2->setVisible(true);
    ui->imageLayersCPlot->yAxis2->setVisible(true);

    ui->imageLayersCPlot->xAxis->grid()->setVisible(false);
    ui->imageLayersCPlot->yAxis->grid()->setVisible(false);

    // legend
//    ui->imageLayersCPlot->legend->setVisible(true);
//    QFont legendFont = font();  // start out with MainWindow's font..
//    legendFont.setPointSize(8); // and make a bit smaller for legend
//    ui->imageLayersCPlot->legend->setFont(legendFont);
//    ui->imageLayersCPlot->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignBottom|Qt::AlignRight);
//    ui->imageLayersCPlot->legend->setAutoMargins(QCP::msNone);

    // add graphs
    ui->imageLayersCPlot->addGraph();
    ui->imageLayersCPlot->graph(0)->setPen(QPen(Qt::red));
    ui->imageLayersCPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->imageLayersCPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,1.5));
//    ui->imageLayersCPlot->graph(0)->setName("manual annotation");
    ui->imageLayersCPlot->addGraph();
    ui->imageLayersCPlot->graph(1)->setPen(QPen(Qt::blue));
    ui->imageLayersCPlot->graph(1)->setLineStyle(QCPGraph::lsNone);
    ui->imageLayersCPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,1.5));
    ui->imageLayersCPlot->addGraph();
    ui->imageLayersCPlot->graph(2)->setPen(QPen(QColor(255,127,0)));
    ui->imageLayersCPlot->graph(2)->setLineStyle(QCPGraph::lsNone);
    ui->imageLayersCPlot->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,1.5));
    ui->imageLayersCPlot->addGraph();
    ui->imageLayersCPlot->graph(3)->setPen(QPen(Qt::yellow));
    ui->imageLayersCPlot->graph(3)->setLineStyle(QCPGraph::lsNone);
    ui->imageLayersCPlot->graph(3)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,1.5));
    ui->imageLayersCPlot->addGraph();
    ui->imageLayersCPlot->graph(4)->setPen(QPen(Qt::darkRed));
    ui->imageLayersCPlot->graph(4)->setLineStyle(QCPGraph::lsNone);
    ui->imageLayersCPlot->graph(4)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,1.5));
    ui->imageLayersCPlot->addGraph();
    ui->imageLayersCPlot->graph(5)->setPen(QPen(Qt::green));
    ui->imageLayersCPlot->graph(5)->setLineStyle(QCPGraph::lsNone);
    ui->imageLayersCPlot->graph(5)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,1.5));
    ui->imageLayersCPlot->addGraph();
    ui->imageLayersCPlot->graph(6)->setPen(QPen(QColor(255,127,0)));
    ui->imageLayersCPlot->graph(6)->setLineStyle(QCPGraph::lsNone);
    ui->imageLayersCPlot->graph(6)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,1.5));

//    ui->imageLayersCPlot->addGraph();
//    ui->imageLayersCPlot->graph(1)->setPen(QPen(Qt::green)); // line color red for second graph
//    ui->imageLayersCPlot->graph(1)->setLineStyle(QCPGraph::lsNone);
//    ui->imageLayersCPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,3));
//    ui->imageLayersCPlot->graph(1)->setName("auto annotation");

//    ui->imageLayersCPlot->addGraph();
//    ui->imageLayersCPlot->graph(2)->setPen(QPen(Qt::blue)); // line color blue for first graph
//    ui->imageLayersCPlot->graph(2)->setLineStyle(QCPGraph::lsNone);
//    ui->imageLayersCPlot->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,2));
//    ui->imageLayersCPlot->graph(2)->setName("ILM manual annotation");
//    ui->imageLayersCPlot->addGraph();
//    ui->imageLayersCPlot->graph(3)->setPen(QPen(Qt::cyan)); // line color red for second graph
//    ui->imageLayersCPlot->graph(3)->setLineStyle(QCPGraph::lsNone);
//    ui->imageLayersCPlot->graph(3)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,2));
//    ui->imageLayersCPlot->graph(3)->setName("ILM auto annotation");
}

void OCTAnnotate::setupBScanPlots(){
    ui->bScanHCPlot->clearGraphs();

    // configure axis
    ui->bScanHCPlot->xAxis->setLabel("B-scan pixel (horizontal direction)");
    ui->bScanHCPlot->yAxis->setLabel("B-scan pixel (vertical direction)");
    if (!patientData.getImageFileList().isEmpty()){
        ui->bScanHCPlot->xAxis->setRange(0,patientData.getBscanWidth());
        ui->bScanHCPlot->yAxis->setRange(0,patientData.getBscanHeight());
    } else {
        ui->bScanHCPlot->xAxis->setRange(0,800);
        ui->bScanHCPlot->yAxis->setRange(0,1010);
    }

    ui->bScanHCPlot->xAxis->grid()->setVisible(false);
    ui->bScanHCPlot->yAxis->grid()->setVisible(false);

    // add graphs
    QList<Layers> allLayers = getAllLayers();
    int graphID = 0;
    foreach (Layers layer, allLayers) {
        ui->bScanHCPlot->addGraph();
        ui->bScanHCPlot->graph(graphID)->setPen(QPen(getLayerColor(layer)));
        ui->bScanHCPlot->graph(graphID)->setLineStyle(QCPGraph::lsLine);
        ui->bScanHCPlot->graph(graphID)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,1.5)); // 3.5 do publikacji
        graphID++;
    }

    // normal image
    ui->bScanVCPlot->clearGraphs();

    ui->bScanVCPlot->xAxis->setLabel("B-scan cross-section");
    ui->bScanVCPlot->yAxis->setLabel("B-scan pixel (vertical direction)");
    if (!patientData.getImageFileList().isEmpty()){
        ui->bScanVCPlot->xAxis->setRange(0,patientData.getBscansNumber());
        ui->bScanVCPlot->yAxis->setRange(0,patientData.getBscanHeight());
    } else {
        ui->bScanVCPlot->xAxis->setRange(0,100);
        ui->bScanVCPlot->yAxis->setRange(0,1010);
    }

    ui->bScanVCPlot->xAxis->grid()->setVisible(false);
    ui->bScanVCPlot->yAxis->grid()->setVisible(false);

    graphID = 0;
    foreach (Layers layer, allLayers) {
        ui->bScanVCPlot->addGraph();
        ui->bScanVCPlot->graph(graphID)->setPen(QPen(getLayerColor(layer)));
        ui->bScanVCPlot->graph(graphID)->setLineStyle(QCPGraph::lsLine);
        ui->bScanVCPlot->graph(graphID)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,1.5));
        graphID++;
    }
}

void OCTAnnotate::setupCircProfilePlot(){
    ui->virtualMapCircProfileCPlot->clearGraphs();

    // configure axis
    //ui->virtualMapCircProfileCPlot->xAxis->setLabel("B-scan pixel (horizontal direction)");
    ui->virtualMapCircProfileCPlot->yAxis->setLabel("Virtual Map value [um]");
    ui->virtualMapCircProfileCPlot->xAxis->setRange(0,360);
    ui->virtualMapCircProfileCPlot->yAxis->setRange(0,700);
    QVector<QString> labels;
    labels << "T" << "" << "S" << "" << "N" << "" << "I" << "" << "T";
    ui->virtualMapCircProfileCPlot->xAxis->setAutoTickStep(false);
    ui->virtualMapCircProfileCPlot->xAxis->setAutoTickLabels(false);
    ui->virtualMapCircProfileCPlot->xAxis->setTickStep(45);
    ui->virtualMapCircProfileCPlot->xAxis->setTickVectorLabels(labels);

    ui->virtualMapCircProfileCPlot->addGraph();
    ui->virtualMapCircProfileCPlot->graph(0)->setPen(QPen(Qt::black,1,Qt::SolidLine));
    //ui->virtualMapCircProfileCPlot->graph(0)->setLineStyle(QCPGraph::lsDiscontinuousLine);
    ui->virtualMapCircProfileCPlot->graph(0)->setName("Circular Profile d=1[mm]");
    ui->virtualMapCircProfileCPlot->addGraph();
    ui->virtualMapCircProfileCPlot->graph(1)->setPen(QPen(Qt::black,1,Qt::DashLine));
    //ui->virtualMapCircProfileCPlot->graph(1)->setLineStyle(QCPGraph::lsLine);
    ui->virtualMapCircProfileCPlot->graph(1)->setName("Circular Profile d=3[mm]");
    ui->virtualMapCircProfileCPlot->addGraph();
    ui->virtualMapCircProfileCPlot->graph(2)->setPen(QPen(Qt::black,1,Qt::DotLine));
    //ui->virtualMapCircProfileCPlot->graph(2)->setLineStyle(QCPGraph::lsLine);
    ui->virtualMapCircProfileCPlot->graph(2)->setName("Circular Profile d=6[mm]");


    ui->virtualMapCircProfileCPlot->clearItems();
    QColor rect50color = Qt::black;
    rect50color.setAlpha(32);
    QCPItemRect *rect50 = new QCPItemRect(ui->virtualMapCircProfileCPlot);
    ui->virtualMapCircProfileCPlot->addItem(rect50);
    rect50->setBrush(QBrush(rect50color));
    rect50->setPen(Qt::NoPen);
    rect50->topLeft->setCoords(0,50);
    rect50->bottomRight->setCoords(360,0);

    QColor rect100color = Qt::blue;
    rect100color.setAlpha(32);
    QCPItemRect *rect100 = new QCPItemRect(ui->virtualMapCircProfileCPlot);
    ui->virtualMapCircProfileCPlot->addItem(rect100);
    rect100->setBrush(QBrush(rect100color));
    rect100->setPen(Qt::NoPen);
    rect100->topLeft->setCoords(0,100);
    rect100->bottomRight->setCoords(360,50);

    QColor rect300color = Qt::green;
    rect300color.setAlpha(32);
    QCPItemRect *rect300 = new QCPItemRect(ui->virtualMapCircProfileCPlot);
    ui->virtualMapCircProfileCPlot->addItem(rect300);
    rect300->setBrush(QBrush(rect300color));
    rect300->setPen(Qt::NoPen);
    rect300->topLeft->setCoords(0,300);
    rect300->bottomRight->setCoords(360,100);

    QColor rect500color = Qt::yellow;
    rect500color.setAlpha(32);
    QCPItemRect *rect500 = new QCPItemRect(ui->virtualMapCircProfileCPlot);
    ui->virtualMapCircProfileCPlot->addItem(rect500);
    rect500->setBrush(QBrush(rect500color));
    rect500->setPen(Qt::NoPen);
    rect500->topLeft->setCoords(0,500);
    rect500->bottomRight->setCoords(360,300);

    QColor rect700color = Qt::red;
    rect700color.setAlpha(32);
    QCPItemRect *rect700 = new QCPItemRect(ui->virtualMapCircProfileCPlot);
    ui->virtualMapCircProfileCPlot->addItem(rect700);
    rect700->setBrush(QBrush(rect700color));
    rect700->setPen(Qt::NoPen);
    rect700->topLeft->setCoords(0,700);
    rect700->bottomRight->setCoords(360,500);
}

void OCTAnnotate::setupHistPlot(){
    ui->virtualMapHistCPlot->clearPlottables();
    histBars = new QCPBars(ui->virtualMapHistCPlot->xAxis, ui->virtualMapHistCPlot->yAxis);
    ui->virtualMapHistCPlot->addPlottable(histBars);

    QPen pen;
    pen.setWidthF(1.2);
    histBars->setName("Virtual Map Value");
    pen.setColor(QColor(1,92,191));
    histBars->setPen(pen);
    histBars->setBrush(QColor(1,92,191,50));

    QVector<double> ticks;
    QVector<QString> labels;
    ticks << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8;
    labels << "0-100" << "100-200" << "200-300" << "300-400" << "400-500" << "500-600" << "600-700" << "> 700";
    ui->virtualMapHistCPlot->xAxis->setAutoTicks(false);
    ui->virtualMapHistCPlot->xAxis->setAutoTickLabels(false);
    ui->virtualMapHistCPlot->xAxis->setTickVector(ticks);
    ui->virtualMapHistCPlot->xAxis->setTickVectorLabels(labels);
    ui->virtualMapHistCPlot->xAxis->setSubTickCount(0);
    ui->virtualMapHistCPlot->xAxis->setRange(0.5,8.5);
    ui->virtualMapHistCPlot->xAxis->setLabel("Distance between layers in virtual map [um]");

    ui->virtualMapHistCPlot->yAxis->setRange(0,50000);
    ui->virtualMapHistCPlot->yAxis->setLabel("Number of points in virtual map");
}

void OCTAnnotate::setupVolumeGridPlot(){
    ui->ETDRSgridCPlot->clearItems();

    ui->ETDRSgridCPlot->xAxis->setRange(-3.2,3.2);
    ui->ETDRSgridCPlot->yAxis->setRange(-3.2,3.2);

    // draw CF circle
    QCPItemEllipse *ellipseCF = new QCPItemEllipse(ui->ETDRSgridCPlot);
    ui->ETDRSgridCPlot->addItem(ellipseCF);
    ellipseCF->setPen(QPen(Qt::black,3.0));
    double radius = 1.0/2.0;
    ellipseCF->topLeft->setCoords(-radius, radius);
    ellipseCF->bottomRight->setCoords(radius, -radius);
    // draw IM circle
    QCPItemEllipse *ellipseIM = new QCPItemEllipse(ui->ETDRSgridCPlot);
    ui->ETDRSgridCPlot->addItem(ellipseIM);
    ellipseIM->setPen(QPen(Qt::black,3.0));
    radius = 3.0/2.0;
    ellipseIM->topLeft->setCoords(-radius, radius);
    ellipseIM->bottomRight->setCoords(radius, -radius);
    // draw OM circle
    QCPItemEllipse *ellipseOM = new QCPItemEllipse(ui->ETDRSgridCPlot);
    ui->ETDRSgridCPlot->addItem(ellipseOM);
    ellipseOM->setPen(QPen(Qt::black,3.0));
    radius = 6.0/2.0;
    ellipseOM->topLeft->setCoords(-radius, radius);
    ellipseOM->bottomRight->setCoords(radius, -radius);
    //lines
    QCPItemLine *line1 = new QCPItemLine(ui->ETDRSgridCPlot);
    QCPItemLine *line2 = new QCPItemLine(ui->ETDRSgridCPlot);
    QCPItemLine *line3 = new QCPItemLine(ui->ETDRSgridCPlot);
    QCPItemLine *line4 = new QCPItemLine(ui->ETDRSgridCPlot);
    ui->ETDRSgridCPlot->addItem(line1);
    ui->ETDRSgridCPlot->addItem(line2);
    ui->ETDRSgridCPlot->addItem(line3);
    ui->ETDRSgridCPlot->addItem(line4);
    line1->setPen(QPen(Qt::black,3.0));
    line2->setPen(QPen(Qt::black,3.0));
    line3->setPen(QPen(Qt::black,3.0));
    line4->setPen(QPen(Qt::black,3.0));
    double alfa = 2 * 3.1415926535 * 45 / 360.0;
    line1->start->setCoords(qCos(alfa)*0.5,qSin(alfa)*0.5);
    line1->end->setCoords(qCos(alfa)*3.0,qSin(alfa)*3.0);
    alfa = 2 * 3.1415926535 * 135 / 360.0;
    line2->start->setCoords(qCos(alfa)*0.5,qSin(alfa)*0.5);
    line2->end->setCoords(qCos(alfa)*3.0,qSin(alfa)*3.0);
    alfa = 2 * 3.1415926535 * 225 / 360.0;
    line3->start->setCoords(qCos(alfa)*0.5,qSin(alfa)*0.5);
    line3->end->setCoords(qCos(alfa)*3.0,qSin(alfa)*3.0);
    alfa = 2 * 3.1415926535 * 315 / 360.0;
    line4->start->setCoords(qCos(alfa)*0.5,qSin(alfa)*0.5);
    line4->end->setCoords(qCos(alfa)*3.0,qSin(alfa)*3.0);

    ui->ETDRSgridCPlot->xAxis->setVisible(false);
    ui->ETDRSgridCPlot->yAxis->setVisible(false);
}

void OCTAnnotate::resetImageLayersLabels(){
    QPoint center = patientData.getScanCenter();

    int scanWidth = (int)patientData.getVoxelWidth();
    int m = patientData.getBscanWidth();
    double x1 = -(double)(scanWidth * (center.x() - (double)m/2))/m - scanWidth/2;
    double x2 = -(double)(scanWidth * (center.x() - (double)m/2))/m + scanWidth/2;
    QVector<QString> labels;
    for (int i=0; i<8; i++){
        labels << QString::number(x1 + i, 'f', 1);
    }
    labels << QString::number(x2, 'f', 1);
    ui->imageLayersCPlot->xAxis2->setTickVectorLabels(labels);
    ui->imageLayersCPlot->xAxis2->setAutoTickLabels(false);

    ui->imageLayersCPlot->replot();
}

void OCTAnnotate::on_computeVirtualMapButton_clicked()
{
    patientData.computeVirtualMap(vMapLayer1, vMapLayer2);
    patientData.computeVirtualMapAuto(vMapLayer1, vMapLayer2);
    // TODO: pobrac z ustawien, dla ktorych warstw ma byc liczony blad
    QList<Layers> layerList = getAllLayers();

    foreach (Layers layer, layerList) {
        patientData.computeLayerError(layer);
    }
//    patientData.computeErrorAll();
    patientData.computeVirtualMapError();

    on_tabWidget_currentChanged();  // refresh images
}

void OCTAnnotate::displayVirtualMap(QCustomPlot *customPlot, bool isAuto){
    customPlot->clearPlottables();

    if (!patientData.getImageFileList().isEmpty()){
        // display virtual map
        QCPColorMap *colorMap = new QCPColorMap(customPlot->xAxis, customPlot->yAxis);
        customPlot->addPlottable(colorMap);
        int nx = patientData.getBscanWidth();
        int ny = patientData.getBscansNumber();
        colorMap->data()->setSize(nx, ny); // we want the color map to have nx * ny data points

        int scanWidth = (int)patientData.getVoxelWidth();
        int scanHeight = (int)patientData.getVoxelHeight();
        QCPRange xRange = QCPRange(-scanWidth/2,scanWidth/2);
        QCPRange yRange = QCPRange(-scanHeight/2,scanHeight/2);

        QPoint center = patientData.getScanCenter();
        if ((center.x() != -1) && (center.y() != -1)){
            double deltaX = patientData.getVoxelWidth() / (patientData.getBscanWidth() - 1);
            double deltaY = patientData.getVoxelHeight() / (patientData.getBscansNumber() - 1);
            double x1 = - center.x() * deltaX;
            double x2 = patientData.getVoxelWidth() + x1;
            double y1 = - (patientData.getBscansNumber() - center.y()) * deltaY;
            double y2 = patientData.getVoxelHeight() + y1;
            xRange = QCPRange(x1, x2);
            yRange = QCPRange(y1, y2);
        }
        colorMap->data()->setRange(xRange, yRange); // and span the coordinate range -4..4 in both key (x) and value (y) dimensions

        int z;
        for (int xIndex=0; xIndex<nx; ++xIndex)
        {
            for (int yIndex=0; yIndex<ny; ++yIndex)
            {
                if (isAuto)
                    z = patientData.getVirtualMapAutoValue(xIndex, ny-yIndex-1, "um");
                else
                    z = patientData.getVirtualMapValue(xIndex, ny-yIndex-1, "um");
                colorMap->data()->setCell(xIndex, yIndex, z);
            }
        }
        QCPColorScale *colorScale = new QCPColorScale(customPlot);
        int c = customPlot->plotLayout()->elementCount();
        if (c > 1)
            customPlot->plotLayout()->remove(customPlot->plotLayout()->element(1,0));
        customPlot->plotLayout()->addElement(1, 0, colorScale); // add it to the right of the main axis rect
        colorScale->setType(QCPAxis::atBottom); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
        colorMap->setColorScale(colorScale); // associate the color map with the color scale
        colorScale->axis()->setLabel("Distance between layers [um]");
        //colorScale->axis()->setLabel("Odległość pomiędzy warstwami [um]");
        colorScale->axis()->setLabelFont(QFont("Arial",16));
        colorScale->axis()->setTickLabelFont(QFont("Arial",12));
        colorMap->setGradient(QCPColorGradient::gpJet);
        colorMap->setDataRange(QCPRange(0,700));

        // draw B-scan line
        customPlot->clearItems();
        QCPItemLine *line = new QCPItemLine(customPlot);
        customPlot->addItem(line);
        line->setPen(QPen(Qt::green));
        int n = patientData.getBscansNumber();
        double y1 = yRange.lower;
        double y2 = yRange.upper;
        double y = -((double)(y2 - y1)/n * (currentImageNumber) - y2);
        double x1 = xRange.lower;
        double x2 = xRange.upper;
        line->start->setCoords(x1,y);
        line->end->setCoords(x2,y);

        if (showETDRSGrid){
            // draw CF circle
            QCPItemEllipse *ellipseCF = new QCPItemEllipse(customPlot);
            customPlot->addItem(ellipseCF);
            ellipseCF->setPen(QPen(Qt::black));
            double radius = 1.0/2.0;
            ellipseCF->topLeft->setCoords(-radius, radius);
            ellipseCF->bottomRight->setCoords(radius, -radius);
            // draw IM circle
            QCPItemEllipse *ellipseIM = new QCPItemEllipse(customPlot);
            customPlot->addItem(ellipseIM);
            ellipseIM->setPen(QPen(Qt::black));
            radius = 3.0/2.0;
            ellipseIM->topLeft->setCoords(-radius, radius);
            ellipseIM->bottomRight->setCoords(radius, -radius);
            // draw OM circle
            QCPItemEllipse *ellipseOM = new QCPItemEllipse(customPlot);
            customPlot->addItem(ellipseOM);
            ellipseOM->setPen(QPen(Qt::black));
            radius = 6.0/2.0;
            ellipseOM->topLeft->setCoords(-radius, radius);
            ellipseOM->bottomRight->setCoords(radius, -radius);
            //lines
            QCPItemLine *line1 = new QCPItemLine(customPlot);
            QCPItemLine *line2 = new QCPItemLine(customPlot);
            QCPItemLine *line3 = new QCPItemLine(customPlot);
            QCPItemLine *line4 = new QCPItemLine(customPlot);
            customPlot->addItem(line1);
            customPlot->addItem(line2);
            customPlot->addItem(line3);
            customPlot->addItem(line4);
            line1->setPen(QPen(Qt::black));
            line2->setPen(QPen(Qt::black));
            line3->setPen(QPen(Qt::black));
            line4->setPen(QPen(Qt::black));
            double alfa = 2 * 3.1415926535 * 45 / 360.0;
            line1->start->setCoords(qCos(alfa)*0.5,qSin(alfa)*0.5);
            line1->end->setCoords(qCos(alfa)*3.0,qSin(alfa)*3.0);
            alfa = 2 * 3.1415926535 * 135 / 360.0;
            line2->start->setCoords(qCos(alfa)*0.5,qSin(alfa)*0.5);
            line2->end->setCoords(qCos(alfa)*3.0,qSin(alfa)*3.0);
            alfa = 2 * 3.1415926535 * 225 / 360.0;
            line3->start->setCoords(qCos(alfa)*0.5,qSin(alfa)*0.5);
            line3->end->setCoords(qCos(alfa)*3.0,qSin(alfa)*3.0);
            alfa = 2 * 3.1415926535 * 315 / 360.0;
            line4->start->setCoords(qCos(alfa)*0.5,qSin(alfa)*0.5);
            line4->end->setCoords(qCos(alfa)*3.0,qSin(alfa)*3.0);
        }

        QCPMarginGroup *marginGroup = new QCPMarginGroup(customPlot);
        customPlot->axisRect()->setMarginGroup(QCP::msLeft|QCP::msRight, marginGroup);
        colorScale->setMarginGroup(QCP::msLeft|QCP::msRight, marginGroup);
        customPlot->rescaleAxes();
        //double ratio = patientData.getVoxelWidth()/patientData.getVoxelHeight();
        //customPlot->xAxis->setScaleRatio(customPlot->yAxis,1);
        customPlot->replot();
    }
}

void OCTAnnotate::displayImageLayersPlot(int bscanNumber, Layers selectedLayer){

    setupImageLayersPlot();
    QList<int> flatDiff;

    if (!patientData.getImageFileList().isEmpty()){
        for (int col=0; col < patientData.getBscanWidth(); col++){
            flatDiff.append(0);
        }

        if (showBscanOnErrorPlot){
            QImage image(patientData.getImageFileList().at(bscanNumber));

            // contrast enhancement
            Calculate *calc = new Calculate();
            if (flattenImage){
                if (patientData.hasManualAnnotations())
                    flatDiff = patientData.getFlatDifferencesRPE(bscanNumber);
                else
                    flatDiff = patientData.getFlatDifferences(bscanNumber);
                image = calc->flattenImage(&image,flatDiff);
            }
            calc->imageEnhancement(&image, contrast, brightness);

            // display image
            ui->imageLayersCPlot->axisRect()->setBackground(QPixmap::fromImage(image),true,Qt::IgnoreAspectRatio);
        } else {
            ui->imageLayersCPlot->axisRect()->setBackground(QPixmap());
        }

        // get data
        int vectorWidth = patientData.getBscanWidth();
        int imageHeight = patientData.getBscanHeight();
    //    QList<QPoint> pcvPoints = patientData.getLayerPoints(bscanNumber, PCV, 0, vectorWidth-1);
    //    QList<QPoint> pcvPointsAuto = patientData.getLayerPointsAuto(bscanNumber, PCV, 0, vectorWidth-1);

    //    QList<QPoint> points = patientData.getLayerPoints(bscanNumber, selectedLayer, 0, vectorWidth-1);
    //    QList<QPoint> pointsAuto = patientData.getLayerPointsAuto(bscanNumber, selectedLayer, 0, vectorWidth-1);

        QList<QPoint> ilmPoints = patientData.getLayerPoints(bscanNumber, ILM, 0, vectorWidth-1);
        QList<QPoint> gclPoints = patientData.getLayerPoints(bscanNumber, GCL, 0, vectorWidth-1);
    //    QList<QPoint> iplPoints = patientData.getLayerPoints(bscanNumber, IPL, 0, vectorWidth-1);
        QList<QPoint> inlPoints = patientData.getLayerPoints(bscanNumber, INL, 0, vectorWidth-1);
        QList<QPoint> oplPoints = patientData.getLayerPoints(bscanNumber, OPL, 0, vectorWidth-1);
        QList<QPoint> onlPoints = patientData.getLayerPoints(bscanNumber, ONL, 0, vectorWidth-1);
        QList<QPoint> isosPoints = patientData.getLayerPoints(bscanNumber, IOS, 0, vectorWidth-1);
        QList<QPoint> chrPoints = patientData.getLayerPoints(bscanNumber, CHR, 0, vectorWidth-1);

    //    QVector<double> pixelNr(vectorWidth), layer(vectorWidth), layerAuto(vectorWidth), ilm(vectorWidth), ilmAuto(vectorWidth), pcv(vectorWidth), pcvAuto(vectorWidth);
    //    QVector<double> layer(vectorWidth), layerAuto(vectorWidth);
        QVector<double> pixelNr(vectorWidth), ilm(vectorWidth), inl(vectorWidth), opl(vectorWidth), onl(vectorWidth), isos(vectorWidth), chr(vectorWidth);
        QVector<double> gcl(vectorWidth);
    //    QVector<double> poly(vectorWidth);
        for (int i=0; i < vectorWidth; i++)
        {
            pixelNr[i] = i;
    //        if (pcvPoints[i].x() != -1)
    //            pcv[i] = imageHeight - pcvPoints[i].y() - flatDiff[i];
    //        if (points[i].x() != -1)
    //            layer[i] = imageHeight - points[i].y() - flatDiff[i];
            if (ilmPoints[i].x() != -1)
                ilm[i] = imageHeight - ilmPoints[i].y() - flatDiff[i];
            if (gclPoints[i].x() != -1)
                gcl[i] = imageHeight - gclPoints[i].y() - flatDiff[i];
            if (inlPoints[i].x() != -1)
                inl[i] = imageHeight - inlPoints[i].y() - flatDiff[i];
            if (oplPoints[i].x() != -1)
                opl[i] = imageHeight - oplPoints[i].y() - flatDiff[i];
            if (onlPoints[i].x() != -1)
                onl[i] = imageHeight - onlPoints[i].y() - flatDiff[i];
            if (isosPoints[i].x() != -1)
                isos[i] = imageHeight - isosPoints[i].y() - flatDiff[i];
            if (chrPoints[i].x() != -1)
                chr[i] = imageHeight - chrPoints[i].y() - flatDiff[i];
    //        poly[i] = imageHeight - flatDiff[i];
        }
        // pass data points to graphs:
    //    ui->imageLayersCPlot->graph(1)->setData(pixelNr, pcv);
    //    ui->imageLayersCPlot->graph(0)->setData(pixelNr, layer);

        ui->imageLayersCPlot->graph(0)->setData(pixelNr, ilm);
        ui->imageLayersCPlot->graph(1)->setData(pixelNr, gcl);
    //    ui->imageLayersCPlot->graph(1)->setData(pixelNr, inl);
        ui->imageLayersCPlot->graph(2)->setData(pixelNr, inl);
        ui->imageLayersCPlot->graph(3)->setData(pixelNr, opl);
        ui->imageLayersCPlot->graph(4)->setData(pixelNr, onl);
        ui->imageLayersCPlot->graph(5)->setData(pixelNr, isos);
        ui->imageLayersCPlot->graph(6)->setData(pixelNr, chr);

    //    ui->imageLayersCPlot->addGraph();
    //    ui->imageLayersCPlot->graph(2)->setPen(QPen(Qt::blue));
    //    ui->imageLayersCPlot->graph(2)->setLineStyle(QCPGraph::lsNone);
    //    ui->imageLayersCPlot->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,3));
    //    ui->imageLayersCPlot->graph(2)->setName("polynomial fit");
    //    ui->imageLayersCPlot->graph(2)->setData(pixelNr, poly);

    //    if (patientData.hasAutoAnnotations()){
    //        for (int i=0; i < vectorWidth; i++){
    ////            if (pcvPointsAuto[i].x() != -1)
    ////                pcvAuto[i] = imageHeight - pcvPointsAuto[i].y();
    //            if (pointsAuto[i].x() != -1)
    //                layerAuto[i] = imageHeight - pointsAuto[i].y();
    //        }
    ////        ui->imageLayersCPlot->graph(1)->setData(pixelNr, pcvAuto);
    //        ui->imageLayersCPlot->graph(1)->setData(pixelNr, layerAuto);
    //    }

    //    ui->imageLayersCPlot->graph(0)->setName("Layer manual annotation");
    //    ui->imageLayersCPlot->graph(1)->setName("Layer auto annotation");

        // replot
        ui->imageLayersCPlot->replot();
        ui->currImageNumberLayersLEdit->setText(QString::number(currentImageLayersNumber));

        ui->nextImageLayersButton->setEnabled(true);
        if (bscanNumber <= 0){
            ui->prevImageLayersButton->setEnabled(false);
        } else {
            ui->prevImageLayersButton->setEnabled(true);
        }
        if (bscanNumber >= patientData.getBscansNumber()){
            ui->nextImageLayersButton->setEnabled(false);
        }
    }
}

void OCTAnnotate::on_actionShowETDRSGrid_toggled(bool checked)
{
    showETDRSGrid = checked;
    displayVirtualMap(ui->virtualMapImageCPlot);
    displayVirtualMap(ui->virtualMapAutoImageCPlot,true);
}


// display statistics -----------------------------------------------------------------------------
void OCTAnnotate::displayCircProfile(){
    setupCircProfilePlot();

    if (!patientData.getImageFileList().isEmpty()){
        QList<double> c1 = patientData.getCircProfileCF();
        QList<double> c3 = patientData.getCircProfileIM();
        QList<double> c6 = patientData.getCircProfileOM();

        double depthCoeff = (double)(patientData.getBscanHeight() - 1) / patientData.getVoxelDepth() / 1000;

        QVector<double> x(360), curve1(360), curve3(360), curve6(360);
        for (int i=0; i<360; ++i){
          x[i] = i;
          curve1[i] = c1[i] / depthCoeff;
          curve3[i] = c3[i] / depthCoeff;
          curve6[i] = c6[i] / depthCoeff;
        }
        ui->virtualMapCircProfileCPlot->graph(0)->setData(x, curve1);
        ui->virtualMapCircProfileCPlot->graph(1)->setData(x, curve3);
        ui->virtualMapCircProfileCPlot->graph(2)->setData(x, curve6);

        //ui->virtualMapCircProfileCPlot->legend->setVisible(true);

        ui->virtualMapCircProfileCPlot->replot();
    }
}

void OCTAnnotate::displayHistogram(){
    setupHistPlot();

    if (!patientData.getImageFileList().isEmpty()){
        QList<double> histVal = patientData.getVirtualMapHistogram();
        QVector<double> hist(8);
        double max = 0;
        for (int i=0; i < 8; i++){
            hist[i] = histVal[i];
            max = qMax(max, hist[i]);
        }
        QVector<double> ticks = ui->virtualMapHistCPlot->xAxis->tickVector();
        histBars->setData(ticks,hist);

        ui->virtualMapHistCPlot->yAxis->setRange(0,max);
        ui->virtualMapHistCPlot->yAxis->setLabel("Number of points in the range");

        ui->virtualMapHistCPlot->replot();
    }
}

void OCTAnnotate::displayVolumes(){
    setupVolumeGridPlot();

    if (!patientData.getImageFileList().isEmpty()){
        QList<double> volumes = patientData.getVolumeGrid();

        QCPItemText *volumeLabelCF = new QCPItemText(ui->ETDRSgridCPlot);
        QCPItemText *volumeLabelIMbottom = new QCPItemText(ui->ETDRSgridCPlot);
        QCPItemText *volumeLabelIMleft = new QCPItemText(ui->ETDRSgridCPlot);
        QCPItemText *volumeLabelIMtop = new QCPItemText(ui->ETDRSgridCPlot);
        QCPItemText *volumeLabelIMright = new QCPItemText(ui->ETDRSgridCPlot);
        QCPItemText *volumeLabelOMbottom = new QCPItemText(ui->ETDRSgridCPlot);
        QCPItemText *volumeLabelOMleft = new QCPItemText(ui->ETDRSgridCPlot);
        QCPItemText *volumeLabelOMtop = new QCPItemText(ui->ETDRSgridCPlot);
        QCPItemText *volumeLabelOMright = new QCPItemText(ui->ETDRSgridCPlot);
        QCPItemText *volumeLabelSum = new QCPItemText(ui->ETDRSgridCPlot);

        ui->ETDRSgridCPlot->addItem(volumeLabelCF);
        ui->ETDRSgridCPlot->addItem(volumeLabelIMbottom);
        ui->ETDRSgridCPlot->addItem(volumeLabelIMleft);
        ui->ETDRSgridCPlot->addItem(volumeLabelIMtop);
        ui->ETDRSgridCPlot->addItem(volumeLabelIMright);
        ui->ETDRSgridCPlot->addItem(volumeLabelOMbottom);
        ui->ETDRSgridCPlot->addItem(volumeLabelOMleft);
        ui->ETDRSgridCPlot->addItem(volumeLabelOMtop);
        ui->ETDRSgridCPlot->addItem(volumeLabelOMright);
        ui->ETDRSgridCPlot->addItem(volumeLabelSum);

        volumeLabelCF->setColor(Qt::blue);
        volumeLabelIMbottom->setColor(Qt::blue);
        volumeLabelIMleft->setColor(Qt::blue);
        volumeLabelIMtop->setColor(Qt::blue);
        volumeLabelIMright->setColor(Qt::blue);
        volumeLabelOMbottom->setColor(Qt::blue);
        volumeLabelOMleft->setColor(Qt::blue);
        volumeLabelOMtop->setColor(Qt::blue);
        volumeLabelOMright->setColor(Qt::blue);
        volumeLabelSum->setColor(Qt::blue);

        volumeLabelCF->position->setCoords(0,0);
        volumeLabelIMbottom->position->setCoords(0,-1);
        volumeLabelIMleft->position->setCoords(-1,0);
        volumeLabelIMtop->position->setCoords(0,1);
        volumeLabelIMright->position->setCoords(1,0);
        volumeLabelOMbottom->position->setCoords(0,-2.2);
        volumeLabelOMleft->position->setCoords(-2.2,0);
        volumeLabelOMtop->position->setCoords(0,2.2);
        volumeLabelOMright->position->setCoords(2.2,0);
        volumeLabelSum->position->setCoords(-2.2,2.6);
        volumeLabelSum->setTextAlignment(Qt::AlignLeft);

        int fontSize = 10;
        volumeLabelCF->setFont(QFont(font().family(), fontSize));
        volumeLabelIMbottom->setFont(QFont(font().family(), fontSize));
        volumeLabelIMleft->setFont(QFont(font().family(), fontSize));
        volumeLabelIMtop->setFont(QFont(font().family(), fontSize));
        volumeLabelIMright->setFont(QFont(font().family(), fontSize));
        volumeLabelOMbottom->setFont(QFont(font().family(), fontSize));
        volumeLabelOMleft->setFont(QFont(font().family(), fontSize));
        volumeLabelOMtop->setFont(QFont(font().family(), fontSize));
        volumeLabelOMright->setFont(QFont(font().family(), fontSize));
        volumeLabelSum->setFont(QFont(font().family(), fontSize));

        volumeLabelCF->setText(QString::number(volumes[0],'f',2));
        volumeLabelIMbottom->setText(QString::number(volumes[1],'f',2));
        volumeLabelIMleft->setText(QString::number(volumes[2],'f',2));
        volumeLabelIMtop->setText(QString::number(volumes[3],'f',2));
        volumeLabelIMright->setText(QString::number(volumes[4],'f',2));
        volumeLabelOMbottom->setText(QString::number(volumes[5],'f',2));
        volumeLabelOMleft->setText(QString::number(volumes[6],'f',2));
        volumeLabelOMtop->setText(QString::number(volumes[7],'f',2));
        volumeLabelOMright->setText(QString::number(volumes[8],'f',2));
        volumeLabelSum->setText("Vol [mm3]:\n" + QString::number(volumes[9],'f',2));

        QCPItemText *labelLeft = new QCPItemText(ui->ETDRSgridCPlot);
        QCPItemText *labelRight = new QCPItemText(ui->ETDRSgridCPlot);
        ui->ETDRSgridCPlot->addItem(labelLeft);
        ui->ETDRSgridCPlot->addItem(labelRight);
        labelLeft->position->setCoords(-2.5,-2.8);
        labelRight->position->setCoords(2.5,-2.8);
        labelLeft->setFont(QFont(font().family(), 12));
        labelRight->setFont(QFont(font().family(), 12));

        if (patientData.getEye() == 1){    // right eye
            labelLeft->setText("T <-");
            labelRight->setText("-> N");
        } else if (patientData.getEye() == 0){ // left eye
            labelLeft->setText("N <-");
            labelRight->setText("-> T");
        }

        ui->ETDRSgridCPlot->replot();
    }
}


// recalculate virtual map for selected values ----------------------------------------------------
void OCTAnnotate::on_layer1CBox_currentIndexChanged(int index)
{
    vMapLayer1 = (Layers)index;

    if (vMapLayer1 > vMapLayer2){
        Layers temp;
        temp = vMapLayer1;
        vMapLayer1 = vMapLayer2;
        vMapLayer2 = temp;
        ui->layer1CBox->setCurrentIndex((int)vMapLayer1);
        ui->layer2CBox->setCurrentIndex((int)vMapLayer2);
    }

    if (!patientData.getImageFileList().isEmpty()){
        // oblicz i przerysuj mape
        on_computeVirtualMapButton_clicked();
    }
}

void OCTAnnotate::on_layer2CBox_currentIndexChanged(int index)
{
    vMapLayer2 = (Layers)index;

    if (vMapLayer1 > vMapLayer2){
        Layers temp;
        temp = vMapLayer1;
        vMapLayer1 = vMapLayer2;
        vMapLayer2 = temp;
        ui->layer1CBox->setCurrentIndex((int)vMapLayer1);
        ui->layer2CBox->setCurrentIndex((int)vMapLayer2);
    }

    if (!patientData.getImageFileList().isEmpty()){
        // oblicz i przerysuj mape
        on_computeVirtualMapButton_clicked();
    }
}

void OCTAnnotate::on_contactThresholdCBox_currentIndexChanged(int index)
{
    contactThreshold = thresholds[index];
    if (!patientData.getImageFileList().isEmpty()){
        patientData.setContactThreshold(contactThreshold);
        patientData.calculateContactArea(1.0);
        patientData.calculateContactArea(3.0);
        patientData.calculateContactArea(6.0);

        ui->contactAreaCFLEdit->setText(QString::number(patientData.getContactAreaCF()));
        ui->contactAreaIMLEdit->setText(QString::number(patientData.getContactAreaIM()));
        ui->contactAreaOMLEdit->setText(QString::number(patientData.getContactAreaOM()));
        double max1 = 3.1415926535 * 1.0 /2 * 1.0 /2;
        double max2 = 3.1415926535 * 3.0 /2 * 3.0 /2;
        double max3 = 3.1415926535 * 6.0 /2 * 6.0 /2;
        ui->contactAreaProcCFLabel->setText(QString::number(patientData.getContactAreaCF() / max1 * 100,3,0) + " %");
        ui->contactAreaProcIMLabel->setText(QString::number(patientData.getContactAreaIM() / max2 * 100,3,0) + " %");
        ui->contactAreaProcOMLabel->setText(QString::number(patientData.getContactAreaOM() / max3 * 100,3,0) + " %");
    }
}

// auto annotations -------------------------------------------------------------------------------
void OCTAnnotate::on_actionReadAutoSegmentation_triggered(QString pathAutoSegment)
{
    if (!patientData.getImageFileList().isEmpty()){
        // read data from file
        QString autoSegmentFilePath = pathAutoSegment;
        if (autoSegmentFilePath.isEmpty())
            autoSegmentFilePath = QFileDialog::getOpenFileName(this, tr("Otwórz plik z automatyczną segmentacją"), autoDir.path(), tr("Plik tekstowy (*.txt);;Plik aplikacji OCTExplorer (*.xml)"));
        if (autoSegmentFilePath.isEmpty()){
            if (patientData.hasAutoAnnotations()){
                QMessageBox msgBox;
                msgBox.addButton(" Zostaw dane ", QMessageBox::RejectRole);
                msgBox.addButton(" Zamknij ", QMessageBox::AcceptRole);

                msgBox.setText("Czy zamknąć dane automatycznej segmentacji?");
                if (msgBox.exec() == QMessageBox::Accepted){
                    on_actionCloseAutoSegmentation_triggered();
                }
            }
            return;
        }
        errorFilePath = autoSegmentFilePath;
        errorFilePath.chop(4);
        errorFilePath += "_error.txt";

        ReadWriteData *rwData = new ReadWriteData();
        rwData->setDataObject(&patientData);
        rwData->setDataSaveStrucure(dataSaveStructure);
        rwData->setAutoFilePath(autoSegmentFilePath);
        rwData->addDirective("readAutoSegmentationData");

        QThread *thread = new QThread;
        rwData->moveToThread(thread);
        connect(thread, SIGNAL(started()), rwData, SLOT(process()));
        connect(rwData, SIGNAL(errorOccured(QString)), this, SLOT(on_errorOccured(QString)));
        connect(rwData, SIGNAL(processingData(double,QString)), this, SLOT(on_processingData(double,QString)));
        connect(rwData, SIGNAL(readingDataFinished(QString)), this, SLOT(on_readingDataFinished(QString)));
        connect(rwData, SIGNAL(finished()), thread, SLOT(quit()));
        connect(rwData, SIGNAL(finished()), rwData, SLOT(deleteLater()));
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
        thread->start();

        QFile autoSegmentFile(autoSegmentFilePath);
        QFileInfo autoSegmentFileInfo(autoSegmentFile);
        ui->autoSegmentFileLabel->setText(autoSegmentFileInfo.fileName());
    }
}

void OCTAnnotate::on_actionSetAutoSegmentationAsManual_triggered()
{
    AutoAsManualDialog autoAsManualdialog(this);

    QList<Layers> selectedLayers;
    if (autoAsManualdialog.exec()){
        QList<QString> layers = autoAsManualdialog.getSelectedLayers();
        foreach (QString str, layers) {
            selectedLayers.append(decodeLayer(str));
        }

        int selectedScans = autoAsManualdialog.getSelectedScans();

        foreach (Layers layer, selectedLayers) {
            if (selectedScans < 2){ // all scans or empty scans
                for (int i=0; i < patientData.getBscansNumber(); i++){
                    if (selectedScans == 0){    // all scans
                        patientData.setLayerPoints(i, patientData.getLayerPointsAuto(i, layer), layer);
                        patientData.countAnnotatedPixelsInLayer(i, layer);
                    } else if ((selectedScans == 1) && !patientData.getLayerAnnotation(i, layer)){ // empty scans && if empty
                        patientData.setLayerPoints(i, patientData.getLayerPointsAuto(i, layer), layer);
                        patientData.countAnnotatedPixelsInLayer(i, layer);
                    }
                }
            } else if (selectedScans == 2){ // current scan
                patientData.setLayerPoints(currentImageNumber, patientData.getLayerPointsAuto(currentImageNumber, layer), layer);
                patientData.countAnnotatedPixelsInLayer(currentImageNumber, layer);
            }
        }
        on_tabWidget_currentChanged();
        octDataModified = true;

        QMessageBox::information(this, "OCTAnnotate", "Oznaczenia automatycznej segmentacji warstw zostały skopiowane jako oznaczenia ręczne.");
    }
}

void OCTAnnotate::on_actionCloseAutoSegmentation_triggered()
{
    patientData.resetAutoAnnotations();

    ui->autoSegmentFileLabel->setText("Nie znaleziono pliku");
    ui->virtualMapAutoImageCPlot->clearPlottables();
    ui->virtualMapAutoImageCPlot->replot();
    ui->imageLayersCPlot->removeGraph(1);// auto pcv
    ui->imageLayersCPlot->removeGraph(2);// auto ilm
    //ui->imageLayersCPlot->clearGraphs();
    ui->imageLayersCPlot->replot();

    // clear displayed values
    ui->virtualMapErrorAvgLEdit->setText("");
    ui->virtualMapErrorAvgLEdit_um->setText("");
    ui->virtualMapErrorDevLEdit->setText("");
    ui->virtualMapErrorDevLEdit_um->setText("");
    ui->layerErrorAvgLEdit->setText("");
    ui->layerErrorAvgLEdit_um->setText("");
    ui->layerErrorDevLEdit->setText("");
    ui->layerErrorDevLEdit_um->setText("");
    ui->layerErrorProcLEdit->setText("");
    ui->pcvLineErrorAvgLEdit->setText("");
    ui->pcvLineErrorAvgLEdit_um->setText("");
    ui->pcvLineErrorDevLEdit->setText("");
    ui->pcvLineErrorDevLEdit_um->setText("");
    ui->ilmLineErrorAvgLEdit->setText("");
    ui->ilmLineErrorAvgLEdit_um->setText("");
    ui->ilmLineErrorDevLEdit->setText("");
    ui->ilmLineErrorDevLEdit_um->setText("");
}

void OCTAnnotate::on_actionSaveAutoAnnotationsAs_triggered()
{
    if (!patientData.getImageFileList().isEmpty()){
        QString autoSegmentFilePath = QFileDialog::getSaveFileName(this, tr("Zapisz automatyczne oznaczenia jako..."), autoDir.path(), tr("Plik tekstowy (*.txt)"));
        if (!autoSegmentFilePath.isEmpty()){

            ReadWriteData *rwData = new ReadWriteData();
            rwData->setDataObject(&patientData);
            rwData->setAutoFilePath(autoSegmentFilePath);
            rwData->addDirective("saveAutoSegmentationData");
            rwData->setDataSaveStrucure(dataSaveStructure);

            QThread *thread = new QThread;
            rwData->moveToThread(thread);
            connect(thread, SIGNAL(started()), rwData, SLOT(process()));
            connect(rwData, SIGNAL(errorOccured(QString)), this, SLOT(on_errorOccured(QString)));
            connect(rwData, SIGNAL(processingData(double,QString)), this, SLOT(on_processingData(double,QString)));
            connect(rwData, SIGNAL(savingDataFinished(QString)), this, SLOT(on_savingDataFinished(QString)));
            connect(rwData, SIGNAL(finished()), thread, SLOT(quit()));
            connect(rwData, SIGNAL(finished()), rwData, SLOT(deleteLater()));
            connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
            thread->start();
        }
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Aby zapisać dane, trzeba najpierw je wczytać!"));
    }
}

void OCTAnnotate::saveErrorToFile(){
    QFile errorSaveFile(errorFilePath);
    if(errorSaveFile.open(QIODevice::WriteOnly)){
        QTextStream stream(&errorSaveFile);
        QString errPsnr = QString::number(patientData.getErrorPSNR(),0,2).replace(".",",");
        QString errMse = QString::number(patientData.getErrorMSE(),0,2).replace(".",",");
        QString errAvg = QString::number(patientData.getErrorAvg(),0,2).replace(".",",");
        QString errDev = QString::number(patientData.getErrorDev(),0,2).replace(".",",");
        stream << "Layer" << "\t" << "PSNR" << "\t" << "MSE" << "\t" << "Avg" << "\t" << "Dev" << "\t" << "Proc" << endl;
        stream << "All" << "\t" << errPsnr << "\t" << errMse << "\t" << errAvg << "\t" << errDev << endl;
        QList<Layers> allLayers = getAllLayers();
        foreach (Layers layer, allLayers) {
            errPsnr = QString::number(patientData.getLayerErrorPSNR(layer),0,2).replace(".",",");
            errMse = QString::number(patientData.getLayerErrorMSE(layer),0,2).replace(".",",");
            errAvg = QString::number(patientData.getLayerErrorAvg(layer),0,2).replace(".",",");
            errDev = QString::number(patientData.getLayerErrorDev(layer),0,2).replace(".",",");
            QString errProc = QString::number(patientData.getLayerErrorProc(layer),0,2).replace(".",",");
            stream << encodeLayer(layer) << "\t" << errPsnr << "\t" << errMse << "\t" << errAvg << "\t" << errDev << "\t" << errProc << endl;
        }
        errorSaveFile.close();
    }
}

void OCTAnnotate::on_actionConvertAutoAnnotations_triggered()
{
    QList<QList<int> > pointsList;

    // read data from file
    QString autoSegmentFilePath = QFileDialog::getOpenFileName(this, tr("Wybierz plik z automatyczną segmentacją do konwersji"), autoDir.path(), tr("Plik aplikacji OCTExplorer (*.xml)"));
    if (!autoSegmentFilePath.isEmpty()){
        QFile autoSegmentFile(autoSegmentFilePath);
        if (!autoSegmentFile.open(QIODevice::ReadOnly)){
            QMessageBox::critical(this, tr("Error"), tr("Nie można otworzyć pliku z automatyczną segmentacją warstw"));
            return;
        } else {
            QXmlStreamReader xml(&autoSegmentFile);
            xml.readNextStartElement();
            while (!xml.atEnd() && !xml.hasError()){
                QXmlStreamReader::TokenType token = xml.readNext();
                if (token == QXmlStreamReader::StartElement){
                    if (xml.name() == "surface"){
                        pointsList = convertSurfaceLines(xml, pointsList);
                    } else {
                        continue;
                    }
                }
            }
        }
        if (autoSegmentFile.isOpen())
            autoSegmentFile.close();
    }

    // write data
    QFile autoSegmentFile(autoSegmentFilePath + ".txt");
    if (autoSegmentFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream autoSegmentText(&autoSegmentFile);
        for (int i=0; i < pointsList.count(); i++){
            QList<int> list = pointsList[i];
            foreach(int p, list){
                autoSegmentText << p << ",";
            }
            autoSegmentText << endl;
        }
        autoSegmentFile.close();
    }

    QMessageBox::information(this, "OCTAnnotate", "Wykonano konwersję danych.");
}

QList<QList<int> > OCTAnnotate::convertSurfaceLines(QXmlStreamReader &xml, QList<QList<int> > lineMap){
    QList<int> line;
    int lineNumber = 0;

    xml.readNext();
    while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "surface")){
        if (xml.tokenType() == QXmlStreamReader::StartElement){
            if (xml.name() == "label"){
                xml.readNext();
                lineNumber = xml.text().toInt();
                if (lineNumber == 1)
                    lineMap.clear();
            }
            if (lineNumber == 1){
                if (xml.name() == "bscan"){
                    line.clear();
                    xml.readNext();
                    while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "bscan")){
                        if (xml.tokenType() == QXmlStreamReader::StartElement){
                            if (xml.name() == "z"){
                                xml.readNext();
                                line.append(xml.text().toInt());
                            }
                        }
                        xml.readNext();
                    }
                    lineMap.append(line);
                }
            }
        }
        xml.readNext();
    }

    return lineMap;
}

// read manual annotations from selected file -----------------------------------------------------
void OCTAnnotate::on_actionReadManualAnnotations_triggered()
{
    if (!patientData.getImageFileList().isEmpty()){
        QMessageBox msgBox;
        QPushButton *saveButton = msgBox.addButton(" Zapisz i wczytaj nowe badanie ", QMessageBox::YesRole);
        QPushButton *cancelButton = msgBox.addButton(" Anuluj ", QMessageBox::RejectRole);
        msgBox.addButton(" Wczytaj nowe badanie bez zapisywania ", QMessageBox::NoRole);
        bool selectNew = true;

        if (octDataModified){
            msgBox.setText("Anotacje badania OCT zostały zmienione. Czy zapisać zmiany przed wczytaniem nowego badania?");
            msgBox.exec();
            if (msgBox.clickedButton() == saveButton){
                on_actionSaveOCTExam_triggered();
            } else if (msgBox.clickedButton() == cancelButton) {
                selectNew = false;
            }
        }

        if (selectNew){
            // read data from file
            QString manualSegmentFilePath = QFileDialog::getOpenFileName(this, tr("Otwórz plik z ręczną segmentacją"), manualDir.path(), tr("Plik aplikacji OCTAnnotate (*.mvri);;Wszystkie pliki (*.*)"));
            if (!manualSegmentFilePath.isEmpty()){

                ReadWriteData *rwData = new ReadWriteData();
                rwData->setDataObject(&patientData);
                rwData->setDataSaveStrucure(dataSaveStructure);
                rwData->setManualFilePath(manualSegmentFilePath);
                rwData->addDirective("readManualSegmentationData");

                QThread *thread = new QThread;
                rwData->moveToThread(thread);
                connect(thread, SIGNAL(started()), rwData, SLOT(process()));
                connect(rwData, SIGNAL(errorOccured(QString)), this, SLOT(on_errorOccured(QString)));
                connect(rwData, SIGNAL(processingData(double,QString)), this, SLOT(on_processingData(double,QString)));
                connect(rwData, SIGNAL(readingDataFinished(QString)), this, SLOT(on_readingDataFinished(QString)));
                connect(rwData, SIGNAL(finished()), thread, SLOT(quit()));
                connect(rwData, SIGNAL(finished()), rwData, SLOT(deleteLater()));
                connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
                thread->start();
            }
        }
    }
}


// other ------------------------------------------------------------------------------------------
void OCTAnnotate::on_actionFillFromILM_triggered()
{
    if (!patientData.getImageFileList().isEmpty()){
        patientData.fillLayerFromILM(PCV, currentImageNumber);
        loadImage(currentImageNumber);
    }
}

void OCTAnnotate::on_actionFillStraight_triggered()
{
    // TODO: tylko aktywna warstwa?
    if (!patientData.getImageFileList().isEmpty()){
        QList<Layers> layers = getAllLayers();
        foreach (Layers layer, layers) {
            patientData.fillLayerStraight(layer,currentImageNumber);
        }
        loadImage(currentImageNumber);
    }
}

void OCTAnnotate::on_layerErrorCBox_currentIndexChanged(int index)
{
    selectedErrorLayer = (Layers)index;

    if (!patientData.getImageFileList().isEmpty()){
        // oblicz blad
        //patientData.computeLayerError(selectedErrorLayer);
        ui->layerErrorAvgLEdit->setText(QString::number(patientData.getLayerErrorMSE(selectedErrorLayer)) + " px");
        ui->layerErrorDevLEdit->setText(QString::number(patientData.getLayerErrorDev(selectedErrorLayer)) + " px");
        ui->layerErrorAvgLEdit_um->setText(QString::number(patientData.getLayerErrorMSE(selectedErrorLayer, "um")) + " um");
        ui->layerErrorDevLEdit_um->setText(QString::number(patientData.getLayerErrorDev(selectedErrorLayer, "um")) + " um");
        ui->layerErrorProcLEdit->setText(QString::number(patientData.getLayerErrorProc(selectedErrorLayer)) + " %");

        displayImageLayersPlot(currentImageLayersNumber,selectedErrorLayer);
    }
}


// automatic segmentation error calculations ------------------------------------------------------
void OCTAnnotate::on_actionComputeErrorAllScans_triggered()
{
    ComputeErrorSettingsDialog *cesDialog = new ComputeErrorSettingsDialog();
    if(cesDialog->exec() == QDialog::Accepted){

        ui->statusBar->showMessage("Trwa odczyt danych...");
        progressBar->setMaximum(100);
        progressBar->setValue(0);
        progressBar->setVisible(true);

        QList<QString> folderList;
        // AVANTI SERIA 2 VMT?
        folderList.append("Dabrowski, Adam _27_3D Retina_OS_2014-09-12_10.22.21_1"); // 1
        folderList.append("Fabiszak, Kazimierz _29_3D Retina_OD_2014-09-13_10.51.00_1"); // 2
        folderList.append("Gretowicz-Miernicka, Teresa _51_3D Retina_OD_2014-09-19_10.32.10_1"); // 3
//        folderList.append("Gromadzinski, Marek _60_3D Retina_OD_2014-09-23_11.05.18_1"); // 5
        folderList.append("Gromadzinski, Marek _60_3D Retina_OS_2014-09-23_11.03.19_1"); // 6
        folderList.append("Grzesiak, Barbara _59_3D Retina_OS_2014-09-23_10.51.34_1"); // 7
        folderList.append("Jakubowska, Eugenia _45_3D Retina_OD_2014-09-18_10.53.51_1"); // 8
//        folderList.append("Jakubowska, Eugenia _45_3D Retina_OS_2014-09-18_10.56.09_1"); // 9
//        folderList.append("Kawa, Zofia _39_3D Retina_OD_2014-09-16_11.37.39_1"); // 10
        //folderList.append("Kokowski, Piotr _52_3D Retina_OD_2014-09-19_10.42.19_1"); // 11
        folderList.append("Kolt, Franciszka _31_3D Retina_OD_2014-09-13_12.10.54_1"); // 12
        //folderList.append("Mackowiak, Pawel _36_3D Retina_OD_2014-09-15_12.09.21_1"); // 14
        //folderList.append("Namerla, Wojciech _46_3D Retina_OD_2014-09-18_11.15.50_1"); // 16
//        folderList.append("Paruszewski, Andrzej _24_3D Retina_OS_2014-09-11_13.05.09_1"); // 17
        folderList.append("Poznanski, Waclaw _40_3D Retina_OS_2014-09-16_11.58.27_1"); // 19
        folderList.append("Silska, Genowefa _42_3D Retina_OD_2014-09-16_14.40.13_1"); // 20
        folderList.append("Silska, Genowefa _42_3D Retina_OS_2014-09-16_14.41.43_1"); // 21
//        folderList.append("Stellmach, Henryk _56_3D Retina_OS_2014-09-19_11.24.49_1"); // 22

        // KKE 2016 STUDY - ZDROWI
//        folderList.append("Bera, Jacek _15_3D Retina_OD_2015-11-02_13.49.45_1"); // 1
//        folderList.append("Stankiewicz, Agnieszka _23_3D Retina_OD_2015-11-03_12.19.07_1"); // 2
//        folderList.append("Karbowniczek, Ksawery _26_3D Retina_OD_2015-11-03_13.08.29_1"); // 3
//        folderList.append("Karbowniczek, Ksawery _26_3D Retina_OS_2015-11-03_13.13.10_1");
//        folderList.append("Kroma-Skrzypczak, Donata _30_3D Retina_OD_2015-11-04_09.00.46_1"); // 4
//        folderList.append("Lis, Dariusz _27_3D Retina_OS_2015-11-03_13.26.31_1"); // 5
//        folderList.append("Stankiewicz, Agnieszka _23_3D Retina_OS_2015-11-03_12.21.38_1"); // 6
//        folderList.append("test, test _8_3D Retina_OS_2015-10-30_12.20.41_1"); // 7
//        folderList.append("Traczynska, Beata _24_3D Retina_OD_2015-11-03_12.45.35_1");
//        folderList.append("Traczynska, Beata _24_3D Retina_OS_2015-11-03_12.48.54_1"); // 9

        // ZDROWI 2
//        folderList.append("Bera, Jacek _15_3D Retina_OS_2015-11-02_14.00.16_1");
//        folderList.append("test, test _8_3D Retina_OD_2015-10-30_12.13.42_1"); // 8
//        folderList.append("Wolinski, Michal _25_3D Retina_OD_2015-11-03_12.57.33_1"); // 10
//        folderList.append("Wolinski, Michal _25_3D Retina_OS_2015-11-03_13.02.38_1");

        // best cases
    //    folderList.append("Gretowicz-Miernicka, Teresa _51_3D Retina_OD_2014-09-19_10.32.10_1"); // 3
    //    folderList.append("Gromadzinski, Marek _60_3D Retina_OD_2014-09-23_11.05.18_1"); // 5
    //    folderList.append("Gromadzinski, Marek _60_3D Retina_OS_2014-09-23_11.03.19_1"); // 6
    //    folderList.append("Grzesiak, Barbara _59_3D Retina_OS_2014-09-23_10.51.34_1"); // 7
    //    folderList.append("Poznanski, Waclaw _40_3D Retina_OS_2014-09-16_11.58.27_1"); // 19
    //    folderList.append("Silska, Genowefa _42_3D Retina_OD_2014-09-16_14.40.13_1"); // 20
    //    folderList.append("Silska, Genowefa _42_3D Retina_OS_2014-09-16_14.41.43_1"); // 21

        // AVANTI SERIA 2 VMA
        //folderList.append("Gretowicz-Miernicka, Teresa _51_3D Retina_OS_2014-09-19_10.34.27_1"); // 4
        //folderList.append("Kozlowska, Maria _26_3D Retina_OD_2014-09-12_09.16.26_1"); // 13
        //folderList.append("Mackowiak, Pawel _36_3D Retina_OS_2014-09-15_12.13.12_1"); // 15
        //folderList.append("Pietrzak, Piotr _53_3D Retina_OS_2014-09-19_11.00.17_1"); // 18

        // COPERNICUS SERIA 2 VMT
    //    folderList.append("DABROWSKI_ADAM_2014_09_12_09_47_45_L_Centralna_3D_8,00 mm_ 800x100"); // 1
    //    //folderList.append("DABROWSKI_ADAM_2014_09_12_09_48_08_L_Centralna_3D_8,00 mm_ 800x100"); // 1
    //    //folderList.append("FABISZAK_KAZIMIERZ_2014_09_13_10_23_18_R_Centralna_3D_8,00 mm_ 800x100"); // 2
    //    folderList.append("GREJTOWICZ_MIERNICKA_TERESA_2014_09_19_09_51_09_R_Centralna_3D_8,00 mm_ 800x100"); // 3
    //    folderList.append("GREJTOWICZ_MIERNICKA_TERESA_2014_09_19_09_54_48_L_Centralna_3D_8,00 mm_ 799x100"); // 4
    //    folderList.append("GROMADZINSKI_MAREK_2014_09_23_10_23_00_R_Centralna_3D_8,00 mm_ 800x100"); // 5
    //    folderList.append("GROMADZINSKI_MAREK_2014_09_23_10_27_43_L_Centralna_3D_8,00 mm_ 799x100"); // 6
    //    folderList.append("GRZESIAK_BARBARA_2014_09_23_10_20_06_L_Centralna_3D_8,00 mm_ 799x100"); // 7
    //    //folderList.append("GRZYBOWSKA_GABRIELLA_2014_09_15_10_50_39_L_Centralna_3D_8,00 mm_ 800x100"); // 8
    //    //folderList.append("JAKUBOWSKA_EUGENIA_2014_09_18_10_57_17_R_Centralna_3D_8,00 mm_ 800x100"); // 9
    //    folderList.append("JAKUBOWSKA_EUGENIA_2014_09_18_11_01_35_L_Centralna_3D_8,00 mm_ 800x100"); // 10
    //    folderList.append("KAWA_ZOFIA_2014_09_16_11_18_02_R_Centralna_3D_8,00 mm_ 800x100"); // 11
    //    folderList.append("KOKOWSKI_PIOTR_2014_09_19_10_00_38_R_Centralna_3D_8,00 mm_ 800x100"); // 12
    //    //folderList.append("KOLT_FRANCISZKA_2014_09_13_11_49_27_R_Centralna_3D_8,00 mm_ 800x100"); // 13
    //    //folderList.append("KOZLOWSKA_MARIA_2014_09_12_08_56_31_R_Centralna_3D_8,00 mm_ 800x100"); // 14
    //    folderList.append("MACKOWIAK_PAWEL_2014_09_15_11_20_54_L_Centralna_3D_8,00 mm_ 800x100"); // 15
    //    folderList.append("MACKOWIAK_PAWEL_2014_09_15_11_46_36_R_Centralna_3D_8,00 mm_ 800x100"); // 16
    //    folderList.append("NAMERLA_WOJCIECH_2014_09_18_11_36_48_R_Centralna_3D_8,00 mm_ 800x100"); // 17
    //    //folderList.append("PARUSZEWSKI_ANDRZEJ_2014_09_11_12_37_58_L_Centralna_3D_8,00 mm_ 799x100"); // 18
    //    folderList.append("PIETRZAK_PIOTR_2014_09_19_10_14_23_L_Centralna_3D_8,00 mm_ 799x100"); // 19
    //    folderList.append("POZNANSKI_WACLAW_2014_09_16_11_35_16_L_Centralna_3D_8,00 mm_ 800x100"); // 20
    //    //folderList.append("SILSKA_GENOWEFA_2014_09_16_14_14_56_R_Centralna_3D_8,00 mm_ 800x100"); // 21
    //    //folderList.append("SILSKA_GENOWEFA_2014_09_16_14_18_23_L_Centralna_3D_8,00 mm_ 800x100"); // 22
    //    folderList.append("STELLMACH_HENRYK_2014_09_19_11_05_59_L_Centralna_3D_8,00 mm_ 800x100"); // 23

        Calculate *calc = new Calculate();
        calc->setFolderList(folderList);
//        calc->setLayers(getLayersForError());
        calc->setLayers(cesDialog->getLayers());
    //    calc->examDataDir = "D:/Egnyte/Private/agis/Qt/Deployment/OCTAnnotate/examData/"; //currentDir.path().append("/examData/");
    //    calc->autoFilePath = "auto matlab/Avanti imgradientPrewitt cut0 trackILMandISOS/";
    //    calc->alg = "_imgradientPrewitt_cut0_trackILMandISOS";
        calc->examDataDir = cesDialog->getExamDataDir();
        calc->autoFilePath = cesDialog->getAutoDir();
        calc->alg = cesDialog->getAlgorithm();

        calc->setupMatrixes(AVANTI);

        QThread *thread = new QThread;
        calc->moveToThread(thread);
        connect(calc, SIGNAL(errorOccured(QString)), this, SLOT(on_errorOccured(QString)));
        connect(thread, SIGNAL(started()), calc, SLOT(process()));
        connect(calc, SIGNAL(processingData(double,QString)), this, SLOT(on_processingData(double,QString)));
        connect(calc, SIGNAL(averageErrorCalculated(double)), this, SLOT(on_averageErrorCalculated(double)));
        connect(calc, SIGNAL(mseErrorCaluclated(double)), this, SLOT(on_mseErrorCalculated(double)));
        connect(calc, SIGNAL(finished()), thread, SLOT(quit()));
        connect(calc, SIGNAL(finished()), calc, SLOT(deleteLater()));
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
        thread->start();
    }
}


// thread handling --------------------------------------------------------------------------------
void OCTAnnotate::on_errorOccured(QString err){
    QMessageBox::critical(this, tr("Error"), err);
}

void OCTAnnotate::on_processingData(double procent, QString msg){
    if(!progressBar->isVisible())
        progressBar->setVisible(true);
    progressBar->setValue((int)procent);
    if (msg != "")
        ui->statusBar->showMessage(msg);
    if ((procent) >= progressBar->maximum()){
        ui->statusBar->clearMessage();
        progressBar->setVisible(false);
    }
}

void OCTAnnotate::on_averageErrorCalculated(double value){
    ui->layerErrorAvgAllLEdit->setText(QString::number(value,6,2));
}

void OCTAnnotate::on_mseErrorCalculated(double value){
    ui->layerErrorMSEAllLEdit->setText(QString::number(value,6,2));
}

void OCTAnnotate::on_readingDataFinished(QString data){

    if ((data != "manualOnly") && (data != "autoOnly")){
        // display patients data
        ui->visOPLEdit->setText(patientData.getVisOP());
        ui->visOLLEdit->setText(patientData.getVisOL());
        ui->snOPLEdit->setText(patientData.getSnOP());
        ui->snOLLEdit->setText(patientData.getSnOL());
        ui->mcVOPLEdit->setText(patientData.getMcvOP());
        ui->mcVOLLEdit->setText(patientData.getMcvOL());
        ui->mcHOPLEdit->setText(patientData.getMchOP());
        ui->mcHOLLEdit->setText(patientData.getMchOL());
        generalDataModified = false;

        // display information
        this->setWindowTitle("OCTAnnotate " + appVersion + " - " + octDir.dirName());
    }

    ui->statusBar->clearMessage();
    progressBar->setValue(100);
    progressBar->setVisible(false);

    QString msg = "Wczytano dane badania OCT";
    if (data == "manualOnly")
        msg = "Wczytano dane ręcznej segmentacji";
    if (data == "autoOnly")
        msg = "Wczytano dane automatycznej segmentacji";

    QMessageBox::information(this, "Odczyt danych OCT", msg);

    QPoint center = patientData.getScanCenter();
    if (center.y() != -1)
        currentImageNumber = center.y();
    else
        currentImageNumber = patientData.getBscansNumber()/2;   // middle B-scan
    currentImageLayersNumber = currentImageNumber;
    currentNormalImageNumber = patientData.getBscanWidth()/2;

    // enable navigation buttons
    if (currentImageNumber < (patientData.getBscansNumber()-1)){
        ui->nextImageButton->setEnabled(true);
        ui->nextImageLayersButton->setEnabled(true);
    } else {
        ui->nextImageButton->setEnabled(false);
        ui->nextImageLayersButton->setEnabled(false);
    }
    if (currentImageNumber > 0){
        ui->prevImageButton->setEnabled(true);
        ui->prevImageLayersButton->setEnabled(true);
    } else {
        ui->prevImageButton->setEnabled(false);
        ui->prevImageLayersButton->setEnabled(false);
    }
    ui->actionSaveGeneralExam->setEnabled(true);
    ui->actionSaveOCTExam->setEnabled(true);
    ui->computeVirtualMapButton->setEnabled(true);
    ui->actionReadAutoSegmentation->setEnabled(true);
    ui->actionSetScanCenter->setEnabled(true);
    ui->actionFillFromILM->setEnabled(true);
    ui->actionFillStraight->setEnabled(true);
    ui->actionSetAutoSegmentationAsManual->setEnabled(true);
    ui->zoomInButton->setEnabled(true);

    // setup plots
    setupVirtualMapPlot(ui->virtualMapImageCPlot);
    setupVirtualMapPlot(ui->virtualMapAutoImageCPlot);
    setupVirtualMapPlot(ui->errorVirtualMapCPlot);
    setupImageLayersPlot();
    setupBScanPlots();

    on_computeVirtualMapButton_clicked();

    if (data == "autoOnly"){
        Calculate *calc = new Calculate();
        calc->setLayers(getLayersForError());
        calc->calculateError(&patientData,true);
        saveErrorToFile();
        QString virtualMapAutoImagePath = errorFilePath;
        virtualMapAutoImagePath.chop(10);
        virtualMapAutoImagePath += ".png";
        ui->virtualMapAutoImageCPlot->savePng(virtualMapAutoImagePath);
    }

    //on_tabWidget_currentChanged();
}

void OCTAnnotate::on_savingDataFinished(QString data){
    if (data == "generalData"){
        QMessageBox::information(this, tr("Save"), tr("Dane badania ogólnego zostały zapisane."));
        generalDataModified = false;
    }
    if (data == "manualData"){
        QMessageBox::information(this, tr("Zapisz"), tr("Dane ręcznej segmentacji zostały zapisane"));
        octDataModified = false;
    }
    if (data == "autoData"){
        QMessageBox::information(this, tr("Zapisz"), tr("Dane automatycznej segmentacji zostały zapisane"));
    }
}

void OCTAnnotate::on_returnNewDirectory(QString newDir){
    octDir = QDir(newDir);
}

// calculate statistics for CAVRI analysis -------------------------------------------------------
void OCTAnnotate::on_actionComputeStatistics_triggered()
{
    ui->statusBar->showMessage("Trwa odczyt danych...");
    progressBar->setMaximum(100);
    progressBar->setValue(0);
    progressBar->setVisible(true);

    QList<QString> folderList;
    // AVANTI SERIA 2 VMT?
    // VMT OD
    folderList.append("Fabiszak, Kazimierz _29_3D Retina_OD_2014-09-13_10.51.00_1"); // 2
    folderList.append("Gretowicz-Miernicka, Teresa _51_3D Retina_OD_2014-09-19_10.32.10_1"); // 3
    folderList.append("Kawa, Zofia _39_3D Retina_OD_2014-09-16_11.37.39_1"); // 10
    folderList.append("Kokowski, Piotr _52_3D Retina_OD_2014-09-19_10.42.19_1"); // 11
    folderList.append("Kolt, Franciszka _31_3D Retina_OD_2014-09-13_12.10.54_1"); // 12
    folderList.append("Mackowiak, Pawel _36_3D Retina_OD_2014-09-15_12.09.21_1"); // 14
    folderList.append("Namerla, Wojciech _46_3D Retina_OD_2014-09-18_11.15.50_1"); // 16
    folderList.append("Silska, Genowefa _42_3D Retina_OD_2014-09-16_14.40.13_1"); // 20

    // VMT OS
    folderList.append("Dabrowski, Adam _27_3D Retina_OS_2014-09-12_10.22.21_1"); // 1
    folderList.append("Gromadzinski, Marek _60_3D Retina_OS_2014-09-23_11.03.19_1"); // 6
    folderList.append("Paruszewski, Andrzej _24_3D Retina_OS_2014-09-11_13.05.09_1"); // 17
    folderList.append("Poznanski, Waclaw _40_3D Retina_OS_2014-09-16_11.58.27_1"); // 19
    folderList.append("Silska, Genowefa _42_3D Retina_OS_2014-09-16_14.41.43_1"); // 21

    // VMA OD
    folderList.append("Gromadzinski, Marek _60_3D Retina_OD_2014-09-23_11.05.18_1"); // 5
    folderList.append("Jakubowska, Eugenia _45_3D Retina_OD_2014-09-18_10.53.51_1"); // 8
    folderList.append("Kozlowska, Maria _26_3D Retina_OD_2014-09-12_09.16.26_1"); // 13

    // VMA OS
    folderList.append("Gretowicz-Miernicka, Teresa _51_3D Retina_OS_2014-09-19_10.34.27_1"); // 4
    folderList.append("Grzesiak, Barbara _59_3D Retina_OS_2014-09-23_10.51.34_1"); // 7
    folderList.append("Jakubowska, Eugenia _45_3D Retina_OS_2014-09-18_10.56.09_1"); // 9
    folderList.append("Mackowiak, Pawel _36_3D Retina_OS_2014-09-15_12.13.12_1"); // 15
    folderList.append("Pietrzak, Piotr _53_3D Retina_OS_2014-09-19_11.00.17_1"); // 18
    folderList.append("Grzegorzewska, Maria _12_3D Retina_OS_2015-11-02_12.59.00_1"); // 18

    Calculate *calc = new Calculate();
    calc->setFolderList(folderList);
    calc->examDataDir = "C:/DANE/Egnyte/Private/agis/Qt/Deployment/OCTAnnotate/examData/mvri/"; //currentDir.path().append("/examData/");
    calc->octDirPath = "C:/DANE/Egnyte/Private/agis/BADANIA/OCT exams/Avanti VMT_VMA/";

    QList<Layers> allLayers;
    allLayers.append(PCV);
    allLayers.append(ILM);
    allLayers.append(CHR);
    calc->setLayers(allLayers);

    calc->setupMatrixes(AVANTI);

    QThread *thread = new QThread;
    calc->moveToThread(thread);
    connect(calc, SIGNAL(errorOccured(QString)), this, SLOT(on_errorOccured(QString)));
    connect(thread, SIGNAL(started()), calc, SLOT(processStatistics()));
    connect(calc, SIGNAL(processingData(double,QString)), this, SLOT(on_processingData(double,QString)));
    connect(calc, SIGNAL(finished()), thread, SLOT(quit()));
    connect(calc, SIGNAL(finished()), calc, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}


// data visualization for publication purposes ---------------------------------------------------
void OCTAnnotate::on_plotLayersButton_clicked()
{
    ui->layerCPlot->clearGraphs();

    // configure axis
    ui->layerCPlot->xAxis->setLabel("B-scan pixel (horizontal direction)");
    ui->layerCPlot->yAxis->setLabel("B-scan pixel (vertical direction)");
//    ui->layerCPlot->xAxis->setLabel("B-scan pixel (horizontal direction)");
//    ui->layerCPlot->yAxis->setLabel("Retina depth [um]");
    if (!patientData.getImageFileList().isEmpty()){
//        ui->layerCPlot->xAxis->setRange(0,patientData.getBscanWidth());
//        ui->layerCPlot->yAxis->setRange(0,patientData.getBscanHeight()-470);
        ui->layerCPlot->xAxis->setRange(0,patientData.getBscanWidth());
//        ui->layerCPlot->yAxis->setRange(100,patientData.getVoxelDepth()*1000/2);
//        ui->layerCPlot->yAxis->setRange(150,650);
        ui->layerCPlot->yAxis->setRange(-30,30);
    } else {
        ui->layerCPlot->xAxis->setRange(0,800);
//        ui->layerCPlot->yAxis->setRange(0,1010);
        ui->layerCPlot->yAxis->setRange(-30,30);
    }
//    ui->layerCPlot->xAxis2->setLabel("Horizontal distance from center [mm]");
//    ui->layerCPlot->yAxis2->setLabel("Scan width [mm]");

    int scanWidth = patientData.getVoxelWidth();
    QPoint center = patientData.getScanCenter();
    if ((center.x() != -1) && (center.y() != -1)){
        int m = patientData.getBscanWidth();
        double x1 = -(double)(scanWidth * (center.x() - (double)m/2))/m - scanWidth/2;
        double x2 = -(double)(scanWidth * (center.x() - (double)m/2))/m + scanWidth/2;
//        ui->layerCPlot->xAxis2->setRange(x1,x2);
    } else {
        center.setX(patientData.getBscanWidth()/2);
        center.setY(patientData.getBscansNumber()/2);
//        ui->layerCPlot->xAxis2->setRange(-scanWidth/2,scanWidth/2);
    }
//    ui->layerCPlot->yAxis2->setRange(0,patientData.getVoxelDepth());
//    ui->layerCPlot->xAxis2->setVisible(true);
//    ui->layerCPlot->yAxis2->setVisible(true);

    // legend
    ui->layerCPlot->legend->setVisible(true);
    QFont legendFont = font();  // start out with MainWindow's font..
    legendFont.setPointSize(8); // and make a bit smaller for legend
    ui->layerCPlot->legend->setFont(legendFont);
    ui->layerCPlot->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignTop|Qt::AlignLeft);
    ui->layerCPlot->legend->setAutoMargins(QCP::msNone);

    // add graphs
//    ui->layerCPlot->addGraph();
//    ui->layerCPlot->graph(0)->setPen(QPen(Qt::red));
//    ui->layerCPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
//    ui->layerCPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,2));
//    ui->layerCPlot->graph(0)->setName("ILM");
//    ui->layerCPlot->addGraph();
//    ui->layerCPlot->graph(1)->setPen(QPen(Qt::blue)); // line color red for second graph
//    ui->layerCPlot->graph(1)->setLineStyle(QCPGraph::lsLine);
//    ui->layerCPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,2));
//    ui->layerCPlot->graph(1)->setName("RPE");

    ui->layerCPlot->addGraph();
    ui->layerCPlot->graph(0)->setPen(QPen(Qt::darkGreen));
    ui->layerCPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->layerCPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,2));
    ui->layerCPlot->graph(0)->setName("dy");

    // get data
//    int vectorWidth = patientData.getBscanWidth();
    int vectorWidth = patientData.getBscanWidth()-1;
//    int imageHeight = patientData.getBscanHeight();
    QList<QPoint> pcvPoints = patientData.getLayerPoints(center.y(), PCV, 0, vectorWidth);
//    QList<QPoint> ilmPoints = patientData.getLayerPoints(center.y(), ILM, 0, vectorWidth-1);
//    QList<QPoint> rpePoints = patientData.getLayerPoints(center.y(), CHR, 0, vectorWidth-1);
    QVector<double> pixelNr(vectorWidth), diff(vectorWidth); //rpe(vectorWidth), pcv(vectorWidth),
    for (int i=0; i < vectorWidth; i++)
    {
        pixelNr[i] = i;
//        if (rpePoints[i].x() != -1)
//            rpe[i] = imageHeight - rpePoints[i].y();
//        if (ilmPoints[i].x() != -1)
//            ilm[i] = imageHeight - ilmPoints[i].y();
//        diff[i] = (double)(ilm[i] - rpe[i]) * patientData.getDepthCoeff();
        if ((pcvPoints[i].x() != -1) && (pcvPoints[i+1].x() != -1))
            diff[i] = (double)(pcvPoints[i+1].y() - pcvPoints[i].y());
        else
            diff[i] = 0;
    }
    // pass data points to graphs:
//    ui->layerCPlot->graph(0)->setData(pixelNr, ilm);
//    ui->layerCPlot->graph(1)->setData(pixelNr, rpe);

    ui->layerCPlot->graph(0)->setData(pixelNr,diff);

    // replot
    ui->layerCPlot->replot();

    /* ------------------------------------------------------------------------------ */

    ui->errorVirtualMapCPlot->clearGraphs();

    // configure axis
    ui->errorVirtualMapCPlot->xAxis->setLabel("B-scan pixel (horizontal direction)");
    ui->errorVirtualMapCPlot->yAxis->setLabel("B-scan pixel (vertical direction)");
    if (!patientData.getImageFileList().isEmpty()){
        ui->errorVirtualMapCPlot->xAxis->setRange(0,patientData.getBscanWidth());
        ui->errorVirtualMapCPlot->yAxis->setRange(0,patientData.getBscanHeight());
    } else {
        ui->errorVirtualMapCPlot->xAxis->setRange(0,800);
        ui->errorVirtualMapCPlot->yAxis->setRange(0,1010);
    }
    ui->errorVirtualMapCPlot->xAxis2->setLabel("Horizontal distance from center [mm]");
    ui->errorVirtualMapCPlot->yAxis2->setLabel("Scan width [mm]");

    if ((center.x() != -1) && (center.y() != -1)){
        int m = patientData.getBscanWidth();
        double x1 = -(double)(scanWidth * (center.x() - (double)m/2))/m - scanWidth/2;
        double x2 = -(double)(scanWidth * (center.x() - (double)m/2))/m + scanWidth/2;
        ui->errorVirtualMapCPlot->xAxis2->setRange(x1,x2);
    } else {
        ui->errorVirtualMapCPlot->xAxis2->setRange(-scanWidth/2,scanWidth/2);
    }
    ui->errorVirtualMapCPlot->yAxis2->setRange(0,patientData.getVoxelDepth());
    ui->errorVirtualMapCPlot->xAxis2->setVisible(true);
    ui->errorVirtualMapCPlot->yAxis2->setVisible(true);

    // legend
    ui->errorVirtualMapCPlot->legend->setVisible(true);
    ui->errorVirtualMapCPlot->legend->setFont(legendFont);
    ui->errorVirtualMapCPlot->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignBottom|Qt::AlignRight);
    ui->errorVirtualMapCPlot->legend->setAutoMargins(QCP::msNone);

    // add graphs
    ui->errorVirtualMapCPlot->addGraph();
    ui->errorVirtualMapCPlot->graph(0)->setPen(QPen(Qt::green));
    ui->errorVirtualMapCPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->errorVirtualMapCPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,2));
    ui->errorVirtualMapCPlot->addGraph();
    ui->errorVirtualMapCPlot->graph(1)->setPen(QPen(Qt::red)); // line color red for second graph
    ui->errorVirtualMapCPlot->graph(1)->setLineStyle(QCPGraph::lsNone);
    ui->errorVirtualMapCPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,2));
    ui->errorVirtualMapCPlot->addGraph();
    ui->errorVirtualMapCPlot->graph(2)->setPen(QPen(Qt::blue)); // line color red for second graph
    ui->errorVirtualMapCPlot->graph(2)->setLineStyle(QCPGraph::lsNone);
    ui->errorVirtualMapCPlot->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,2));

    QImage image(patientData.getImageFileList().at(center.y()));
    // contrast enhancement
    Calculate *calc = new Calculate();
    calc->imageEnhancement(&image, contrast, brightness);

    // display image
    ui->errorVirtualMapCPlot->axisRect()->setBackground(QPixmap::fromImage(image),true,Qt::IgnoreAspectRatio);

    // get data
    vectorWidth = patientData.getBscanWidth();
    int imageHeight = patientData.getBscanHeight();
    pcvPoints.clear();
    pcvPoints = patientData.getLayerPoints(center.y(), PCV, 0, vectorWidth-1);
    QList<QPoint> ilmPoints = patientData.getLayerPoints(center.y(), ILM, 0, vectorWidth-1);
    QList<QPoint> chrPoints = patientData.getLayerPoints(center.y(), CHR, 0, vectorWidth-1);
    pixelNr = QVector<double>(vectorWidth);
    QVector<double> pcv(vectorWidth), ilm(vectorWidth), chr(vectorWidth);
    for (int i=0; i < vectorWidth; i++)
    {
        pixelNr[i] = i;
        if (pcvPoints[i].x() != -1)
            pcv[i] = imageHeight - pcvPoints[i].y();
        if (ilmPoints[i].x() != -1)
            ilm[i] = imageHeight - ilmPoints[i].y();
        if (chrPoints[i].x() != -1)
            chr[i] = imageHeight - chrPoints[i].y();
    }
    // pass data points to graphs:
    ui->errorVirtualMapCPlot->graph(0)->setData(pixelNr, pcv);
    ui->errorVirtualMapCPlot->graph(1)->setData(pixelNr, ilm);
    ui->errorVirtualMapCPlot->graph(2)->setData(pixelNr, chr);

    ui->errorVirtualMapCPlot->graph(0)->setName("PCV");
    ui->errorVirtualMapCPlot->graph(1)->setName("ILM");
    ui->errorVirtualMapCPlot->graph(2)->setName("CHR");

    // replot
    ui->errorVirtualMapCPlot->replot();
}


// patients database -----------------------------------------------------------------------------
void OCTAnnotate::on_addPatientDBButton_clicked()
{
    NewPatientDialog *newPatientDialog = new NewPatientDialog();
    int result = newPatientDialog->exec();

    if (result == QDialog::Accepted) {
        QString lastName = newPatientDialog->getLastName();
        QString firstName = newPatientDialog->getFirstName();
        QDate birthDate = newPatientDialog->getBirthDate();
        int gender = newPatientDialog->getGender();
        QString fileNo = newPatientDialog->getFileNo();
        QString notes = newPatientDialog->getNotes();
        QString pathologyOD = newPatientDialog->getPathologyOD();
        QString pathologyOS = newPatientDialog->getPathologyOS();

        patientsDB->addNewPatient(lastName, firstName, birthDate, gender, pathologyOD, pathologyOS, fileNo, notes);
        modelPatients->select();
    }
}

void OCTAnnotate::on_editPatientDBButton_clicked()
{
    // get selected record
    QModelIndex currentIndex = ui->patientsListTableView->currentIndex();
    int currentRow = currentIndex.row();
    if (currentRow > -1){
        QSqlRecord selectedRecord = modelPatients->record(currentRow);
        int gender = patientsDB->getPatientGender(selectedRecord.value("id").toInt());

        NewPatientDialog *newPatientDialog = new NewPatientDialog(this, selectedRecord, gender);
        int result = newPatientDialog->exec();

        if (result == QDialog::Accepted) {
            int id = selectedRecord.value("id").toInt();
            QString lastName = newPatientDialog->getLastName();
            QString firstName = newPatientDialog->getFirstName();
            QDate birthDate = newPatientDialog->getBirthDate();
            int gender = newPatientDialog->getGender();
            QString fileNo = newPatientDialog->getFileNo();
            QString notes = newPatientDialog->getNotes();
            QString pathologyOD = newPatientDialog->getPathologyOD();
            QString pathologyOS = newPatientDialog->getPathologyOS();

            patientsDB->editPatient(id, lastName, firstName, birthDate, gender, pathologyOD, pathologyOS, fileNo, notes);

            // update patient's age
            QDateTime patientsOldestScanDate = patientsDB->getPatientOldestScan(id);
            if ((!patientsOldestScanDate.isNull()) && (birthDate.year() > 1900)){
                 patientsDB->calculatePatientAge(id, patientsOldestScanDate);
            }

            modelPatients->select();
        }
    } else {
        QMessageBox::information(this, tr("Edytuj rekord"), tr("Nie wybrano żadnego rekordu do edycji!"));
    }
}

void OCTAnnotate::on_deletePatientDBButton_clicked()
{
    QModelIndex currentIndex = ui->patientsListTableView->currentIndex();
    int currentRow = currentIndex.row();
    if (currentRow > -1){
        QMessageBox msgBox;
        msgBox.addButton(" Anuluj ", QMessageBox::RejectRole);
        msgBox.addButton(" Usuń ", QMessageBox::AcceptRole);

        msgBox.setText("Czy na pewno usunąć wybrany rekord?");
        if (msgBox.exec() == QMessageBox::Accepted){
            QSqlRecord record = modelPatients->record(currentRow);
            int rowID = record.value(0).toInt();
            patientsDB->deletePatient(rowID);
            modelPatients->select();
            modelScans->select();
        }
    } else {
        QMessageBox::information(this, tr("Usuń rekord"), tr("Nie wybrano żadnego rekordu do usunięcia!"));
    }
}

void OCTAnnotate::on_addScanFolderButton_clicked(QString folderPath)
{
    QString pathOctExam;
    if (folderPath.isEmpty())
        pathOctExam = QFileDialog::getExistingDirectory(this, tr("Open Directory"), octDir.absolutePath(), QFileDialog::ShowDirsOnly);
    else
        pathOctExam = folderPath;

    if (!pathOctExam.isEmpty()){
//        QDir newExamDir = QDir(pathOctExam);
//        QString examPath = newExamDir.dirName();//examDir.relativeFilePath(pathOctExam);
        addScanToDB(pathOctExam);
    } else {
        QMessageBox::critical(this, tr("Error"), "Scan folder path is empty!");
    }
}

void OCTAnnotate::on_addScanFileButton_clicked(QString filePath)
{
    QString pathOctExam;
    if (filePath.isEmpty())
        pathOctExam = QFileDialog::getOpenFileName(this, tr("Open OCT file"), octDir.absolutePath(), tr("Avanti RTvue raw OCT data file (*.OCT)"));
    else
        pathOctExam = filePath;

    if (!pathOctExam.isEmpty()){
//        QFile newExamDir(pathOctExam);
//        QString examPath = newExamDir.fileName();
        addScanToDB(pathOctExam);//examPath);
    } else {
        QMessageBox::critical(this, tr("Error"), "Scan file path is empty");
    }
}

void OCTAnnotate::addScanToDB(QString octPath){

    qDebug() << "Adding scan to DB: " << octPath;

    QDir newExamDir = QDir(octPath);
    bool isBinary = false;

    if (octPath.contains(".OCT")){
        octPath.chop(4);
        newExamDir = QDir(octPath);
        isBinary = true;
    }

    QString scanName = newExamDir.dirName();

    if (patientsDB->getScanID(scanName) != -1){
        QMessageBox::critical(this, tr("Error"), tr("Scan already exists in the database!"));
    } else {
        QString infoFilePathAvanti = newExamDir.absolutePath().append(".txt");
        QFile infoFileAvanti(infoFilePathAvanti);
        if (!infoFileAvanti.exists()){
            infoFilePathAvanti = newExamDir.absolutePath().append("/" + newExamDir.dirName() + ".txt");
            infoFileAvanti.setFileName(infoFilePathAvanti);
        }
        // search for info file in another location...
        if (!infoFileAvanti.exists()){
            QMessageBox::critical(this, tr("Error"), tr("Could not open scan info file (.txt)"));
        } else {
            QDate birth_date = QDate(1900,1,1);
            int gender = 3; // undefined

            QStringList fileName = newExamDir.dirName().split(",");
            if (fileName.count() < 2){   // zabezpieczenie przed nieodpowiednim katalogiem...
                QMessageBox::critical(this, tr("Error"), "Incorrect folder name! Could not load data info.");
            } else {
                QStringList fileName2 = fileName.at(1).split(" ");

                QString lastName = fileName.at(0);
                QString firstName = fileName2.at(1);

                QStringList examId = fileName2.at(2).split("_");
                QStringList examCode = fileName2.at(3).split("_");
                QString examType = examId.at(2) + " " + examCode.at(0);
                QString eye = examCode.at(1);
                QDate examDate = QDate::fromString(examCode.at(2),"yyyy-MM-dd");
                QTime examTime = QTime::fromString(examCode.at(3),"hh.mm.ss");

                int scanWidth = 0;
                int scanHeight = 0;
                int scansNumber = 0;
                int scansNumberAll = 0;
                QString device = "?";

                QString line;

                device = "Avanti";
                if (infoFileAvanti.open(QIODevice::ReadOnly)){    // AVANTI <<-------------------------------
                    QTextStream infoText(&infoFileAvanti);
                    bool scanLengthRead = false;
                    bool scanUsageRead = false;
                    do {
                        line = infoText.readLine();
                        if (line.contains("=")){
                            QStringList data = line.split("=");
                            if (data.at(0) == "XY Scan Length" && !scanLengthRead){
                                scanWidth = data.at(1).toInt();
                                scanLengthRead = true;
                            }
                            if (data.at(0) == "XY Scan Usage" && !scanUsageRead){
                                scansNumber = data.at(1).toInt();
                                scanUsageRead = true;
                            }
                            if (data.at(0) == "OCT Window Height"){
                                scanHeight = data.at(1).toInt();
                            }
                            if (data.at(0) == "Frames Per Data Group"){
                                scansNumberAll = data.at(1).toInt();
                            }
                            // brith date - not possible
                            // gender - not possible
                        }
                    } while (!line.isNull());
                    if (infoFileAvanti.isOpen())
                        infoFileAvanti.close();
                }

                QString examDimensions = QString::number(scanWidth) + " x " + QString::number(scansNumber);
                double examQI = 0;

                QString direction = "0";

                // check if patient exists in database
                int patientID = patientsDB->getPatientID(lastName,firstName);
                if (patientID == -1){
                    // add patient to the database
                    patientsDB->addNewPatient(lastName,firstName,birth_date,gender,"undef.","undef.","","");
                    patientID = patientsDB->getPatientID(lastName,firstName);
                    modelPatients->select();
                }

                // add scan to the database
                bool success = patientsDB->addNewScan(patientID, eye, QDateTime(examDate, examTime), device, examType, direction, examDimensions, examQI, scanName);
                if (success){
                    modelScans->select();

                    // check if exists Iowa analysis in ./examData/iowa/ folder
                    QString iowaPath = examDir.absolutePath().append("/iowa/");
                    QString iowaFundusPath = iowaPath.append(scanName + "_Proj_Iowa.tif");
                    QFile iowaFundusFile(iowaFundusPath);
                    if (iowaFundusFile.exists()){
                        int scanID = patientsDB->getScanID(scanName);
                        patientsDB->editScanHasAutoExplorer(scanID, true);
                        modelScans->select();
                    }

                    // create previews
                    createPreview(scanName, scanWidth, scanHeight, scansNumber, scansNumberAll, isBinary);

                } else {
                    QMessageBox::critical(this,"Error","Error while adding new scan!");
                }

                qDebug() << "All done!";
            }
        }
    }
}

void OCTAnnotate::createPreview(QString scanName, int scanWidth, int scanHeight, int scansNumber, int scansNumberAll, bool isBinary){

    qDebug() << "Creating previews for scan: " << scanName;

    QImage crossH(scanWidth,scanHeight,QImage::Format_Indexed8);
    QImage crossV(scanWidth,scanHeight,QImage::Format_Indexed8);
    QImage fundus(scanWidth,scansNumber,QImage::Format_Indexed8);
    QList< QList< QList<int> > > octData;
    QList< QList< QList<float> > > octDataTemp;
    QList<float> maxList;
    int maxAll = 0;

    if (isBinary){
        // read binary file
        QString octFileName = octDir.absolutePath().append("/" + scanName + ".OCT");
        QFile octFile(octFileName);
        if (!octFile.open(QIODevice::ReadOnly)){
            qDebug() << "Could not open file!";
            QMessageBox::critical(this,"Error","Could not open OCT binary file!");
        } else {
            qDebug() << "File opened!";
        }

        QDataStream in(&octFile);
        in.setFloatingPointPrecision(QDataStream::SinglePrecision);
        in.setByteOrder(QDataStream::LittleEndian);

        for (int p=0; p < scansNumberAll; p++){
            QList< QList<float> > img;
            float max = 0;

            for (int i=0; i < scanWidth; i++){
                QList<float> column;
                for (int j=0; j < scanHeight; j++){
                    float val = 0;
                    in >> val;
                    column.append(val);
                    if (val > max){
                        max = val;
                    }
                }
                img.append(column);
            }
            octDataTemp.append(img);
            maxList.append(max);
        }

        octFile.close();
        qDebug() << "File read sucessfully!";

        // normalize data
        float min = 800;
        for (int p=0; p < scansNumberAll; p++){
            QList< QList<int> > img;

            for (int c=0; c < scanWidth; c++){
                QList<int> column;

                for (int r=0; r < scanHeight; r++){
                    float tmp = octDataTemp[p][c][r];
                    tmp = (tmp-min)/(maxList[p]-min);
                    tmp = qBound(0,(int)(tmp*255),255);
                    column.append(tmp);
                    if (tmp > maxAll){
                        maxAll = tmp;
                    }
                }
                img.append(column);
            }
            octData.append(img);
        }

        // create projections
        QRgb rgbValue;
        for (int c=0; c < scanWidth; c++){
            for (int r=0; r < scanHeight; r++){
                int val = octData[(int)(scansNumber/2)][c][r];
                rgbValue = qRgb(val,val,val);
                crossH.setColor(val,rgbValue);
                crossH.setPixel(c,r,val);
            }
        }
        crossH = crossH.mirrored(false,true);

        for (int c=0; c < scanWidth; c++){
            for (int r=0; r < scanHeight; r++){
                int val = octData[(int)(scansNumberAll-2)][c][r];
                rgbValue = qRgb(val,val,val);
                crossV.setColor(val,rgbValue);
                crossV.setPixel(c,r,val);
            }
        }
        crossV = crossV.mirrored(false,true);

    } else {
        // read bmp files
        crossH = QImage(octDir.absolutePath().append("/" + scanName + "/Skan_nr_71.bmp"));
        crossV = QImage(octDir.absolutePath().append("/" + scanName + "/Skan_nr_143.bmp"));
    }
    crossH.save(examDir.absolutePath().append("/preview/" + scanName + "_crossH.tif"));
    crossV.save(examDir.absolutePath().append("/preview/" + scanName + "_crossV.tif"));

    QString iowaFundusPath = examDir.absolutePath().append("/iowa/" + scanName + "_Proj_Iowa.tif");
    QFile iowaFundusFile(iowaFundusPath);
    if (iowaFundusFile.exists()){
        fundus = QImage(iowaFundusPath);
    } else {
        if (!isBinary){
            fundus = QImage(octDir.absolutePath().append("/fnds_rec.bmp"));
        } else {
            // TODO: create fundus projection from binary data
            QRgb rgbValue;
            for (int y=0; y < scansNumber; y++){
                for (int x=0; x < scanWidth; x++){
                    int sum = 0;
                    for (int z=0; z < scanHeight; z++){
                        sum += octData[y][x][z];
                    }
                    sum = sum / maxAll;
                    sum = qBound(0,(int)sum,255);
                    rgbValue = qRgb(sum,sum,sum);
                    fundus.setColor(sum,rgbValue);
                    fundus.setPixel(x,y,sum);
                }
            }
        }
    }
    fundus.save(examDir.absolutePath().append("/preview/" + scanName + "_fundus.tif"));

    qDebug() << "Finished creating previews!";
}

void OCTAnnotate::on_searchForScansButton_clicked()
{
    // TODO:
    // 1. zaladuj liste katalogow ze wskazanego katalogu
    QString folderSearchPath = QFileDialog::getExistingDirectory(this, tr("Open Directory"), examDir.absolutePath(), QFileDialog::ShowDirsOnly);
    if (!folderSearchPath.isEmpty()){
        QDir folderSearchDir = QDir(folderSearchPath);
        QStringList folderList = folderSearchDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

        // 2. dla kazdego katalogu dodaj skan do bazy
        foreach(QString folderName, folderList){
            qDebug() << folderSearchPath + "/" + folderName;
            on_addScanFolderButton_clicked(folderSearchPath + "/" + folderName);
        }
    }
}

// batch processing -------------------------------------------------------------------------------
void OCTAnnotate::on_batchProcessingButton_clicked()
{
    QList<QString> folderList;

    // foreach scan in the database
    for (int r = 0; r < modelScans->rowCount(); r++){
        QSqlRecord record = modelScans->record(r);

        // 1. calculate age, if less than existing -> update
//        int patientID = record.value("patient_id").toInt();
//        QDateTime examDate = record.value("date").toDateTime();

//        int patientAge = patientsDB->getPatientAge(patientID);
//        if (patientAge == 0){
//            patientsDB->calculatePatientAge(patientID, examDate);
//        }

        // 2. find 1 of 2 fundus images, save and display

        // 3. import cross-sections, calculate cross, save and display

        // 4. check if OCTExplorer annotations are present in the folder
        QString folderName = record.value("scan_name").toString();
        QString subfolder = record.value("device").toString() + " " + record.value("series").toString();
//        QString temp = "/" + subfolder + "/" + folderName + "/" + folderName + "_Proj_Iowa.tif";
//        QString fundusPath = examDir.absolutePath().append(temp);
//        bool hasAutoExplorer = record.value("has_autoExplorer").toBool();
//        if (!hasAutoExplorer){
//            if (QFile(fundusPath).exists()){
//                qDebug() << "Found analysis for: " + folderName;
//                patientsDB->editScanHasAutoExplorer(patientsDB->getScanID(folderName),true);
//            } else {
//                qDebug() << "No OCTExplorer analysis for scan: " + temp;
//            }
//        }

        folderList.append(examDir.absolutePath() + "/" + subfolder + "/" + folderName);
    }

    BatchProcessingDialog *batchDialog = new BatchProcessingDialog(folderList, manualDir);
    batchDialog->setModal(true);
    batchDialog->show();

    // refresh data views
//    modelPatients->select();
//    modelScans->select();
}

// search / filter / show data --------------------------------------------------------------------
void OCTAnnotate::on_patientsListTableView_clicked(const QModelIndex &currentIndex)
{
    // If user whants to see only scans for selected patient:
    //int i = ui->scanListGroupCBox->currentIndex();
    //if (i == 0){
        int currentRow = currentIndex.row();
        QSqlRecord selectedRecord = modelPatients->record(currentRow);
        QString filter = "patient_id = " + selectedRecord.value("id").toString();
        if (ui->showOnlyDefaultScanRButton->isChecked())
            filter += " AND is_default = 1";
        modelScans->setFilter(filter);
    //}

    ui->fundusDBLabel->setPixmap(QPixmap());
    ui->bscanHLabel->setPixmap(QPixmap());
    ui->bscanVLabel->setPixmap(QPixmap());

    // TODO: odczyt danyc z pliku .CAVRI na temat ostrości wzroku...
}

void OCTAnnotate::on_scanListGroupCBox_currentIndexChanged(int index)
{
    QModelIndex currentIndex;
    int currentRow = -1;
    QSqlRecord selectedRecord;
    QSqlRecord recordPatient;
    QString filter1 = "";
    QString filter2 = "";

    QString pathologyStr = "";
    bool pOD = false;
    bool pOS = false;

    switch (index){
    case 0:
        currentIndex = ui->patientsListTableView->currentIndex();
        currentRow = currentIndex.row();
        selectedRecord = modelPatients->record(currentRow);
        filter1 = "patient_id = " + selectedRecord.value("id").toString();
        if (ui->showOnlyDefaultScanRButton->isChecked())
            filter1 += " AND is_default = 1";
        modelScans->setFilter(filter1);
        break;
    case 1:
        // wyszukaj skany wg. filtracji
        if (ui->searchPathologyCBox->currentIndex() > 0){
            switch (ui->searchPathologyCBox->currentIndex()){
            case 1:
                pathologyStr = "---";
                break;
            case 2:
                pathologyStr = "VMA";
                break;
            case 3:
                pathologyStr = "VMT";
                break;
            case 4:
                pathologyStr = "ERM";
                break;
            case 5:
                pathologyStr = "MH";
                break;
            case 6:
                pathologyStr = "CNV";
                break;
            case 7:
                pathologyStr = "other";
                break;
            }
        }

        for (int r = 0; r < modelPatients->rowCount(); r++){
            recordPatient = modelPatients->record(r);
            if (filter1.length() > 0){
                filter1 += " OR ";
            }
            if (ui->searchPathologyCBox->currentIndex() > 0){
                if (recordPatient.value("od").toString().contains(pathologyStr)){
                    // eye == OD
                    pOD = true;
                } else {
                    pOD = false;
                }
                if (recordPatient.value("os").toString().contains(pathologyStr)){
                    // eye == OS
                    pOS = true;
                } else {
                    pOS = false;
                }
                if (pOD && !pOS){
                    filter2 = " AND eye = 'OD'";
                } else if (!pOD && pOS){
                    filter2 = " AND eye = 'OS'";
                } else {
                    filter2 = "";
                }
            }
            filter1 += "(patient_id = " + recordPatient.value("id").toString();
            filter1 += filter2;
            if (ui->showOnlyDefaultScanRButton->isChecked())
                filter1 += " AND is_default = 1";
            filter1 += ")";
//            qDebug() << "scan search filter: " + filter1;
        }
        modelScans->setFilter(filter1);
        break;
    case 2:
        if (ui->showOnlyDefaultScanRButton->isChecked())
            filter1 = "is_default = 1";
        else
            filter1 = "";
        modelScans->setFilter(filter1);
        break;
    }
}

void OCTAnnotate::on_searchPatientDBButton_clicked()
{
    QString last_name = ui->searchLastNameLEdit->text();
    QString first_name = ui->searchFirstNameLEdit->text();
    int gender = ui->searchGenderCBox->currentIndex();

    QString pathologyStr = "";
    switch (ui->searchPathologyCBox->currentIndex()){
    case 1:
        pathologyStr = "---";
        break;
    case 2:
        pathologyStr = "VMA";
        break;
    case 3:
        pathologyStr = "VMT";
        break;
    case 4:
        pathologyStr = "ERM";
        break;
    case 5:
        pathologyStr = "MH";
        break;
    case 6:
        pathologyStr = "CNV";
        break;
    case 7:
        pathologyStr = "other";
        break;
    }

    QString file_no = ui->searchFileNoLEdit->text();
    QString notes = ui->searchNotesLEdit->text();
    int ageFrom = ui->searchAgeFromLEdit->text().toInt();
    int ageTo = ui->searchAgeToLEdit->text().toInt();

    QString filter = "";
    if (last_name.length() > 0){
        filter += "last_name LIKE '%" + last_name + "%'";
    }
    if (first_name.length() > 0){
        if (filter.length() > 0){
            filter += " AND ";
        }
        filter += "first_name LIKE '%" + first_name + "%'";
    }
    if (file_no.length() > 0){
        if (filter.length() > 0){
            filter += " AND ";
        }
        filter += "file_no LIKE '%" + file_no + "%'";
    }
    if (notes.length() > 0){
        if (filter.length() > 0){
            filter += " AND ";
        }
        filter += "notes LIKE '%" + notes + "%'";
    }
    if (pathologyStr.length() > 0){
        if (filter.length() > 0){
            filter += " AND ";
        }
        filter += "(od LIKE '%" + pathologyStr + "%' OR os LIKE '%" + pathologyStr + "%')";
    }
    if (gender > 0){
        if (filter.length() > 0){
            filter += " AND ";
        }
        filter += "gender = " + QString::number(gender);
    }
    if (ageFrom > 0){
        if (filter.length() > 0){
            filter += " AND ";
        }
        filter += "age > " + QString::number(ageFrom);
    }
    if (ageTo > 0){
        if (filter.length() > 0){
            filter += " AND ";
        }
        filter += "age < " + QString::number(ageTo);
    }
    qDebug() << "search filter: " << filter;
    modelPatients->setFilter(filter);

    on_scanListGroupCBox_currentIndexChanged(1);
}

void OCTAnnotate::on_scansListTableView_clicked(const QModelIndex &index)
{
    int currentRow = index.row();
    QSqlRecord selectedRecord = modelScans->record(currentRow);
    QString scanName = selectedRecord.value("scan_name").toString();

    QString fundusPath = examDir.absolutePath().append("/preview/" + scanName + "_fundus.tif");
    if (QFile(fundusPath).exists()){
        QImage fundusImage(fundusPath);
        ui->fundusDBLabel->setPixmap(QPixmap::fromImage(fundusImage));
    } else {
        // if preview does not exist but the .oct scan does, then create the preview
        QString scanPath = octDir.absolutePath().append("/" + scanName + ".oct");
        if (QFile(scanPath).exists()){
            createPreview(scanName, 385, 640, 141, 144, true);
            ui->fundusDBLabel->setPixmap(QPixmap::fromImage(QImage(fundusPath)));
        } else {
            ui->fundusDBLabel->setPixmap(QPixmap());
        }
    }

    QString bscanHPath = examDir.absolutePath().append("/preview/" + scanName + "_crossH.tif");
    QString bscanVPath = examDir.absolutePath().append("/preview/" + scanName + "_crossV.tif");

    if (QFile(bscanHPath).exists()){
        QImage imageH(bscanHPath);
        // contrast enhancement
        Calculate *calc = new Calculate();
        calc->imageEnhancement(&imageH, 1.2, 40);
        imageH = imageH.convertToFormat(QImage::Format_ARGB32);
        QPainter * painter = new QPainter(&imageH);
        painter->setPen(Qt::white);
        painter->setFont(QFont("Arial",30,20));
        painter->drawText(QPoint(5,40), "-");
        painter->end();
        ui->bscanHLabel->setPixmap(QPixmap::fromImage(imageH));
    } else {
        ui->bscanHLabel->setPixmap(QPixmap());
    }

    if (QFile(bscanVPath).exists()){
        QImage imageV(bscanVPath);
        // contrast enhancement
        Calculate *calc = new Calculate();
        calc->imageEnhancement(&imageV, 1.2, 40);
        imageV = imageV.convertToFormat(QImage::Format_ARGB32);
        QPainter * painter = new QPainter(&imageV);
        painter->setPen(Qt::white);
        painter->setFont(QFont("Arial",30));
        painter->drawText(QPoint(5,40), "|");
        painter->end();
        ui->bscanVLabel->setPixmap(QPixmap::fromImage(imageV));
    } else {
        ui->bscanVLabel->setPixmap(QPixmap());
    }
}

void OCTAnnotate::on_showAllScansRButton_toggled()
{
    on_scanListGroupCBox_currentIndexChanged(ui->scanListGroupCBox->currentIndex());
}

void OCTAnnotate::on_scansListTableView_doubleClicked(const QModelIndex &currentIndex)
{
    int row = currentIndex.row();
    QSqlRecord record = modelScans->record(row);
    QString scanFolder = examDir.absolutePath() + "/" + record.value("scan_name").toString();
    on_actionLoadOCTSequence_triggered(scanFolder);
}
