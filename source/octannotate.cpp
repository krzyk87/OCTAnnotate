#include "octannotate.h"
#include "ui_octannotate.h"
#include "autoasmanualdialog.h"
#include "settingsdialog.h"
//#include "qmath.h"
#include "calculate.h"
#include "readwritedata.h"

//#include <QFileDialog>
#include <QFile>
//#include <QMessageBox>
//#include <QTextStream>
//#include <QMouseEvent>
//#include <QPainter>
//#include <QColorDialog>
//#include <QClipboard>
//#include <QXmlStreamReader>
//#include <QDebug>

const QString OCTAnnotate::settingsFilePath = QDir::currentPath().append("/config.ini");

OCTAnnotate::OCTAnnotate(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::OCTAnnotate)
{
    ui->setupUi(this);
    QFont font;
    font.setPixelSize(11);
    this->setFont(font);
    quit = false;
    flattenImage = false;
    editAnnotations = false;
    ui->actionImageFlattening->setChecked(flattenImage);
    ui->actionEditAnnotations->setChecked(editAnnotations);
    ui->actionSaveOCTExam->setEnabled(false);

    // configurations
    SettingsDialog *settingsDialog = new SettingsDialog(settingsFilePath);
    loadConfigurations(settingsDialog);

    // setup patients database

    patientData = PatientData();
    scanName = "";

    currentImageNumber = 0;
    currentNormalImageNumber = 0;
    octDataModified = false;
    settingScanCenter = false;

    QRegularExpression rx("[0-9]{1,3}");
    QRegularExpressionValidator *rxv = new QRegularExpressionValidator(rx);
    ui->currImageNumberLEdit->setValidator(rxv);
    ui->verticalLayout_6->setAlignment(ui->fundusImageLabel, Qt::AlignHCenter);

    contrast = 1.5;
    brightness = 0;
    ui->contrastSlider->setValue(contrast*10);

    ui->fundusImageLabel->setScaledContents(true);
    ui->fundusImageLabel->installEventFilter(this);
    ui->bScanHCPlot->installEventFilter(this);
    ui->bScanVCPlot->installEventFilter(this);

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
    myPalette.setColor(QPalette::Window, pcvColor);
    ui->pcvColorLabel->setAutoFillBackground(true);
    ui->pcvColorLabel->setPalette(myPalette);
    myPalette.setColor(QPalette::Window, ermColor);
    ui->ermColorLabel->setAutoFillBackground(true);
    ui->ermColorLabel->setPalette(myPalette);
    myPalette.setColor(QPalette::Window, ilmColor);
    ui->ilmColorLabel->setAutoFillBackground(true);
    ui->ilmColorLabel->setPalette(myPalette);
    myPalette.setColor(QPalette::Window, gclColor);
    ui->gclColorLabel->setAutoFillBackground(true);
    ui->gclColorLabel->setPalette(myPalette);
    myPalette.setColor(QPalette::Window, iplColor);
    ui->iplColorLabel->setAutoFillBackground(true);
    ui->iplColorLabel->setPalette(myPalette);
    myPalette.setColor(QPalette::Window, inlColor);
    ui->inlColorLabel->setAutoFillBackground(true);
    ui->inlColorLabel->setPalette(myPalette);
    myPalette.setColor(QPalette::Window, oplColor);
    ui->oplColorLabel->setAutoFillBackground(true);
    ui->oplColorLabel->setPalette(myPalette);
    myPalette.setColor(QPalette::Window, onlColor);
    ui->onlColorLabel->setAutoFillBackground(true);
    ui->onlColorLabel->setPalette(myPalette);
    myPalette.setColor(QPalette::Window, elmColor);
    ui->elmColorLabel->setAutoFillBackground(true);
    ui->elmColorLabel->setPalette(myPalette);
    myPalette.setColor(QPalette::Window, mezColor);
    ui->mezColorLabel->setAutoFillBackground(true);
    ui->mezColorLabel->setPalette(myPalette);
    myPalette.setColor(QPalette::Window, iosColor);
    ui->iosColorLabel->setAutoFillBackground(true);
    ui->iosColorLabel->setPalette(myPalette);
    myPalette.setColor(QPalette::Window, rpeColor);
    ui->rpeColorLabel->setAutoFillBackground(true);
    ui->rpeColorLabel->setPalette(myPalette);
    myPalette.setColor(QPalette::Window, chrColor);
    ui->chrColorLabel->setAutoFillBackground(true);
    ui->chrColorLabel->setPalette(myPalette);

    setupBScanPlots();

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

    appVersion = "v2.0.0";
    this->setWindowTitle("OCTAnnotate " + appVersion);
    progressBar = new QProgressBar();
    progressBar->setMaximumWidth(250);
    progressBar->setMaximum(100);
    ui->statusBar->addPermanentWidget(progressBar);
    progressBar->setVisible(false);
}

OCTAnnotate::~OCTAnnotate()
{
//    patientsDB->closeDatabase();
    delete ui;
}

// initialization ---------------------------------------------------------------------------------
void OCTAnnotate::loadConfigurations(SettingsDialog *sDialog){

    databasePath = sDialog->getDatabasePath();
    octBaseDir = QDir(sDialog->getPathOctData());
    examDir = QDir(sDialog->getPathExamData());
    manualDir = QDir(sDialog->getPathExamData().append("/mvri/"));
    autoDir = QDir(sDialog->getPathExamData().append("/avri/"));

    dataSaveStructure = sDialog->getDataSaveStructure();
    blockPCV = sDialog->getBlockPCV();

    on_actionShowCenterOnBscan_toggled(sDialog->getShowCenterOnBscan());
//    showETDRSGrid = sDialog->getShowETDRSGrid();
//    showCenterOnBscan = sDialog->getShowCenterOnBscan();
}

void OCTAnnotate::on_actionSettings_triggered()
{
    SettingsDialog *settingsDialog = new SettingsDialog(settingsFilePath);
    if(settingsDialog->exec() == QDialog::Accepted){
        loadConfigurations(settingsDialog);
    }
}

void OCTAnnotate::on_actionClose_triggered()
{
    QMessageBox msgBox;
    QPushButton *saveButton = msgBox.addButton(" Zapisz i zamknij ", QMessageBox::YesRole);
    QPushButton *cancelButton = msgBox.addButton(" Anuluj ", QMessageBox::RejectRole);
    msgBox.addButton(" Wyjdź bez zapisywania ", QMessageBox::NoRole);

    quit = true;

    // ask if save before quit
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
    if (patientData.getIsLoaded()) // !patientData.getImageFileList().isEmpty())
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

// select OCT exam --------------------------------------------------------------------------------
void OCTAnnotate::on_actionLoadOCTSequence_triggered(QString scanFolderPath)
{
    bool selectNew = true;

    if (patientData.getIsLoaded()){
        QMessageBox msgBox;
        QPushButton *saveButton = msgBox.addButton(" Zapisz i wczytaj nowe badanie ", QMessageBox::YesRole);
        QPushButton *cancelButton = msgBox.addButton(" Anuluj ", QMessageBox::RejectRole);
        msgBox.addButton(" Wczytaj nowe badanie bez zapisywania ", QMessageBox::NoRole);

        if (octDataModified){
            msgBox.setText("Anotacje badania OCT zostały zmienione. Czy zapisać zmiany przed wczytaniem nowego badania?");
            msgBox.exec();
            if (msgBox.clickedButton() == saveButton){
                on_actionSaveOCTExam_triggered();
            } else if (msgBox.clickedButton() == cancelButton) {
                selectNew = false;
            }
        }
    }

    if (selectNew){
        QString dirName = scanFolderPath;
        if (dirName == "")
            dirName = QFileDialog::getExistingDirectory(this, tr("Open Directory"), octBaseDir.path(), QFileDialog::ShowDirsOnly);

        qDebug() << "Opening scan: " << dirName;

        if (!dirName.isEmpty()){
            octDir = QDir(dirName);
            scanName = octDir.dirName();

            patientData = PatientData();
            patientData.setIsBinary(false);

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
            rwData->addDirective("readOctExamData");

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

void OCTAnnotate::on_actionLoadOCTFile_triggered(QString scanFilePath)
{
    bool selectNew = true;

    if (patientData.getIsLoaded()){
        QMessageBox msgBox;
        QPushButton *saveButton = msgBox.addButton(" Zapisz i wczytaj nowe badanie ", QMessageBox::YesRole);
        QPushButton *cancelButton = msgBox.addButton(" Anuluj ", QMessageBox::RejectRole);
        msgBox.addButton(" Wczytaj nowe badanie bez zapisywania ", QMessageBox::NoRole);

        if (octDataModified){
            msgBox.setText("Anotacje badania OCT zostały zmienione. Czy zapisać zmiany przed wczytaniem nowego badania?");
            msgBox.exec();
            if (msgBox.clickedButton() == saveButton){
                on_actionSaveOCTExam_triggered();
            } else if (msgBox.clickedButton() == cancelButton) {
                selectNew = false;
            }
        }
    }

    if (selectNew){
        QString fileName = scanFilePath;
        if (fileName == "")
            fileName = QFileDialog::getOpenFileName(this, tr("Open OCT file"), octBaseDir.absolutePath(), tr("Avanti RTvue raw OCT data file (*.OCT)"));

        qDebug() << "Opening scan: " << fileName;

        if (!fileName.isEmpty()){
            octFile.setFileName(fileName);
            QFileInfo fileInfo(octFile);
            scanName = fileInfo.fileName();

            patientData = PatientData();
            patientData.setIsBinary(true);

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
            rwData->addDirective("readOctExamFile");

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

// load image -------------------------------------------------------------------------------------
void OCTAnnotate::loadImage(int imageNumber){
    QFileInfo imageFileInfo;
    QImage image;

    // load image
    if (patientData.getIsBinary()){ // isBinary
        image = patientData.getImage(imageNumber);
    } else {
        image = QImage(patientData.getImageFileList().at(imageNumber));
        imageFileInfo.setFile(patientData.getImageFileList().at(imageNumber));
        ui->imageNumberLabel->setText(imageFileInfo.fileName());
    }

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
    if (patientData.getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
        if (currentImageNumber < (patientData.getBscansNumber() - 1)){
            loadImage(currentImageNumber + 1);
            loadNormalImage(currentNormalImageNumber);
            fundusAnnotate = true;
        }
    }
}

void OCTAnnotate::on_prevImageButton_clicked(){
    if (patientData.getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
        if (currentImageNumber > 0){
            loadImage(currentImageNumber - 1);
            loadNormalImage(currentNormalImageNumber);
            fundusAnnotate = true;
        }
    }
}

void OCTAnnotate::on_nextNormalImageButton_clicked()
{
    if (patientData.getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
        if (currentNormalImageNumber < (patientData.getBscanWidth() - 1)){
            loadNormalImage(currentNormalImageNumber + 1);
            loadImage(currentImageNumber);
            fundusAnnotate = true;
        }
    }
}

void OCTAnnotate::on_prevNormalImageButton_clicked()
{
    if (patientData.getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
        if (currentNormalImageNumber > 0){
            loadNormalImage(currentNormalImageNumber - 1);
            loadImage(currentImageNumber);
            fundusAnnotate = true;
        }
    }
}

void OCTAnnotate::on_currImageNumberLEdit_returnPressed()
{
    if (patientData.getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
        int value = ui->currImageNumberLEdit->text().toInt();
        if (value < 0)
            value = 0;
        if (value >= patientData.getBscansNumber())
            value = patientData.getBscansNumber()-1;

        loadImage(value);
        fundusAnnotate = true;
        ui->currImageNumberLEdit->clearFocus();
    }
}

void OCTAnnotate::on_currNormalImageNumberLEdit_returnPressed()
{
    if (patientData.getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
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

void OCTAnnotate::on_actionImageFlattening_toggled(bool state)
{
    flattenImage = state;
    if (patientData.getIsLoaded()){
        // !patientData.getImageFileList().isEmpty()){
        loadImage(currentImageNumber);
        loadNormalImage(currentNormalImageNumber);
    }
}


// adjust contrast and brightness -----------------------------------------------------------------
void OCTAnnotate::on_contrastSlider_valueChanged(int value)
{
    contrast = (float)value / 10.0;
    if (patientData.getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
        loadImage(currentImageNumber);
        loadNormalImage(currentNormalImageNumber);
    }
}

void OCTAnnotate::on_brightnessSlider_valueChanged(int value)
{
    brightness = value;
    if (patientData.getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
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
        if (patientData.getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
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
    }
}


// save data --------------------------------------------------------------------------------------
void OCTAnnotate::on_actionSaveOCTExam_triggered()
{
    ReadWriteData *rwData = new ReadWriteData();
    rwData->setDataObject(&patientData);
    rwData->setDirectoryManual(&manualDir);
    rwData->setDirectoryOct(&octDir);
    rwData->setOctFile(&octFile);
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
    if (patientData.getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
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

                fundusAnnotate = true; // ,selectedErrorLayer);
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
        if (patientData.getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
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
    if (patientData.getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){

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
//            ui->bScanHCPlot->addItem(centerLine);
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
    if (patientData.getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
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
//            ui->bScanVCPlot-> addItem(centerLine);
            centerLine->setPen(QPen(Qt::red));
            centerLine->start->setCoords(center.y(),0);
            centerLine->end->setCoords(center.y(),patientData.getBscanHeight());
        }

        // show line where current image
        QCPItemLine *bScanHLine = new QCPItemLine(ui->bScanVCPlot);
//        ui->bScanVCPlot->addItem(bScanHLine);
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
        ui->bScanHCPlot->graph(graphID)->data()->remove(min,max);

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
        ui->bScanVCPlot->graph(graphID)->data()->remove(min,max);

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

        ui->bScanHCPlot->graph(graphID)->data()->remove(min,max);
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

        ui->bScanVCPlot->graph(graphID)->data()->remove(min,max);
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


// aditional settings -----------------------------------------------------------------------------
void OCTAnnotate::setScanCenter(){
    ui->actionSetScanCenter->setChecked(false);

    fundusAnnotate = true;

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
        myPalette.setColor(QPalette::Window, newColor);
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
    if (patientData.getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
        scaleFactor++;
        rescaleImage();
    }
}

void OCTAnnotate::on_zoomOutButton_clicked()
{
    if (patientData.getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
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

    QImage image(patientData.getImage(currentImageNumber));
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
        QImage image(patientData.getImage(currentImageNumber));
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

// m-charts edit ----------------------------------------------------------------------------------

// virtual map and layers plot --------------------------------------------------------------------
void OCTAnnotate::setupBScanPlots(){
    ui->bScanHCPlot->clearGraphs();

    // configure axis
    ui->bScanHCPlot->xAxis->setLabel("B-scan pixel (horizontal direction)");
    ui->bScanHCPlot->yAxis->setLabel("B-scan pixel (vertical direction)");
    if (patientData.getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
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
    if (patientData.getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
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

// display statistics -----------------------------------------------------------------------------

// recalculate virtual map for selected values ----------------------------------------------------

// auto annotations -------------------------------------------------------------------------------
void OCTAnnotate::on_actionReadAutoSegmentation_triggered(QString pathAutoSegment)
{
    if (patientData.getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
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
}

void OCTAnnotate::on_actionSaveAutoAnnotationsAs_triggered()
{
    if (patientData.getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
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
                    if (xml.name().toString() == "surface"){
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
            autoSegmentText << Qt::endl;
        }
        autoSegmentFile.close();
    }

    QMessageBox::information(this, "OCTAnnotate", "Wykonano konwersję danych.");
}

QList<QList<int> > OCTAnnotate::convertSurfaceLines(QXmlStreamReader &xml, QList<QList<int> > lineMap){
    QList<int> line;
    int lineNumber = 0;

    xml.readNext();
    while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name().toString() == "surface")){
        if (xml.tokenType() == QXmlStreamReader::StartElement){
            if (xml.name().toString() == "label"){
                xml.readNext();
                lineNumber = xml.text().toInt();
                if (lineNumber == 1)
                    lineMap.clear();
            }
            if (lineNumber == 1){
                if (xml.name().toString() == "bscan"){
                    line.clear();
                    xml.readNext();
                    while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name().toString() == "bscan")){
                        if (xml.tokenType() == QXmlStreamReader::StartElement){
                            if (xml.name().toString() == "z"){
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
    if (patientData.getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
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

// automatic segmentation error calculations ------------------------------------------------------

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

void OCTAnnotate::on_readingDataFinished(QString data){

    patientData.setIsLoaded(true);

    if ((data != "manualOnly") && (data != "autoOnly")){
        // display patients data
        // display information
        this->setWindowTitle("OCTAnnotate " + appVersion + " - " + scanName);
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
    currentNormalImageNumber = patientData.getBscanWidth()/2;

    // enable navigation buttons
    if (currentImageNumber < (patientData.getBscansNumber()-1)){
        ui->nextImageButton->setEnabled(true);
    } else {
        ui->nextImageButton->setEnabled(false);
    }
    if (currentImageNumber > 0){
        ui->prevImageButton->setEnabled(true);
    } else {
        ui->prevImageButton->setEnabled(false);
    }
    ui->actionSaveOCTExam->setEnabled(true);
    ui->actionReadAutoSegmentation->setEnabled(true);
    ui->actionSetScanCenter->setEnabled(true);
    ui->actionSetAutoSegmentationAsManual->setEnabled(true);
    ui->zoomInButton->setEnabled(true);

    // setup plots
    setupBScanPlots();

    //on_tabWidget_currentChanged();
}

void OCTAnnotate::on_savingDataFinished(QString data){
    if (data == "manualData"){
        QMessageBox::information(this, tr("Zapisz"), tr("Dane ręcznej segmentacji zostały zapisane"));
        octDataModified = false;
    }
    if (data == "autoData"){
        QMessageBox::information(this, tr("Zapisz"), tr("Dane automatycznej segmentacji zostały zapisane"));
    }
}

// calculate statistics for CAVRI analysis -------------------------------------------------------

// data visualization for publication purposes ---------------------------------------------------

// patients database -----------------------------------------------------------------------------

// batch processing -------------------------------------------------------------------------------

// search / filter / show data --------------------------------------------------------------------
