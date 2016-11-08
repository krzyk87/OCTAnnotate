#include "readwritedata.h"

#include <QTextStream>
#include <QDir>
#include <QColor>
#include <QDebug>
//#include <QtXml>

ReadWriteData::ReadWriteData(QObject *parent) : QObject(parent)
{
    pData = new PatientData();
    directives.clear();
    octDir = new QDir(".");
    manualDir = new QDir(".");
    autoDir = new QDir(".");
    manualFilePath = "";
    autoFilePath = "";
    showMessage = true;
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
            } else {
                emit returnNewDirectory(octDir->absolutePath());
            }
        }
        if (dir == "readGeneralExamData"){
            this->readGeneralExamData();
        }
        if (dir == "readOctExamData"){
            this->readOctExamData();
            emit readingDataFinished("");
        }
        if (dir == "readManualSegmentationData"){
            QFile manualFile(manualFilePath);
            readFileManualSegmentation(&manualFile);
            emit readingDataFinished("manualOnly");
        }
        if (dir == "readAutoSegmentationData"){
            QFile autoFile(autoFilePath);
            readFileAutoSegmentation(&autoFile);
            emit readingDataFinished("autoOnly");
        }

        if (dir == "saveGeneralExamData"){
            this->saveGeneralExamData();
            emit savingDataFinished("generalData");
        }
        if (dir == "saveManualSegmentationData"){
            this->saveManualSegmentationData();
            emit savingDataFinished("manualData");
        }
        if (dir == "saveAutoSegmentationData"){
            this->saveAutoSegmentationData();
            emit savingDataFinished("autoData");
        }

        if (dir == "copyAutoAsManual"){
            this->copyAutoAsManual();
        }
    }
    qDebug() << "Finished with " + octDir->dirName();
    emit finished();
}

void ReadWriteData::setDataObject(PatientData *patientData){
    pData = patientData;
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

void ReadWriteData::setDirectoryManual(QDir *mDir){
    manualDir = mDir;
}

void ReadWriteData::setDirectoryAuto(QDir *aDir){
    autoDir = aDir;
}

void ReadWriteData::setManualFilePath(QString mfPath){
    this->manualFilePath = mfPath;
}

void ReadWriteData::setAutoFilePath(QString afPath){
    this->autoFilePath = afPath;
}

// read patient's and exam data -------------------------------------------------------------------
bool ReadWriteData::readPatientData(){
    QString line;
    bool fileOpened = false;
    double tasks = 13;
    double count = 0;

    // read info.ini file
    QString iniFilePath = octDir->absolutePath().append("/info.ini");
    QFile iniFile(iniFilePath);
    if (iniFile.open(QIODevice::ReadOnly)){     // COPERNICUS <<-------------------------------
        fileOpened = true;
        pData->setOCTDevice(COPERNICUS);
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
                    pData->setBscansNumber(data.at(1).toInt());
                    emit processingData((++count)/tasks*100,"");
                }
                if (data.at(0) == "ilaskan"){
                    pData->setBscanWidth(data.at(1).toInt());
                    pData->setBscanHeight(1010);
                    emit processingData((++count)/tasks*100,"");
                }
                if (data.at(0) == "eye"){
                    pData->setEye(data.at(1).toInt());
                    emit processingData((++count)/tasks*100,"");
                }
                if (data.at(0) == "date"){
                    pData->setExamDate(QDate::fromString(data.at(1),"yyyy-MM-dd"));
                    emit processingData((++count)/tasks*100,"");
                }
                if (data.at(0) == "time"){
                    pData->setExamTime(QTime::fromString(data.at(1),"hh:mm:ss"));
                    emit processingData((++count)/tasks*100,"");
                }
            }
        } while (!line.isNull());
        pData->setBirthDate(QDate(year,month,day));
        emit processingData((++count)/tasks*100,"");
        pData->setAge(pData->getExamDate().year() - pData->getBirthDate().year());
        emit processingData((++count)/tasks*100,"");
        if (iniFile.isOpen())
            iniFile.close();

    } else {
        QString infoFilePath = octDir->absolutePath().append("/" + octDir->dirName() + ".txt");
        QFile infoFile(infoFilePath);
        if (infoFile.open(QIODevice::ReadOnly)){    // AVANTI <<-------------------------------
            fileOpened = true;
            pData->setOCTDevice(AVANTI);
            emit processingData((++count)/tasks*100,"");

            // Nazwisko, Imie _27_3D Retina_OS_2014-09-12_10.21.36_1
            QStringList fileName = octDir->dirName().split(",");
            QStringList fileName2 = fileName.at(1).split(" ");
            pData->setLastName(fileName.at(0).toUpper());
            emit processingData((++count)/tasks*100,"");
            pData->setFirstName(fileName2.at(1).toUpper());
            emit processingData((++count)/tasks*100,"");
            QStringList examCode = fileName2.at(3).split("_");
            pData->setExamDate(QDate::fromString(examCode.at(2),"yyyy-MM-dd"));
            emit processingData((++count)/tasks*100,"");
            pData->setExamTime(QTime::fromString(examCode.at(3),"hh.mm.ss"));
            emit processingData((++count)/tasks*100,"");

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
                        pData->setBscanWidth(data.at(1).toInt());
                        scanLengthRead = true;
                        emit processingData((++count)/tasks*100,"");
                    }
                    if (data.at(0) == "XY Scan Usage" && !scanUsageRead){
                        pData->setBscansNumber(data.at(1).toInt());
                        scanUsageRead = true;
                        emit processingData((++count)/tasks*100,"");
                    }
                    if (data.at(0) == "OCT Window Height"){
                        pData->setBscanHeight(data.at(1).toInt());
                        emit processingData((++count)/tasks*100,"");
                    }
                    if (data.at(0) == "Scan Depth"){ //Scan Depth=2.401 mm
                        QStringList val = data.at(1).split(" ");
                        pData->setVoxelDepth(val.at(0).toFloat());
                        emit processingData((++count)/tasks*100,"");
                    }
                    if (data.at(0) == "XY Scan Size1" && !scanWidth){   //XY Scan Size1= 12.00 mm
                        QStringList val = data.at(1).split(" ");
                        pData->setVoxelWidth(val.at(1).toFloat());
                        emit processingData((++count)/tasks*100,"");
                        scanWidth = true;
                    }
                    if (data.at(0) == "XY Scan Interval1" && !scanHeight){ //XY Scan Interval1= 0.028 mm
                        QStringList val = data.at(1).split(" ");
                        QString v = QString::number(val.at(1).toFloat(),'f',3);
                        pData->setVoxelHeight(v.toFloat() * pData->getBscansNumber());
                        emit processingData((++count)/tasks*100,"");
                        scanHeight = true;
                    }
                    if (data.at(0) == "Eye Scanned"){
                        if (data.at(1) == "OD"){
                            pData->setEye(1);
                        } else if (data.at(1) == "OS"){
                            pData->setEye(0);
                        }
                        emit processingData((++count)/tasks*100,"");
                    }
                }
            } while (!line.isNull());
            if (infoFile.isOpen())
                infoFile.close();
        }
    }

    if (!fileOpened){
        emit errorOccured(tr("Nie odnaleziono pliku z danymi badania OCT (*.ini lub *.txt)"));
        return false;
    } else {
        emit processingData(100, "Odczytano dane badania OCT.");
        return true;
    }
}

void ReadWriteData::readGeneralExamData(){
    QString line;
    emit processingData(0, "Trwa odczyt danych badania ogólnego...");
    double tasks = 17;
    double count = 0;

    // read exam data if exists
    QString examFilePath = manualDir->path().append("/" + pData->getLastName() + "_" + pData->getFirstName() + ".cavri");
    QFile examFile(examFilePath);
    if (!examFile.open(QIODevice::ReadWrite)){
        emit errorOccured(tr("Could not open general exam info file"));
        return;
    }

    QTextStream examText(&examFile);
    do {
        line = examText.readLine();
        if (line.contains("=")){
            QStringList data = line.split("=");
            if (data.at(0) == "VIS_OP"){
                pData->setVisOP(data.at(1));
                emit processingData((++count)/tasks*100,"");
            }
            if (data.at(0) == "VIS_OL"){
                pData->setVisOL(data.at(1));
                emit processingData((++count)/tasks*100,"");
            }
            if (data.at(0) == "SN_OP"){
                pData->setSnOP(data.at(1));
                emit processingData((++count)/tasks*100,"");
            }
            if (data.at(0) == "SN_OL"){
                pData->setSnOL(data.at(1));
                emit processingData((++count)/tasks*100,"");
            }
            if (data.at(0).startsWith("amsler")){
                pData->decodeAmslerData(data.at(0),data.at(1));
                emit processingData((++count)/tasks*100,"");
            }
            if (data.at(0) == "amsler_R_comment"){
                pData->setAmslerComment(data.at(1), "R");
                emit processingData((++count)/tasks*100,"");
            }
            if (data.at(0) == "amsler_L_comment"){
                pData->setAmslerComment(data.at(1), "L");
                emit processingData((++count)/tasks*100,"");
            }
            if (data.at(0) == "MCV_OP"){
                pData->setMcvOP(data.at(1));
                emit processingData((++count)/tasks*100,"");
            }
            if (data.at(0) == "MCV_OL"){
                pData->setMcvOL(data.at(1));
                emit processingData((++count)/tasks*100,"");
            }
            if (data.at(0) == "MCH_OP"){
                pData->setMchOP(data.at(1));
                emit processingData((++count)/tasks*100,"");
            }
            if (data.at(0) == "MCH_OL"){
                pData->setMchOL(data.at(1));
                emit processingData((++count)/tasks*100,"");
            }
            if (data.at(0) == "Birth_Date"){
                pData->setBirthDate(QDate::fromString(data.at(1),"yyyy-MM-dd"));
                emit processingData((++count)/tasks*100,"");
                pData->setAge(pData->getExamDate().year() - pData->getBirthDate().year());
            }
            if (data.at(0) == "Pathology_OP"){
                pData->setPathologyOP(decodePathology(data.at(1)));
                emit processingData((++count)/tasks*100,"");
            }
            if (data.at(0) == "Pathology_OL"){
                pData->setPathologyOL(decodePathology(data.at(1)));
                emit processingData((++count)/tasks*100,"");
            }
        }
    } while(!line.isNull());

    if (examFile.isOpen())
        examFile.close();

    emit processingData(100, "Odczytano dane badania ogólnego.");
}

void ReadWriteData::readOctExamData(){
    emit processingData(0, "Trwa pobieranie listy skanów...");
    double tasks = pData->getBscansNumber()*3;
    double count = 0;
    OCTDevice device = pData->getOCTDevice();

    // read exam images list
    QStringList imageList;
    for (int i=0; i < pData->getBscansNumber(); i++){
        if (device == COPERNICUS)
            imageList.append(octDir->absolutePath().append("/skan") + QString::number(i) + ".bmp");
        else if (device == AVANTI)
            imageList.append(octDir->absolutePath().append("/Skan_nr_") + QString::number(i+1) + ".bmp");
        emit processingData((++count)/tasks*100,"");
    }
    pData->setImageFileList(imageList);
    pData->resetBscansData();  // Bscans memory reset

    int imageNumber = 0;
    foreach (QString imagePath, imageList) {
        QImage img(imagePath);
        pData->setOCTdata(img, imageNumber);
        imageNumber++;
        emit processingData((++count)/tasks*100,"");
    }

    // read fundus image
    emit processingData(count, "Trwa odczyt obrazu fundus...");
    QImage fundus;
    QString fundusFilePath = octDir->absolutePath().append("/fnds_rec.bmp");
    if (QFile(fundusFilePath).exists()){
        fundus = QImage(fundusFilePath);
        if (device == COPERNICUS){
            fundus = fundus.mirrored(false,true);
        } else if (device == AVANTI){
            // contrast enhancement
//            for (int j=0; j<fundus.height(); j++){
//                for (int i=0; i<fundus.width(); i++){
//                    int value = QColor::fromRgb(fundus.pixel(i,j)).red();
//                    value = value * 8;
//                    value = qBound(0,value,255);
//                    fundus.setPixel(i,j,value);
//                }
//                emit processingData((++count)/tasks*100,"");
//            }
        } else {
            fundus = QImage(pData->getBscanWidth(), pData->getBscansNumber(), QImage::Format_Indexed8);
            fundus.fill(0);
        }
    } else {
        fundus = QImage(pData->getBscanWidth(), pData->getBscansNumber(), QImage::Format_Indexed8);
        fundus.fill(0);
    }
    pData->setFundusImage(fundus);

    // read oct exam data if exists
    QString octExamFilePath = manualDir->path().append("/" + octDir->dirName() + ".mvri");
    QFile octExamFile(octExamFilePath);
    readFileManualSegmentation(&octExamFile);
}

void ReadWriteData::readFileManualSegmentation(QFile *dataFile){
    QString line;
    QPoint p(-1,-1);
    double tasks = 1 + 12*pData->getBscansNumber() + 12 + pData->getBscansNumber();
    double count = 0;
    emit processingData(0, "Trwa odczyt danych ręcznej segmentacji...");

    if (!dataFile->open(QIODevice::ReadWrite)){
        emit errorOccured(tr("Nie można otworzyć pliku z ręczną segmentacją warstw: ") + manualDir->dirName());
    } else {
        pData->resetManualAnnotations();

        QTextStream octSegmentText(dataFile);
        line = octSegmentText.readLine();

        if (line.contains("<?")){
            dataFile->reset();
            // read as xml
            tasks = 1 + 12 + 1 + 12 + pData->getBscansNumber();
            QList<int> voxelSize;
            QXmlStreamReader xml(dataFile);
            xml.readNextStartElement();
            while (!xml.atEnd() && !xml.hasError()){
                QXmlStreamReader::TokenType token = xml.readNext();
                if (token == QXmlStreamReader::StartElement){
                    if (xml.name() == "scan_characteristics"){
                        voxelSize = parseXmlVoxelSize(xml);
                        emit processingData((++count)/tasks*100,"");
                    } else if (xml.name() == "surface"){
                        parseXmlSurfaceLines(xml);
                        emit processingData((++count)/tasks*100,"");
                    } else if (xml.name() == "undefined_region"){
                        parseUndefinedRegion(xml);
                        emit processingData((++count)/tasks*100,"");
                    } else {
                        continue;
                    }
                }
            }
        } else {
            if (line.contains("=")){
                // read as txt
                QStringList data = line.split("=");
                if ((data.at(0) == "scanCenter") && (data.at(1) != "")){
                    QStringList point = data.at(1).split(",");
                    QPoint center = QPoint(point.at(0).toInt(), point.at(1).toInt());
                    if (center.x() == -1){
                        if (pData->getOCTDevice() == COPERNICUS)
                            center.setX(pData->getBscanWidth() / 2);
                        else if (pData->getOCTDevice() == AVANTI)
                            center.setX(pData->getBscanWidth() / 2);
                    }
                    if (center.y() == -1)
                        center.setY(pData->getBscansNumber() / 2);
                    pData->setScanCenter(center);
                    emit processingData((++count)/tasks*100,"");
                }
                do {
                    line = octSegmentText.readLine();
                    if (line.contains("=")){
                        QStringList data = line.split("=");
                        if (data.at(1) != ""){
                            QStringList code = data.at(0).split("_");   // PCV_i...
                            Layers layer = decodeLayer(code.at(0));
                            int bscanNumber = code.at(1).toInt();

                            QStringList points = data.at(1).split(";");
                            for (int j=0; j < points.count(); j++){
                                QString p_str = points.at(j);
                                if (p_str != ""){
                                    p.setX(p_str.split(",").at(0).toInt());
                                    p.setY(p_str.split(",").at(1).toInt());
                                    pData->setPoint(bscanNumber, layer, p);
                                }
                            }
                            pData->countAnnotatedPixelsInLayer(bscanNumber, layer);
                            emit processingData((++count)/tasks*100,"");
                        }
                    }
                } while(!line.isNull());
            }
        }

        // check if no holes in the dataset
        emit processingData(count, "Trwa wygładzanie warstw...");
        QList<Layers> layers = getAllLayers();
        foreach (Layers layer, layers) {
            pData->smoothLayer(layer);
            emit processingData((++count)/tasks*100,"");
        }
        emit processingData(count, "Trwa uzupełnianie danych...");
        for (int i=0; i < pData->getBscansNumber(); i++){
            pData->fillContactArea(i);
            emit processingData((++count)/tasks*100,"");
        }
    }

    if (dataFile->isOpen())
        dataFile->close();
}

void ReadWriteData::readFileAutoSegmentation(QFile *dataFile){
    double tasks = 12*pData->getBscansNumber();
    double count = 0;
    emit processingData(0, "Trwa odczyt danych automatycznej segmentacji...");

    if (!dataFile->open(QIODevice::ReadOnly)){
        emit errorOccured(tr("Nie można otworzyć pliku z automatyczną segmentacją warstw: ") + autoDir->dirName());
        return;
    } else {
        pData->resetAutoAnnotations();
        QString line;
        QPoint p(-1,-1);
        int startPoint = 0; // 75
        int endPoint = 0;  // 75+325

        QFileInfo autoSegmentFileInfo(*dataFile);
        if (autoSegmentFileInfo.suffix() == "txt"){
            tasks *= pData->getBscanWidth();
            QTextStream autoSegmentText(dataFile);
            do {
                line = autoSegmentText.readLine();
                if (line.contains("=")){
                    QStringList data = line.split("=");

                    if (data.at(0) == "leftCut"){
                        startPoint = data.at(1).toInt() - 1;
                    } else if (data.at(0) == "rightCut"){
                        endPoint = pData->getBscanWidth() - data.at(1).toInt();
                    } else {
                        QStringList code = data.at(0).split("_");   // PCV_i...
                        Layers layer = decodeLayer(code.at(0));
                        int bscanNumber = code.at(1).toInt();
                        if (bscanNumber >= pData->getBscansNumber()){
                            emit errorOccured("wiecej auto b-skanow niz obrazow w badaniu");
                            return;
                        }

                        QStringList points = data.at(1).split(";");
                        for (int j=0; j < points.count(); j++){
                            QString p_str = points.at(j);
                            if (p_str != ""){
                                p.setX(p_str.split(",").at(0).toInt()-1);
                                p.setY(p_str.split(",").at(1).toInt());
                                if (p.x() >= pData->getBscanWidth()){
                                    emit errorOccured("x auto wieksze niz obraz");
                                } else {
                                    pData->setPointAuto(bscanNumber, layer, p);
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
                    if (xml.name() == "scan_characteristics"){
                        voxelSize = parseXmlVoxelSize(xml, 1);
                        emit processingData((++count)/tasks*100,"");
                    } else if (xml.name() == "surface"){
                        parseXmlSurfaceLines(xml, 1);
                        emit processingData((++count)/tasks*100,"");
                    } else if (xml.name() == "undefined_region"){
                        parseUndefinedRegion(xml, 1);
                        emit processingData((++count)/tasks*100,"");
                    } else {
                        continue;
                    }
                }
            }
        } else {
            emit errorOccured("Nie obsługiwany typ pliku");
        }
    }

    if (dataFile->isOpen())
        dataFile->close();

    emit processingData(100, "Odczytano dane automatycznej segmentacji...");
}

QList<int> ReadWriteData::parseXmlVoxelSize(QXmlStreamReader &xml, bool isAuto){
    QList<int> voxel;
    voxel.append(0);
    voxel.append(0);
    voxel.append(0);
    QPoint center(-1,-1);

    xml.readNext();
    while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "size")){
        if (xml.tokenType() == QXmlStreamReader::StartElement){
            if (xml.name() == "x"){
                xml.readNext();
                voxel[0] = xml.text().toInt();
            }
            if (xml.name() == "y"){
                xml.readNext();
                voxel[1] = xml.text().toInt();
            }
            if (xml.name() == "z"){
                xml.readNext();
                voxel[2] = xml.text().toInt();
            }
        }
        xml.readNext();
    }

    if (!isAuto){
        while(!(xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == "scan_center")){
            xml.readNext();
        }
        while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "scan_center")){
            if (xml.tokenType() == QXmlStreamReader::StartElement){
                if (xml.name() == "x"){
                    xml.readNext();
                    center.setX(xml.text().toInt());
                }
                if (xml.name() == "y"){
                    xml.readNext();
                    center.setY(xml.text().toInt());
                }
            }
            xml.readNext();
        }
        pData->setScanCenter(center);
    }

    return voxel;
}

void ReadWriteData::parseXmlSurfaceLines(QXmlStreamReader &xml, bool isAuto){
    int counter = 0;
    int lineNumber = 0;
    int scanNumber = 0; // y
    Layers layer = NONE;

    xml.readNext();
    while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "surface")){
        if (xml.tokenType() == QXmlStreamReader::StartElement){
            if (xml.name() == "label"){
                xml.readNext();
                lineNumber = xml.text().toInt();
                switch (lineNumber) {
                case -1:
                    layer = PCV;
                    break;
                case 0:
                    layer = ERM;
                    break;
                case 1:
                    layer = ILM;
                    break;
                case 2:
                    layer = GCL;
                    break;
                case 3:
                    layer = IPL;
                    break;
                case 4:
                    layer = INL;
                    break;
                case 5:
                    layer = OPL;
                    break;
                case 6:
                    layer = ONL;
                    break;
                case 7:
                    layer = ELM;
                    break;
                case 10:
                    layer = MEZ;
                    break;
                case 11:
                    layer = IOS;
                    break;
                case 14:
                    layer = RPE;
                    break;
                case 15:
                    layer = CHR;
                    break;
                default:
                    layer = NONE;
                    break;
                }
            }
            if ((xml.name() == "bscan") && (layer != NONE)){
                xml.readNext();
                counter = 0;
                while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "bscan")){
                    if (xml.tokenType() == QXmlStreamReader::StartElement){
                        if (xml.name() == "z"){
                            xml.readNext();
                            if (isAuto){
                                pData->setPointAuto(scanNumber,layer,QPoint(counter,xml.text().toInt()));
                            } else {
                                pData->setPoint(scanNumber,layer,QPoint(counter,xml.text().toInt()));
                            }
                            counter++;
                        }
                    }
                    xml.readNext();
                }
                scanNumber++;
                emit processingData((scanNumber)/pData->getBscansNumber()*100,"");
            }
        }
        xml.readNext();
    }
}

void ReadWriteData::parseUndefinedRegion(QXmlStreamReader &xml, bool isAuto){
    int x,y;
    QList<Layers> allLayers = getAllLayers();

    xml.readNext();
    while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "undefined_region")){
        if (xml.tokenType() == QXmlStreamReader::StartElement){
            if (xml.name() == "ascan"){
                xml.readNext();
                x = -1;
                y = -1;
                while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "ascan")){
                    if (xml.tokenType() == QXmlStreamReader::StartElement){
                        if (xml.name() == "x"){
                            xml.readNext();
                            x = xml.text().toInt();
                        }
                        if (xml.name() == "y"){
                            xml.readNext();
                            y = xml.text().toInt();
                        }
                    }
                    if (x != -1 && y != -1){
                        foreach (Layers layer, allLayers) {
                            if (isAuto){
                                pData->setPointAuto(y,layer,QPoint(x,-1));
                            } else {
                                pData->setPoint(y,layer,QPoint(x,-1));
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
void ReadWriteData::saveGeneralExamData(){
    emit processingData(0, "Trwa zapis danych badania ogólnego...");
    double tasks = 17;
    double count = 0;

    QString examFilePath = manualDir->path().append("/" + pData->getLastName() + "_" + pData->getFirstName() + ".cavri");
    QFile examFile(examFilePath);
    if (!examFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)){
        emit errorOccured(tr("Could not open general exam info file for saving"));
        return;
    }

    QTextStream stream(&examFile);
    stream << "VIS_OP=" + pData->getVisOP() << endl;
    emit processingData((++count)/tasks*100,"");
    stream << "VIS_OL=" + pData->getVisOL() << endl;
    emit processingData((++count)/tasks*100,"");
    stream << "SN_OP=" + pData->getSnOP() << endl;
    emit processingData((++count)/tasks*100,"");
    stream << "SN_OL=" + pData->getSnOL() << endl;
    emit processingData((++count)/tasks*100,"");
    QList<AmslerDist> distR = pData->getAmslerDistList("R");
    foreach (AmslerDist dist, distR) {
        stream << pData->encodeAmslerData(dist, "R") << endl;
    }
    emit processingData((++count)/tasks*100,"");
    QList<AmslerDist> distL = pData->getAmslerDistList("L");
    foreach (AmslerDist dist, distL) {
        stream << pData->encodeAmslerData(dist, "L") << endl;
    }
    emit processingData((++count)/tasks*100,"");
    stream << "amsler_R_comment=" + pData->getAmslerComment("R") << endl;
    emit processingData((++count)/tasks*100,"");
    stream << "amsler_L_comment=" + pData->getAmslerComment("L") << endl;
    emit processingData((++count)/tasks*100,"");
    stream << "MCV_OP=" + pData->getMcvOP() << endl;
    emit processingData((++count)/tasks*100,"");
    stream << "MCV_OL=" + pData->getMcvOL() << endl;
    emit processingData((++count)/tasks*100,"");
    stream << "MCH_OP=" + pData->getMchOP() << endl;
    emit processingData((++count)/tasks*100,"");
    stream << "MCH_OL=" + pData->getMchOL() << endl;
    emit processingData((++count)/tasks*100,"");
    stream << "Pathology_OP=" + encodePathology(pData->getPathologyOP()) << endl;
    emit processingData((++count)/tasks*100,"");
    stream << "Pathology_OL=" + encodePathology(pData->getPathologyOL()) << endl;
    emit processingData((++count)/tasks*100,"");
    stream << "Birth_Date=" + pData->getBirthDate().toString("yyyy-MM-dd") << endl;
    emit processingData((++count)/tasks*100,"");

    if (examFile.isOpen())
        examFile.close();

    emit processingData(100, "Zapisano dane badania ogólnego.");
}

void ReadWriteData::saveManualSegmentationData(){
    emit processingData(0, "Trwa zapis danych ręcznej segmentacji OCT...");
    double tasks = 1 + 12*pData->getBscansNumber();
    double count = 0;

    QList<Layers> layers = getAllLayers();
    // check if no holes in the dataset
    foreach (Layers layer, layers) {
        pData->smoothLayer(layer);
        emit processingData((++count)/tasks*100,"");
    }

    // save to file
    QString octExamFilePath = manualDir->path().append("/" + octDir->dirName() + ".mvri");
    QFile octExamFile(octExamFilePath);
    if (!octExamFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)){
        emit errorOccured(tr("Brak dostępu do pliku z ręczną segmentacją warstw"));
        return;
    } else {
        dataSaveStrucure = "xml";
        if (dataSaveStrucure == "txt"){
            QTextStream stream(&octExamFile);
            QPoint center = pData->getScanCenter();
            stream << "scanCenter=" + QString::number(center.x()) + "," + QString::number(center.y()) << endl;
            emit processingData((++count)/tasks*100,"");

            QList<QPoint> pointsList;
            // PCV_0=0,0;1,1;2,2;5,5;7,7;9,9;
            // others...
            foreach (Layers layer, layers) {
                QString sLayer = encodeLayer(layer);
                for (int i=0; i < pData->getBscansNumber(); i++){ //(i = B-scan number)
                    stream << sLayer + "_" + QString::number(i) + "=";
                    pointsList = pData->getLayerPoints(i, layer);
                    foreach (QPoint p, pointsList) {
                        stream << p.x() << "," << p.y() << ";";
                    }
                    stream << endl;
                    emit processingData((++count)/tasks*100,"");
                }
            }
        } else if (dataSaveStrucure == "xml"){
            QXmlStreamWriter xmlWriter(&octExamFile);
            xmlWriter.setAutoFormatting(true);
            xmlWriter.writeStartDocument();
            xmlWriter.writeStartElement("surfaces");

            // scan_characteristics
            xmlWriter.writeStartElement("scan_characteristics");
                // manufacturer
                if (pData->getOCTDevice() == COPERNICUS)
                    xmlWriter.writeTextElement("manufacturer", "Optopol");
                else if (pData->getOCTDevice() == AVANTI)
                    xmlWriter.writeTextElement("manufacturer", "Optovue");
                // size
                xmlWriter.writeStartElement("size");
                xmlWriter.writeTextElement("unit", "voxel");
                xmlWriter.writeTextElement("x", QString::number(pData->getBscanWidth()));
                xmlWriter.writeTextElement("y", QString::number(pData->getBscansNumber()));
                xmlWriter.writeTextElement("z", QString::number(pData->getBscanHeight()));
                xmlWriter.writeEndElement();    // end of size
                // voxel_size
                xmlWriter.writeStartElement("voxel_size");
                xmlWriter.writeTextElement("unit", "um");
                xmlWriter.writeTextElement("x", QString::number(pData->getVoxelWidth()*1000/pData->getBscanWidth()));
                xmlWriter.writeTextElement("y", QString::number(pData->getVoxelHeight()*1000/pData->getBscansNumber()));
                xmlWriter.writeTextElement("z", QString::number(pData->getVoxelDepth()*1000/pData->getBscanHeight()));
                xmlWriter.writeEndElement();    // end of voxel_size
                // center_type
                xmlWriter.writeTextElement("center_type", "macula");
                // scan_center
                xmlWriter.writeStartElement("scan_center");
                xmlWriter.writeTextElement("x", QString::number(pData->getScanCenter().x()));
                xmlWriter.writeTextElement("y", QString::number(pData->getScanCenter().y()));
                xmlWriter.writeTextElement("z", "0");
                xmlWriter.writeEndElement();    // end of scan_center
            xmlWriter.writeEndElement();    // end of scan_characteristics
            emit processingData((++count)/tasks*100,"");

            // unit
            xmlWriter.writeTextElement("unit", "voxel");
            // surface_size
            xmlWriter.writeStartElement("surface_size");
                xmlWriter.writeTextElement("x", QString::number(pData->getBscanWidth()));
                xmlWriter.writeTextElement("y", QString::number(pData->getBscansNumber()));
            xmlWriter.writeEndElement();    // end of surface_size
            // surface_num
            xmlWriter.writeTextElement("surface_num", QString::number(11)); // number of segmented layers

            // for each layer:
            foreach (Layers layer, layers) {
                xmlWriter.writeStartElement("surface");
                int label;
                QString name = "";
                switch (layer) {
                case PCV:
                    label = -1;
                    name = "PCV (PCV)";
                    break;
                case ERM:
                    label = 0;
                    name = "ERM (ERM)";
                    break;
                case ILM:
                    label = 1;
                    name = "ILM (ILM)";
                    break;
                case GCL:
                    label = 2;
                    name = "RNFL-GCL (RNFL-GCL)";
                    break;
                case IPL:
                    label = 3;
                    name = "GCL-IPL (GCL-IPL)";
                    break;
                case INL:
                    label = 4;
                    name = "IPL-INL (IPL-INL)";
                    break;
                case OPL:
                    label = 5;
                    name = "INL-OPL (INL-OPL)";
                    break;
                case ONL:
                    label = 6;
                    name = "OPL-Henles fiber layer (OPL-HFL)";
                    break;
                case ELM:
                    label = 7;
                    name = "ELM (ELM)";
                    break;
                case MEZ:
                    label = 10;
                    name = "Boundary of myoid and ellipsoid of inner segments (BMEIS)";
                    break;
                case IOS:
                    label = 11;
                    name = "IS/OS junction (IS/OSJ)";
                    break;
                /*case IB_OPR:
                    label = 12;
                    name = "Inner boudary of OPR (IB_OPR)";
                    break;*/
                case RPE:
                    label = 14;
                    name = "Inner boudary of RPE (IB_RPE)";
                    break;
                case CHR:
                    label = 15;
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
                for (int b_nr=0; b_nr < pData->getBscansNumber(); b_nr++){
                    xmlWriter.writeStartElement("bscan");
                    // for each point:
                    for (int px=0; px < pData->getBscanWidth(); px++){
                        xmlWriter.writeTextElement("z", QString::number(pData->getLayerPoint(b_nr,layer,px).y()));    // bscan number, layer, x
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

            qDebug() << "Finished!";
        }
    }

    if (octExamFile.isOpen())
        octExamFile.close();
    emit processingData(100, "Zapisano dane ręcznej segmentacji.");
}

void ReadWriteData::saveAutoSegmentationData(){
    emit processingData(0, "Trwa zapis danych automatycznej segmentacji OCT...");
    double tasks = 12*pData->getBscansNumber();
    double count = 0;
    QList<Layers> layers = getAllLayers();

    QFile autoSegmentFile(autoFilePath);
    QFileInfo autoSegmentFileInfo(autoSegmentFile);
//    if (dataSaveStrucure == "xml"){
//        QString fileName = autoSegmentFileInfo.completeBaseName() + ".xml";
//        autoFilePath = fileName;
//    }
//    autoSegmentFile = QFile(autoFilePath);
    if (!autoSegmentFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        emit errorOccured(tr("Brak dostępu do pliku z automatyczną segmentacją warstw"));
        return;
    } else {
        if (dataSaveStrucure == "txt"){
            QTextStream autoSegmentText(&autoSegmentFile);
            QList<QPoint> pointsList;

            foreach (Layers layer, layers){
                QString sLayer = encodeLayer(layer);
                for (int i=0; i < pData->getBscansNumber(); i++){ //(i = B-scan number)
                    autoSegmentText << sLayer + "_" + QString::number(i) + "=";
                    pointsList = pData->getLayerPointsAuto(i, layer, 0, pData->getBscanWidth()-1);
                    foreach (QPoint p, pointsList) {
                        autoSegmentText << p.x() << "," << p.y() << ";";
                    }
                    autoSegmentText << endl;
                    emit processingData((++count)/tasks*100,"");
                }
            }
        } else if (dataSaveStrucure == "xml"){
            QXmlStreamWriter xmlWriter(&autoSegmentFile);
            xmlWriter.setAutoFormatting(true);
            xmlWriter.writeStartDocument();
            xmlWriter.writeStartElement("surfaces");

            // scan_characteristics
            xmlWriter.writeStartElement("scan_characteristics");
                // manufacturer
                if (pData->getOCTDevice() == COPERNICUS)
                    xmlWriter.writeTextElement("manufacturer", "Optopol");
                else if (pData->getOCTDevice() == AVANTI)
                    xmlWriter.writeTextElement("manufacturer", "Optovue");
                // size
                xmlWriter.writeStartElement("size");
                xmlWriter.writeTextElement("unit", "voxel");
                xmlWriter.writeTextElement("x", QString::number(pData->getBscanWidth()));
                xmlWriter.writeTextElement("y", QString::number(pData->getBscansNumber()));
                xmlWriter.writeTextElement("z", QString::number(pData->getBscanHeight()));
                xmlWriter.writeEndElement();    // end of size
                // voxel_size
                xmlWriter.writeStartElement("voxel_size");
                xmlWriter.writeTextElement("unit", "um");
                xmlWriter.writeTextElement("x", QString::number(pData->getVoxelWidth()*1000/pData->getBscanWidth()));
                xmlWriter.writeTextElement("y", QString::number(pData->getVoxelHeight()*1000/pData->getBscansNumber()));
                xmlWriter.writeTextElement("z", QString::number(pData->getVoxelDepth()*1000/pData->getBscanHeight()));
                xmlWriter.writeEndElement();    // end of voxel_size
                // center_type
                xmlWriter.writeTextElement("center_type", "macula");
                // scan_center
                xmlWriter.writeStartElement("scan_center");
                xmlWriter.writeTextElement("x", QString::number(pData->getScanCenter().x()));
                xmlWriter.writeTextElement("y", QString::number(pData->getScanCenter().y()));
                xmlWriter.writeTextElement("z", "0");
                xmlWriter.writeEndElement();    // end of scan_center
            xmlWriter.writeEndElement();    // end of scan_characteristics

            // unit
            xmlWriter.writeTextElement("unit", "voxel");
            // surface_size
            xmlWriter.writeStartElement("surface_size");
                xmlWriter.writeTextElement("x", QString::number(pData->getBscanWidth()));
                xmlWriter.writeTextElement("y", QString::number(pData->getBscansNumber()));
            xmlWriter.writeEndElement();    // end of surface_size
            // surface_num
            xmlWriter.writeTextElement("surface_num", QString::number(11)); // number of segmented layers

            // for each layer:
            foreach (Layers layer, layers) {
                xmlWriter.writeStartElement("surface");
                int label;
                QString name = "";
                switch (layer) {
                case PCV:
                    label = -1;
                    name = "PCV (PCV)";
                    break;
                case ERM:
                    label = 0;
                    name = "ERM (ERM)";
                    break;
                case ILM:
                    label = 1;
                    name = "ILM (ILM)";
                    break;
                case GCL:
                    label = 2;
                    name = "RNFL-GCL (RNFL-GCL)";
                    break;
                case IPL:
                    label = 3;
                    name = "GCL-IPL (GCL-IPL)";
                    break;
                case INL:
                    label = 4;
                    name = "IPL-INL (IPL-INL)";
                    break;
                case OPL:
                    label = 5;
                    name = "INL-OPL (INL-OPL)";
                    break;
                case ONL:
                    label = 6;
                    name = "OPL-Henles fiber layer (OPL-HFL)";
                    break;
                case ELM:
                    label = 7;
                    name = "ELM (ELM)";
                    break;
                case MEZ:
                    label = 10;
                    name = "Boundary of myoid and ellipsoid of inner segments (BMEIS)";
                    break;
                case IOS:
                    label = 11;
                    name = "IS/OS junction (IS/OSJ)";
                    break;
                /*case IB_OPR:
                    label = 12;
                    name = "Inner boudary of OPR (IB_OPR)";
                    break;*/
                case RPE:
                    label = 14;
                    name = "Inner boudary of RPE (IB_RPE)";
                    break;
                case CHR:
                    label = 15;
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
                for (int b_nr=0; b_nr < pData->getBscansNumber(); b_nr++){
                    xmlWriter.writeStartElement("bscan");
                    // for each point:
                    for (int px=0; px < pData->getBscanWidth(); px++){
                        xmlWriter.writeTextElement("z", QString::number(pData->getLayerPoint(b_nr,layer,px).y()));    // bscan number, layer, x
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

            qDebug() << "Finished!";
        }
    }
    if (autoSegmentFile.isOpen())
        autoSegmentFile.close();
    emit processingData(100, "Zapisano dane automatycznej segmentacji.");
}

void ReadWriteData::copyAutoAsManual(){
    emit processingData(0, "Trwa copiowanie danych automatycznej segmentacji jako manualnej...");
    double tasks = 12*pData->getBscansNumber();
    double count = 0;
    QList<Layers> layers = getAllLayers();

    foreach (Layers layer, layers) {
        for (int i=0; i < pData->getBscansNumber(); i++){
            pData->setLayerPoints(i, pData->getLayerPointsAuto(i, layer), layer);
            emit processingData((++count)/tasks*100,"");
        }
    }
}
