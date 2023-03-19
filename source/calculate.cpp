#include "calculate.h"
#include "readwritedata.h"

#include <QDebug>
#include <QColor>

Calculate::Calculate(QObject *parent) :
    QObject(parent)
{

}

void Calculate::process(){
    qDebug() << "Starting calculations...";
    emit processingData(0, "Starting calculations...");

    // TODO: check if file exists - the data exists...
    double count = folderCount*3;

    foreach (QString str, folderList) {
        qDebug() << "read manual folder: " << str;
        readManual(str);
        emit processingData((double)(folderNumber*3 + 1)/count*100,"");

        qDebug() << "read auto folder: " << str;
        readAuto(str);
        emit processingData((double)(folderNumber*3 + 2)/count*100,"");

        folderNumber++;
    }


    qDebug() << "End of calculations...";
    emit processingData(100, "End of calculations");

    emit finished();
}

void Calculate::processStatistics(){
    qDebug() << "Starting calculations...";
    emit processingData(0, "Starting calculations...");

    // TODO: check if file exists - the data exists...
    double tasks = folderCount*4;

    foreach (QString str, folderList) {
        qDebug() << "reading manual segmentations: " << str;
        readManual(str);
        emit processingData((double)(folderNumber*4 + 1)/tasks*100,"");
    }

    qDebug() << "End of calculations...";
    emit processingData(100, "End of calculations");

    emit finished();
}

void Calculate::readManual(QString folder){
    // read manual ----------------------------------------------------------------------------
    QFile manualFile(examDataDir + folder + ".mvri");
    QPoint p(-1,-1);

    QDir octDir(octDirPath + folder);
    QDir manualDir(examDataDir);

    patientData = PatientData();

    ReadWriteData *rwData = new ReadWriteData();
    rwData->setDataObject(&patientData);
    rwData->setDirectoryOct(&octDir);
    rwData->setDirectoryManual(&manualDir);
    rwData->readPatientData();
    rwData->readGeneralExamData();
    patientData.resetBscansData();
    rwData->readFileManualSegmentation(&manualFile);

    foreach (Layers layer, allLayers) {
        for (int bscan=0; bscan<patientData.getBscansNumber(); bscan++){
            for (int x = 0; x < patientData.getBscanWidth(); x++){
                p = patientData.getLayerPoint(bscan,layer,x);
                if (p.x() != -1)
                    manualSeg[folderNumber].layers[allLayers.indexOf(layer)].map[bscan].bscan[p.x()] = p.y();
            }
        }
    }

    initials.append(patientData.getLastName().left(1) + patientData.getFirstName().left(1));
    age.append(patientData.getAge());
    centers[folderNumber] = patientData.getScanCenter();
}

void Calculate::readAuto(QString folder){
    // read auto ------------------------------------------------------------------------------
    QString line;
    QPoint p(-1,-1);
    QFile autoFile(examDataDir + autoFilePath + folder + alg + "_auto.txt");
    if (!autoFile.open(QIODevice::ReadOnly)){
        emit errorOccured(tr("Nie można otworzyć pliku z automatyczną segmentacją warstw: \n") + folder);
        qDebug() << "ERROR: Could not open file...";
    } else {
        QTextStream autoText(&autoFile);
        do {
            line = autoText.readLine();
            if (line.contains("=")){

                QStringList data = line.split("=");
                if ((data.at(1) != "") && (data.at(0) != "leftCut") && (data.at(0) != "rightCut")){
                    QStringList code = data.at(0).split("_");   // PCV_i...
                    Layers layer = decodeLayer(code.at(0));
                    int bscanNumber = code.at(1).toInt();

                    if (allLayers.contains(layer)){
                        QStringList points = data.at(1).split(";");
                        //qDebug() << "layer: " << code.at(0) << ", bscan number: " << bscanNumber << ", points: " << points.count();

                        for (int j=0; j < points.count()-1; j++){
                            QStringList p_str = points.at(j).split(",");
                            p.setX(p_str.at(0).toInt());
                            p.setY(p_str.at(1).toInt());
                            if (p.x() < bscanWidth){
                                if (p.y() < 1)
                                    p.setY(-1);
                                autoSeg[folderNumber].layers[allLayers.indexOf(layer)].map[bscanNumber].bscan[p.x()] = p.y();
                                patientData.setPointAuto(bscanNumber,layer,p);
                            }
                        }
                    }
                }
            }
        } while(!line.isNull());
    }
    if (autoFile.isOpen())
        autoFile.close();
}

void Calculate::calculateLayersDeviation(bool etdrsOnly){
    int mVal,aVal;
    double errVal;

    double distance = 0.0;
    double deltaX = (double)(patientData.getVoxelWidth() / (bscanWidth - 1));
    double deltaY = (double)(patientData.getVoxelHeight() / (bscansCount - 1));

    for (int layer = 0; layer < allLayers.count(); layer++){
        for (int folderNr = 0; folderNr < folderCount; folderNr++){
            int xc = centers[folderNr].x();
            int yc = centers[folderNr].y();
            if (xc == -1)
                xc = bscanWidth / 2;
            if (yc == -1)
                yc = bscansCount / 2;
            for (int bscan = 0; bscan < bscansCount; bscan++){
                for (int i = 0; i < bscanWidth; i++){
                    if (etdrsOnly){
//                        int x = i-xc;
//                        int y = bscan-yc;
                        distance = patientData.calculateDistance(QPoint(xc,yc),QPoint(i,bscan),deltaX,deltaY);
                    }
                    mVal = manualSeg[folderNr].layers[layer].map[bscan].bscan[i];
                    aVal = autoSeg[folderNr].layers[layer].map[bscan].bscan[i];
                    if ((mVal != -1) && (aVal != -1) && (distance <= 3.0)){
                        errVal = double(double(mVal) - double(aVal)); //error[folderNr].layers[layer].map[bscan].bscan[i];
                        if (qAbs(errVal) <= 5){
                            errVal = 0;
                        }
                        errVal = qAbs(errVal);
                    }
                }
            }
        }   
    }
}

void Calculate::setFolderList(QList<QString> list){
    this->folderList = list;
    this->folderCount = list.count();
    this->folderNumber = 0;
}

void Calculate::setLayers(QList<Layers> list){
    this->allLayers = list;
    this->layersCount = list.count();
}

void Calculate::setupMatrixes(OCTDevice device){
    this->voxelDepth = 2;

    switch (device){
    case COPERNICUS:
        this->bscanWidth = 800;
        this->bscanHeight = 1010;
        this->bscansCount = 100;
        break;
    case AVANTI:
        this->bscanWidth = 385;
        this->bscanHeight = 640;
        this->bscansCount = 141;
        break;
    }

    Scan s;
    for (int i=0; i<bscanWidth; i++){
        s.bscan.append(-1);
    }
    sLayer sl;
    for (int j=0; j<bscansCount; j++){
        sl.map.append(s);
    }
    RetinaLayers rls;
    for (int k=0; k<layersCount; k++){
        rls.layers.append(sl);
    }
    for (int l=0; l<folderCount; l++){
        manualSeg.append(rls);
        autoSeg.append(rls);
    }

    QList<double> row;
    for (int j=0; j<folderCount; j++){
        row.append(0);
    }

    // volume

    for (int f=0; f < folderCount; f++){
        centers.append(QPoint(-1,-1));
    }
}

void Calculate::imageEnhancement(QImage *img, float contrast, int brightness){

    // contrast enhancement
    int max = 0;    // TODO: wykorzystac OpenCV
    for (int i=0; i<img->width(); i++){
        for (int j=0; j<img->height(); j++){
            int value = QColor::fromRgb(img->pixel(i,j)).red();
            if (value > max)
                max = value;
        }
    }
    if (max == 0)
        max = 1;
    for (int i=0; i<img->width(); i++){
        for (int j=0; j<img->height(); j++){
            int value = QColor::fromRgb(img->pixel(i,j)).red();
            value = value * 255 / max * contrast + brightness;
            value = qBound(0,value,255);
            img->setColor(value,qRgb(value,value,value));
            img->setPixel(i,j,value);
        }
    }
}

QList<int> Calculate::calculateFlatteningDifferences(QImage *img){
    QList<int> differences;

    QList<QList<int> > imgList;
    for (int c=0; c < img->width(); c++){
        QList<int> column;
        for (int r=0; r < img->height(); r++){
            int value = QColor::fromRgb(img->pixel(c,r)).red();
            column.append(value);
        }
        imgList.append(column);
    }

    differences = calculateFlatteningDifferences(imgList);

    return differences;
}

QList<int> Calculate::calculateFlatteningDifferences(QList<QList<int> > img){

    // finding most hyperreflective line
    QList<int> maxValueIndex;
    for (int c=0; c < img.count(); c++){
        QList<int> column = img.at(c);
        int max = 0;
        maxValueIndex.append(0);
        for (int r=0; r < column.count(); r++){
            int value = column.at(r);
            if (value > max){
                max = value;
                maxValueIndex[c] = r;
            }
        }
    }

    int half = img.count()/2;
    int quater = img.count()/4;
    int sum1 = 0;
    int sum2 = 0;
    int sum3 = 0;
    int sum4 = 0;

    for (int c=0; c < quater; c++)
        sum1 += maxValueIndex[c];
    for (int c=quater; c < half; c++)
        sum2 += maxValueIndex[c];
    for (int c=half; c < (half+quater); c++)
        sum3 += maxValueIndex[c];
    for (int c=(half+quater); c < img.count(); c++)
        sum4 += maxValueIndex[c];
    double mean1 = sum1/quater;
    double mean2 = sum2/quater;
    double mean3 = sum3/quater;
    double mean4 = sum4/quater;
    int threshold = 25;

    QList<int> x, y;
    for (int c=0; c < quater; c++){
        if (qAbs(maxValueIndex[c] - mean1) <= threshold){
            x.append(c);
            y.append(maxValueIndex[c]);
        }
    }
    for (int c=quater; c < half; c++){
        if (qAbs(maxValueIndex[c] - mean2) <= threshold){
            x.append(c);
            y.append(maxValueIndex[c]);
        }
    }
    for (int c=half; c < (half+quater); c++){
        if (qAbs(maxValueIndex[c] - mean3) <= threshold){
            x.append(c);
            y.append(maxValueIndex[c]);
        }
    }
    for (int c=(half+quater); c < img.count(); c++){
        if (qAbs(maxValueIndex[c] - mean4) <= threshold){
            x.append(c);
            y.append(maxValueIndex[c]);
        }
    }

    // calculating polynomial
    QList<int> x_full, y_full;
    for (int c=0; c < img.count(); c++)
        x_full.append(c);
    int poly_deg = 2;
    int points_num = x.count();

    double sigmaX[2*poly_deg+1];    //Array that will store the values of sigma(xi),sigma(xi^2),sigma(xi^3)....sigma(xi^2n)
    for (int i=0; i < (2*poly_deg+1); i++){
        double sigma_temp = 0;
        for (int j=0; j < points_num; j++){
            sigma_temp = sigma_temp + qPow(x[j],i);
        }
        sigmaX[i] = sigma_temp; //consecutive positions of the array will store N,sigma(xi),sigma(xi^2),sigma(xi^3)....sigma(xi^2n)
    }

    //the Normal matrix(augmented) that will store the equations, 'a' is for value of the final coefficients
    double Normal_matrix[poly_deg+1][poly_deg+2], coeff[poly_deg+1];
    for (int i=0; i <= poly_deg; i++){
        for (int j=0; j <= poly_deg; j++){
            Normal_matrix[i][j] = sigmaX[i+j];  //Build the Normal matrix by storing the corresponding coefficients at the right positions except the last column of the matrix
        }
    }

    double sigmaY[poly_deg+1];  //Array to store the values of sigma(yi),sigma(xi*yi),sigma(xi^2*yi)...sigma(xi^n*yi)
    for (int i=0; i < (poly_deg+1); i++){
        double sigma_temp = 0;
        for (int j=0; j < points_num; j++){
            sigma_temp = sigma_temp + qPow(x[j],i)*y[j];
        }
        sigmaY[i] = sigma_temp; //consecutive positions will store sigma(yi),sigma(xi*yi),sigma(xi^2*yi)...sigma(xi^n*yi)
    }

    for (int i=0; i <= poly_deg; i++){
        Normal_matrix[i][poly_deg+1] = sigmaY[i];   //load the values of Y as the last column of B(Normal Matrix but augmented)
    }

    poly_deg++;

    // Gaussian Elimination
    for (int i=0; i < poly_deg; i++){
        for (int k=i+1; k < poly_deg; k++){
            if (Normal_matrix[i][i] < Normal_matrix[k][i]){
                for (int j=0; j <= poly_deg; j++){
                    double temp = Normal_matrix[i][j];
                    Normal_matrix[i][j] = Normal_matrix[k][j];
                    Normal_matrix[k][j] = temp;
                }
            }
        }
    }
    for (int i=0; i < poly_deg-1; i++){
        for (int k=i+1; k < poly_deg; k++){
            double t = Normal_matrix[k][i] / Normal_matrix[i][i];
            for (int j=0; j <= poly_deg; j++){
                Normal_matrix[k][j] = Normal_matrix[k][j] - t*Normal_matrix[i][j];
            }
        }
    }
    // back-subsitution
    for (int i=poly_deg-1; i >= 0; i--){
        coeff[i] = Normal_matrix[i][poly_deg];
        for (int j=0; j < poly_deg; j++){
            if (j != i){
                coeff[i] = coeff[i] - Normal_matrix[i][j]*coeff[j];
            }
        }
        coeff[i] = coeff[i] / Normal_matrix[i][i];
    }

    for (int c=0; c < img.count(); c++){
        double temp = 0;
        for (int i=0; i < poly_deg; i++){
            temp = temp + coeff[i]*qPow(x_full[c],i);
        }
        y_full.append(temp);
    }

    // flattening image
    QList<int> differences;
    int bottom_val = 320+160;//0;
    //for (int c=0; c < img->width(); c++){
    //    if (y_full[c] > bottom_val)
    //        bottom_val = y_full[c];
    //}
    for (int c=0; c < img.count(); c++){
        differences.append(bottom_val - y_full[c]);
    }

    return differences;
}

QImage Calculate::flattenImage(QImage *img, QList<int> flatDiff){
    QImage imgFlat = QImage(img->size(),img->format());
    imgFlat.fill(0);
    uint value = 0;

    int imgHeight = img->height();
    for (int c=0; c < img->width(); c++){
        if (flatDiff[c] >= 0){
            for (int y=0; y < (imgHeight - flatDiff[c]); y++){
                value = (uint)qRed(img->pixel(c,y));
                imgFlat.setColor(value, qRgb(value, value, value));
                imgFlat.setPixel(c, y + flatDiff[c], value);
            }
        } else {
            for (int y=(0-flatDiff[c]); y < imgHeight; y++){
                value = (uint)qRed(img->pixel(c,y));
                imgFlat.setColor(value, qRgb(value, value, value));
                imgFlat.setPixel(c, y + flatDiff[c], value);
            }
        }
    }
    return imgFlat;
}
