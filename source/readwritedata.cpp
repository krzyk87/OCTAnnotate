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
            emit readingDataFinished("manualOnly");
        }
        // TODO: uncomment below lines when adding functionalities for automatic segmentation
//        if (dir == "readAutoSegmentationData"){
//            QFile autoFile(autoFilePath);
//            readFileAutoSegmentation(&autoFile);
//            emit readingDataFinished("autoOnly");
//        }
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

    QStringList scanNameParts;
    QStringList filesList;
    QImage firstImg;

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
                     layer = ERM_UP;    // IB_ERM, 9: OB_ERM
                     break;
                 case 10:
                     layer = ILM;
                     break;
                 case 20:
                     layer = NFL_GCL;
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
                     layer = IS_OS;     // 120: IB_OPR
                     break;
                 case 140:
                     layer = OS_RPE;    // IB_RPE
                     break;
                 case 150:
                     layer = RPE_CHR;   // OB_RPE
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
                     layer = ERM_UP;    // IB_ERM, 1: OB_ERM
                     break;
                 case 2:
                     layer = ILM;
                     break;
                 case 3:
                     layer = NFL_GCL;
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
                     layer = IS_OS;     // 13: IB_OPR
                     break;
                 case 14:
                     layer = OS_RPE;    // IB_RPE
                     break;
                 case 15:
                     layer = RPE_CHR;   // OB_RPE
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
                     layer = ERM_UP;    // OB_ERM
                     break;
                 case 1:
                     layer = ILM;
                     break;
                 case 2:
                     layer = NFL_GCL;
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
                     layer = IS_OS;     // 12: IB_OPR
                     break;
                 case 13:
                     layer = OS_RPE;    // IB_RPE
                     break;
                 case 15:
                     layer = RPE_CHR;   // OB_RPE
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
                     if (xml.name().toString() == "z"){
                         xml.readNext();
                         scan->setPoint(layer, scanNumber, counter, xml.text().toInt());
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
                            scan->setPoint(layer, y, x, -1);
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
