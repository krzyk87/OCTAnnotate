#include "readwritedata.h"
#include "calculate.h"
#include "functions.h"
#include "scan.h"

#include <QTextStream>
#include <QDir>
#include <QColor>
#include <QDebug>
#include <QMessageBox>
#include <QRegularExpression>
//#include <QtXml>

ReadWriteData::ReadWriteData(QObject *parent) : QObject(parent)
{
    pData = new PatientData();
    scan = new Scan(this);

    directives.clear();
    octDir = new QDir(".");
    octFile = new QFile();
    scanName = "";
    manualFilePath = "";
    appVersion = "";
    showMessage = true;
    dataSaveStrucure = "xml";
}

ReadWriteData::~ReadWriteData()
{
//    scan->deleteLater();
}

void ReadWriteData::process(){

    foreach (QString dir, directives) {
        if (dir == "readPatientData"){
            if(!this->readPatientData()){
                emit errorOccured("Nie można odczytać katalogu z badaniem OCT");
                break;
            }
        }
        if (dir == "readOctSequence"){
            this->readOctSequence();
            emit readingDataFinished("octData");
        }
        if (dir == "readOctFile"){
            this->readOctFile();
            emit readingDataFinished("octData");
        }
        if (dir == "readManualSegmentationData"){
            QFile manualFile(manualFilePath);
            readFileManualSegmentation(&manualFile);
            emit readingDataFinished("manualRead");
        }
        if (dir == "readAutoSegmentationData"){
            QFile autoFile(autoFilePath);
            readFileAutoSegmentation(&autoFile);
            emit readingDataFinished("autoRead");
        }
        // TODO: uncomment below lines when adding functionalities for automatic segmentation
        if (dir == "saveManualSegmentationData"){
            this->saveManualSegmentationData();
            emit savingDataFinished("manualSave");
        }
        if (dir == "saveAutoSegmentationData"){
            this->saveAutoSegmentationData();
            emit savingDataFinished("autoSave");
        }
//        if (dir == "copyAutoAsManualAll"){
//            this->copyAutoAsManual(getAllLayers());
//        }
//        if (dir == "copyAutoAsManualPCV"){
//            QList<Layers> list;
//            list.append(PCV);
//            this->copyAutoAsManual(list);
//        }
    }
    qDebug() << "Finished processing scan: " + scanName;
    emit finished();
}

void ReadWriteData::setDataObject(PatientData *patientData, Scan *tscan){
    pData = patientData;
    scan = tscan;
}

void ReadWriteData::addDirective(QString directive){
    directives.append(directive);
}

void ReadWriteData::setAppVersion(QString version)
{
    this->appVersion = version;
}

void ReadWriteData::setShowMessage(bool show){
    showMessage = show;
}

void ReadWriteData::setDataSaveStrucure(QString structure){
    dataSaveStrucure = structure;
}

void ReadWriteData::setDirectoryOct(QDir *dataDir){
    octDir = dataDir;
}

void ReadWriteData::setOctFile(QFile *dataFile){
    octFile = dataFile;
}

void ReadWriteData::setManualFilePath(QString mfPath){
    this->manualFilePath = mfPath;
}

void ReadWriteData::setAutoFilePath(QString afPath)
{
    this->autoFilePath = afPath;
}

// read patient's and exam data -------------------------------------------------------------------
bool ReadWriteData::readPatientData(){
    QString line;
    bool fileOpened = false;
    double tasks = 14;
    double count = 0;

    QString scanName;
    QString iniFilePath;
    QFile iniFile;
    QFile infoFile;
    QString infoFilePath;

    switch (scan->getOCTDevice()){
    case COPERNICUS:                // COPERNICUS <<-------------------------------
        // read info.ini file
        iniFilePath = octDir->absolutePath().append("/info.ini");
        iniFile.setFileName(iniFilePath);
        if (iniFile.open(QIODevice::ReadOnly)){
            fileOpened = true;
            emit processingData((++count)/tasks*100,"");

            QTextStream iniText(&iniFile);
            int year = 2000;
            int month = 1;
            int day = 1;
            do {
                line = iniText.readLine();
                if (line.contains("=")){
                    QStringList data = line.split("=");
                    if (data.at(0) == "last name"){  // get patient last name
                        pData->setLastName(data.at(1));
                        emit processingData((++count)/tasks*100,"");
                    }
                    if (data.at(0) == "first name"){ // get patient fist name
                        pData->setFirstName(data.at(1));
                        emit processingData((++count)/tasks*100,"");
                    }
                    if (data.at(0) == "dob_y"){  // get patient birth year
                        year = data.at(1).toInt();
                    }
                    if (data.at(0) == "dob_m"){  // get patient birth month
                        month = data.at(1).toInt();
                    }
                    if (data.at(0) == "dob_d"){  // get patient birth day
                        day = data.at(1).toInt();
                    }
                    if (data.at(0) == "gender"){
                        if (data.at(1).toInt() == 1)
                            pData->setGender(1); // male
                        else
                            pData->setGender(0); // female
                        emit processingData((++count)/tasks*100,"");
                    }
                    if (data.at(0) == "ilbskan"){
                        scan->setBscansNumber(data.at(1).toInt());
                        emit processingData((++count)/tasks*100,"");
                    }
                    if (data.at(0) == "ilaskan"){
                        scan->setBscanWidth(data.at(1).toInt());
                        scan->setBscanHeight(1010);
                        emit processingData((++count)/tasks*100,"");
                    }
                    if (data.at(0) == "eye"){
                        scan->setEye(data.at(1).toInt());
                        emit processingData((++count)/tasks*100,"");
                    }
                    if (data.at(0) == "date"){
                        scan->setExamDate(QDate::fromString(data.at(1),"yyyy-MM-dd"));
                        emit processingData((++count)/tasks*100,"");
                    }
                    if (data.at(0) == "time"){
                        scan->setExamTime(QTime::fromString(data.at(1),"hh:mm:ss"));
                        emit processingData((++count)/tasks*100,"");
                    }
                }
            } while (!line.isNull());
            pData->setBirthDate(QDate(year,month,day));
            emit processingData((++count)/tasks*100,"");
            pData->setAge(scan->getExamDate().year() - pData->getBirthDate().year());
            emit processingData((++count)/tasks*100,"");
            if (iniFile.isOpen())
                iniFile.close();
        }
        break;
    case AVANTI:                    // AVANTI <<-------------------------------
        if (!scan->getIsBinary()){    // !isBinary
            infoFilePath = octDir->absolutePath().append("/" + octDir->dirName() + ".txt");
            infoFile.setFileName(infoFilePath);
            if (!infoFile.exists()){
                infoFilePath = octDir->absolutePath().append(".txt");
                infoFile.setFileName(infoFilePath);
            }
            scanName = octDir->dirName();
        } else {    // isBinary
            infoFilePath = octFile->fileName();
            infoFilePath.chop(4);
            QFileInfo fileInfo(octFile->fileName());
            scanName = fileInfo.fileName();
            scanName.chop(4);
            infoFilePath = infoFilePath.append(".txt");
            infoFile.setFileName(infoFilePath);
        }

        if (infoFile.open(QIODevice::ReadOnly)){
            fileOpened = true;
            emit processingData((++count)/tasks*100,"");

            // Nazwisko, Imie _27_3D Retina_OS_2014-09-12_10.21.36_1
            QStringList fileName = scanName.split(",");
            if (fileName.length() > 1){
                QStringList fileName2 = fileName.at(1).split(" ");
                pData->setLastName(fileName.at(0).toUpper());
                emit processingData((++count)/tasks*100,"");
                pData->setFirstName(fileName2.at(1).toUpper());
                emit processingData((++count)/tasks*100,"");
                QStringList examCode = fileName2.at(3).split("_");
                scan->setExamDate(QDate::fromString(examCode.at(2),"yyyy-MM-dd"));
                emit processingData((++count)/tasks*100,"");
                scan->setExamTime(QTime::fromString(examCode.at(3),"hh.mm.ss"));
                emit processingData((++count)/tasks*100,"");
            } else {
                // File name does not contain exam info
                count += 4;
            }

            QTextStream infoText(&infoFile);
            bool scanLengthRead = false;
            bool scanUsageRead = false;
            bool scanWidth = false;
            bool scanHeight = false;
            do {
                line = infoText.readLine();
                if (line.contains("=")){
                    QStringList data = line.split("=");
                    if (data.at(0) == "XY Scan Length" && !scanLengthRead){
                        scan->setBscanWidth(data.at(1).toInt());
                        scanLengthRead = true;
                        emit processingData((++count)/tasks*100,"");
                    }
                    if (data.at(0) == "XY Scan Usage" && !scanUsageRead){
                        scan->setBscansNumber(data.at(1).toInt());
                        scanUsageRead = true;
                        emit processingData((++count)/tasks*100,"");
                    }
                    if (data.at(0) == "Frames Per Data Group"){
                        scan->setBscansNumberAll(data.at(1).toInt());
                        emit processingData((++count)/tasks*100,"");
                    }
                    if (data.at(0) == "OCT Window Height"){
                        scan->setBscanHeight(data.at(1).toInt());
                        emit processingData((++count)/tasks*100,"");
                    }
                    if (data.at(0) == "Scan Depth"){ //Scan Depth=2.401 mm
                        QStringList val = data.at(1).split(" ");
                        scan->setScanDepth(val.at(0).toDouble());
                        emit processingData((++count)/tasks*100,"");
                    }
                    if (data.at(0) == "XY Scan Size1" && !scanWidth){   //XY Scan Size1= 12.00 mm
                        QStringList val = data.at(1).split(" ");
                        scan->setScanWidth(val.at(1).toDouble());
                        emit processingData((++count)/tasks*100,"");
                        scanWidth = true;
                    }
                    if (data.at(0) == "XY Scan Interval1" && !scanHeight){ //XY Scan Interval1= 0.028 mm
                        QStringList val = data.at(1).split(" ");
                        QString v = QString::number(val.at(1).toDouble(),'f',3);
                        scan->setScanHeight(v.toDouble() * scan->getBscansNumber());
                        emit processingData((++count)/tasks*100,"");
                        scanHeight = true;
                    }
                    if (data.at(0) == "Eye Scanned"){
                        if (data.at(1) == "OD"){
                            scan->setEye(1);
                        } else if (data.at(1) == "OS"){
                            scan->setEye(0);
                        }
                        emit processingData((++count)/tasks*100,"");
                    }
                }
            } while (!line.isNull());
            if (infoFile.isOpen())
                infoFile.close();
        } // TODO: allow reading in scan cross sections (not binary) without info file
        break;
    }

    if (!fileOpened){
        emit errorOccured(tr("Nie odnaleziono pliku z danymi badania OCT (*.ini lub *.txt)"));
        return false;
    } else {
        emit processingData(100, "Odczytano dane badania OCT.");
        return true;
    }
}

// read OCT files and images ----------------------------------------------------------------------
void ReadWriteData::readOctSequence(){
    emit processingData(0, "Trwa pobieranie listy skanów...");
    double tasks = scan->getBscansNumber() + 2; // gdy contrast enhancement to 4 zamiast 3
    double count = 0;
    OCTDevice device = scan->getOCTDevice();
    scanName = octDir->dirName();

    // read exam images list
    QStringList imageList;
    imageList.resize(scan->getBscansNumber());
    QStringList filter;
    filter << "*.bmp" << "*.tiff" << "*.jpeg" << "*.jpg";
    QFileInfoList fileInfoList = octDir->entryInfoList(filter);
    static QRegularExpression rg("[0-9]+");
    QRegularExpressionMatch match;
    foreach (const QFileInfo &fileInfo, fileInfoList) {
        QString imageFileName = fileInfo.fileName();
        match = rg.match(imageFileName);
        if (match.hasMatch()){
            int imageNumber = match.captured(0).toInt();
//            qDebug() << imageFileName << ": " << QString::number(imageNumber);
            if (imageNumber <= scan->getBscansNumber())
                imageList[imageNumber-1] = fileInfo.absoluteFilePath();
            else
                qDebug() << "image number out of range -> image " << QString::number(imageNumber) << " not loaded!";
        }
    }
    scan->resetBscansData();  // Bscans memory reset

    emit processingData((count)/tasks*100,"Trwa odczyt danych OCT...");
    int imageNumber = 0;
    foreach (QString imagePath, imageList) {
        QImage img(imagePath);
        if (img.format() != QImage::Format_Indexed8)
            img = img.convertToFormat(QImage::Format_Indexed8);
        scan->setOCTdata(img, imageNumber);
        imageNumber++;
        emit processingData((++count)/tasks*100,"Trwa odczyt danych OCT...");
    }

    // read fundus image
    emit processingData((++count)/tasks*100, "Trwa odczyt obrazu fundus...");
    readFundusImage(device);

    // read oct exam data if exists
//    QFile octExamFile(manualFilePath);
//    readFileManualSegmentation(&octExamFile);
}

void ReadWriteData::readOctFile(){
    emit processingData(0, "Trwa pobieranie listy skanów...");
    double tasks = scan->getBscansNumber()*2 + 1; // gdy contrast enhancement to 5 zamiast 4
    double count = 0;
    OCTDevice device = scan->getOCTDevice();

    QFileInfo octFileInfo(*octFile);
    scanName = octFileInfo.fileName();
    scanName.chop(4);

    scan->resetBscansData();  // Bscans memory reset

    // read OCT file
    emit processingData((count)/tasks*100,"Trwa odczyt danych OCT...");
    readBinaryFile(octFile, &count, &tasks);

    // read fundus image
    emit processingData(++count/tasks*100, "Trwa odczyt obrazu fundus...");
    readFundusImage(device);

    // read oct exam data if exists
//    QFile octExamFile(manualFilePath);
//    readFileManualSegmentation(&octExamFile);
}

void ReadWriteData::readBinaryFile(QFile *dataFile, double *count, double *tasks){
    double c = *count;
    double t = *tasks;

    QList< QList< QList<float> > > octDataTemp;
    QList<float> maxList;
//    int maxAll = 0;

    if (!dataFile->open(QIODevice::ReadOnly)){
        qDebug() << "Could not open file!";
        emit errorOccured(tr("Could not open OCT binary file!"));
    } else {
        qDebug() << "File opened!";
    }

    QDataStream in(dataFile);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);
    in.setByteOrder(QDataStream::LittleEndian);

    for (int p=0; p < scan->getBscansNumber(); p++){
        QList< QList<float> > img;
        float max = 0;

        for (int i=0; i < scan->getBscanWidth(); i++){
            QList<float> column;
            for (int j=0; j < scan->getBscanHeight(); j++){
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
        emit processingData((++c)/t*100,"Reading in binary data...");
    }

    dataFile->close();
    qDebug() << "File read sucessfully!";

    // normalize data
    float min = 800;
    for (int p=0; p < scan->getBscansNumber(); p++){
        QList< QList<int> > img;

        for (int w=0; w < scan->getBscanWidth(); w++){
            QList<int> column;

            for (int r=scan->getBscanHeight()-1; r >= 0; r--){
                float tmp = octDataTemp[p][w][r];
                tmp = (tmp-min)/(maxList[p]-min);
                tmp = qBound(0,(int)(tmp*255),255);
                column.append(tmp);
            }
            img.append(column);
        }
        scan->setOCTdata(img, p);
        emit processingData((++c)/t*100,"Reading in binary data... (normalization)");
    }

    *count = c;
    *tasks = t;
}

void ReadWriteData::readFundusImage(OCTDevice octDevice){
    QImage fundus = QImage(scan->getBscanWidth(), scan->getBscansNumber(), QImage::Format_Indexed8);
    fundus.fill(0);

    QString fundusFilePath = octDir->absolutePath().append("/fnds_rec.bmp");    // fundus image in the sequence directory
    QString fundusFilePathExpl = octDir->absolutePath().append("/../iowa/" + octDir->dirName() + "_Proj_Iowa.tif"); // fundus image form iowa directory
    if (QFile(fundusFilePath).exists()){
        fundus = QImage(fundusFilePath);
    } else if (QFile(fundusFilePathExpl).exists()){
        fundus = QImage(fundusFilePathExpl);
    } else {
        emit processingData(98, "Trwa tworzenie projekcji fundus z danych OCT...");
        Calculate *calc = new Calculate();
        fundus = calc->calculateFundus(scan->getOCTdata());
    }

    if (octDevice == COPERNICUS){
        fundus = fundus.mirrored(false,true);
    } else if (octDevice == AVANTI){
        emit processingData(99, "Trwa poprawa kontrastu obrazu fundus...");
        Calculate *calc = new Calculate();
        calc->imageEnhancement(&fundus, 1.0, 0);
    }
    scan->setFundusImage(fundus);
}

// read annotation data ---------------------------------------------------------------------------
void ReadWriteData::readFileManualSegmentation(QFile *dataFile)
{
    QString line;
    QList<LayerName> allLayers = getAllLayers();
    int layersCount = allLayers.count();
    double tasks = 1 + layersCount*scan->getBscansNumber() + layersCount;
    double count = 0;
    emit processingData(0, "Trwa odczyt danych ręcznej segmentacji...");
    scanName = QFileInfo(dataFile->fileName()).completeBaseName();

    if (!dataFile->exists() || !dataFile->open(QIODevice::ReadWrite)){
        emit errorOccured(tr("Nie można otworzyć pliku z ręczną segmentacją warstw: ") + dataFile->fileName());
    } else {
        scan->resetManualAnnotations();

        QTextStream octSegmentText(dataFile);
        line = octSegmentText.readLine();

        if (line.contains("<?")){   // read as xml
            dataFile->reset();
            int bn = scan->getBscansNumber();
            QString version = "";
            QList<int> voxelSize;
            QXmlStreamReader xml(dataFile);
            xml.readNextStartElement();
            while (!xml.atEnd() && !xml.hasError()){
                QXmlStreamReader::TokenType token = xml.readNext();
                if (token == QXmlStreamReader::StartElement){
                    if (xml.name().toString() == "scan_characteristics"){
                        voxelSize = parseXmlVoxelSize(xml);
                        emit processingData((++count)/tasks*100,"");
                    } else if (xml.name().toString() == "executable"){
                        version = parseXmlExeVersion(xml);
                    } else if (xml.name().toString() == "surface"){
                        parseXmlSurfaceLines(xml, version);
                        count += bn;
                        emit processingData((count)/tasks*100,"");
                    } else if (xml.name().toString() == "undefined_region"){
                        parseUndefinedRegion(xml);
                        emit processingData((++count)/tasks*100,"");
                    } else {
                        continue;
                    }
                }
            }
        } else {
            if (line.contains("=")){    // read as txt
                QStringList data = line.split("=");
                if ((data.at(0) == "scanCenter") && (data.at(1) != "")){
                    QStringList point = data.at(1).split(",");
                    QPoint center = QPoint(point.at(0).toInt(), point.at(1).toInt());
                    if (center.x() == -1){
                        if (scan->getOCTDevice() == COPERNICUS)
                            center.setX(scan->getBscanWidth() / 2);
                        else if (scan->getOCTDevice() == AVANTI)
                            center.setX(scan->getBscanWidth() / 2);
                    }
                    if (center.y() == -1)
                        center.setY(scan->getBscansNumber() / 2);
                    scan->setScanCenter(center);
                    emit processingData((++count)/tasks*100,"");
                }
                do {
                    line = octSegmentText.readLine();
                    if (line.contains("=")){
                        QStringList data = line.split("=");
                        if (data.at(1) != ""){
                            QStringList code = data.at(0).split("_");   // PCV_i...
                            LayerName layer = decodeLayer(code.at(0));
                            int bscanNumber = code.at(1).toInt();

                            QStringList points = data.at(1).split(";");
                            for (int j=0; j < points.count(); j++){
                                QString p_str = points.at(j);
                                if (p_str != ""){
                                    scan->setPoint(layer, bscanNumber, p_str.split(",").at(0).toInt(), p_str.split(",").at(1).toInt());
                                }
                            }
                            emit processingData((++count)/tasks*100,"");
                        }
                    }
                } while(!line.isNull());
            }
        }

//        // check if no holes in the dataset
//        emit processingData(count, "Trwa wygładzanie warstw...");
//        QList<Layers> layers = getAllLayers();
//        foreach (Layers layer, layers) {
//            scan->smoothLayer(layer);
//            emit processingData((++count)/tasks*100,"");
//        }
//        emit processingData(count, "Trwa uzupełnianie danych...");
//        for (int i=0; i < scan->getBscansNumber(); i++){
//            scan->fillContactArea(i);
//            emit processingData((++count)/tasks*100,"");
//        }
//        emit processingData(count, "Trwa obliczanie współczynników wypłaszczania warstw...");
//        for (int i=0; i < scan->getBscansNumber(); i++){
//            scan->calculateFlatDifferencesRPE(i);
//            emit processingData((++count)/tasks*100,"");
//        }
    }

    if (dataFile->isOpen())
        dataFile->close();
}

void ReadWriteData::readFileAutoSegmentation(QFile *dataFile)
{
    double tasks = 12*scan->getBscansNumber();
    double count = 0;
    emit processingData(0, "Reading in auto segmentation data...");
    scanName = QFileInfo(dataFile->fileName()).baseName();

    if (!dataFile->open(QIODevice::ReadOnly)){
        emit errorOccured(tr("Could not open file with auto layers segmentations: ") + dataFile->fileName());
        qDebug() << "Auto segmentation data NOT loaded!";
        return;
    } else {
        scan->resetAutoAnnotations();
        QString line;
        int x,z;
        QString version = "";

        QFileInfo autoSegmentFileInfo(*dataFile);
        if (autoSegmentFileInfo.suffix() == "txt"){
            tasks *= scan->getBscanWidth();
            QTextStream autoSegmentText(dataFile);
            do {
                line = autoSegmentText.readLine();
                if (line.contains("=")){
                    QStringList data = line.split("=");

                    if (data.at(0) == "leftCut"){
//                        startPoint = data.at(1).toInt() - 1;
                    } else if (data.at(0) == "rightCut"){
//                        endPoint = pData->getBscanWidth() - data.at(1).toInt();
                    } else {
                        QStringList code = data.at(0).split("_");   // PCV_i...
                        LayerName layer = decodeLayer(code.at(0));
                        int bscanNumber = code.at(1).toInt();
                        if (bscanNumber >= scan->getBscansNumber()){
                            emit errorOccured("there are more auto segmentations for B-scans than images in the exam");
                            return;
                        }

                        QStringList points = data.at(1).split(";");
                        for (int j=0; j < points.count(); j++){
                            QString p_str = points.at(j);
                            if (p_str != ""){
                                x = p_str.split(",").at(0).toInt()-1;
                                z = p_str.split(",").at(1).toInt();
                                if (x >= scan->getBscanWidth()){
                                    emit errorOccured("x index for auto segmentation is bigger than the image width");
                                } else {
                                    scan->setPointAuto(layer,bscanNumber,x,z);
                                    emit processingData((++count)/tasks*100,"");
                                }
                            }
                        }
                    }
                }
            } while (!line.isNull());
        } else if (autoSegmentFileInfo.suffix() == "xml"){
            tasks = 1 + 12 + 1;
            QList<int> voxelSize;
            QXmlStreamReader xml(dataFile);
            xml.readNextStartElement();
            while (!xml.atEnd() && !xml.hasError()){
                QXmlStreamReader::TokenType token = xml.readNext();
                if (token == QXmlStreamReader::StartElement){
                    if (xml.name().toString() == "scan_characteristics"){
                        voxelSize = parseXmlVoxelSize(xml, 1);
                        emit processingData((++count)/tasks*100,"");
                    } else if (xml.name().toString() == "executable"){
                        version = parseXmlExeVersion(xml);
                    } else if (xml.name().toString() == "surface"){
                        parseXmlSurfaceLines(xml, version, 1);
                        emit processingData((++count)/tasks*100,"");
                    } else if (xml.name().toString() == "undefined_region"){
                        parseUndefinedRegion(xml, 1);
                        emit processingData((++count)/tasks*100,"");
                    } else {
                        continue;
                    }
                }
            }
        } else if (autoSegmentFileInfo.suffix() == "json"){
            tasks = 3*scan->getBscanWidth();
            QString allText = dataFile->readAll();
            qDebug() << "json text loaded";
            QJsonDocument doc = QJsonDocument::fromJson(allText.toUtf8());
            QJsonObject obj = doc.object();
            QStringList layerList = obj.keys();
            for (int layerNo = 0; layerNo < layerList.size(); layerNo++) {
                qDebug() << "Layers in file: " << layerList.at(layerNo);
                QString layerName = layerList.at(layerNo);
                LayerName layer = decodeLayer(layerName);
                QJsonArray layerArray = obj.value(layerName).toArray();
                for (int bscanNumber=0; bscanNumber < scan->getBscansNumber(); bscanNumber++) {
                    if (layerArray.size() > bscanNumber){
                        QJsonArray lineArray = layerArray[bscanNumber].toArray();
                        for (int x=1; x < scan->getBscanWidth(); x++) {
                            if (lineArray.size() > (x-1)){
                                scan->setPointAuto(layer, bscanNumber, x, lineArray[x-1].toInt());
                            } else {
                                qDebug() << "More points in B-scan that elements in line array " << layerName;
                            }
                        }
                    } else {
                        qDebug() << "More cross-sections than arrays in the layer " << layerName;
                    }
                    emit processingData((++count)/tasks*100,"");
                }
            }
        } else {
            emit errorOccured("Unsupported file type!");
        }
    }

    if (dataFile->isOpen())
        dataFile->close();

    qDebug() << "Auto segmentation data loaded";
    emit processingData(100, "Auto segmentation data loaded.");
}

QList<int> ReadWriteData::parseXmlVoxelSize(QXmlStreamReader &xml, bool isAuto)
{
    QList<int> voxel;
    voxel.append(0);
    voxel.append(0);
    voxel.append(0);
    QPoint center(-1,-1);

    xml.readNext();
    while (!((xml.tokenType() == QXmlStreamReader::EndElement) && (xml.name().toString() == "size"))){
        if (xml.tokenType() == QXmlStreamReader::StartElement){
            if (xml.name().toString() == "x"){
                xml.readNext();
                voxel[0] = xml.text().toInt();
            }
            if (xml.name().toString() == "y"){
                xml.readNext();
                voxel[1] = xml.text().toInt();
            }
            if (xml.name().toString() == "z"){
                xml.readNext();
                voxel[2] = xml.text().toInt();
            }
        }
        xml.readNext();
    }

    if (!isAuto){
        while(!(xml.tokenType() == QXmlStreamReader::StartElement && (xml.name().toString() == "scan_center"))){
            xml.readNext();
        }
        while(!(xml.tokenType() == QXmlStreamReader::EndElement && (xml.name().toString() == "scan_center"))){
            if (xml.tokenType() == QXmlStreamReader::StartElement){
                if (xml.name().toString() == "x"){
                    xml.readNext();
                    center.setX(xml.text().toInt());
                }
                if (xml.name().toString() == "y"){
                    xml.readNext();
                    center.setY(xml.text().toInt());
                }
            }
            xml.readNext();
        }
        scan->setScanCenter(center);
    }

    return voxel;
}

QString ReadWriteData::parseXmlExeVersion(QXmlStreamReader &xml)
{
    // TODO: use QVersion variable
    QString appName = "";

    xml.readNext();
    while (!(xml.tokenType() == QXmlStreamReader::StartElement && (xml.name().toString() == "name"))) {
        xml.readNext();
    }
    xml.readNext();
    appName = xml.text().toString();

    xml.readNext();
    while(!(xml.tokenType() == QXmlStreamReader::StartElement && (xml.name().toString() == "version"))){
        xml.readNext();
    }
    xml.readNext();
    QString verStr = xml.text().toString();
    verStr = verStr.remove(1,1);
    verStr = verStr.remove(2,1);

    return (appName + "_" + verStr);
}

void ReadWriteData::parseXmlSurfaceLines(QXmlStreamReader &xml, QString versionName, bool isAuto)
{
    int counter = 0;
    int lineNumber = 0;
    int scanNumber = 0; // y
    LayerName layer = NONE;
    if (versionName.isEmpty())
        versionName = "OCTAnnotate_175";
    QStringList versionList = versionName.split("_");
    QString versionApp = versionList.at(0);
    int versionNr = versionList.at(1).toInt();

    xml.readNext();
    while (!(xml.tokenType() == QXmlStreamReader::EndElement && (xml.name().toString() == "surface"))){
     if (xml.tokenType() == QXmlStreamReader::StartElement){
         if (xml.name().toString() == "label"){
             xml.readNext();
             lineNumber = xml.text().toInt();
             if (((versionApp == "OCTLayerSeg") && versionNr >= 380) || ((versionApp == "OCTAnnotate") && (versionNr >= 181))){
                 switch (lineNumber) {
                 case 5:
                     layer = PCV;
                     break;
                 case 6:
                     layer = IB_ERM;
                     break;
                 case 9:
                     layer = OB_ERM;
                     break;
                 case 10:
                     layer = ILM;
                     break;
                 case 20:
                     layer = RNFL_GCL;
                     break;
                 case 30:
                     layer = GCL_IPL;
                     break;
                 case 40:
                     layer = IPL_INL;
                     break;
                 case 50:
                     layer = INL_OPL;
                     break;
                 case 60:
                     layer = OPL_ONL;
                     break;
                 case 70:
                     layer = ELM;
                     break;
                 case 100:
                     layer = MEZ;
                     break;
                 case 110:
                     layer = IS_OS;
                     break;
                 case 120:
                     layer = IB_OPR;
                     break;
                 case 140:
                     layer = IB_RPE;    // IB_RPE
                     break;
                 case 150:
                     layer = OB_RPE;   // OB_RPE
                     break;
                 default:
                     layer = NONE;
                     break;
                 }
             } else if ((versionApp == "OCTAnnotate") && (versionNr == 180)){
                 switch (lineNumber) {
                 case -1:
                     layer = PCV;
                     break;
                 case 0:
                     layer = IB_ERM;
                     break;
                 case 1:
                     layer = OB_ERM;
                     break;
                 case 2:
                     layer = ILM;
                     break;
                 case 3:
                     layer = RNFL_GCL;
                     break;
                 case 4:
                     layer = GCL_IPL;
                     break;
                 case 5:
                     layer = IPL_INL;
                     break;
                 case 6:
                     layer = INL_OPL;
                     break;
                 case 7:
                     layer = OPL_ONL;
                     break;
                 case 8:
                     layer = ELM;
                     break;
                 case 10:
                     layer = MEZ;
                     break;
                 case 11:
                     layer = IS_OS;
                     break;
                 case 13:
                     layer = IB_OPR;
                     break;
                 case 14:
                     layer = IB_RPE;    //
                     break;
                 case 15:
                     layer = OB_RPE;   //
                     break;
                 default:
                     layer = NONE;
                     break;
                 }
             } else {   // up to OCTLayerSeg 3.6, OCTAnnotate 1.7 or lower
                 switch (lineNumber) {
                 case -1:
                     layer = PCV;
                     break;
                 case 0:
                     layer = IB_ERM;    //
                     break;
                 case 1:
                     layer = ILM;
                     break;
                 case 2:
                     layer = RNFL_GCL;
                     break;
                 case 3:
                     layer = GCL_IPL;
                     break;
                 case 4:
                     layer = IPL_INL;
                     break;
                 case 5:
                     layer = INL_OPL;
                     break;
                 case 6:
                     layer = OPL_ONL;
                     break;
                 case 7:
                     layer = ELM;
                     break;
                 case 10:
                     layer = MEZ;
                     break;
                 case 11:
                     layer = IS_OS;
                     break;
                 case 12:
                     layer = IB_OPR;
                     break;
                 case 13:
                     layer = IB_RPE;
                     break;
                 case 15:
                     layer = OB_RPE;
                     break;
                 default:
                     layer = NONE;
                     break;
                 }
             }
         }
         if ((xml.name().toString() == "bscan") && (layer != NONE)){
             xml.readNext();
             counter = 0;
             while (!(xml.tokenType() == QXmlStreamReader::EndElement && (xml.name().toString() == "bscan"))){
                 if (xml.tokenType() == QXmlStreamReader::StartElement){
                     if ((xml.name().toString() == "z") || (xml.name().toString() == "y")){
                         xml.readNext();
                         if (isAuto){
                             scan->setPointAuto(layer, scanNumber, counter, xml.text().toInt()+1);
                         } else {
                             scan->setPoint(layer, scanNumber, counter, xml.text().toInt());
                         }
                         counter++;
                     }
                 }
                 xml.readNext();
             }
             scanNumber++;
    //                emit processingData((scanNumber)/pData->getBscansNumber()*100,"");
         }
     }
     xml.readNext();
    }
}

void ReadWriteData::parseUndefinedRegion(QXmlStreamReader &xml, bool isAuto)
{
    int x,y;
    QList<LayerName> allLayers = getAllLayers();

    xml.readNext();
    while (!(xml.tokenType() == QXmlStreamReader::EndElement && (xml.name().toString() == "undefined_region"))){
        if (xml.tokenType() == QXmlStreamReader::StartElement){
            if (xml.name().toString() == "ascan"){
                xml.readNext();
                x = -1;
                y = -1;
                while (!(xml.tokenType() == QXmlStreamReader::EndElement && (xml.name().toString() == "ascan"))){
                    if (xml.tokenType() == QXmlStreamReader::StartElement){
                        if (xml.name().toString() == "x"){
                            xml.readNext();
                            x = xml.text().toInt();
                        }
                        if (xml.name().toString() == "y"){
                            xml.readNext();
                            y = xml.text().toInt();
                        }
                    }
                    if (x != -1 && y != -1){
                        foreach (LayerName layer, allLayers) {
                            if (isAuto){
                                scan->setPointAuto(layer, y, x, -1);
                            } else {
                                scan->setPoint(layer, y, x, -1);
                            }
                        }
                        x = -1;
                        y = -1;
                    }
                    xml.readNext();
                }
            }
        }
        xml.readNext();
    }
}

// save patient's and exam data -------------------------------------------------------------------
void ReadWriteData::saveManualSegmentationData()
{
    emit processingData(0, "Saving OCT manual segmentations...");
    double tasks = 1 + 12*scan->getBscansNumber();
    double count = 0;
    QList<LayerName> layers = getAllLayers();

//    // check if no holes in the dataset
//    foreach (LayerName layer, layers) {
//        scan->smoothLayer(layer);
//        emit processingData((++count)/tasks*100,"");
//    }

    // save to file
    QFile manualSegmentFile(manualFilePath);

    if (!manualSegmentFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)){
        emit errorOccured(tr("Could not access file with manual layers segmentations"));
        return;
    } else {
        if (dataSaveStrucure == "txt"){
            QTextStream stream(&manualSegmentFile);
            QPoint center = scan->getScanCenter();
            stream << "scanCenter=" + QString::number(center.x()) + "," + QString::number(center.y()) << Qt::endl;
            emit processingData((++count)/tasks*100,"");

            QList<QVector3D> pointsList;
            // PCV_0=0,0;1,1;2,2;5,5;7,7;9,9;
            // others...
            foreach (LayerName layer, layers) {
                QString sLayer = encodeLayer(layer);
                for (int i=0; i < scan->getBscansNumber(); i++){ //(i = B-scan number)
                    stream << sLayer + "_" + QString::number(i) + "=";
                    pointsList = scan->getLayerPoints(layer, i, 0, scan->getBscanWidth()-1, false);
                    foreach (QVector3D p, pointsList) {
                        stream << p.x() << "," << p.z() << ";";
                    }
                    stream << Qt::endl;
                    emit processingData((++count)/tasks*100,"");
                }
            }
        } else if (dataSaveStrucure == "xml"){
            QXmlStreamWriter xmlWriter(&manualSegmentFile);
            xmlWriter.setAutoFormatting(true);
            xmlWriter.writeStartDocument();
            xmlWriter.writeStartElement("surfaces");

            // executable
            xmlWriter.writeStartElement("executable");
                xmlWriter.writeTextElement("name", "OCTAnnotate");
                xmlWriter.writeTextElement("version", this->appVersion);
                xmlWriter.writeTextElement("date", QDate::currentDate().toString("dd/MM/yyyy"));
                xmlWriter.writeTextElement("authors", "Agnieszka Stankiewicz");
                xmlWriter.writeTextElement("institute", "Poznan University of Technology");
                xmlWriter.writeTextElement("comment", "For investigational use only");
            xmlWriter.writeEndElement();

            // scan_characteristics
            xmlWriter.writeStartElement("scan_characteristics");
                // manufacturer
                if (scan->getOCTDevice() == COPERNICUS)
                    xmlWriter.writeTextElement("manufacturer", "Optopol");
                else if (scan->getOCTDevice() == AVANTI)
                    xmlWriter.writeTextElement("manufacturer", "Optovue");
                // size
                xmlWriter.writeStartElement("size");
                xmlWriter.writeTextElement("unit", "voxel");
                xmlWriter.writeTextElement("x", QString::number(scan->getBscanWidth()));
                xmlWriter.writeTextElement("y", QString::number(scan->getBscansNumber()));
                xmlWriter.writeTextElement("z", QString::number(scan->getBscanHeight()));
                xmlWriter.writeEndElement();    // end of size
                // voxel_size
                xmlWriter.writeStartElement("voxel_size");
                xmlWriter.writeTextElement("unit", "um");
                xmlWriter.writeTextElement("x", QString::number(scan->getScanWidth()*1000/scan->getBscanWidth()));
                xmlWriter.writeTextElement("y", QString::number(scan->getScanHeight()*1000/scan->getBscansNumber()));
                xmlWriter.writeTextElement("z", QString::number(scan->getScanDepth()*1000/scan->getBscanHeight()));
                xmlWriter.writeEndElement();    // end of voxel_size
                // center_type
                xmlWriter.writeTextElement("center_type", "macula");
                // scan_center
                xmlWriter.writeStartElement("scan_center");
                xmlWriter.writeTextElement("x", QString::number(scan->getScanCenter().x()));
                xmlWriter.writeTextElement("y", QString::number(scan->getScanCenter().y()));
                xmlWriter.writeTextElement("z", "0");
                xmlWriter.writeEndElement();    // end of scan_center
            xmlWriter.writeEndElement();    // end of scan_characteristics
            emit processingData((++count)/tasks*100,"");

            // unit
            xmlWriter.writeTextElement("unit", "voxel");
            // surface_size
            xmlWriter.writeStartElement("surface_size");
                xmlWriter.writeTextElement("x", QString::number(scan->getBscanWidth()));
                xmlWriter.writeTextElement("y", QString::number(scan->getBscansNumber()));
            xmlWriter.writeEndElement();    // end of surface_size
            // surface_num
            xmlWriter.writeTextElement("surface_num", QString::number(layers.length())); // number of segmented layers

            // for each layer:
            foreach (LayerName layer, layers) {
                xmlWriter.writeStartElement("surface");
                int label;
                QString name = "";
                switch (layer) {
                case PCV:
                    label = 5;
                    name = "PCV (PCV)";
                    break;
                case IB_ERM:
                    label = 6;
                    name = "Inner boundary of ERM (IB_ERM)";
                    break;
                case OB_ERM:
                    label = 9;
                    name = "Outer boundary of ERM (OB_ERM)";
                    break;
                case ILM:
                    label = 10;
                    name = "ILM (ILM)";
                    break;
                case RNFL_GCL:
                    label = 20;
                    name = "RNFL-GCL (RNFL-GCL)";
                    break;
                case GCL_IPL:
                    label = 30;
                    name = "GCL-IPL (GCL-IPL)";
                    break;
                case IPL_INL:
                    label = 40;
                    name = "IPL-INL (IPL-INL)";
                    break;
                case INL_OPL:
                    label = 50;
                    name = "INL-OPL (INL-OPL)";
                    break;
                case OPL_ONL:
                    label = 60;
                    name = "OPL-Henles fiber layer (OPL-HFL)";
                    break;
                case ELM:
                    label = 70;
                    name = "ELM (ELM)";
                    break;
                case MEZ:
                    label = 100;
                    name = "Boundary of myoid and ellipsoid of inner segments (BMEIS)";
                    break;
                case IS_OS:
                    label = 110;
                    name = "IS/OS junction (IS/OSJ)";
                    break;
                case IB_OPR:
                    label = 120;
                    name = "Inner boudary of OPR (IB_OPR)";
                    break;
                case IB_RPE:
                    label = 140;
                    name = "Inner boudary of RPE (IB_RPE)";
                    break;
                case OB_RPE:
                    label = 150;
                    name = "Outer boudary of RPE (OB_RPE)";
                    break;
                default:
                    label = 0;
                    name = "None";
                    break;
                }
                xmlWriter.writeTextElement("label", QString::number(label));    // layer index
                xmlWriter.writeTextElement("name", name);       // layer name

                // for each B-scan:
                for (int b_nr=0; b_nr < scan->getBscansNumber(); b_nr++){
                    xmlWriter.writeStartElement("bscan");
                    // for each point:
                    for (int px=0; px < scan->getBscanWidth(); px++){
                        xmlWriter.writeTextElement("z", QString::number(scan->getLayerPoint(layer, b_nr, px)));    // layer, bscan number, x
                    }
                    xmlWriter.writeEndElement();    // end of bscan
                    emit processingData((++count)/tasks*100,"");
                }
                xmlWriter.writeEndElement();    // end of surface
            }

            // undefined_region
            xmlWriter.writeStartElement("undefined_region");
            xmlWriter.writeEndElement();    // end of undefined_region

            xmlWriter.writeEndElement();    // end of surfaces
            xmlWriter.writeEndDocument();

            qDebug() << "Finished writng data to file!";
        }
    }

    if (manualSegmentFile.isOpen())
        manualSegmentFile.close();
    emit processingData(100, "Manual segmentation data saved.");
}

void ReadWriteData::saveAutoSegmentationData()
{
    emit processingData(0, "Saving OCT auto segmentation data...");
    double tasks = 12*scan->getBscansNumber();
    double count = 0;
    QList<LayerName> layers = getAllLayers();

    QFile autoSegmentFile(autoFilePath);

    if (!autoSegmentFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        emit errorOccured(tr("Could not access file with auto layers segmentations"));
        return;
    } else {
        dataSaveStrucure = QFileInfo(autoFilePath).suffix();
        if (dataSaveStrucure == "txt"){
            QTextStream autoSegmentText(&autoSegmentFile);
            QList<QVector3D> pointsList;

            foreach (LayerName layer, layers){
                QString sLayer = encodeLayer(layer);
                for (int i=0; i < scan->getBscansNumber(); i++){ //(i = B-scan number)
                    autoSegmentText << sLayer + "_" + QString::number(i) + "=";
                    pointsList = scan->getLayerPointsAuto(layer, i, 0, scan->getBscanWidth()-1, false);
                    foreach (QVector3D p, pointsList) {
                        autoSegmentText << p.x() << "," << p.z() << ";";
                    }
                    autoSegmentText << Qt::endl;
                    emit processingData((++count)/tasks*100,"");
                }
            }
        } else if (dataSaveStrucure == "xml"){
            QXmlStreamWriter xmlWriter(&autoSegmentFile);
            xmlWriter.setAutoFormatting(true);
            xmlWriter.writeStartDocument();
            xmlWriter.writeStartElement("surfaces");

            // executable
            xmlWriter.writeStartElement("executable");
                xmlWriter.writeTextElement("name", "OCTAnnotate");
                xmlWriter.writeTextElement("version", this->appVersion);
                xmlWriter.writeTextElement("date", QDate::currentDate().toString("dd/MM/yyyy"));
                xmlWriter.writeTextElement("authors", "Agnieszka Stankiewicz");
                xmlWriter.writeTextElement("institute", "Poznan University of Technology");
                xmlWriter.writeTextElement("comment", "For investigational use only");
            xmlWriter.writeEndElement();

            // scan_characteristics
            xmlWriter.writeStartElement("scan_characteristics");
                // manufacturer
                if (scan->getOCTDevice() == COPERNICUS)
                    xmlWriter.writeTextElement("manufacturer", "Optopol");
                else if (scan->getOCTDevice() == AVANTI)
                    xmlWriter.writeTextElement("manufacturer", "Optovue");
                // size
                xmlWriter.writeStartElement("size");
                xmlWriter.writeTextElement("unit", "voxel");
                xmlWriter.writeTextElement("x", QString::number(scan->getBscanWidth()));
                xmlWriter.writeTextElement("y", QString::number(scan->getBscansNumber()));
                xmlWriter.writeTextElement("z", QString::number(scan->getBscanHeight()));
                xmlWriter.writeEndElement();    // end of size
                // voxel_size
                xmlWriter.writeStartElement("voxel_size");
                xmlWriter.writeTextElement("unit", "um");
                xmlWriter.writeTextElement("x", QString::number(scan->getScanWidth()*1000/scan->getBscanWidth()));
                xmlWriter.writeTextElement("y", QString::number(scan->getScanHeight()*1000/scan->getBscansNumber()));
                xmlWriter.writeTextElement("z", QString::number(scan->getScanDepth()*1000/scan->getBscanHeight()));
                xmlWriter.writeEndElement();    // end of voxel_size
                // center_type
                xmlWriter.writeTextElement("center_type", "macula");
                // scan_center
                xmlWriter.writeStartElement("scan_center");
                xmlWriter.writeTextElement("x", QString::number(scan->getScanCenter().x()));
                xmlWriter.writeTextElement("y", QString::number(scan->getScanCenter().y()));
                xmlWriter.writeTextElement("z", "0");
                xmlWriter.writeEndElement();    // end of scan_center
            xmlWriter.writeEndElement();    // end of scan_characteristics

            // unit
            xmlWriter.writeTextElement("unit", "voxel");
            // surface_size
            xmlWriter.writeStartElement("surface_size");
                xmlWriter.writeTextElement("x", QString::number(scan->getBscanWidth()));
                xmlWriter.writeTextElement("y", QString::number(scan->getBscansNumber()));
            xmlWriter.writeEndElement();    // end of surface_size
            // surface_num
            xmlWriter.writeTextElement("surface_num", QString::number(layers.length())); // number of segmented layers

            // for each layer:
            foreach (LayerName layer, layers) {
                xmlWriter.writeStartElement("surface");
                int label;
                QString name = "";
                switch (layer) {
                case PCV:
                    label = 5;
                    name = "PCV (PCV)";
                    break;
                case IB_ERM:
                    label = 6;
                    name = "Inner boundary of ERM (IB_ERM)";
                    break;
                case OB_ERM:
                    label = 9;
                    name = "Outer boundary of ERM (OB_ERM)";
                    break;
                case ILM:
                    label = 10;
                    name = "ILM (ILM)";
                    break;
                case RNFL_GCL:
                    label = 20;
                    name = "RNFL-GCL (RNFL-GCL)";
                    break;
                case GCL_IPL:
                    label = 30;
                    name = "GCL-IPL (GCL-IPL)";
                    break;
                case IPL_INL:
                    label = 40;
                    name = "IPL-INL (IPL-INL)";
                    break;
                case INL_OPL:
                    label = 50;
                    name = "INL-OPL (INL-OPL)";
                    break;
                case OPL_ONL:
                    label = 60;
                    name = "OPL-Henles fiber layer (OPL-HFL)";
                    break;
                case ELM:
                    label = 70;
                    name = "ELM (ELM)";
                    break;
                case MEZ:
                    label = 100;
                    name = "Boundary of myoid and ellipsoid of inner segments (BMEIS)";
                    break;
                case IS_OS:
                    label = 110;
                    name = "IS/OS junction (IS/OSJ)";
                    break;
                case IB_OPR:
                    label = 120;
                    name = "Inner boudary of OPR (IB_OPR)";
                    break;
                case IB_RPE:
                    label = 140;
                    name = "Inner boudary of RPE (IB_RPE)";
                    break;
                case OB_RPE:
                    label = 150;
                    name = "Outer boudary of RPE (OB_RPE)";
                    break;
                default:
                    label = 0;
                    name = "None";
                    break;
                }
                xmlWriter.writeTextElement("label", QString::number(label));    // layer index
                xmlWriter.writeTextElement("name", name);       // layer name

                // for each B-scan:
                for (int b_nr=0; b_nr < scan->getBscansNumber(); b_nr++){
                    xmlWriter.writeStartElement("bscan");
                    // for each point:
                    for (int px=0; px < scan->getBscanWidth(); px++){
                        xmlWriter.writeTextElement("z", QString::number(scan->getLayerPointAuto(layer,b_nr,px)));    // layer, bscan number, x
                    }
                    xmlWriter.writeEndElement();    // end of bscan
                }
                xmlWriter.writeEndElement();    // end of surface
            }

            // undefined_region
            xmlWriter.writeStartElement("undefined_region");
            xmlWriter.writeEndElement();    // end of undefined_region

            xmlWriter.writeEndElement();    // end of surfaces
            xmlWriter.writeEndDocument();

            qDebug() << "Finished writing data to file!";
        } else if (dataSaveStrucure == "json"){
            QJsonObject segmentations; // <-- main object
            foreach (LayerName layer, layers) {
                QJsonArray layerArray;
                for (int bscanNumber=0; bscanNumber < scan->getBscansNumber(); bscanNumber++) {
                    QJsonArray bscanArray;
                    for (int x=1; x < scan->getBscanWidth(); x++) {
                        bscanArray.push_back(scan->getLayerPointAuto(layer, bscanNumber, x));
                    }
                    layerArray.push_back(bscanArray);
                }
                segmentations.insert(encodeLayer(layer), layerArray);
            }
            QJsonDocument jsonDoc;
            jsonDoc.setObject(segmentations);
            autoSegmentFile.write(jsonDoc.toJson(QJsonDocument::Compact));
//            QTextStream out(&autoSegmentFile);        // another method of saving to file
//            out.setEncoding(QStringConverter::Utf8);
//            out << jsonDoc.toJson(QJsonDocument::Compact);
        } else {
            qDebug() << "Incorrect dataSaveStructure (file extension)!";
        }
    }
    if (autoSegmentFile.isOpen())
        autoSegmentFile.close();
    emit processingData(100, "Auto segmentation data saved.");
}
