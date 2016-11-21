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

OCTAnnotate::OCTAnnotate(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::OCTAnnotate)
{
    ui->setupUi(this);
    QFont font;
    font.setPixelSize(11);
    this->setFont(font);
    quit = false;

    SettingsDialog *settingsDialog = new SettingsDialog();
    databasePath = settingsDialog->getDatabasePath();
    dataSaveStructure = settingsDialog->getDataSaveStructure();
    openBscanNumber = settingsDialog->getOpenBskan();
    showETDRSGrid = settingsDialog->getShowETDRSGrid();
    showCenterOnBscan = settingsDialog->getShowCenterOnBscan();
    showBscanOnErrorPlot = settingsDialog->getShowBscanOnErrorPlot();
    blockPCV = settingsDialog->getBlockPCV();
    examDir = QDir(settingsDialog->getPathOctExam());
    manualDir = QDir(settingsDialog->getPathManualSegm());
    autoDir = QDir(settingsDialog->getPathAutoSegm());
    tmpDir = examDir;
//    ui->actionImageFlattening->setChecked(true);
    ui->actionEditAnnotations->setChecked(true);

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

    ui->actionSaveGeneralExam->setEnabled(false);
    ui->actionSaveOCTExam->setEnabled(false);

    currentDir = QDir::current();

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
    ui->amslerRImageLabel->installEventFilter(this);
    ui->amslerLImageLabel->installEventFilter(this);
    ui->bScanHCPlot->installEventFilter(this);
    ui->bScanVCPlot->installEventFilter(this);
    ui->errorVirtualMapCPlot->installEventFilter(this);

    myPix = QPixmap();
    myPix.fill(Qt::transparent);
    myPenColor = Qt::red;
    myPenWidth = 2;
    isControlPressed = false;
    drawing = false;
    erasing = false;
    //displayNormalAnnotations = false;
    eraseAnnotations = false;
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

    //ui->penButton->setStyleSheet("background-color: red");
    //ui->sprayLightButton->setStyleSheet("background-color: white");
    //ui->sprayDarkButton->setStyleSheet("background-color: black; color: white");

    // Amsler grid lines
    drawGrid = true;
    drawAmsler = false;
    repaintAmsler = false;
    amsLinesNumber = 21;
    amsLinePixDist = 15;
    maxDistWidth = 0;
    for (int i=0; i < amsLinesNumber; i++){
        gridVLines.append(QLine(i*amsLinePixDist, 0, i*amsLinePixDist, amsLinePixDist*(amsLinesNumber-1)));
        gridHLines.append(QLine(0, i*amsLinePixDist , amsLinePixDist*(amsLinesNumber-1), i*amsLinePixDist));
    }
    amsGridPen = QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    amsPen = QPen(Qt::red, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    amsSprayLight = QPen(Qt::white, 10, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    amsSprayDark = QPen(Qt::black, 10, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    amsDistIdSelected = -1;
    amsDistEyeSelected = "";

    QStringList shapesList;
    //shapesList << "Soczewka wypukła";
    //shapesList << "Soczewka wklęsła";
    shapesList << "Spirala w prawo";
    shapesList << "Spirala w lewo";
    //shapesList << "Falista siatka";
    ui->distShapeCBox->addItems(shapesList);

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
    thresholds.append(50);
    foreach (double value, thresholds) {
        ui->contactThresholdCBox->addItem(QString::number(value) + " [um]");
    }
    ui->contactThresholdCBox->setCurrentIndex(1);

    ui->tabWidget->removeTab(1);
    ui->tabWidget->removeTab(2);
    appVersion = "v1.6";
    this->setWindowTitle("OCTAnnotate " + appVersion);
    progressBar = new QProgressBar();
    progressBar->setMaximumWidth(200);
    progressBar->setMaximum(100);
    ui->statusBar->addPermanentWidget(progressBar);
    progressBar->setVisible(false);

    patientData = PatientData();
}

OCTAnnotate::~OCTAnnotate()
{
    patientsDB->closeDatabase();
    delete ui;
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
    modelScans->setHeaderData(10, Qt::Horizontal, QObject::tr("File path"));
    modelScans->setHeaderData(11, Qt::Horizontal, QObject::tr("Folder path"));
    modelScans->setHeaderData(12, Qt::Horizontal, QObject::tr("Notes"));
    modelScans->setHeaderData(13, Qt::Horizontal, QObject::tr("Default"));
    modelScans->setHeaderData(14, Qt::Horizontal, QObject::tr("AutoExplorer"));

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
    ui->scansListTableView->setColumnHidden(10, true);
    ui->scansListTableView->resizeColumnsToContents();
//    ui->scansListTableView->horizontalHeader()->setStretchLastSection(true);
    ui->scansListTableView->horizontalHeader()->moveSection(11,0);
    ui->scansListTableView->sortByColumn(11,Qt::AscendingOrder);
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


// select OCT exam --------------------------------------------------------------------------------
void OCTAnnotate::on_actionLoadPatientOCT_triggered(QString scanFolderPath)
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
            dirName = QFileDialog::getExistingDirectory(this, tr("Open Directory"), examDir.path(), QFileDialog::ShowDirsOnly);

        qDebug() << "Opening scan: " << dirName;

        if (!dirName.isEmpty()){
            octDir = QDir(dirName);

            patientData = PatientData();

            ui->statusBar->showMessage("Trwa odczyt danych badania OCT...");
            progressBar->setMaximum(100);
            progressBar->setVisible(true);
            progressBar->setValue(0);

            ReadWriteData *rwData = new ReadWriteData();
            rwData->setDataObject(&patientData);
            rwData->setDirectoryOct(&octDir);
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

void OCTAnnotate::listAmslerDistortions(){
    QStringList list;

    QList<AmslerDist> distR = patientData.getAmslerDistList("R");
    foreach (AmslerDist dist, distR) {
        list.append(QString::number(dist.getId()+1) + ". " + encodeDistType(dist.getType()));
    }
    ui->amslerRDistList->clear();
    ui->amslerRDistList->addItems(list);

    list.clear();
    QList<AmslerDist> distL = patientData.getAmslerDistList("L");
    foreach (AmslerDist dist, distL) {
        list.append(QString::number(dist.getId()+1) + ". " + encodeDistType(dist.getType()));
    }
    ui->amslerLDistList->clear();
    ui->amslerLDistList->addItems(list);
}


// load image -------------------------------------------------------------------------------------
void OCTAnnotate::loadImage(int imageNumber){
    QFileInfo imageFileInfo(patientData.getImageFileList().at(imageNumber));

    // load image
    QImage image(patientData.getImageFileList().at(imageNumber));
    if (!image.isNull()){
        orgImageSize = image.size();

        currentImageNumber = imageNumber;
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
            flatDiff = patientData.getFlatDifferences(currentImageNumber);
            image = calc->flattenImage(&image,flatDiff);
        }
        calc->imageEnhancement(&image, contrast, brightness);

        // rescale image
        scaleFactor = qBound(0,scaleFactor,scales.count()-1);
        int h = patientData.getBscanHeight() / scales[scaleFactor]; // TODO: zle liczone h, nie bierze pod uwage przesuniecia rolką
        double y2 = (int)((double)(patientData.getBscanHeight() - h) / 2.0);
        double y1 = (double)patientData.getBscanHeight() - y2;
        QImage newImage = image.copy(0,y2,patientData.getBscanWidth(),y1);

        // display image
        ui->bScanHCPlot->yAxis->setRange(y1,y2);
        ui->bScanHCPlot->axisRect()->setBackground(QPixmap::fromImage(newImage),true,Qt::IgnoreAspectRatio);
        // display annotations
        displayAnnotations(flatDiff);

        //scaleFactor = 0;
        //rescaleImage();
//        if (scaleFactorY < 2.0)
            ui->zoomInButton->setEnabled(true);
//        if (scaleFactorY > 0.5)
            ui->zoomOutButton->setEnabled(true);
//        bscanLabel->resize(scrollArea->width()-20, orgImageSize.height() * scaleFactorY);

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

    // contrast enhancement
    Calculate *calc = new Calculate();
    QList<int> flatDiffNormal;
    for (int col=0; col < patientData.getBscansNumber(); col++){
        flatDiffNormal.append(0);
    }
    if (flattenImage){
        flatDiffNormal = patientData.getFlatDifferencesNormal(currentNormalImageNumber);
        normalImage = calc->flattenImage(&normalImage,flatDiffNormal);
    }
    calc->imageEnhancement(&normalImage, contrast, brightness);

    // display image
    ui->bScanVCPlot->axisRect()->setBackground(QPixmap::fromImage(normalImage),true,Qt::IgnoreAspectRatio);
    ui->currNormalImageNumberLEdit->setText(QString::number(currentNormalImageNumber));
    // scale

    // display annotations
    displayNormalAnnotations(flatDiffNormal);

    // enable / disable buttons
}

void OCTAnnotate::on_nextImageButton_clicked()
{
    if (!patientData.getImageFileList().isEmpty()){
        if (currentImageNumber < (patientData.getBscansNumber() - 1)){
            loadImage(currentImageNumber + 1);
            loadNormalImage(currentNormalImageNumber);
            fundusAnnotate = true;
            //on_computeVirtualMapButton_clicked();
            currentImageLayersNumber = currentImageNumber;
            //displayImageLayersPlot(currentImageLayersNumber);
        }
    }
}

void OCTAnnotate::on_prevImageButton_clicked(){
    if (!patientData.getImageFileList().isEmpty()){
        if (currentImageNumber > 0){
            loadImage(currentImageNumber - 1);
            loadNormalImage(currentNormalImageNumber);
            fundusAnnotate = true;
            //on_computeVirtualMapButton_clicked();
            currentImageLayersNumber = currentImageNumber;
            //displayImageLayersPlot(currentImageLayersNumber);
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
        //displayVirtualMap(ui->virtualMapImageCPlot);
        //displayVirtualMap(ui->virtualMapAutoImageCPlot, true);
        currentImageLayersNumber = currentImageNumber;
        //displayImageLayersPlot(currentImageLayersNumber);
        ui->currImageNumberLEdit->clearFocus();
    }
}

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

void OCTAnnotate::on_tabWidget_currentChanged()
{
    QWidget *currWidget = ui->tabWidget->currentWidget();

    if (currWidget == ui->tabAmslerCharts){    // Amsler
        drawGrid = true;
    } else if (currWidget == ui->tabOCTExam){    // OCT Exam Tab
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
    patientData.setAmslerComment(ui->amslerRCommentTEdit->toPlainText(),"R");
    patientData.setAmslerComment(ui->amslerLCommentTEdit->toPlainText(),"L");

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
        } else if (target == ui->amslerRImageLabel){

            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (event->type() == QEvent::MouseButtonPress){
                if (mouseEvent->button() == Qt::LeftButton){
                    lastAPoint = mouseEvent->pos();
                    amsLinesTemp.clear();
                    drawAmsler = true;
                }
            }
            if (event->type() == QEvent::MouseMove){
                if ((mouseEvent->buttons() & Qt::LeftButton) && drawAmsler){
                    myPix = QPixmap(*ui->amslerRImageLabel->pixmap());
                    drawOnAmsler(mouseEvent->pos());
                    ui->amslerRImageLabel->setPixmap(myPix);
                    if (currDistType < Convex)
                        lastAPoint = mouseEvent->pos();
                }
            }
            if (event->type() == QEvent::MouseButtonRelease){
                if (mouseEvent->button() == Qt::LeftButton && drawAmsler){
                    myPix = QPixmap(*ui->amslerRImageLabel->pixmap());
                    drawOnAmsler(mouseEvent->pos());
                    ui->amslerRImageLabel->setPixmap(myPix);
                    lastAPoint = mouseEvent->pos();
                    if ((currDistType < Convex) || (amsWidthTemp >= amsLinePixDist)){
                        patientData.addAmslerDist("R", currDistType, amsLinesTemp, amsPointTemp, amsWidthTemp);
                    }
                    listAmslerDistortions();
                    drawAmsler = false;
                    maxDistWidth = 0;
                    amsLinesTemp.clear();
                    amsPointTemp = QPoint(-1,-1);
                    amsWidthTemp = 0;
                }
            }

        } else if (target == ui->amslerLImageLabel){

            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (event->type() == QEvent::MouseButtonPress){
                if (mouseEvent->button() == Qt::LeftButton){
                    lastAPoint = mouseEvent->pos();
                    amsLinesTemp.clear();
                    drawAmsler = true;
                }
            }
            if (event->type() == QEvent::MouseMove){
                if ((mouseEvent->buttons() & Qt::LeftButton) && drawAmsler){
                    myPix = QPixmap(*ui->amslerLImageLabel->pixmap());
                    drawOnAmsler(mouseEvent->pos());
                    ui->amslerLImageLabel->setPixmap(myPix);
                    if (currDistType < Convex)
                        lastAPoint = mouseEvent->pos();
                }
            }
            if (event->type() == QEvent::MouseButtonRelease){
                if (mouseEvent->button() == Qt::LeftButton && drawAmsler){
                    myPix = QPixmap(*ui->amslerLImageLabel->pixmap());
                    drawOnAmsler(mouseEvent->pos());
                    ui->amslerLImageLabel->setPixmap(myPix);
                    lastAPoint = mouseEvent->pos();
                    if ((currDistType < Convex) || (amsWidthTemp >= amsLinePixDist)){
                        patientData.addAmslerDist("L", currDistType, amsLinesTemp, amsPointTemp, amsWidthTemp);
                    }
                    listAmslerDistortions();
                    drawAmsler = false;
                    maxDistWidth = 0;
                    amsLinesTemp.clear();
                    amsPointTemp = QPoint(-1,-1);
                    amsWidthTemp = 0;
                }
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

void OCTAnnotate::resizeEvent(QResizeEvent*){
//    scaleFactorX = (double)(scrollArea->width()-20) / (double)(patientData.getBscanWidth());
//    scaleFactorY = (double)(scrollArea->height()-2) / (double)(patientData.getBscanHeight());
//    bscanLabel->resize(scrollArea->width()-20, orgImageSize.height() * scaleFactorY);
//    bscan2Label->resize(scrollArea2->width()-20, orgImageSize.height() * scaleFactorY);
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

    if ((ui->tabWidget->currentWidget() == ui->tabAmslerCharts) && drawGrid){

        QPixmap pixR = QPixmap((amsLinesNumber-1)*amsLinePixDist+1, (amsLinesNumber-1)*amsLinePixDist+1);
        QPixmap pixL = QPixmap((amsLinesNumber-1)*amsLinePixDist+1, (amsLinesNumber-1)*amsLinePixDist+1);
        pixR.fill(Qt::transparent);
        pixL.fill(Qt::transparent);

        pixR = drawAmslerGrid(&pixR);
        pixL = drawAmslerGrid(&pixL);
        pixR = drawAmslerAnnotations(&pixR,"R");
        pixL = drawAmslerAnnotations(&pixL,"L");
        ui->amslerRImageLabel->setPixmap(pixR);
        ui->amslerLImageLabel->setPixmap(pixL);

        drawGrid = false;
    }

    /*if (repaintAmsler){
        QPixmap pix;
        if (amsDistEyeSelected == "R"){
            pix = *ui->amslerRImageLabel->pixmap();
            pix = drawAmslerAnnotation(&pix,"R",amsDistIdSelected);
            ui->amslerRImageLabel->setPixmap(pix);

        } else if (amsDistEyeSelected == "L"){
            pix = *ui->amslerLImageLabel->pixmap();
            pix = drawAmslerAnnotation(&pix,"L",amsDistIdSelected);
            ui->amslerLImageLabel->setPixmap(pix);
        }

        repaintAmsler = false;
    }*/
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

        // draw new pixels
        QList<QPoint> pointsList = patientData.getLayerPoints(currentImageNumber, selectedLayer, min, max);
        int pointsCount = pointsList.count();
        int height = patientData.getBscanHeight();
        QVector<double> x(pointsCount), y(pointsCount);
        for (int i=0; i<pointsCount; i++){
            x[i] = pointsList.at(i).x();
            y[i] = height - pointsList.at(i).y();
            if (flattenImage){
                int diff = patientData.getFlatDifference(currentImageNumber,x[i]);
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

        // draw new pixels
        QList<QPoint> pointsList = patientData.getLayerPoints(currentNormalImageNumber, selectedLayer, min, max, true);
        int pointsCount = pointsList.count();
        int height = patientData.getBscanHeight();
        QVector<double> x(pointsCount), y(pointsCount);
        for (int i=0; i<pointsCount; i++){
            x[i] = min+i+1;
            y[i] = height - pointsList.at(i).y();
            if (flattenImage){
                int diff = patientData.getFlatDifference(x[i],currentNormalImageNumber);
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

// draw asmler ------------------------------------------------------------------------------------
QPixmap OCTAnnotate::drawAmslerGrid(QPixmap *pixmap){

    // draw grid lines
    QPainter painter(pixmap);

    painter.setPen(amsGridPen);
    foreach (QLine l, gridVLines) {
        painter.drawLine(l);
    }
    foreach (QLine l, gridHLines) {
        painter.drawLine(l);
    }

    QPoint middlePoint = QPoint(amsLinePixDist*(amsLinesNumber-1)/2, amsLinePixDist*(amsLinesNumber-1)/2);
    painter.setPen(QPen(Qt::black, 8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawPoint(middlePoint);

    return *pixmap;
}

QPixmap OCTAnnotate::drawAmslerAnnotations(QPixmap *pixmap, QString eye){
    QPainter painter(pixmap);
    QList<QLine> lines;
    QPoint point, topLeft, bottomRight;
    int width, i, lineNumber;

    QList<AmslerDist> amslerDistList = patientData.getAmslerDistList(eye);
    foreach (AmslerDist dist, amslerDistList) {
        switch (dist.getType()) {
        case Pen:
            painter.setPen(amsPen);
            lines = dist.getLines();
            foreach (QLine l, lines) {
                painter.drawLine(l);
            }
            break;

        case WhiteSpot:
            painter.setPen(amsSprayLight);
            lines = dist.getLines();
            foreach (QLine l, lines) {
                painter.drawLine(l);
            }
            break;

        case BlackSpot:
            painter.setPen(amsSprayDark);
            lines = dist.getLines();
            foreach (QLine l, lines) {
                painter.drawLine(l);
            }
            break;

        case Convex:
        case Concave:
            point = dist.getPoint();
            width = dist.getWidth();
            topLeft = QPoint(point.x() - width, point.y() - width);
            bottomRight = QPoint(point.x() + width - 1, point.y() + width - 1);

            painter.setPen(QPen(Qt::white, 1));
            painter.setBrush(QBrush(Qt::white));
            painter.drawRect(QRect(topLeft, bottomRight));
            i = 0;
            lineNumber = width/amsLinePixDist + 1;
            maxDistWidth = width;
            painter.setPen(amsGridPen);

            while (i < lineNumber){
                drawAmslerStraightLines(point, i, &painter);
                if (i > 0){
                    drawAmslerCurvedLines(point, i, &painter, dist.getType());
                }
                i++;
            }
            maxDistWidth = 0;
            break;

        case RightSpiral:
        case LeftSpiral:
        {
            point = dist.getPoint();
            width = dist.getWidth();
            topLeft = QPoint(point.x() - width, point.y() - width);
            bottomRight = QPoint(point.x() + width - 1, point.y() + width - 1);

            painter.setPen(QPen(Qt::white, 1));
            painter.setBrush(QBrush(Qt::white));
            painter.drawRect(QRect(topLeft, bottomRight));

            painter.setPen(QPen(Qt::red, 1));
            QList<QPoint> points;
            QList<QPoint> pointList;
            QPoint p1, p2;

            for (int i=0; i <= width; i++){

                // compute points' coordinates
                int linesCount = (i/amsLinePixDist);
                float angle = (width-i+1)*3.14/180;
                if ( dist.getType() == LeftSpiral )
                    angle = -angle;
                pointList.clear();

                for (int j=0; j <= linesCount; j++){
                    p1 = QPoint(point.x() + j*amsLinePixDist, point.y() - i);
                    p2 = QPoint(point.x() + j*amsLinePixDist, point.y() + i);
                    pointList.append(p1);
                    pointList.append(p2);

                    p1 = QPoint(point.x() - j*amsLinePixDist, point.y() - i);
                    p2 = QPoint(point.x() - j*amsLinePixDist, point.y() + i);
                    pointList.append(p1);
                    pointList.append(p2);

                    p1 = QPoint(point.x() - i, point.y() + j*amsLinePixDist);
                    p2 = QPoint(point.x() + i, point.y() + j*amsLinePixDist);
                    pointList.append(p1);
                    pointList.append(p2);

                    p1 = QPoint(point.x() - i, point.y() - j*amsLinePixDist);
                    p2 = QPoint(point.x() + i, point.y() - j*amsLinePixDist);
                    pointList.append(p1);
                    pointList.append(p2);
                }

                foreach (QPoint p, pointList) {
                    p.setX(qCos(angle)*(p.x()-point.x()) - qSin(angle)*(p.y() - point.y()) + point.x());
                    p.setY(qSin(angle)*(p.x()-point.x()) + qCos(angle)*(p.y() - point.y()) + point.y());
                    points.append(p);
                }
            }

            // draw points
            foreach (QPoint p, points) {
                painter.drawPoint(p);
            }

            maxDistWidth = 0;
        }
            break;
        case None:
            break;
        }
        // TODO: other types...
    }

    return *pixmap;
}

QPixmap OCTAnnotate::drawAmslerAnnotation(QPixmap *pixmap, QString eye, int id){
    QPainter painter(pixmap);
    QList<QLine> lines;
    QPoint point, topLeft, bottomRight;
    int width, i, lineNumber;

    AmslerDist dist = patientData.getAmslerDist(eye, id);

    switch (dist.getType()) {
    case Pen:
        painter.setPen(QPen(Qt::blue, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        lines = dist.getLines();
        foreach (QLine l, lines) {
            painter.drawLine(l);
        }
        break;

    case WhiteSpot:
        painter.setPen(QPen(Qt::blue, 10, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        lines = dist.getLines();
        foreach (QLine l, lines) {
            painter.drawLine(l);
        }
        break;

    case BlackSpot:
        painter.setPen(QPen(Qt::blue, 10, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        lines = dist.getLines();
        foreach (QLine l, lines) {
            painter.drawLine(l);
        }
        break;

    case Convex:
    case Concave:
        point = dist.getPoint();
        width = dist.getWidth();
        topLeft = QPoint(point.x() - width, point.y() - width);
        bottomRight = QPoint(point.x() + width - 1, point.y() + width - 1);

        painter.setPen(QPen(Qt::white, 1));
        painter.setBrush(QBrush(Qt::white));
        painter.drawRect(QRect(topLeft, bottomRight));
        i = 0;
        lineNumber = width/amsLinePixDist + 1;
        maxDistWidth = width;
        painter.setPen(amsGridPen);

        while (i < lineNumber){ // TODO: other color...
            drawAmslerStraightLines(point, i, &painter);
            if (i > 0){
                drawAmslerCurvedLines(point, i, &painter, dist.getType());
            }
            i++;
        }
        maxDistWidth = 0;
        break;

    case RightSpiral:
    case LeftSpiral:
    {
        point = dist.getPoint();
        width = dist.getWidth();
        topLeft = QPoint(point.x() - width, point.y() - width);
        bottomRight = QPoint(point.x() + width - 1, point.y() + width - 1);

        painter.setPen(QPen(Qt::white, 1));
        painter.setBrush(QBrush(Qt::white));
        painter.drawRect(QRect(topLeft, bottomRight));

        painter.setPen(QPen(Qt::blue, 1));
        QList<QPoint> points;
        QList<QPoint> pointList;
        QPoint p1, p2;

        for (int i=0; i <= width; i++){

            // compute points' coordinates
            int linesCount = (i/amsLinePixDist);
            float angle = (width-i+1)*3.14/180;
            if ( dist.getType() == LeftSpiral )
                angle = -angle;
            pointList.clear();

            for (int j=0; j <= linesCount; j++){
                p1 = QPoint(point.x() + j*amsLinePixDist, point.y() - i);
                p2 = QPoint(point.x() + j*amsLinePixDist, point.y() + i);
                pointList.append(p1);
                pointList.append(p2);

                p1 = QPoint(point.x() - j*amsLinePixDist, point.y() - i);
                p2 = QPoint(point.x() - j*amsLinePixDist, point.y() + i);
                pointList.append(p1);
                pointList.append(p2);

                p1 = QPoint(point.x() - i, point.y() + j*amsLinePixDist);
                p2 = QPoint(point.x() + i, point.y() + j*amsLinePixDist);
                pointList.append(p1);
                pointList.append(p2);

                p1 = QPoint(point.x() - i, point.y() - j*amsLinePixDist);
                p2 = QPoint(point.x() + i, point.y() - j*amsLinePixDist);
                pointList.append(p1);
                pointList.append(p2);
            }

            foreach (QPoint p, pointList) {
                p.setX(qCos(angle)*(p.x()-point.x()) - qSin(angle)*(p.y() - point.y()) + point.x());
                p.setY(qSin(angle)*(p.x()-point.x()) + qCos(angle)*(p.y() - point.y()) + point.y());
                points.append(p);
            }
        }

        // draw points
        foreach (QPoint p, points) {
            painter.drawPoint(p);
        }

        maxDistWidth = 0;
    }
        break;
    case None:
        break;
    }
    // TODO: other types...

    return *pixmap;
}

void OCTAnnotate::drawOnAmsler(QPoint endPoint){

    QPoint startPoint = lastAPoint;
    QPainter painter(&myPix);

    switch (currDistType) {
    case Pen:
        painter.setPen(amsPen);         // draw with red pen
        break;
    case WhiteSpot:
        painter.setPen(amsSprayLight);  // draw with white spray
        break;
    case BlackSpot:
        painter.setPen(amsSprayDark);   // draw with dark spray
        break;
    default:
        break;
    }

    int min = qMin(startPoint.x(),endPoint.x());
    int max = qMax(startPoint.x(),endPoint.x());

    if ((min >= 0) && (max < myPix.width())){

        switch (currDistType) {
        case None:
            break;
        case Pen:
        case WhiteSpot:
        case BlackSpot:
            painter.drawLine(startPoint, endPoint);
            amsLinesTemp.append(QLine(startPoint, endPoint));
            generalDataModified = true;
            break;

        case Convex:
        case Concave:
        {
            startPoint = findNearestCross(lastAPoint);
            painter.setBrush(QBrush(Qt::white));

            int dx = qAbs(endPoint.x() - startPoint.x());
            int dy = qAbs(endPoint.y() - startPoint.y());
            int distWidth = qMax(dx, dy);

            if (distWidth < maxDistWidth){  // new area is smaller then the last
                eraseAmslerArea(startPoint, &painter);
            }

            QPoint topLeft(startPoint.x() - distWidth, startPoint.y() - distWidth);
            QPoint bottomRight(startPoint.x() + distWidth - 1, startPoint.y() + distWidth - 1);
            painter.setPen(QPen(Qt::white, 1));
            painter.drawRect(QRect(topLeft, bottomRight));
            int i=0;
            int lineNumber = distWidth/amsLinePixDist + 1;
            painter.setPen(amsGridPen);

            while (i < lineNumber){
                drawAmslerStraightLines(startPoint, i, &painter);
                if (i > 0){
                    drawAmslerCurvedLines(startPoint, i, &painter, currDistType);
                }
                i++;
            }
            amsPointTemp = startPoint;
            amsWidthTemp = distWidth;
            maxDistWidth = qMax(maxDistWidth, distWidth);
            generalDataModified = true;
        }
            break;

        case RightSpiral:   // TODO: poprawić - brakuje części wewnętrznych linii... (to samo w drawAmslerAnnotations())
        case LeftSpiral:
        {
            startPoint = findNearestCross(lastAPoint);
            painter.setBrush(QBrush(Qt::white));

            int dx = qAbs(endPoint.x() - startPoint.x());
            int dy = qAbs(endPoint.y() - startPoint.y());
            int distWidth = qMax(dx, dy);
            if (distWidth < maxDistWidth){  // new area is smaller then the last
                eraseAmslerArea(startPoint, &painter);
            }

            QPoint topLeft(startPoint.x() - distWidth, startPoint.y() - distWidth);
            QPoint bottomRight(startPoint.x() + distWidth - 1, startPoint.y() + distWidth - 1);
            painter.setPen(QPen(Qt::white, 1));
            painter.drawRect(QRect(topLeft, bottomRight));

            painter.setPen(QPen(Qt::red, 1));
            QList<QPoint> points;
            QPoint p1,p2;

            QList<QPoint> pointList;
            for (int i=0; i <= distWidth; i++){

                // compute points' coordinates
                int linesCount = (i/amsLinePixDist);
                float angle = (distWidth-i+1)*3.14/180;
                if (currDistType == LeftSpiral)
                    angle = -angle;
                pointList.clear();

                for (int j=0; j <= linesCount; j++){
                    p1 = QPoint(startPoint.x() + j*amsLinePixDist, startPoint.y() - i);
                    p2 = QPoint(startPoint.x() + j*amsLinePixDist, startPoint.y() + i);
                    pointList.append(p1);
                    pointList.append(p2);

                    p1 = QPoint(startPoint.x() - j*amsLinePixDist, startPoint.y() - i);
                    p2 = QPoint(startPoint.x() - j*amsLinePixDist, startPoint.y() + i);
                    pointList.append(p1);
                    pointList.append(p2);

                    p1 = QPoint(startPoint.x() - i, startPoint.y() + j*amsLinePixDist);
                    p2 = QPoint(startPoint.x() + i, startPoint.y() + j*amsLinePixDist);
                    pointList.append(p1);
                    pointList.append(p2);

                    p1 = QPoint(startPoint.x() - i, startPoint.y() - j*amsLinePixDist);
                    p2 = QPoint(startPoint.x() + i, startPoint.y() - j*amsLinePixDist);
                    pointList.append(p1);
                    pointList.append(p2);
                }

                // rotate points
                foreach (QPoint p, pointList) {
                    p.setX(qCos(angle)*(p.x()-startPoint.x()) - qSin(angle)*(p.y() - startPoint.y()) + startPoint.x());
                    p.setY(qSin(angle)*(p.x()-startPoint.x()) + qCos(angle)*(p.y() - startPoint.y()) + startPoint.y());
                    points.append(p);
                }
            }

            // draw points
            foreach (QPoint p, points) {
                painter.drawPoint(p);
            }

            amsPointTemp = startPoint;
            amsWidthTemp = distWidth;
            maxDistWidth = qMax(maxDistWidth, distWidth);
            generalDataModified = true;
        }
            break;
        }
        // TODO: other types...
    }
}

void OCTAnnotate::on_eraseAmslerButton_clicked()
{
    if ((amsDistEyeSelected != "") && (amsDistIdSelected != -1)){
        patientData.deleteAmslerData(amsDistEyeSelected, amsDistIdSelected);

        drawGrid = true;
        generalDataModified = true;

        listAmslerDistortions();
    }
}

QPoint OCTAnnotate::findNearestCross(QPoint point){
    QPoint p;

    float minH = 1000;
    float minV = 1000;
    float distH = 0;
    float distV = 0;
    int lineHNumber = -1;
    int lineVNumber = -1;
    for (int i=0; i<amsLinesNumber; i++){
        distH = qAbs(computeDistance(gridHLines[i], point));
        distV = qAbs(computeDistance(gridVLines[i], point));
        if (distH < minH){
            lineHNumber = i;
            minH = distH;
        }
        if (distV < minV){
            lineVNumber = i;
            minV = distV;
        }
    }

    p = QPoint(amsLinePixDist*lineVNumber, amsLinePixDist*lineHNumber);
    return p;
}

float OCTAnnotate::computeDistance(QLine line, QPoint point){
    float d = 0;

    d = (line.dy()*point.x() - line.dx()*point.y() - line.x1()*line.y2() + line.x2()*line.y1())/(qSqrt(qPow(line.dx(),2) + qPow(line.dy(),2)));

    return d;
}

void OCTAnnotate::eraseAmslerArea(QPoint startPoint, QPainter *painter){
    // erase what was before
    QPoint topLeft(startPoint.x() - maxDistWidth, startPoint.y() - maxDistWidth);
    QPoint bottomRight(startPoint.x() + maxDistWidth - 1, startPoint.y() + maxDistWidth - 1);
    painter->setPen(QPen(Qt::white, 1));
    painter->drawRect(QRect(topLeft, bottomRight));

    // draw grid
    QPoint p1, p2;
    painter->setPen(amsGridPen);
    int i=0;
    int lineNumber = maxDistWidth/amsLinePixDist + 1;
    while (i < lineNumber){
        // vertical lines
        p1 = QPoint(startPoint.x() + i*amsLinePixDist, startPoint.y() - maxDistWidth);
        p2 = QPoint(startPoint.x() + i*amsLinePixDist, startPoint.y() + maxDistWidth);
        painter->drawLine(p1, p2);
        p1 = QPoint(startPoint.x() - i*amsLinePixDist, startPoint.y() - maxDistWidth);
        p2 = QPoint(startPoint.x() - i*amsLinePixDist, startPoint.y() + maxDistWidth);
        painter->drawLine(p1, p2);

        // horizontal lines
        p1 = QPoint(startPoint.x() - maxDistWidth, startPoint.y() + i*amsLinePixDist);
        p2 = QPoint(startPoint.x() + maxDistWidth, startPoint.y() + i*amsLinePixDist);
        painter->drawLine(p1, p2);
        p1 = QPoint(startPoint.x() - maxDistWidth, startPoint.y() - i*amsLinePixDist);
        p2 = QPoint(startPoint.x() + maxDistWidth, startPoint.y() - i*amsLinePixDist);
        painter->drawLine(p1, p2);
        i++;
    }
}

void OCTAnnotate::drawAmslerStraightLines(QPoint startPoint, int i, QPainter *painter){
    QPoint p1, p2;

    // vertical straight lines
    p1 = QPoint(startPoint.x() + i*amsLinePixDist, startPoint.y() - maxDistWidth);
    p2 = QPoint(startPoint.x() + i*amsLinePixDist, startPoint.y() - i*amsLinePixDist);
    painter->drawLine(p1, p2);
    p1 = QPoint(startPoint.x() - i*amsLinePixDist, startPoint.y() - maxDistWidth);
    p2 = QPoint(startPoint.x() - i*amsLinePixDist, startPoint.y() - i*amsLinePixDist);
    painter->drawLine(p1, p2);
    p1 = QPoint(startPoint.x() + i*amsLinePixDist, startPoint.y() + maxDistWidth);
    p2 = QPoint(startPoint.x() + i*amsLinePixDist, startPoint.y() + i*amsLinePixDist);
    painter->drawLine(p1, p2);
    p1 = QPoint(startPoint.x() - i*amsLinePixDist, startPoint.y() + maxDistWidth);
    p2 = QPoint(startPoint.x() - i*amsLinePixDist, startPoint.y() + i*amsLinePixDist);
    painter->drawLine(p1, p2);

    // horizontal straight lines
    p1 = QPoint(startPoint.x() - maxDistWidth, startPoint.y() + i*amsLinePixDist);
    p2 = QPoint(startPoint.x() - i*amsLinePixDist, startPoint.y() + i*amsLinePixDist);
    painter->drawLine(p1, p2);
    p1 = QPoint(startPoint.x() - maxDistWidth, startPoint.y() - i*amsLinePixDist);
    p2 = QPoint(startPoint.x() - i*amsLinePixDist, startPoint.y() - i*amsLinePixDist);
    painter->drawLine(p1, p2);
    p1 = QPoint(startPoint.x() + maxDistWidth, startPoint.y() + i*amsLinePixDist);
    p2 = QPoint(startPoint.x() + i*amsLinePixDist, startPoint.y() + i*amsLinePixDist);
    painter->drawLine(p1, p2);
    p1 = QPoint(startPoint.x() + maxDistWidth, startPoint.y() - i*amsLinePixDist);
    p2 = QPoint(startPoint.x() + i*amsLinePixDist, startPoint.y() - i*amsLinePixDist);
    painter->drawLine(p1, p2);
}

void OCTAnnotate::drawAmslerCurvedLines(QPoint startPoint, int i, QPainter *painter, DistType type){
    QPoint p1, p2;
    QRect rectangle;
    int startAngle = 0;
    int spanAngle = 0;

    if (type == Convex){

        // curved vertical lines
        p1 = QPoint(startPoint.x() + i*amsLinePixDist - amsLinePixDist/2, startPoint.y() - i*amsLinePixDist); // topleft
        p2 = QPoint(startPoint.x() + (i+1)*amsLinePixDist - amsLinePixDist/2, startPoint.y() + i*amsLinePixDist); // bottomright
        rectangle = QRect(p1, p2);
        startAngle = 90*16;
        spanAngle = -180*16;
        painter->drawArc(rectangle, startAngle, spanAngle);

        p1 = QPoint(startPoint.x() - i*amsLinePixDist + amsLinePixDist/2, startPoint.y() - i*amsLinePixDist); // topleft
        p2 = QPoint(startPoint.x() - (i+1)*amsLinePixDist + amsLinePixDist/2, startPoint.y() + i*amsLinePixDist); // bottomright
        rectangle = QRect(p1, p2);
        startAngle = 90*16;
        spanAngle = 180*16;
        painter->drawArc(rectangle, startAngle, spanAngle);

        // curved horizontal lines
        p1 = QPoint(startPoint.x() - i*amsLinePixDist, startPoint.y() - i*amsLinePixDist + amsLinePixDist/2); // topleft
        p2 = QPoint(startPoint.x() + i*amsLinePixDist, startPoint.y() - (i+1)*amsLinePixDist + amsLinePixDist/2); // bottomright
        rectangle = QRect(p1, p2);
        startAngle = 0*16;
        spanAngle = 180*16;
        painter->drawArc(rectangle, startAngle, spanAngle);

        p1 = QPoint(startPoint.x() - i*amsLinePixDist, startPoint.y() + i*amsLinePixDist - amsLinePixDist/2); // topleft
        p2 = QPoint(startPoint.x() + i*amsLinePixDist, startPoint.y() + (i+1)*amsLinePixDist - amsLinePixDist/2); // bottomright
        rectangle = QRect(p1, p2);
        startAngle = 180*16;
        spanAngle = 180*16;
        painter->drawArc(rectangle, startAngle, spanAngle);

    } else {

        // curved vertical lines
        p1 = QPoint(startPoint.x() + i*amsLinePixDist - amsLinePixDist/2, startPoint.y() - i*amsLinePixDist); // topleft
        p2 = QPoint(startPoint.x() + (i+1)*amsLinePixDist - amsLinePixDist/2, startPoint.y() + i*amsLinePixDist); // bottomright
        rectangle = QRect(p1, p2);
        startAngle = 90*16;
        spanAngle = 180*16;
        painter->drawArc(rectangle, startAngle, spanAngle);

        p1 = QPoint(startPoint.x() - i*amsLinePixDist + amsLinePixDist/2, startPoint.y() - i*amsLinePixDist); // topleft
        p2 = QPoint(startPoint.x() - (i+1)*amsLinePixDist + amsLinePixDist/2, startPoint.y() + i*amsLinePixDist); // bottomright
        rectangle = QRect(p1, p2);
        startAngle = 90*16;
        spanAngle = -180*16;
        painter->drawArc(rectangle, startAngle, spanAngle);

        // curved horizontal lines
        p1 = QPoint(startPoint.x() - i*amsLinePixDist, startPoint.y() - i*amsLinePixDist + amsLinePixDist/2); // topleft
        p2 = QPoint(startPoint.x() + i*amsLinePixDist, startPoint.y() - (i+1)*amsLinePixDist + amsLinePixDist/2); // bottomright
        rectangle = QRect(p1, p2);
        startAngle = 0*16;
        spanAngle = -180*16;
        painter->drawArc(rectangle, startAngle, spanAngle);

        p1 = QPoint(startPoint.x() - i*amsLinePixDist, startPoint.y() + i*amsLinePixDist - amsLinePixDist/2); // topleft
        p2 = QPoint(startPoint.x() + i*amsLinePixDist, startPoint.y() + (i+1)*amsLinePixDist - amsLinePixDist/2); // bottomright
        rectangle = QRect(p1, p2);
        startAngle = 180*16;
        spanAngle = -180*16;
        painter->drawArc(rectangle, startAngle, spanAngle);
    }
}


// bscan image navigation -------------------------------------------------------------------------
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

    int h = patientData.getBscanHeight() / scales[scaleFactor];
    double dy = (int)((double)(patientData.getBscanHeight() - h) / 2.0);
    double y1 = (double)patientData.getBscanHeight() - dy;
    double y2 = dy;
    ui->bScanHCPlot->yAxis->setRange(y1,y2);
    ui->bScanVCPlot->yAxis->setRange(y1,y2);

    QImage image(patientData.getImageFileList().at(currentImageNumber));
    Calculate *calc = new Calculate();
    calc->imageEnhancement(&image, contrast, brightness);
    QImage newImage = image.copy(0,dy,patientData.getBscanWidth(),h);
    ui->bScanHCPlot->axisRect()->setBackground(QPixmap::fromImage(newImage),true,Qt::IgnoreAspectRatio);
    ui->bScanHCPlot->replot();

    QImage normalImage = patientData.getNormalImage(currentNormalImageNumber);
    calc->imageEnhancement(&normalImage, contrast, brightness);
    QImage newNormalImage = normalImage.copy(0,dy,patientData.getBscansNumber(),h);
    ui->bScanVCPlot->axisRect()->setBackground(QPixmap::fromImage(newNormalImage),true,Qt::IgnoreAspectRatio);
    ui->bScanVCPlot->replot();
}

void OCTAnnotate::changeImageRange(int dir){
    int imageHeight = patientData.getBscanHeight() / scales[scaleFactor];
    QCPRange plotRange = ui->bScanHCPlot->yAxis->range();
    QCPRange newRange;

    if ((dir > 0) && (plotRange.upper < patientData.getBscanHeight())){
        double upper = qBound(0.0, plotRange.upper + 20, (double)patientData.getBscanHeight());
        newRange = QCPRange(upper - imageHeight, upper);
    } else if ((dir < 0) && (plotRange.lower > 0)){
        double lower = qBound(0.0, plotRange.lower - 20, (double)patientData.getBscanHeight());
        newRange = QCPRange(lower, lower + imageHeight);
    }
    ui->bScanHCPlot->yAxis->setRange(newRange);
    ui->bScanVCPlot->yAxis->setRange(newRange);

    QImage image(patientData.getImageFileList().at(currentImageNumber));
    Calculate *calc = new Calculate();
    calc->imageEnhancement(&image, contrast, brightness);
    double dy = qBound(0, patientData.getBscanHeight() - (int)newRange.upper, patientData.getBscanHeight()-imageHeight);
    QImage newImage = image.copy(0,dy,patientData.getBscanWidth(),imageHeight);
    ui->bScanHCPlot->axisRect()->setBackground(QPixmap::fromImage(newImage),true,Qt::IgnoreAspectRatio);
    ui->bScanHCPlot->replot();

    QImage normalImage = patientData.getNormalImage(currentNormalImageNumber);
    calc->imageEnhancement(&normalImage, contrast, brightness);
    QImage newNormalImage = normalImage.copy(0,dy,patientData.getBscansNumber(),imageHeight);
    ui->bScanVCPlot->axisRect()->setBackground(QPixmap::fromImage(newNormalImage),true,Qt::IgnoreAspectRatio);
    ui->bScanVCPlot->replot();
}

void OCTAnnotate::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep()/2)));
}


// amsler drawing navigation ----------------------------------------------------------------------
void OCTAnnotate::on_penButton_toggled(bool checked)
{
    if (checked){
        ui->sprayLightButton->setChecked(false);
        ui->sprayDarkButton->setChecked(false);
        ui->lineButton->setChecked(false);
        currDistType = Pen;
    } else {
        currDistType = None;
    }
}

void OCTAnnotate::on_sprayLightButton_toggled(bool checked)
{
    if (checked){
        ui->penButton->setChecked(false);
        ui->sprayDarkButton->setChecked(false);
        ui->lineButton->setChecked(false);
        currDistType = WhiteSpot;
    } else {
        currDistType = None;
    }
}

void OCTAnnotate::on_sprayDarkButton_toggled(bool checked)
{
    if (checked){
        ui->penButton->setChecked(false);
        ui->sprayLightButton->setChecked(false);
        ui->lineButton->setChecked(false);
        currDistType = BlackSpot;
    } else {
        currDistType = None;
    }
}

void OCTAnnotate::on_lineButton_toggled(bool checked)
{
    if (checked){
        ui->penButton->setChecked(false);
        ui->sprayLightButton->setChecked(false);
        ui->sprayDarkButton->setChecked(false);

        ui->distShapeCBox->setEnabled(true);
        switch (ui->distShapeCBox->currentIndex()) {
        case 0:
            currDistType = RightSpiral;
            break;
        case 1:
            currDistType = LeftSpiral;
            break;
        case 2:
            currDistType = Convex;
            break;
        case 3:
            currDistType = Concave;
            break;
        // TODO: other types...
        }
    } else {
        ui->distShapeCBox->setEnabled(false);
        currDistType = None;
    }
}

void OCTAnnotate::on_distShapeCBox_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
        currDistType = RightSpiral;
        break;
    case 1:
        currDistType = LeftSpiral;
        break;
    case 2:
        currDistType = Convex;
        break;
    case 3:
        currDistType = Concave;
        break;
    // TODO: other types...
    }
}

void OCTAnnotate::on_amslerRDistList_itemClicked()
{
    amsDistEyeSelected = "R";
    amsDistIdSelected = ui->amslerRDistList->currentRow();
    ui->amslerLDistList->setCurrentRow(-1);

    // TODO: color selected item
    repaintAmsler = true;
}

void OCTAnnotate::on_amslerLDistList_itemClicked()
{
    amsDistEyeSelected = "L";
    amsDistIdSelected = ui->amslerLDistList->currentRow();
    ui->amslerRDistList->setCurrentRow(-1);

    // TODO: color selected item
    repaintAmsler = true;
}


// data edit --------------------------------------------------------------------------------------
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

void OCTAnnotate::on_amslerRCommentTEdit_textChanged()
{
    generalDataModified = true;
}

void OCTAnnotate::on_amslerLCommentTEdit_textChanged()
{
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
        //ui->testWidget->graph(graphID)->setName("PCV manual annotation");
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
        //ui->testWidget->graph(graphID)->setName("PCV manual annotation");
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
                flatDiff = calc->calculateFlatteningDifferences(&image);
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
        labelLeft->position->setCoords(-2.8,-2.8);
        labelRight->position->setCoords(2.8,-2.8);
        labelLeft->setFont(QFont(font().family(), 16));
        labelRight->setFont(QFont(font().family(), 16));

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

void OCTAnnotate::on_actionShowETDRSGrid_toggled(bool checked)
{
    showETDRSGrid = checked;
    displayVirtualMap(ui->virtualMapImageCPlot);
    displayVirtualMap(ui->virtualMapAutoImageCPlot,true);
}

void OCTAnnotate::on_actionShowCenterOnBscan_toggled(bool checked)
{
    showCenterOnBscan = checked;
    if (!patientData.getImageFileList().isEmpty())
        loadImage(currentImageNumber);
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
//        QList<int> flatDiff;
//        displayAnnotations(flatDiff);

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
void OCTAnnotate::on_actionSetScanCenter_toggled(bool checked)
{
    if (checked){
        settingScanCenter = true;
        ui->statusBar->showMessage("Ustawianie środka skanu: Proszę kliknąć na obrazie B-skan...");
    } else {
        settingScanCenter = false;
    }
}

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

/*void OCTAnnotate::on_actionAddPatientNew_triggered()
{
    NewPatientDialog *newPatientDialog = new NewPatientDialog();
    int result = newPatientDialog->exec();

    if (result == QDialog::Accepted) {
        ui->lastNameLEdit->setText(newPatientDialog->getLastName());
        ui->firstNameLEdit->setText(newPatientDialog->getFirstName());
        ui->birthDateEdit->setDate(newPatientDialog->getBirthDate());
        ui->genderCBox->setCurrentIndex(newPatientDialog->getGender());
    }
}*/

/*void OCTAnnotate::on_actionLoadPatientGeneral_triggered()
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
        // okno wyboru osoby z listy plików
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open General Exam File"), manualDir.path(), tr("General Exam Data File (*.ged)"));
        if (!fileName.isEmpty()){

            QFileInfo fileInfo = QFileInfo(fileName);
            QString baseName = fileInfo.baseName();
            QStringList list = baseName.split("_");
            patientData.lastName = list.at(0);
            patientData.firstName = list.at(1);

            readGeneralExamData();
            ui->lastNameLEdit->setText(patientData.lastName);
            ui->firstNameLEdit->setText(patientData.firstName);
            ui->actionSaveGeneralExam->setEnabled(true);
        }
    }
}*/

void OCTAnnotate::on_actionSettings_triggered()
{
    SettingsDialog *settingsDialog = new SettingsDialog();
    if(settingsDialog->exec() == QDialog::Accepted){
        on_actionShowETDRSGrid_toggled(settingsDialog->getShowETDRSGrid());
        on_actionShowCenterOnBscan_toggled(settingsDialog->getShowCenterOnBscan());
        blockPCV = settingsDialog->getBlockPCV();
        openBscanNumber = settingsDialog->getOpenBskan();
        examDir = settingsDialog->getPathOctExam();
        manualDir = settingsDialog->getPathManualSegm();
        autoDir = settingsDialog->getPathAutoSegm();
        dataSaveStructure = settingsDialog->getDataSaveStructure();
        showBscanOnErrorPlot = settingsDialog->getShowBscanOnErrorPlot();
        displayImageLayersPlot(currentImageLayersNumber,selectedErrorLayer);
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

void OCTAnnotate::on_errorOccured(QString err){
    QMessageBox::critical(this, tr("Error"), err);

    // TODO (if przy readAuto):
    // on_actionCloseAutoSegmentation_triggered();
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
        ui->amslerRCommentTEdit->setText(patientData.getAmslerComment("R"));
        ui->amslerLCommentTEdit->setText(patientData.getAmslerComment("L"));
        ui->mcVOPLEdit->setText(patientData.getMcvOP());
        ui->mcVOLLEdit->setText(patientData.getMcvOL());
        ui->mcHOPLEdit->setText(patientData.getMchOP());
        ui->mcHOLLEdit->setText(patientData.getMchOL());
        listAmslerDistortions();
        generalDataModified = false;

        // display information
        this->setWindowTitle("OCTAnnotate " + appVersion + " - " + octDir.dirName());
        drawGrid = true;
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

    if (openBscanNumber == "m"){
        QPoint center = patientData.getScanCenter();
        if (center.y() != -1)
            currentImageNumber = center.y();
        else
            currentImageNumber = patientData.getBscansNumber()/2;   // middle B-scan
    } else {
        currentImageNumber = 0;
    }
    currentImageLayersNumber = currentImageNumber;
    currentNormalImageNumber = patientData.getBscanWidth()/2;

//    scaleFactorX = (double)(scrollArea->width()-20) / (double)(patientData.getBscanWidth());
//    scaleFactorY = (double)(scrollArea->height()-2) / (double)(patientData.getBscanHeight());

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
    ui->penButton->setEnabled(true);
    ui->sprayLightButton->setEnabled(true);
    ui->sprayDarkButton->setEnabled(true);
    ui->lineButton->setEnabled(true);
    ui->eraseAmslerButton->setEnabled(true);
    ui->computeVirtualMapButton->setEnabled(true);
    ui->actionReadAutoSegmentation->setEnabled(true);
    ui->actionSetScanCenter->setEnabled(true);
    ui->actionFillFromILM->setEnabled(true);
    ui->actionFillStraight->setEnabled(true);
    ui->actionSetAutoSegmentationAsManual->setEnabled(true);

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

void OCTAnnotate::on_actionCreateManualSegmentationFromOCTExplorer_triggered(QList<QString> folderList)
{
    // MGR BERA
//    folderList.append("NAZWISKO1_IMIE1_2014_07_01_12_32_22_R_Centralna_3D_8,00 mm_ 800x100"); // 1
//    folderList.append("NAZWISKO2_IMIE2_2014_09_19_09_51_09_R_Centralna_3D_8,00 mm_ 800x100"); // 1
//    folderList.append("NAZWISKO3_IMIE3_2014_08_12_13_06_42_R_Centralna_3D_8,00 mm_ 800x100"); // 1
//    folderList.append("NAZWISKO4_IMIE4_2014_09_23_10_20_06_L_Centralna_3D_8,00 mm_ 799x100"); // 1
//    folderList.append("NAZWISKO5_IMIE5_2013_11_21_10_00_12_L_Centralna_3D_8,00 mm_ 800x100"); // 1
//    folderList.append("NAZWISKO6_IMIE6_2013_11_26_12_19_00_R_Centralna_3D_8,00 mm_ 800x100"); // 1
//    folderList.append("NAZWISKO7_IMIE7_2013_12_03_12_28_38_R_Centralna_3D_8,00 mm_ 800x100"); // 1
//    folderList.append("NAZWISKO8_IMIE8_2015_01_16_11_07_02_L_Centralna_3D_8,00 mm_ 800x100"); // 1
//    folderList.append("NAZWISKO9_IMIE9_2014_04_23_10_46_49_L_Centralna_3D_8,00 mm_ 800x100"); // 1
//    folderList.append("NAZWISKO10_IMIE10_2014_09_16_14_14_56_R_Centralna_3D_8,00 mm_ 800x100"); // 1

    foreach(QString folderName, folderList){
        QDir fn = QDir(folderName);
        QString scanName = fn.dirName();
        qDebug() << "Processing... " + scanName;

        // read oct scan -----------------------------------------------------------------------------------------------------------
        octDir = QDir(folderName + "/");
        autoDir = QDir(folderName + "/");

        patientData = PatientData();

        ReadWriteData *rwData = new ReadWriteData();
        rwData->setDataObject(&patientData);
        rwData->setDirectoryOct(&octDir);
        rwData->setDirectoryManual(&manualDir);
        rwData->setDirectoryAuto(&autoDir);
        rwData->setAutoFilePath(folderName + "/" + scanName + "_Surfaces_Iowa.xml");
        rwData->setDataSaveStrucure(dataSaveStructure);
        rwData->addDirective("readPatientData");
        rwData->addDirective("readOctExamData");
        rwData->addDirective("readAutoSegmentationData");
        rwData->addDirective("copyAutoAsManualAll");
        rwData->addDirective("saveManualSegmentationData");

        QThread *thread = new QThread;
        rwData->moveToThread(thread);
        connect(thread, SIGNAL(started()), rwData, SLOT(process()));
        connect(rwData, SIGNAL(errorOccured(QString)), this, SLOT(on_errorOccured(QString)));
        connect(rwData, SIGNAL(processingData(double,QString)), this, SLOT(on_processingData(double,QString)));
        connect(rwData, SIGNAL(finished()), thread, SLOT(quit()));
        connect(rwData, SIGNAL(finished()), rwData, SLOT(deleteLater()));
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
        thread->start();
        thread->wait(30*1000);
//        thread->wait();
    }
}

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
    calc->examDataDir = "D:/Egnyte/Private/agis/Qt/Deployment/OCTAnnotate/examData/"; //currentDir.path().append("/examData/");
    calc->octDirPath = "D:/Egnyte/Private/agis/BADANIA/OCT exams/Avanti VMT_VMA/";

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

void OCTAnnotate::delay( int secondsToWait )
{
    QTime dieTime = QTime::currentTime().addSecs( secondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

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
        pathOctExam = QFileDialog::getExistingDirectory(this, tr("Open Directory"), examDir.absolutePath(), QFileDialog::ShowDirsOnly);
    else
        pathOctExam = folderPath;
    if (!pathOctExam.isEmpty()){
        QDir newExamDir = QDir(pathOctExam);

        QString examPath = newExamDir.dirName();//examDir.relativeFilePath(pathOctExam);
        QString filePath = "";

        if (patientsDB->getScanID(examPath) != -1){
            QMessageBox::critical(this, tr("Error"), tr("Scan exists in the database!"));
        } else {
            QString infoFilePathAvanti = newExamDir.absolutePath().append("/" + newExamDir.dirName() + ".txt");
            QFile infoFileAvanti(infoFilePathAvanti);
            if (infoFileAvanti.exists()){

                QDate birth_date = QDate(1900,1,1);
                int gender = 3; // undefined

                QStringList fileName = newExamDir.dirName().split(",");
                if (fileName.count() < 2){   // zabezpieczenie przed nieodpowiednim katalogiem...
                    QMessageBox::critical(this, tr("Error"), "Nieprawidłowy katalog! Nie można wczytać danych.");
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
                                    scanHeight = data.at(1).toInt();
                                    scanUsageRead = true;
                                }
                                // brith date - not possible
                                // gender - not possible
                            }
                        } while (!line.isNull());
                        if (infoFileAvanti.isOpen())
                            infoFileAvanti.close();
                    }

                    QString examDimensions = QString::number(scanWidth) + " x " + QString::number(scanHeight);
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
                    patientsDB->addNewScan(patientID, eye, QDateTime(examDate, examTime), device, examType, direction, examDimensions, examQI, filePath, examPath);
                    modelScans->select();
                }

            } else {
                QMessageBox::critical(this, tr("Error"), tr("Nie można otworzyć pliku z danymi badania (.txt)"));
            }
        }
    }
}

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
        QString folderName = record.value("scan_folder_path").toString();
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

//    modelPatients->select();
//    modelScans->select();
}

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
    QString folderName = selectedRecord.value("scan_folder_path").toString();
    QString subfolder = selectedRecord.value("device").toString() + " " + selectedRecord.value("series").toString();

    QString temp = "/" + subfolder + "/" + folderName + "/" + folderName + "_Proj_Iowa.tif";
    QString fundusPath = examDir.absolutePath().append(temp);

    if (QFile(fundusPath).exists()){
        QImage fundusImage(fundusPath);
        ui->fundusDBLabel->setPixmap(QPixmap::fromImage(fundusImage));
    } else {
        QString fundusPath2 = examDir.absolutePath().append("/" + subfolder + "/" + folderName + "/fnds_rec.bmp");
        if (QFile(fundusPath2).exists()){
            QImage fundusImage(fundusPath2);
            ui->fundusDBLabel->setPixmap(QPixmap::fromImage(fundusImage));
        } else {
            ui->fundusDBLabel->setPixmap(QPixmap());
        }
    }

    QString bscanHPath = examDir.absolutePath().append("/" + subfolder + "/" + folderName + "/" + "Skan_nr_71.bmp");
    QString bscanVPath = examDir.absolutePath().append("/" + subfolder + "/" + folderName + "/" + "Skan_nr_143.bmp");

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
    QString scanFolder = examDir.absolutePath() + "/" + record.value("device").toString() + " " + record.value("series").toString() + "/" + record.value("scan_folder_path").toString();
    on_actionLoadPatientOCT_triggered(scanFolder);
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

void OCTAnnotate::on_searchForScansButton_clicked()
{
    // TODO:
    // 1. zaladuj liste katalogow ze wskazanego katalogu
    QString folderSearchPath = QFileDialog::getExistingDirectory(this, tr("Open Directory"), examDir.absolutePath(), QFileDialog::ShowDirsOnly);
    QDir folderSearchDir = QDir(folderSearchPath);
    QStringList folderList = folderSearchDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    // 2. dla kazdego katalogu dodaj skan do bazy
    foreach(QString folderName, folderList){
        qDebug() << folderSearchPath + "/" + folderName;
        on_addScanFolderButton_clicked(folderSearchPath + "/" + folderName);
    }
}

void OCTAnnotate::on_actionEditAnnotations_toggled(bool state)
{
    editAnnotations = state;
}
