#include "octannotate.h"
#include "functions.h"
#include "ui_octannotate.h"
#include "settingsdialog.h"
#include "autoasmanualdialog.h"
//#include "qmath.h"
//#include "calculate.h"
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

    // configurations
    SettingsDialog *settingsDialog = new SettingsDialog(settingsFilePath);
    loadConfigurations(settingsDialog);

    // setup patients database

    patientData = PatientData();
    scan =  new Scan(this);
    scanName = "";

    currentImageNumber = 0;
    currentNormalImageNumber = 0;

    QRegularExpression rx("[0-9]{1,3}");
    QRegularExpressionValidator *rxv = new QRegularExpressionValidator(rx);
    ui->currImageNumberLEdit->setValidator(rxv);
    ui->currNormalImageNumberLEdit->setValidator(rxv);
    ui->currLayersNumberLEdit->setValidator(rxv);
    ui->verticalLayout_6->setAlignment(ui->fundusImageLabel, Qt::AlignHCenter);

    contrast = 1.5;
    brightness = 0;
    ui->contrastSlider->setValue(contrast*10);

    ui->fundusImageLabel->setScaledContents(true);
    ui->fundusImageLabel->installEventFilter(this);
    ui->bScanHCPlot->installEventFilter(this);
//    ui->bScanVCPlot->installEventFilter(this);

    fundusAnnotate = false;

    ui->pcvLayerRButton->setChecked(true);
    scan->setLayerDisplayObjects(PCV, ui->pcvColorLabel, ui->pcvLayerRButton);
    scan->setLayerDisplayObjects(IB_ERM, ui->ibermColorLabel, ui->ibermLayerRButton);
    scan->setLayerDisplayObjects(OB_ERM, ui->obermColorLabel, ui->obermLayerRButton);
    scan->setLayerDisplayObjects(ILM, ui->ilmColorLabel, ui->ilmLayerRButton);
    scan->setLayerDisplayObjects(RNFL_GCL, ui->rnfl_gclColorLabel, ui->rnfl_gclLayerRButton);
    scan->setLayerDisplayObjects(GCL_IPL, ui->gcl_iplColorLabel, ui->gcl_iplLayerRButton);
    scan->setLayerDisplayObjects(IPL_INL, ui->ipl_inlColorLabel, ui->ipl_inlLayerRButton);
    scan->setLayerDisplayObjects(INL_OPL, ui->inl_oplColorLabel, ui->inl_oplLayerRButton);
    scan->setLayerDisplayObjects(OPL_ONL, ui->opl_onlColorLabel, ui->opl_onlLayerRButton);
    scan->setLayerDisplayObjects(ELM, ui->elmColorLabel, ui->elmLayerRButton);
    scan->setLayerDisplayObjects(MEZ, ui->mezColorLabel, ui->mezLayerRButton);
    scan->setLayerDisplayObjects(IS_OS, ui->iosColorLabel, ui->iosLayerRButton);
    scan->setLayerDisplayObjects(IB_OPR, ui->iboprColorLabel,ui->iboprLayerRButton);
    scan->setLayerDisplayObjects(IB_RPE, ui->ibrpeColorLabel, ui->ibrpeLayerRButton);
    scan->setLayerDisplayObjects(OB_RPE, ui->obrpeColorLabel, ui->obrpeLayerRButton);

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

    appVersion = "2.0.0";
    this->setWindowTitle("OCTAnnotate v" + appVersion);
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

    octDir = QDir(sDialog->getPathOctData());
    examDir = QDir(sDialog->getPathExamData());

    dataSaveFormat = sDialog->getDataSaveStructure();
}

void OCTAnnotate::enableNavigationButtons()
{
    if (scan->getIsLoaded()){
        if (currentImageNumber < (scan->getBscansNumber()-1)){
            ui->nextImageButton->setEnabled(true);
            ui->nextLayersButton->setEnabled(true);
        } else {
            ui->nextImageButton->setEnabled(false);
            ui->nextLayersButton->setEnabled(false);
        }
        if (currentImageNumber > 0){
            ui->prevImageButton->setEnabled(true);
            ui->prevLayersButton->setEnabled(true);
        } else {
            ui->prevImageButton->setEnabled(false);
            ui->prevLayersButton->setEnabled(false);
        }
    }
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
     quit = true;
    // TODO: uncomment when functionality for saving new segmentations will be added
//    QMessageBox msgBox;
//    QPushButton *saveButton = msgBox.addButton(" Zapisz i zamknij ", QMessageBox::YesRole);
//    QPushButton *cancelButton = msgBox.addButton(" Anuluj ", QMessageBox::RejectRole);
//    msgBox.addButton(" Wyjdź bez zapisywania ", QMessageBox::NoRole);
//    if (octDataModified){
//            msgBox.setText("Anotacje badania OCT zostały zmienione. Czy zapisać zmiany przed wyjściem?");
//            msgBox.exec();
//            if (msgBox.clickedButton() == saveButton){
//                on_actionSaveOCTExam_triggered();
//                delay(3);
//            } else if (msgBox.clickedButton() == cancelButton) {
//                quit = false;
//            }
//        }

    if (quit)
        qApp->quit();
}

void OCTAnnotate::closeEvent(QCloseEvent *cEvent){
    on_actionClose_triggered();
    if (!quit)
        cEvent->ignore();
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
void OCTAnnotate::on_actionLoadOCTSequence_triggered()
{
    loadOCT(false);
}

void OCTAnnotate::on_actionLoadOCTFile_triggered()
{
    loadOCT(true);
}

void OCTAnnotate::loadOCT(bool isBinary)
{
    bool selectNew = true;

    // TODO: uncomment when functionality for saving new segmentations will be added
//    if (scan->getIsLoaded()){
//        QMessageBox msgBox;
//        QPushButton *saveButton = msgBox.addButton(" Zapisz i wczytaj nowe badanie ", QMessageBox::YesRole);
//        QPushButton *cancelButton = msgBox.addButton(" Anuluj ", QMessageBox::RejectRole);
//        msgBox.addButton(" Wczytaj nowe badanie bez zapisywania ", QMessageBox::NoRole);
//        if (octDataModified){
//            msgBox.setText("Anotacje badania OCT zostały zmienione. Czy zapisać zmiany przed wczytaniem nowego badania?");
//            msgBox.exec();
//            if (msgBox.clickedButton() == saveButton){
//                on_actionSaveOCTExam_triggered();
//            } else if (msgBox.clickedButton() == cancelButton) {
//                selectNew = false;
//            }
//        }
//    }

    if (selectNew){
        QString selectedScanName = "";
        if (isBinary)
            selectedScanName = QFileDialog::getOpenFileName(this, tr("Open OCT file"), octDir.absolutePath(), tr("Avanti RTvue raw OCT data file (*.OCT)"));
        else
            selectedScanName = QFileDialog::getExistingDirectory(this, tr("Open Directory"), octDir.path(), QFileDialog::ShowDirsOnly);

        qDebug() << "Opening scan: " << selectedScanName;   // QFileInfo(selectedScanName).fileName();

        if (!selectedScanName.isEmpty()){
            if (isBinary){
                octFile.setFileName(selectedScanName);
                QFileInfo fileInfo(octFile);
                scanName = fileInfo.fileName();
                scanName.chop(4);
            } else {
                octDir = QDir(selectedScanName);
                scanName = octDir.dirName();
            }

            patientData = PatientData();

            scan->deleteLater();
            scan = new Scan(this);
            scan->setIsBinary(isBinary);

            ui->statusBar->showMessage("Trwa odczyt danych badania OCT...");
            progressBar->setMaximum(100);
            progressBar->setVisible(true);
            progressBar->setValue(0);

            ReadWriteData *rwData = new ReadWriteData();
            rwData->setDataObject(&patientData, scan);
            if (isBinary)
                rwData->setOctFile(&octFile);
            else
                rwData->setDirectoryOct(&octDir);
            rwData->setDataSaveStrucure(dataSaveFormat);
            rwData->setFilePath(MANUAL, examDir.absolutePath().append("/mvri/" + scanName + ".mvri"));
            rwData->addDirective("readPatientData");
            if (isBinary)
                rwData->addDirective("readOctFile");
            else
                rwData->addDirective("readOctSequence");

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
    QImage image;

    // load image
    image = scan->getImage(imageNumber);

    if (!image.isNull()){
        orgImageSize = image.size();

        currentImageNumber = imageNumber;
        // enable / disable buttons
        enableNavigationButtons();

        // contrast enhancement and flattening
//        Calculate *calc = new Calculate();
//        calc->imageEnhancement(&image, contrast, brightness);

        // rescale image
        scaleFactor = qBound(0,scaleFactor,scales.count()-1);
        int imageHeight = scan->getBscanHeight() / scales[scaleFactor];
        double dy = qBound(0, scan->getBscanHeight() - (int)bscanRange.upper, scan->getBscanHeight()-imageHeight);
        QImage newImage = image.copy(0, dy, scan->getBscanWidth(), imageHeight);

        // display image
        ui->bScanHCPlot->yAxis->setRange(bscanRange);
        ui->bScanHCPlot->axisRect()->setBackground(QPixmap::fromImage(newImage),true,Qt::IgnoreAspectRatio);
        displayAnnotations();
        ui->bScanHCPlot->replot();

        ui->currImageNumberLEdit->setText(QString::number(currentImageNumber));
    } else {
        qDebug() << "Image is null!";
    }
}

void OCTAnnotate::loadNormalImage(int normalImageNumber){
    QImage normalImage = scan->getNormalImage(normalImageNumber);

    currentNormalImageNumber = normalImageNumber;

    // enable / disable buttons
    if (currentNormalImageNumber >= (scan->getBscanWidth() - 1))
        ui->nextNormalImageButton->setEnabled(false);
    else
        ui->nextNormalImageButton->setEnabled(true);
    if (currentNormalImageNumber <= 0)
        ui->prevNormalImageButton->setEnabled(false);
    else
        ui->prevNormalImageButton->setEnabled(true);

    // contrast enhancement
//    Calculate *calc = new Calculate();
//    calc->imageEnhancement(&normalImage, contrast, brightness);

    // scale
    scaleFactor = qBound(0,scaleFactor,scales.count()-1);
    int imageHeight = scan->getBscanHeight() / scales[scaleFactor];
    double dy = qBound(0, scan->getBscanHeight() - (int)bscanRange.upper, scan->getBscanHeight()-imageHeight);
    QImage newImage = normalImage.copy(0, dy, scan->getBscansNumber(), imageHeight);

    // display image
    ui->bScanVCPlot->yAxis->setRange(bscanRange);
    ui->bScanVCPlot->axisRect()->setBackground(QPixmap::fromImage(newImage),true,Qt::IgnoreAspectRatio);
    displayNormalAnnotations();
    ui->bScanVCPlot->replot();

    ui->currNormalImageNumberLEdit->setText(QString::number(currentNormalImageNumber));
}

void OCTAnnotate::loadImageLayers(int imageNumber)
{
    QImage image = scan->getImage(imageNumber);
    if (!image.isNull()){

        currentImageNumber = imageNumber;
        // enable / disable buttons
        enableNavigationButtons();

        // display image
        ui->layersManualCPlot->axisRect()->setBackground(QPixmap::fromImage(image),true,Qt::IgnoreAspectRatio);
        ui->layersAutoCPlot->axisRect()->setBackground(QPixmap::fromImage(image),true,Qt::IgnoreAspectRatio);
        displayLayers(ui->layersManualCPlot, false);
        displayLayers(ui->layersAutoCPlot, true);
        ui->layersManualCPlot->replot();
        ui->layersAutoCPlot->replot();

        ui->currLayersNumberLEdit->setText(QString::number(currentImageNumber));
    } else {
        qDebug() << "Image is null!";
    }
}

void OCTAnnotate::on_nextImageButton_clicked()
{
    if (scan->getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
        if (currentImageNumber < (scan->getBscansNumber() - 1)){
            loadImage(currentImageNumber + 1);
            loadNormalImage(currentNormalImageNumber);
            loadImageLayers(currentImageNumber + 1);
            fundusAnnotate = true;
        }
    }
}

void OCTAnnotate::on_prevImageButton_clicked(){
    if (scan->getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
        if (currentImageNumber > 0){
            loadImage(currentImageNumber - 1);
            loadNormalImage(currentNormalImageNumber);
            loadImageLayers(currentImageNumber - 1);
            fundusAnnotate = true;
        }
    }
}

void OCTAnnotate::on_nextNormalImageButton_clicked()
{
    if (scan->getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
        if (currentNormalImageNumber < (scan->getBscanWidth() - 1)){
            loadNormalImage(currentNormalImageNumber + 1);
            loadImage(currentImageNumber);
            fundusAnnotate = true;
        }
    }
}

void OCTAnnotate::on_prevNormalImageButton_clicked()
{
    if (scan->getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
        if (currentNormalImageNumber > 0){
            loadNormalImage(currentNormalImageNumber - 1);
            loadImage(currentImageNumber);
            fundusAnnotate = true;
        }
    }
}

void OCTAnnotate::on_nextLayersButton_clicked()
{
    if (scan->getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
        if (currentImageNumber < (scan->getBscansNumber() - 1)){
            loadImage(currentImageNumber + 1);
            loadNormalImage(currentNormalImageNumber);
            loadImageLayers(currentImageNumber + 1);
            fundusAnnotate = true;
        }
    }
}

void OCTAnnotate::on_prevLayersButton_clicked()
{
    if (scan->getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
        if (currentImageNumber > 0){
            loadImage(currentImageNumber - 1);
            loadNormalImage(currentNormalImageNumber);
            loadImageLayers(currentImageNumber - 1);
            fundusAnnotate = true;
        }
    }
}

void OCTAnnotate::on_currImageNumberLEdit_returnPressed()
{
    if (scan->getIsLoaded()){
        int value = ui->currImageNumberLEdit->text().toInt();
        if (value < 0)
            value = 0;
        if (value >= scan->getBscansNumber())
            value = scan->getBscansNumber()-1;

        loadImage(value);
        loadImageLayers(value);
        fundusAnnotate = true;
        ui->currImageNumberLEdit->clearFocus();
    }
}

void OCTAnnotate::on_currNormalImageNumberLEdit_returnPressed()
{
    if (scan->getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
        int value = ui->currNormalImageNumberLEdit->text().toInt();
        if (value < 0)
            value = 0;
        if (value >= scan->getBscanWidth())
            value = scan->getBscanWidth()-1;

        loadNormalImage(value);
        fundusAnnotate = true;
        ui->currNormalImageNumberLEdit->clearFocus();
    }
}

void OCTAnnotate::on_currLayersNumberLEdit_returnPressed()
{
    if (scan->getIsLoaded()){
        int value = ui->currLayersNumberLEdit->text().toInt();
        if (value < 0)
            value = 0;
        if (value >= scan->getBscansNumber())
            value = scan->getBscansNumber()-1;

        loadImage(value);
        loadImageLayers(value);
        fundusAnnotate = true;
        ui->currLayersNumberLEdit->clearFocus();
    }
}

// adjust contrast and brightness -----------------------------------------------------------------
void OCTAnnotate::on_contrastSlider_valueChanged(int value)
{
    contrast = (float)value / 10.0;
    if (scan->getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
        loadImage(currentImageNumber);
        loadNormalImage(currentNormalImageNumber);
    }
}

void OCTAnnotate::on_brightnessSlider_valueChanged(int value)
{
    brightness = value;
    if (scan->getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
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
        if (scan->getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
            loadImage(currentImageNumber);
            loadNormalImage(currentNormalImageNumber);
            ui->fundusImageLabel->setPixmap(QPixmap::fromImage(scan->getFundusImage())); // fundus
            fundusAnnotate = true;
        }
    } else if (currWidget == ui->tabAutoError){
        if (scan->getIsLoaded()){
            loadImageLayers(currentImageNumber);
        }
    }
}


// save data --------------------------------------------------------------------------------------


// draw / erase bscan layers ----------------------------------------------------------------------

bool OCTAnnotate::eventFilter(QObject *target, QEvent *event){
    if (scan->getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
//        QPoint currPoint;

        if (target == ui->fundusImageLabel){
            if (event->type() == QEvent::MouseButtonPress){
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

                currentImageNumber = mouseEvent->pos().y() * scan->getBscansNumber() / ui->fundusImageLabel->height();
                currentNormalImageNumber = mouseEvent->pos().x() * scan->getBscanWidth() / ui->fundusImageLabel->width();

                loadImage(currentImageNumber);
                loadNormalImage(currentNormalImageNumber);

                fundusAnnotate = true; // ,selectedErrorLayer);
            }
        }
        // TODO: uncomment and edit blow code for manual drawing of retina layers
        else if (target == ui->bScanHCPlot) {
            if (event->type() == QEvent::MouseButtonPress){
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
                if (mouseEvent->button() == Qt::RightButton){   // copy to clipboard
                    QClipboard *clipboard = QApplication::clipboard();
                    clipboard->setPixmap(ui->bScanHCPlot->toPixmap());
//                    QDir virtualMapCPlotPath = examDir;
//                    virtualMapCPlotPath.cdUp();
//                    QString fileName = scanName;
//                    fileName.chop(4);
//                    ui->virtualMapImageCPlot->savePng(virtualMapCPlotPath.absolutePath() + "/report/report_VMA_files/" + fileName + "_VRI_virtual_map.png",300,350);
//                    ui->virtualMapImageCPlot->saveJpg(virtualMapCPlotPath.absolutePath() + "/report/report_VMA_files/" + fileName + "_VRI_virtual_map.jpg",400,450,1,100,300,QCP::ruDotsPerInch);
                    QMessageBox::information(this, "Saved", "The image was saved to the clipboard");
                }
            }
//            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
//            currPoint.setX(ui->bScanHCPlot->xAxis->pixelToCoord(mouseEvent->pos().x()));
//            currPoint.setY(patientData.getBscanHeight() - ui->bScanHCPlot->yAxis->pixelToCoord(mouseEvent->pos().y()));

            if (event->type() == QEvent::Wheel){
                QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
                QPoint numSteps = wheelEvent->angleDelta() / 8 / 15;

                changeImageRange(numSteps.y());
            }
        } else if (target == ui->bScanVCPlot){
//            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
//            currPoint.setX(ui->bScanVCPlot->xAxis->pixelToCoord(mouseEvent->pos().x()));
//            currPoint.setY(patientData.getBscanHeight() - ui->bScanVCPlot->yAxis->pixelToCoord(mouseEvent->pos().y()));

            if (event->type() == QEvent::Wheel){
                QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
                QPoint numSteps = wheelEvent->angleDelta() / 8 / 15;

                changeImageRange(numSteps.y());
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
}

void OCTAnnotate::paintEvent(QPaintEvent *){
    if (fundusAnnotate){ // display annotation of fundus image (current B-skan line)
        if (scan->getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
            QPixmap fundusPixMap = QPixmap::fromImage(scan->getFundusImage());
            QPainter painter(&fundusPixMap);

            // draw line with current scan
            painter.setPen(QPen(Qt::green, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            QPoint startPoint = QPoint(0,currentImageNumber);
            QPoint endPoint = QPoint(fundusPixMap.width(), currentImageNumber);
            painter.drawLine(startPoint,endPoint);
            painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            startPoint = QPoint(currentNormalImageNumber,0);
            endPoint = QPoint(currentNormalImageNumber,fundusPixMap.height());
            painter.drawLine(startPoint,endPoint);

            // mark scan center

            ui->fundusImageLabel->setPixmap(fundusPixMap);
            fundusAnnotate = false;
        }
    }
}

// aditional settings -----------------------------------------------------------------------------

// layers display settings ------------------------------------------------------------------------
void OCTAnnotate::on_allLayersCBox_stateChanged(int state)
{
    if (state){
        ui->pcvLayerCBox->setChecked(true);
        ui->ibermLayerCBox->setChecked(true);
        ui->obermLayerCBox->setChecked(true);
        ui->ilmLayerCBox->setChecked(true);
        ui->rnfl_gclLayerCBox->setChecked(true);
        ui->gcl_iplLayerCBox->setChecked(true);
        ui->ipl_inlLayerCBox->setChecked(true);
        ui->inl_oplLayerCBox->setChecked(true);
        ui->opl_onlLayerCBox->setChecked(true);
        ui->elmLayerCBox->setChecked(true);
        ui->mezLayerCBox->setChecked(true);
        ui->iosLayerCBox->setChecked(true);
        ui->iboprLayerCBox->setChecked(true);
        ui->ibrpeLayerCBox->setChecked(true);
        ui->obrpeLayerCBox->setChecked(true);
    } else {
        ui->pcvLayerCBox->setChecked(false);
        ui->ibermLayerCBox->setChecked(false);
        ui->obermLayerCBox->setChecked(false);
        ui->ilmLayerCBox->setChecked(false);
        ui->rnfl_gclLayerCBox->setChecked(false);
        ui->gcl_iplLayerCBox->setChecked(false);
        ui->ipl_inlLayerCBox->setChecked(false);
        ui->inl_oplLayerCBox->setChecked(false);
        ui->opl_onlLayerCBox->setChecked(false);
        ui->elmLayerCBox->setChecked(false);
        ui->mezLayerCBox->setChecked(false);
        ui->iosLayerCBox->setChecked(false);
        ui->iboprLayerCBox->setChecked(false);
        ui->ibrpeLayerCBox->setChecked(false);
        ui->obrpeLayerCBox->setChecked(false);
    }
}

void OCTAnnotate::setLayerVisibility(bool st, QRadioButton *button, LayerName layer)
{
    int graphID = getAllLayers().indexOf(layer);

    if (st){    // get new values to display
        // bscan image
        QList<int> flatDiff;
        for (int col=0; col < scan->getBscanWidth(); col++){
            flatDiff.append(0);
        }

        QVector<double> z = scan->getLayerPointsVector(MANUAL, layer, currentImageNumber, false);
        QVector<double> x(z.length());
        for (int i=0; i<z.length(); i++){
            x[i] = i;
            z[i] -= flatDiff[i];
        }
        int graphID = getAllLayers().indexOf(layer);
        ui->bScanHCPlot->graph(graphID)->setData(x,z);

        // normal image
        QList<int> flatDiffNormal;
        for (int col=0; col < scan->getBscansNumber(); col++){
            flatDiffNormal.append(0);
        }

        z = scan->getLayerPointsVector(MANUAL, layer, currentNormalImageNumber, true);
        x.resize(z.length());
        for (int i=0; i<z.length(); i++){
            x[i] = i;
            z[i] -= flatDiffNormal[i];
        }
        ui->bScanVCPlot->graph(graphID)->setData(x,z);
    }

    ui->bScanHCPlot->graph(graphID)->setVisible(st);
    ui->bScanHCPlot->replot();

    ui->bScanVCPlot->graph(graphID)->setVisible(st);
    ui->bScanVCPlot->replot();

    button->setEnabled(st);
}

void OCTAnnotate::on_pcvLayerCBox_stateChanged(int state){
    setLayerVisibility(state != 0, ui->pcvLayerRButton, PCV);
}

void OCTAnnotate::on_ibermLayerCBox_stateChanged(int state){
    setLayerVisibility(state != 0, ui->ibermLayerRButton, IB_ERM);
}

void OCTAnnotate::on_obermLayerCBox_stateChanged(int state)
{
    setLayerVisibility(state != 0, ui->obermLayerRButton, OB_ERM);
}

void OCTAnnotate::on_ilmLayerCBox_stateChanged(int state) {
    setLayerVisibility(state != 0, ui->ilmLayerRButton, ILM);
}

void OCTAnnotate::on_rnfl_gclLayerCBox_stateChanged(int state){
    setLayerVisibility(state != 0, ui->rnfl_gclLayerRButton, RNFL_GCL);
}

void OCTAnnotate::on_gcl_iplLayerCBox_stateChanged(int state){
    setLayerVisibility(state != 0, ui->gcl_iplLayerRButton, GCL_IPL);
}

void OCTAnnotate::on_ipl_inlLayerCBox_stateChanged(int state){
    setLayerVisibility(state != 0, ui->ipl_inlLayerRButton, IPL_INL);
}

void OCTAnnotate::on_inl_oplLayerCBox_stateChanged(int state){
    setLayerVisibility(state != 0, ui->inl_oplLayerRButton, INL_OPL);
}

void OCTAnnotate::on_opl_onlLayerCBox_stateChanged(int state){
    setLayerVisibility(state != 0, ui->opl_onlLayerRButton, OPL_ONL);
}

void OCTAnnotate::on_elmLayerCBox_stateChanged(int state){
    setLayerVisibility(state != 0, ui->elmLayerRButton, ELM);
}

void OCTAnnotate::on_mezLayerCBox_stateChanged(int state){
    setLayerVisibility(state != 0, ui->mezLayerRButton, MEZ);
}

void OCTAnnotate::on_iosLayerCBox_stateChanged(int state){
    setLayerVisibility(state != 0, ui->iosLayerRButton, IS_OS);
}

void OCTAnnotate::on_iboprLayerCBox_stateChanged(int state)
{
    setLayerVisibility(state != 0, ui->iboprLayerRButton, IB_OPR);
}

void OCTAnnotate::on_ibrpeLayerCBox_stateChanged(int state){
    setLayerVisibility(state != 0, ui->ibrpeLayerRButton, IB_RPE);
}

void OCTAnnotate::on_obrpeLayerCBox_stateChanged(int state){
    setLayerVisibility(state != 0, ui->obrpeLayerRButton, OB_RPE);
}

void OCTAnnotate::on_pcvLayerRButton_clicked()
{
//    if (ui->pcvLayerRButton->isChecked())
//        selectedLayer = PCV;
}

void OCTAnnotate::on_ibermLayerRButton_clicked()
{
//    if (ui->ibermLayerRButton->isChecked())
//        selectedLayer = IB_ERM;
}

void OCTAnnotate::on_obermLayerRButton_clicked()
{
//    if (ui->obermLayerRButton->isChecked())
//        selectedLayer = OB_ERM;
}

void OCTAnnotate::on_ilmLayerRButton_clicked()
{
//    if (ui->ilmLayerRButton->isChecked())
//        selectedLayer = ILM;
}

void OCTAnnotate::on_rnfl_gclLayerRButton_clicked()
{
//    if (ui->rnfl_gclLayerRButton->isChecked())
//        selectedLayer = RNFL_GCL;
}

void OCTAnnotate::on_gcl_iplLayerRButton_clicked()
{
//    if (ui->gcl_iplLayerRButton->isChecked())
//        selectedLayer = GCL_IPL;
}

void OCTAnnotate::on_ipl_inlLayerRButton_clicked()
{
//    if (ui->ipl_inlLayerRButton->isChecked())
//        selectedLayer = IPL_INL;
}

void OCTAnnotate::on_inl_oplLayerRButton_clicked()
{
//    if (ui->inl_oplLayerRButton->isChecked())
//        selectedLayer = INL_OPL;
}

void OCTAnnotate::on_opl_onlLayerRButton_clicked()
{
//    if (ui->opl_onlLayerRButton->isChecked())
//        selectedLayer = OPL_ONL;
}

void OCTAnnotate::on_elmLayerRButton_clicked()
{
//    if (ui->elmLayerRButton->isChecked())
//        selectedLayer = ELM;
}

void OCTAnnotate::on_mezLayerRButton_clicked()
{
//    if (ui->mezLayerRButton->isChecked())
//        selectedLayer = MEZ;
}

void OCTAnnotate::on_iosLayerRButton_clicked()
{
//    if (ui->iosLayerRButton->isChecked())
//        selectedLayer = IS_OS;
}

void OCTAnnotate::on_iboprLayerRButton_clicked()
{
//    if (ui->iboprLayerRButton->isChecked())
//        selectedLayer = IB_OPR;
}

void OCTAnnotate::on_ibrpeLayerRButton_clicked()
{
//    if (ui->ibrpeLayerRButton->isChecked())
//        selectedLayer = IB_RPE;
}

void OCTAnnotate::on_obrpeLayerRButton_clicked()
{
//    if (ui->obrpeLayerRButton->isChecked())
//        selectedLayer = OB_RPE;
}


// bscan scale / zoom -----------------------------------------------------------------------------
void OCTAnnotate::on_zoomInButton_clicked()
{
    if (scan->getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
        scaleFactor++;
        rescaleImage();
    }
}

void OCTAnnotate::on_zoomOutButton_clicked()
{
    if (scan->getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
        scaleFactor--;
        rescaleImage();
    }
}

void OCTAnnotate::rescaleImage(){
    scaleFactor = qBound(0,scaleFactor,scales.count()-1);

    int imageHeight = scan->getBscanHeight() / scales[scaleFactor];
    double y2 = (int)((double)(scan->getBscanHeight() - imageHeight) / 2.0);
    double y1 = (double)scan->getBscanHeight() - y2;
    double offset = 0;//bscanRange.lower;
    bscanRange = QCPRange(y1-offset,y2-offset);
    ui->bScanHCPlot->yAxis->setRange(bscanRange);
    ui->bScanVCPlot->yAxis->setRange(bscanRange);

    QImage image(scan->getImage(currentImageNumber));
    QImage normalImage = scan->getNormalImage(currentNormalImageNumber);

//    Calculate *calc = new Calculate();
//    calc->imageEnhancement(&image, contrast, brightness);
//    calc->imageEnhancement(&normalImage, contrast, brightness);

    QImage newImage = image.copy(0,bscanRange.lower,scan->getBscanWidth(),imageHeight);
    ui->bScanHCPlot->axisRect()->setBackground(QPixmap::fromImage(newImage),true,Qt::IgnoreAspectRatio);
    ui->bScanHCPlot->replot();

    QImage newNormalImage = normalImage.copy(0,bscanRange.lower,scan->getBscansNumber(),imageHeight);
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

void OCTAnnotate::changeImageRange(int dir)
{
    int imageHeight = static_cast<int>(scan->getBscanHeight() / scales[scaleFactor]);
    QCPRange plotRange = ui->bScanHCPlot->yAxis2->range();
    QCPRange newRange;
    bool rescale = false;

    if ((dir > 0) && (plotRange.upper < scan->getBscanHeight())){
        double upper = qBound(0.0, plotRange.upper + 20, static_cast<double>(scan->getBscanHeight()));
        newRange = QCPRange(upper - imageHeight, upper);
        rescale = true;
    } else if ((dir < 0) && (plotRange.lower > 0)){
        double lower = qBound(0.0, plotRange.lower - 20, static_cast<double>(scan->getBscanHeight()));
        newRange = QCPRange(lower, lower + imageHeight);
        rescale = true;
    }

    if (rescale){
        ui->bScanHCPlot->yAxis2->setRange(newRange);
        ui->bScanVCPlot->yAxis2->setRange(newRange);
        bscanRange = newRange;
        QCPRange bscanDepthRange = bscanRange * scan->getDepthCoeff();
        ui->bScanHCPlot->yAxis->setRange(bscanDepthRange);
        ui->bScanVCPlot->yAxis->setRange(bscanDepthRange);

        QImage image(scan->getImage(currentImageNumber));
        QImage normalImage = scan->getNormalImage(currentNormalImageNumber);

//        Calculate *calc = new Calculate();
//        calc->imageEnhancement(&image, contrast, brightness);
//        calc->imageEnhancement(&normalImage, contrast, brightness);

//        if (flattenImage){
//            QList<int> flatDiff, flatDiffNormal;
//            if (scan->hasManualAnnotations()){
//                flatDiff = scan->getFlatDifferencesRPE(currentImageNumber);
//                flatDiffNormal = scan->getFlatDifferencesNormalRPE(currentNormalImageNumber);
//            } else {
//                flatDiff = scan->getFlatDifferences(currentImageNumber);
//                flatDiffNormal = scan->getFlatDifferencesNormal(currentNormalImageNumber);
//            }
//            image = calc->flattenImage(&image, flatDiff);
//            normalImage = calc->flattenImage(&normalImage, flatDiffNormal);
//        }

        double dy = qBound(0, scan->getBscanHeight() - static_cast<int>(newRange.upper), scan->getBscanHeight()-imageHeight);
        QImage newImage = image.copy(0, static_cast<int>(dy), scan->getBscanWidth(), imageHeight);
        ui->bScanHCPlot->axisRect()->setBackground(QPixmap::fromImage(newImage),true,Qt::IgnoreAspectRatio);
        ui->bScanHCPlot->replot();

        QImage newNormalImage = normalImage.copy(0, static_cast<int>(dy) ,scan->getBscansNumber(),imageHeight);
        ui->bScanVCPlot->axisRect()->setBackground(QPixmap::fromImage(newNormalImage),true,Qt::IgnoreAspectRatio);
        ui->bScanVCPlot->replot();
    }
}

void OCTAnnotate::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep()/2)));
}

// general data edit ------------------------------------------------------------------------------

// layers plot --------------------------------------------------------------------
void OCTAnnotate::setupBScanPlots(){
    ui->bScanHCPlot->clearGraphs();

    // configure axis
    ui->bScanHCPlot->xAxis->setLabel("B-scan pixel (horizontal direction)");    // bottom
    ui->bScanHCPlot->yAxis->setLabel("B-scan pixel (vertical direction)");      // left
    ui->bScanHCPlot->xAxis->setRange(0,scan->getBscanWidth());
    ui->bScanHCPlot->yAxis->setRange(0,scan->getBscanHeight());
    ui->bScanHCPlot->xAxis->grid()->setVisible(false);
    ui->bScanHCPlot->yAxis->grid()->setVisible(false);

    ui->bScanHCPlot->xAxis2->setLabel("Scan width [mm]");
    ui->bScanHCPlot->yAxis2->setLabel("Scan depth [um]");
    ui->bScanHCPlot->xAxis2->setRange(0,scan->getScanWidth());      // top
    ui->bScanHCPlot->yAxis2->setRange(0,scan->getScanDepth()*1000); // right
    QSharedPointer<QCPAxisTicker> ticker(new QCPAxisTicker);
    ticker->setTickCount(static_cast<int>(scan->getScanDepth()*1000/100));
    ui->bScanHCPlot->yAxis2->setTicker(ticker);
    ui->bScanHCPlot->xAxis2->setVisible(true);
    ui->bScanHCPlot->yAxis2->setVisible(true);

    // add graphs
    QList<LayerName> allLayers = getAllLayers();
    int graphID = 0;
    foreach (LayerName layer, allLayers) {
        ui->bScanHCPlot->addGraph();
        ui->bScanHCPlot->graph(graphID)->setPen(QPen(scan->getLayerColor(layer)));
        ui->bScanHCPlot->graph(graphID)->setLineStyle(QCPGraph::lsLine);
        ui->bScanHCPlot->graph(graphID)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,1.5)); // 3.5 do publikacji
        graphID++;
    }

    // normal image
    ui->bScanVCPlot->clearGraphs();

    ui->bScanVCPlot->xAxis->setLabel("B-scan cross-section");
    ui->bScanVCPlot->yAxis->setLabel("B-scan pixel (vertical direction)");
    ui->bScanVCPlot->xAxis->setRange(0,scan->getBscansNumber());
    ui->bScanVCPlot->yAxis->setRange(0,scan->getBscanHeight());
    ui->bScanVCPlot->xAxis->grid()->setVisible(false);
    ui->bScanVCPlot->yAxis->grid()->setVisible(false);

    ui->bScanVCPlot->xAxis2->setLabel("Scan width [mm]");
    ui->bScanVCPlot->yAxis2->setLabel("Scan depth [um]");
    ui->bScanVCPlot->xAxis2->setRange(0,scan->getScanWidth());      // top
    ui->bScanVCPlot->yAxis2->setRange(0,scan->getScanDepth()*1000); // right
    ui->bScanVCPlot->yAxis2->setTicker(ticker);
    ui->bScanVCPlot->xAxis2->setVisible(true);
    ui->bScanVCPlot->yAxis2->setVisible(true);

    graphID = 0;
    foreach (LayerName layer, allLayers) {
        ui->bScanVCPlot->addGraph();
        ui->bScanVCPlot->graph(graphID)->setPen(QPen(scan->getLayerColor(layer)));
        ui->bScanVCPlot->graph(graphID)->setLineStyle(QCPGraph::lsLine);
        ui->bScanVCPlot->graph(graphID)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,1.0));
        graphID++;
    }

    // layers plots (reference)
    ui->layersManualCPlot->clearGraphs();

    ui->layersManualCPlot->xAxis->setLabel("B-scan cross-section");
    ui->layersManualCPlot->yAxis->setLabel("B-scan pixel (vertical direction)");
    ui->layersManualCPlot->xAxis->setRange(0,scan->getBscanWidth());
    ui->layersManualCPlot->yAxis->setRange(0,scan->getBscanHeight());
    ui->layersManualCPlot->xAxis->grid()->setVisible(false);
    ui->layersManualCPlot->yAxis->grid()->setVisible(false);

    graphID = 0;
    foreach (LayerName layer, allLayers) {
        ui->layersManualCPlot->addGraph();
        ui->layersManualCPlot->graph(graphID)->setPen(QPen(scan->getLayerColor(layer)));
        ui->layersManualCPlot->graph(graphID)->setLineStyle(QCPGraph::lsLine);
        ui->layersManualCPlot->graph(graphID)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,0.5));
        graphID++;
    }

    // layers plots (automatic)
    ui->layersAutoCPlot->clearGraphs();

    ui->layersAutoCPlot->xAxis->setLabel("B-scan cross-section");
    ui->layersAutoCPlot->yAxis->setLabel("B-scan pixel (vertical direction)");
    ui->layersAutoCPlot->xAxis->setRange(0,scan->getBscanWidth());
    ui->layersAutoCPlot->yAxis->setRange(0,scan->getBscanHeight());
    ui->layersAutoCPlot->xAxis->grid()->setVisible(false);
    ui->layersAutoCPlot->yAxis->grid()->setVisible(false);

    graphID = 0;
    foreach (LayerName layer, allLayers) {
        ui->layersAutoCPlot->addGraph();
        ui->layersAutoCPlot->graph(graphID)->setPen(QPen(scan->getLayerColor(layer)));
        ui->layersAutoCPlot->graph(graphID)->setLineStyle(QCPGraph::lsLine);
        ui->layersAutoCPlot->graph(graphID)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,0.5));
        graphID++;
    }
}

QList<LayerName> OCTAnnotate::getLayersToDisplay()
{
    QList<LayerName> layers;
    if (ui->pcvLayerCBox->isChecked()){
        layers.append(PCV);
    }
    if (ui->ibermLayerCBox->isChecked()){
        layers.append(IB_ERM);
    }
    if (ui->obermLayerCBox->isChecked()){
        layers.append(OB_ERM);
    }
    if (ui->ilmLayerCBox->isChecked()){
        layers.append(ILM);
    }
    if (ui->rnfl_gclLayerCBox->isChecked()){
        layers.append(RNFL_GCL);
    }
    if (ui->gcl_iplLayerCBox->isChecked()){
        layers.append(GCL_IPL);
    }
    if (ui->ipl_inlLayerCBox->isChecked()){
        layers.append(IPL_INL);
    }
    if (ui->inl_oplLayerCBox->isChecked()){
        layers.append(INL_OPL);
    }
    if (ui->opl_onlLayerCBox->isChecked()){
        layers.append(OPL_ONL);
    }
    if (ui->elmLayerCBox->isChecked()){
        layers.append(ELM);
    }
    if (ui->mezLayerCBox->isChecked()){
        layers.append(MEZ);
    }
    if (ui->iosLayerCBox->isChecked()){
        layers.append(IS_OS);
    }
    if (ui->iboprLayerCBox->isChecked()){
        layers.append(IB_OPR);
    }
    if (ui->ibrpeLayerCBox->isChecked()){
        layers.append(IB_RPE);
    }
    if (ui->obrpeLayerCBox->isChecked()){
        layers.append(OB_RPE);
    }
    return layers;
}

void OCTAnnotate::displayAnnotations(QList<int> flatDiff)
{
    if (scan->getIsLoaded()){
        if (flatDiff.length() < 1){
            for (int col=0; col < scan->getBscanWidth(); col++){
                flatDiff.append(0);
            }
        }

        QList<LayerName> dispLayers = getLayersToDisplay();
        QList<LayerName> allLayers = getAllLayers();

        // set data to graphs
        foreach (LayerName layer, dispLayers) {
            QVector<double> z = scan->getLayerPointsVector(MANUAL, layer, currentImageNumber, false);
            QVector<double> x(z.length());
            for (int i=0; i<z.length(); i++){
                x[i] = i;
                z[i] -= flatDiff[i];
            }
            int graphID = allLayers.indexOf(layer);
            ui->bScanHCPlot->graph(graphID)->setData(x,z);
        }
    }
}

void OCTAnnotate::displayNormalAnnotations(QList<int> flatDiff)
{
    if (scan->getIsLoaded()){
        if (flatDiff.length() < 1){
            for (int col=0; col < scan->getBscansNumber(); col++){
                flatDiff.append(0);
            }
        }

        QList<LayerName> dispLayers = getLayersToDisplay();
        QList<LayerName> allLayers = getAllLayers();

        // set data to graphs
        foreach (LayerName layer, dispLayers) {
            QVector<double> z = scan->getLayerPointsVector(MANUAL, layer, currentNormalImageNumber, true);
            QVector<double> x(z.length());
            for (int i=0; i<z.length(); i++){
                x[i] = i;
                z[i] -= flatDiff[i];
            }
            int graphID = allLayers.indexOf(layer);
            ui->bScanVCPlot->graph(graphID)->setData(x,z);
        }
    }
}

void OCTAnnotate::displayLayers(QCustomPlot *plot, bool isAuto)
{
    QList<LayerName> dispLayers = getLayersToDisplay();
    QList<LayerName> allLayers = getAllLayers();

    // set data to graphs
    foreach (LayerName layer, dispLayers) {
        QVector<double> z = scan->getLayerPointsVector(!isAuto, layer, currentImageNumber, false);
        QVector<double> x(z.length());
        for (int i=0; i<z.length(); i++){
            x[i] = i;
        }
        int graphID = allLayers.indexOf(layer);
        plot->graph(graphID)->setData(x,z);
    }
}

// display statistics -----------------------------------------------------------------------------

// recalculate virtual map for selected values ----------------------------------------------------

// annotations -------------------------------------------------------------------------------
void OCTAnnotate::on_actionReadManualAnnotations_triggered()
{
    readAnnotations(MANUAL);
}

void OCTAnnotate::on_actionReadAutoAnnotations_triggered()
{
    readAnnotations(AUTO);
}

void OCTAnnotate::readAnnotations(bool isManual)
{
    if (scan->getIsLoaded()){
//        QMessageBox msgBox;
//        QPushButton *saveButton = msgBox.addButton(" Save and load new exam ", QMessageBox::YesRole);
//        QPushButton *cancelButton = msgBox.addButton(" Cancel ", QMessageBox::RejectRole);
//        msgBox.addButton(" Load new exam without saving ", QMessageBox::NoRole);
        bool selectNew = true;

//        if (octDataModified){
//            msgBox.setText("OCT segmentation data has been changed. Do you want to save the changes before loading new exam?");
//            msgBox.exec();
//            if (msgBox.clickedButton() == saveButton){
//                on_actionSaveOCTExam_triggered();
//            } else if (msgBox.clickedButton() == cancelButton) {
//                selectNew = false;
//            }
//        }

        if (selectNew){
            // read data from file
            QString segmentFilePath;
            if (isManual)
                segmentFilePath = QFileDialog::getOpenFileName(this, tr("Open file with manual segmentations"), examDir.path(), tr("OCTAnnotate file (*.mvri);;All files (*.*)"));
            else
                segmentFilePath = QFileDialog::getOpenFileName(this, tr("Open file with auto segmentations"), examDir.path(), tr("Json file (*.json);;OCTExplorer file (*.xml);;Text file (*.txt)"));

            if (segmentFilePath.isEmpty()){
                if (scan->hasAnnotations(isManual)){
                    QMessageBox msgBox;
                    msgBox.addButton(" Leave data ", QMessageBox::RejectRole);
                    msgBox.addButton(" Save ", QMessageBox::AcceptRole);

                    msgBox.setText("Do you want to clear segmentation data?");
                    if (msgBox.exec() == QMessageBox::Accepted){
                        scan->resetAnnotations(isManual);
                        ui->layersAutoCPlot->clearGraphs();
                        ui->layersAutoCPlot->replot();
                    }
                }
                return;
            } else {
                if (isManual)
                    ui->statusBar->showMessage("Trwa odczyt referencyjnych segmentacji...");
                else
                    ui->statusBar->showMessage("Trwa odczyt automatycznych segmentacji...");
                progressBar->setMaximum(100);
                progressBar->setVisible(true);
                progressBar->setValue(0);

                ReadWriteData *rwData = new ReadWriteData();
                rwData->setDataObject(&patientData, scan);
                rwData->setDataSaveStrucure(dataSaveFormat);
                rwData->setFilePath(isManual, segmentFilePath);
                if (isManual)
                    rwData->addDirective("readManualSegmentationData");
                else
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
    }
}

void OCTAnnotate::on_actionSaveManualAnnotations_triggered()
{
    saveAnnotations(MANUAL);
}

void OCTAnnotate::on_actionSaveAutoAnnotations_triggered()
{
    saveAnnotations(AUTO);
}

void OCTAnnotate::saveAnnotations(bool isManual)
{
    if (scan->getIsLoaded()){   // && scan->hasAnnotations(isManual)
        QString filePath;
        if (isManual)
            filePath = QFileDialog::getSaveFileName(this, tr("Save reference segmentations as..."), examDir.path() + "/mvri/" + scanName, tr("OCTAnnotate file (*.mvri)"));
        else
            filePath = QFileDialog::getSaveFileName(this, tr("Save auto segmentations as..."), examDir.path() + "/avri/" + scanName, tr("Json file (*.json);;OCTExplorer file (*.xml);;Text file (*.txt)"));;
        if (!filePath.isEmpty()){
            if (isManual)
                ui->statusBar->showMessage("Trwa zapis referencyjnych segmentacji...");
            else
                ui->statusBar->showMessage("Trwa zapis automatycznych segmentacji...");
            progressBar->setMaximum(100);
            progressBar->setVisible(true);
            progressBar->setValue(0);

            ReadWriteData *rwData = new ReadWriteData();
            rwData->setDataObject(&patientData, scan);
            rwData->setAppVersion(appVersion);
            rwData->setFilePath(isManual, filePath);
            rwData->setDataSaveStrucure(dataSaveFormat);
            if (isManual)
                rwData->addDirective("saveManualSegmentationData");
            else
                rwData->addDirective("saveAutoSegmentationData");

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
        QMessageBox::critical(this, tr("Error"), tr("To save the data, it must be first loaded!"));
    }
}

void OCTAnnotate::on_actionCopyAutoSegmentationsAsManual_triggered()
{
    AutoAsManualDialog autoAsManualdialog(this);

    if (autoAsManualdialog.exec()){
        QList<LayerName> selectedLayers = autoAsManualdialog.getSelectedLayers(); // getAllLayers(); //
        int selectedScans = autoAsManualdialog.getSelectedScans(); // 0; //

        foreach (LayerName layer, selectedLayers) {
            if (selectedScans == 0){    // all scans
                scan->copyAuto2ManualLayerPoints(layer);
            } else if (selectedScans == 1){ // empty scans
                // TODO: check for B-scans without segmentations
                // ...
            } else if (selectedScans == 2){ // current scan
                scan->copyAuto2ManualLayerPoints(layer, currentImageNumber);
            }
        }
        on_tabWidget_currentChanged();
//        octDataModified = true;

        QMessageBox::information(this, "OCTAnnotate", "Auto layers segmentations were copied as manual. \n Remember to save them to file before exiting!");
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

    if (data == "octData"){  // (data != "manualOnly") && (data != "autoOnly")
        scan->setIsLoaded(true);

        // display patients data
        // display information
        this->setWindowTitle("OCTAnnotate v" + appVersion + " - " + scanName);

        currentImageNumber = scan->getBscansNumber()/2;   // middle B-scan
        currentNormalImageNumber = scan->getBscanWidth()/2;

        // enable navigation buttons
        enableNavigationButtons();
        ui->zoomInButton->setEnabled(true);
    }

    ui->statusBar->clearMessage();
    progressBar->setValue(100);
    progressBar->setVisible(false);

    QString msg = "Wczytano dane badania OCT";
    if (data == "manualRead")
        msg = "Wczytano dane ręcznej segmentacji";
    if (data == "autoRead")
        msg = "Wczytano dane automatycznej segmentacji";
    QMessageBox::information(this, "Odczyt danych OCT", msg);

    // setup plots
    setupBScanPlots();
    // display data
    on_tabWidget_currentChanged();
}

void OCTAnnotate::on_savingDataFinished(QString data)
{
    if (data == "manualSave"){
        QMessageBox::information(this, tr("Save"), tr("Manual segmentation data saved."));
    }
    if (data == "autoSave"){
        QMessageBox::information(this, tr("Save"), tr("Auto segmentation data saved."));
    }
}

// run automatic segmentations -------------------------------------------------------------------
void OCTAnnotate::on_actionPerformAutomaticSegmentation_triggered()
{
    layersSegmentation();
}


void OCTAnnotate::layersSegmentation()
{
    QImage image;

    Ort::Env env;
    Ort::RunOptions runOptions;
    Ort::Session session(nullptr);

    constexpr int64_t batch_size = 1;
    constexpr int64_t numChannels = 1;
    constexpr int64_t height = 640;
    constexpr int64_t width = 384;
    constexpr int64_t numInputElements = numChannels * height * width;
    auto modelPath = L"..\\models\\model.onnx";

    session = Ort::Session(env, modelPath, Ort::SessionOptions{ nullptr });

    const std::vector<int64_t> inputShape = { 1, numChannels, height, width };
    const std::vector<int64_t> outputShape = { 1, 4, height, width };


    std::vector<float> input (numInputElements);
    std::vector<float> results (4*numInputElements);

    // TEST

    for (int i = 0; i < scan->getBscansNumber(); i++)
    {
        image = scan->getImage(i);

        image = image.convertToFormat(QImage::Format_Grayscale8);

        cv::Mat mat(image.height(), image.width(), CV_8UC1, image.bits(), image.bytesPerLine());

        const std::vector<float> imageVec = loadImageTest(mat, 384, 640);
        std::copy(imageVec.begin(), imageVec.end(), input.begin());
        qDebug() << "loaded image " << QString::number(i);

        auto memory_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
        auto inputTensor = Ort::Value::CreateTensor<float>(memory_info, input.data(), input.size(), inputShape.data(), inputShape.size());
        auto outputTensor = Ort::Value::CreateTensor<float>(memory_info, results.data(), results.size(), outputShape.data(), outputShape.size());

        Ort::AllocatorWithDefaultOptions ort_alloc;
        Ort::AllocatedStringPtr inputName = session.GetInputNameAllocated(0, ort_alloc);
        Ort::AllocatedStringPtr outputName = session.GetOutputNameAllocated(0, ort_alloc);
        const std::array<const char*, 1> inputNames = { inputName.get() };
        const std::array<const char*, 1> outputNames = { outputName.get() };
        inputName.release();
        outputName.release();

        session.Run(runOptions, inputNames.data(), &inputTensor, 1, outputNames.data(), &outputTensor, 1);
        cv::Mat m = composeImage(results, numInputElements);

        cv::Mat layer_1;
        cv::threshold(m, layer_1, 50, 255, cv::THRESH_BINARY);
        cv::Mat layer_2;
        cv::threshold(m, layer_2, 125, 255, cv::THRESH_BINARY);
        cv::Mat layer_3;
        cv::threshold(m, layer_3, 200, 255, cv::THRESH_BINARY);
        cv::Mat edges_1;
        cv::Mat edges_2;
        cv::Mat edges_3;
        cv::Canny(layer_1, edges_1, 20, 100);
        cv::Canny(layer_2, edges_2, 20, 100);
        cv::Canny(layer_3, edges_3, 20, 100);


        QVector<double> layer_z;
        QVector<double> layer_x;
        LayerName layer = PCV;
        findPoints(edges_1, layer_z, layer_x, layer, i);
        layer_x.clear();
        layer_z.clear();
        layer = ILM;
        findPoints(edges_2, layer_z, layer_x, layer, i);
        layer_x.clear();
        layer_z.clear();
        layer = OB_RPE;
        findPoints(edges_3, layer_z, layer_x, layer, i);
        layer_x.clear();
        layer_z.clear();
    }

    /*
    cv::cvtColor(edges_1, edges_1, cv::COLOR_GRAY2BGR);
    cv::cvtColor(edges_2, edges_2, cv::COLOR_GRAY2BGR);
    cv::cvtColor(edges_3, edges_3, cv::COLOR_GRAY2BGR);
    cv::Mat red_1 = edges_1.mul(cv::Scalar(255, 0, 0), 1);
    cv::Mat red_2 = edges_2.mul(cv::Scalar(0, 255, 0), 1);
    cv::Mat red_3 = edges_3.mul(cv::Scalar(0, 0, 255), 1);
    cv::Mat result;
    cv::Mat m_color;
    cv::cvtColor(m, m_color, cv::COLOR_GRAY2RGB);
    m_color.copyTo(result);
    red_1.copyTo(result, edges_1);
    red_2.copyTo(result, edges_2);
    red_3.copyTo(result, edges_3);
    cv::imshow("No Copy 2", result);
    */

    qDebug() << "Finished automatic segmentation!";
    QMessageBox::information(this, "OCTAnnotate", "Automatic segmentation completed!");
    on_tabWidget_currentChanged();
}

void OCTAnnotate::findPoints(cv::Mat edges, QVector<double>& z_data, QVector<double>& x_data, LayerName layer, int imageNumber)
{
int rows = edges.rows;
int cols = edges.cols;
for (int i = 0; i < cols; i++)
{
    for (int j = 0; j < rows; j++)
    {
        if (edges.at<uchar>(j, i) == 255)
        {
            z_data.push_back(rows - j);
            x_data.push_back(i);
            scan->setPoint(false, layer, imageNumber, i, j);
        }
    }
}
}

cv::Mat OCTAnnotate::composeImage(std::vector<float> &results, int64_t numInputElements)
{
    std::vector<uchar> thresholded_results(numInputElements);
    for (int i = 0; i < results.size() / 4; i++)
    {
        std::vector<float> temp{
        results[i],
        results[i + (results.size() / 4)],
        results[i + (2 * results.size() / 4)],
        results[i + (3 * results.size() / 4)]
        };
        int maxElementIndex = std::max_element(temp.begin(), temp.end()) - temp.begin();

        switch (maxElementIndex)
        {
        case 0:
            thresholded_results[i] = 0;
            break;
        case 1:
            thresholded_results[i] = 85;
            break;
        case 2:
            thresholded_results[i] = 170;
            break;
        case 3:
            thresholded_results[i] = 255;
            break;
        default:
            break;
        }
        /*
        if (index < results.size() / 16)
        {
            thresholded_results[index] = 0;
        }
        else if (index < 2* results.size() / 16 && index >= results.size() / 16)
        {
            thresholded_results[index] = 63;
        }
        else if (index < 3 * results.size() / 16 && index >= 2* results.size() / 16)
        {
            thresholded_results[index] = 126;
        }
        else if (index < 4 * results.size() / 16 && index >= 3 * results.size() / 16)
        {
            thresholded_results[index] = 255;
        }
        */
    }
    cv::Mat m = cv::Mat(640, 384, CV_8UC1);
    std::vector<uchar> doubleVec(thresholded_results.begin(), thresholded_results.end());
    std::memcpy(m.data, doubleVec.data(), doubleVec.size() * sizeof(uchar));
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::morphologyEx(m, m, cv::MORPH_CLOSE, kernel);
    return m;
}

// calculate statistics for CAVRI analysis -------------------------------------------------------

// data visualization for publication purposes ---------------------------------------------------

// patients database -----------------------------------------------------------------------------

// batch processing -------------------------------------------------------------------------------

// search / filter / show data --------------------------------------------------------------------
