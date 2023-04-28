#include "octannotate.h"
#include "functions.h"
#include "ui_octannotate.h"
#include "settingsdialog.h"
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
    ui->verticalLayout_6->setAlignment(ui->fundusImageLabel, Qt::AlignHCenter);

    contrast = 1.5;
    brightness = 0;
    ui->contrastSlider->setValue(contrast*10);

    ui->fundusImageLabel->setScaledContents(true);
    ui->fundusImageLabel->installEventFilter(this);
    ui->bScanHCPlot->installEventFilter(this);
    ui->bScanVCPlot->installEventFilter(this);

    fundusAnnotate = false;

    ui->pcvLayerRButton->setChecked(true);
    // TODO: create a LayerColor class to store and get colors for lines and labels
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
    rpeColor = Qt::blue;
    chrColor = QColor(255,85,0); // darkBlue
    QMap<LayerName, QLabel*> colorLabels;
    colorLabels[PCV] = ui->pcvColorLabel;
    colorLabels[ERM_UP] = ui->ermColorLabel;
    colorLabels[ILM] = ui->ilmColorLabel;
    colorLabels[NFL_GCL] = ui->gclColorLabel;
    colorLabels[GCL_IPL] = ui->iplColorLabel;
    colorLabels[IPL_INL] = ui->inlColorLabel;
    colorLabels[INL_OPL] = ui->oplColorLabel;
    colorLabels[OPL_ONL] = ui->onlColorLabel;
    colorLabels[ELM] = ui->elmColorLabel;
    colorLabels[MEZ] = ui->mezColorLabel;
    colorLabels[IS_OS] = ui->iosColorLabel;
    colorLabels[OS_RPE] = ui->rpeColorLabel;
    colorLabels[RPE_CHR] = ui->chrColorLabel;
    QMapIterator<LayerName, QLabel*> iter(colorLabels);
    while (iter.hasNext()) {
        iter.next();
        myPalette.setColor(QPalette::Window, getLayerColor(iter.key()));
        iter.value()->setAutoFillBackground(true);
        iter.value()->setPalette(myPalette);
    }

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

    octDir = QDir(sDialog->getPathOctData());
    examDir = QDir(sDialog->getPathExamData());

    dataSaveStructure = sDialog->getDataSaveStructure();
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
            rwData->setDataSaveStrucure(dataSaveStructure);
            rwData->setManualFilePath(examDir.absolutePath().append("/mvri/" + scanName + ".mvri"));
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
    //QFileInfo imageFileInfo;
    QImage image;

    // load image
    image = scan->getImage(imageNumber);

    if (!image.isNull()){
        orgImageSize = image.size();

        currentImageNumber = imageNumber;
        // enable / disable buttons
        if (currentImageNumber >= (scan->getBscansNumber() - 1))
            ui->nextImageButton->setEnabled(false);
        else
            ui->nextImageButton->setEnabled(true);
        if (currentImageNumber <= 0)
            ui->prevImageButton->setEnabled(false);
        else
            ui->prevImageButton->setEnabled(true);

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

void OCTAnnotate::on_nextImageButton_clicked()
{
    if (scan->getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
        if (currentImageNumber < (scan->getBscansNumber() - 1)){
            loadImage(currentImageNumber + 1);
            loadNormalImage(currentNormalImageNumber);
            fundusAnnotate = true;
        }
    }
}

void OCTAnnotate::on_prevImageButton_clicked(){
    if (scan->getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
        if (currentImageNumber > 0){
            loadImage(currentImageNumber - 1);
            loadNormalImage(currentNormalImageNumber);
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

void OCTAnnotate::on_currImageNumberLEdit_returnPressed()
{
    if (scan->getIsLoaded()){ // !patientData.getImageFileList().isEmpty()){
        int value = ui->currImageNumberLEdit->text().toInt();
        if (value < 0)
            value = 0;
        if (value >= scan->getBscansNumber())
            value = scan->getBscansNumber()-1;

        loadImage(value);
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

void OCTAnnotate::setLayerVisibility(bool st, QRadioButton *button, LayerName layer)
{
    int graphID = getAllLayers().indexOf(layer);

    if (st){    // get new values to display
        // bscan image
        QList<int> flatDiff;
        for (int col=0; col < scan->getBscanWidth(); col++){
            flatDiff.append(0);
        }

        QVector<double> z = scan->getLayerPointsVector(layer, currentImageNumber, false);
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

        z = scan->getLayerPointsVector(layer, currentNormalImageNumber, true);
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

void OCTAnnotate::on_ermLayerCBox_stateChanged(int state){
    setLayerVisibility(state != 0, ui->ermLayerRButton, ERM_UP);
}

void OCTAnnotate::on_ilmLayerCBox_stateChanged(int state) {
    setLayerVisibility(state != 0, ui->ilmLayerRButton, ILM);
}

void OCTAnnotate::on_gclLayerCBox_stateChanged(int state){
    setLayerVisibility(state != 0, ui->gclLayerRButton, NFL_GCL);
}

void OCTAnnotate::on_iplLayerCBox_stateChanged(int state){
    setLayerVisibility(state != 0, ui->iplLayerRButton, GCL_IPL);
}

void OCTAnnotate::on_inlLayerCBox_stateChanged(int state){
    setLayerVisibility(state != 0, ui->inlLayerRButton, IPL_INL);
}

void OCTAnnotate::on_oplLayerCBox_stateChanged(int state){
    setLayerVisibility(state != 0, ui->oplLayerRButton, INL_OPL);
}

void OCTAnnotate::on_onlLayerCBox_stateChanged(int state){
    setLayerVisibility(state != 0, ui->onlLayerRButton, OPL_ONL);
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

void OCTAnnotate::on_rpeLayerCBox_stateChanged(int state){
    setLayerVisibility(state != 0, ui->rpeLayerRButton, OS_RPE);
}

void OCTAnnotate::on_chrLayerCBox_stateChanged(int state){
    setLayerVisibility(state != 0, ui->chrLayerRButton, RPE_CHR);
}

void OCTAnnotate::on_pcvLayerRButton_clicked()
{

}

void OCTAnnotate::on_ermLayerRButton_clicked()
{

}

void OCTAnnotate::on_ilmLayerRButton_clicked()
{

}

void OCTAnnotate::on_gclLayerRButton_clicked()
{

}

void OCTAnnotate::on_iplLayerRButton_clicked()
{

}

void OCTAnnotate::on_inlLayerRButton_clicked()
{

}

void OCTAnnotate::on_oplLayerRButton_clicked()
{

}

void OCTAnnotate::on_onlLayerRButton_clicked()
{

}

void OCTAnnotate::on_elmLayerRButton_clicked()
{

}

void OCTAnnotate::on_mezLayerRButton_clicked()
{

}

void OCTAnnotate::on_iosLayerRButton_clicked()
{

}

void OCTAnnotate::on_rpeLayerRButton_clicked()
{

}

void OCTAnnotate::on_chrLayerRButton_clicked()
{

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

QColor OCTAnnotate::getLayerColor(LayerName layer)
{
    QColor color;

    switch (layer) {
    case PCV:
        color = pcvColor;
        break;
    case ERM_UP:
        color = ermColor;
        break;
//    case OB_ERM:
//        color = obermColor;
//        break;
    case ILM:
        color = ilmColor;
        break;
    case NFL_GCL:
        color = gclColor;
        break;
    case GCL_IPL:
        color = iplColor;
        break;
    case IPL_INL:
        color = inlColor;
        break;
    case INL_OPL:
        color = oplColor;
        break;
    case OPL_ONL:
        color = onlColor;
        break;
    case ELM:
        color = elmColor;
        break;
    case MEZ:
        color = mezColor;
        break;
    case IS_OS:
        color = iosColor;
        break;
//    case IB_OPR:
//        color = iboprColor;
//        break;
    case OS_RPE:
        color = rpeColor;
        break;
    case RPE_CHR:
        color = chrColor;
        break;
    default:
        break;
    }

    return color;
}

QList<LayerName> OCTAnnotate::getLayersToDisplay()
{
    QList<LayerName> layers;
    if (ui->pcvLayerCBox->isChecked()){
        layers.append(PCV);
    }
    if (ui->ermLayerCBox->isChecked()){
        layers.append(ERM_UP);
    }
//    if (ui->obermLayerCBox->isChecked()){
//        layers.append(OB_ERM);
//    }
    if (ui->ilmLayerCBox->isChecked()){
        layers.append(ILM);
    }
    if (ui->gclLayerCBox->isChecked()){
        layers.append(NFL_GCL);
    }
    if (ui->iplLayerCBox->isChecked()){
        layers.append(GCL_IPL);
    }
    if (ui->inlLayerCBox->isChecked()){
        layers.append(IPL_INL);
    }
    if (ui->oplLayerCBox->isChecked()){
        layers.append(INL_OPL);
    }
    if (ui->onlLayerCBox->isChecked()){
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
//    if (ui->iboprLayerCBox->isChecked()){
//        layers.append(IB_OPR);
//    }
    if (ui->rpeLayerCBox->isChecked()){
        layers.append(OS_RPE);
    }
    if (ui->chrLayerCBox->isChecked()){
        layers.append(RPE_CHR);
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
            QVector<double> z = scan->getLayerPointsVector(layer, currentImageNumber, false);
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
            QVector<double> z = scan->getLayerPointsVector(layer, currentNormalImageNumber, true);
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
        ui->bScanHCPlot->graph(graphID)->setPen(QPen(getLayerColor(layer)));
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
        ui->bScanVCPlot->graph(graphID)->setPen(QPen(getLayerColor(layer)));
        ui->bScanVCPlot->graph(graphID)->setLineStyle(QCPGraph::lsLine);
        ui->bScanVCPlot->graph(graphID)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,1.5));
        graphID++;
    }
}

// display statistics -----------------------------------------------------------------------------

// recalculate virtual map for selected values ----------------------------------------------------

// auto annotations -------------------------------------------------------------------------------

// read manual annotations from selected file -----------------------------------------------------

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

    scan->setIsLoaded(true);

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

    currentImageNumber = scan->getBscansNumber()/2;   // middle B-scan
    currentNormalImageNumber = scan->getBscanWidth()/2;

    // enable navigation buttons
    if (currentImageNumber < (scan->getBscansNumber()-1)){
        ui->nextImageButton->setEnabled(true);
    } else {
        ui->nextImageButton->setEnabled(false);
    }
    if (currentImageNumber > 0){
        ui->prevImageButton->setEnabled(true);
    } else {
        ui->prevImageButton->setEnabled(false);
    }
    ui->zoomInButton->setEnabled(true);

    // setup plots
    setupBScanPlots();

    on_tabWidget_currentChanged();
}

// calculate statistics for CAVRI analysis -------------------------------------------------------

// data visualization for publication purposes ---------------------------------------------------

// patients database -----------------------------------------------------------------------------

// batch processing -------------------------------------------------------------------------------

// search / filter / show data --------------------------------------------------------------------
