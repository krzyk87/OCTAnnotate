#include "calculate.h"
#include "readwritedata.h"

#include <QDebug>
#include <QColor>

Calculate::Calculate(QObject *parent) :
    QObject(parent)
{
    errorCountProcAll = 0;
    errorCountAll = 0;
    mse = 0;
    rmse = 0;
    avg = 0;
    dev = 0;
    proc = 0;
    sum = 0;
    squareSum = 0;
    squareDiffSum = 0;
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

        qDebug() << "calculate error folder: " << str;
        calculateError(&patientData,true,true);
        emit processingData((double)(folderNumber*3 + 3)/count*100,"");

//        qDebug() << "calculating error deviation";
//        calculateFolderDeviation(folderNumber);
        folderNumber++;
    }

    double peaksnr = this->bscanHeight;
    for (int i=0; i < allLayers.count(); i++){
        if (errorLayerCount[i] != 0){
            errorLayerMSE[i] = errorLayerSquareSum[i] / errorLayerCount[i];
            errorLayerAvg[i] = errorLayerSum[i] / errorLayerCount[i];
            errorLayerProc[i] = errorLayerCountProc[i] / errorLayerCount[i] * 100;
            errorLayerPSNR[i] = 10 * qLn(peaksnr * peaksnr / errorLayerMSE[i]) / qLn(10);
        }
        errorLayerRMSE[i] = qSqrt(errorLayerMSE[i]);

        squareSum += errorLayerSquareSum[i];
        squareDiffSum += errorLayerSquareDiffSum[i];
        sum += errorLayerSum[i];
    }
    if (errorCountAll != 0){
        mse = squareSum / errorCountAll;
        avg = sum / errorCountAll;
        psnr = 10 * qLn(peaksnr * peaksnr / mse) / qLn(10);
        proc = errorCountProcAll / errorCountAll * 100;
    }
    rmse = qSqrt(mse);
    calculateLayersDeviation(true);
    emit averageErrorCalculated(avg);
    emit mseErrorCaluclated(mse);

    qDebug() << "End of calculations...";
    emit processingData(100, "End of calculations");

    QFile errorSaveFile(examDataDir + autoFilePath + "error" + alg + ".txt");
    if(errorSaveFile.open(QIODevice::WriteOnly)){
        QTextStream stream(&errorSaveFile);
        QString errPsnr = QString::number(psnr,0,2).replace(".",",");
        QString errMse = QString::number(mse,0,2).replace(".",",");
        QString errRmse = QString::number(rmse,0,2).replace(".",",");
        QString errAvg = QString::number(avg,0,2).replace(".",",");
        QString errDev = QString::number(dev,0,2).replace(".",",");
        QString errProc = QString::number(proc,0,2).replace(".",",");
        stream << "General results: " << endl;
        stream << "Layer \t PSNR \t MSE \t RMSE \t AVG \t DEV \t PROC" << endl;
        stream << "All: " << "\t" << errPsnr << "\t" << errMse << "\t" << errRmse << "\t" << errAvg << "\t" << errDev << "\t" << errProc << endl;
        for (int i=0; i < allLayers.count(); i++) {
            errPsnr = QString::number(errorLayerPSNR[i],0,2).replace(".",",");
            errMse = QString::number(errorLayerMSE[i],0,2).replace(".",",");
            errRmse = QString::number(errorLayerRMSE[i],0,2).replace(".",",");
            errAvg = QString::number(errorLayerAvg[i],0,2).replace(".",",");
            errDev = QString::number(errorLayerDev[i],0,2).replace(".",",");
            errProc = QString::number(errorLayerProc[i],0,2).replace(".",",");
            stream << encodeLayer(allLayers.at(i)) << "\t" << errPsnr << "\t" << errMse << "\t" << errRmse << "\t" << errAvg << "\t" << errDev << "\t" << errProc << endl;
        }
        stream << "Folder-detailed MSE:" << endl;
        stream << "Folder:\t";
        for (int j=0; j < folderCount; j++){
            QString str = folderList[j].left(5);
            stream << str << "\t";
        }
        stream << endl;
        for (int i=0; i < layersCount; i++){
            stream << encodeLayer(allLayers.at(i)) << "\t";
            for (int j=0; j < folderCount; j++){
                stream << QString::number(errorFolderMSE[i][j]).replace(".",",") << "\t";
            }
            stream << endl;
        }
        errorSaveFile.close();
    }

    qDebug() << "Data saved to file: " << errorSaveFile.fileName();
    processingData(100,"Data saved to file: " + errorSaveFile.fileName());

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

        qDebug() << "calculating volumes: " << str;
        volumes[folderNumber] = patientData.getVolumeGrid();
        orderVolumes(folderNumber);
        emit processingData((double)(folderNumber*4 + 2)/tasks*100,"");

        qDebug() << "calculating contact area: " << str;
        contactAreas[folderNumber] = patientData.getContactAreaOM();
        emit processingData((double)(folderNumber*4 + 3)/tasks*100,"");

        qDebug() << "calculating retina depth: " << str;
        retinaDepths[folderNumber] = patientData.getRetinaDepth();
        emit processingData((double)(folderNumber*4 + 4)/tasks*100,"");
        folderNumber++;
    }

    qDebug() << "End of calculations...";
    emit processingData(100, "End of calculations");

    // save calculations to file...
    QFile statisticsFile(examDataDir + "statistics.txt");
    if(statisticsFile.open(QIODevice::WriteOnly)){
        QTextStream stream(&statisticsFile);
        stream << "Volume statistics: " << endl;
        stream << "Folder\tCF\tIM_T\tIM_S\tIM_N\tIM_I\tOM_T\tOM_S\tOM_N\tOM_I\tVsum\tCA\tRD" << endl;
        for (int i=0; i < folderCount; i++) {
            stream << folderList[i] << "\t";
            for (int j=0; j < volumes.at(i).count(); j++){
                stream << QString::number(volumes.at(i)[j],0,2).replace(".",",") << "\t";
            }
            stream << QString::number(contactAreas[i],0,3).replace(".",",") << "\t" << QString::number(retinaDepths[i],0,3).replace(".",",") << endl;
        }
        statisticsFile.close();
    }

    qDebug() << "Data saved to file: " << statisticsFile.fileName();
    processingData(100,"Data saved to file: " + statisticsFile.fileName());

    // save calculations to file... [vectors_cavri.R]
    QFile vectorsFile(examDataDir + "vectors_cavri.R");
    if(vectorsFile.open(QIODevice::WriteOnly)){
        QTextStream stream(&vectorsFile);
        for (int i=0; i < folderCount; i++) {
            stream << initials[i] << "_";
            stream << pathology[i] << "=c(";
            for (int j=0; j < volumes.at(i).count(); j++){
                stream << QString::number(volumes.at(i)[j],0,2) << ",";
            }
            stream << QString::number(contactAreas[i],0,3) << ",";
            stream << QString::number(retinaDepths[i],0,3) << ",";
            stream << vis[i] << ",";
            stream << age[i];
            stream << ")" << endl;
        }
        vectorsFile.close();
    }

    qDebug() << "Data saved to file: " << vectorsFile.fileName();
    processingData(100,"Data saved to file: " + vectorsFile.fileName());

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
    patientData.computeVirtualMap(PCV,ILM);

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
    if (patientData.getEye() == 0){ // OS
        vis.append(patientData.getVisOL());
        pathology.append(encodePathology(patientData.getPathologyOL()));
    } else {
        vis.append(patientData.getVisOP());
        pathology.append(encodePathology(patientData.getPathologyOP()));
    }
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

void Calculate::calculateError(PatientData *pData, bool etdrsOnly, bool multiscan){
    QList<Layers> layersList = allLayers; //getLayersForError();
    QList<QPoint> mPoints;
    QList<QPoint> aPoints;

    double errorAvg = 0.0;
    double errorDev = 0.0;
    double errorMSE = 0.0;
    double errorRMSE = 0.0;
    double errorPSNR = 0.0;
    double errorProc = 0.0;

    double errorVal = 0.0;
    double errorSum = 0.0;
    double errorSquareSum = 0.0;
    double errorDiffSquareSum = 0.0;
    double errorCount = 0.0;
    double errorCountProc = 0.0;

    QList<double> errLayerAvg;
    QList<double> errLayerDev;
    QList<double> errLayerMSE;
    QList<double> errLayerRMSE;
    QList<double> errLayerPSNR;
    QList<double> errLayerProc;

    QList<double> errLayerSum;
    QList<double> errLayerSquareSum;
    QList<double> errLayerDiffSquareSum;
    QList<double> errLayerCount;
    QList<double> errLayerCountProc;

    int xc = pData->getScanCenter().x();
    int yc = pData->getScanCenter().y();
    if (xc == -1)
        xc = pData->getBscanWidth() / 2;
    if (yc == -1)
        yc = pData->getBscansNumber() / 2;
    double distance = 0.0;
    double deltaX = (double)(pData->getVoxelWidth() / (pData->getBscanWidth() - 1));
    double deltaY = (double)(pData->getVoxelHeight() / (pData->getBscansNumber() - 1));

    foreach (Layers layer, layersList){
        errLayerAvg.append(0);
        errLayerDev.append(0);
        errLayerMSE.append(0);
        errLayerRMSE.append(0);
        errLayerPSNR.append(0);
        errLayerProc.append(0);

        errLayerSum.append(0);
        errLayerSquareSum.append(0);
        errLayerDiffSquareSum.append(0);
        errLayerCount.append(0);
        errLayerCountProc.append(0);

        int layerNr = layersList.indexOf(layer);

        for (int scan=0; scan < pData->getBscansNumber(); scan++){
            mPoints = pData->getLayerPoints(scan,layer,0,pData->getBscanWidth()-1);
            aPoints = pData->getLayerPointsAuto(scan,layer,0,pData->getBscanWidth()-1);
            for (int px=0; px < pData->getBscanWidth(); px++){
                if (etdrsOnly){
                    int x = px-xc;
                    int y = scan-yc;
                    distance = pData->calculateDistance(QPoint(xc,yc),QPoint(px,scan),deltaX,deltaY);
                }
                if ((mPoints[px].y() != -1) && (aPoints[px].y() != -1) && (distance <= 3.0)){
                    errorVal = mPoints[px].y() - aPoints[px].y(); //  * voxelDepth * 1000 / bscanHeight;
                    errorVal = qAbs(errorVal);
                    if (errorVal <= 5)
                        errorVal = 0;
                    else{
                        errorCountProc++;
                        errLayerCountProc[layerNr]++;
                    }

                    errorSum += errorVal;
                    errorSquareSum += qPow(errorVal,2);
                    errorCount++;

                    errLayerSum[layerNr] += errorVal;
                    errLayerSquareSum[layerNr] += qPow(errorVal,2);
                    errLayerCount[layerNr]++;
                }
            }
        }
        if (errLayerCount[layerNr] != 0){
            errLayerAvg[layerNr] = errLayerSum[layerNr] / errLayerCount[layerNr];
            errLayerMSE[layerNr] = errLayerSquareSum[layerNr] / errLayerCount[layerNr];
            errLayerRMSE[layerNr] = qSqrt(errLayerMSE[layerNr]);
            double peaksnr = pData->getBscanHeight();
            errLayerPSNR[layerNr] = 10 * qLn(peaksnr * peaksnr / errLayerMSE[layerNr]) / qLn(10);
            errLayerProc[layerNr] = errLayerCountProc[layerNr] / errLayerCount[layerNr] * 100;
        }
        if (multiscan){
            errorLayerSum[layerNr] += errLayerSum[layerNr];
            errorLayerSquareSum[layerNr] += errLayerSquareSum[layerNr];
            errorLayerCount[layerNr] += errLayerCount[layerNr];
            errorLayerCountProc[layerNr] += errLayerCountProc[layerNr];

            errorFolderSum[layerNr][folderNumber] += errLayerSum[layerNr];
            errorFolderSquareSum[layerNr][folderNumber] += errLayerSquareSum[layerNr];
            errorFolderCount[layerNr][folderNumber] += errLayerCount[layerNr];
            errorFolderCountProc[layerNr][folderNumber] += errLayerCountProc[layerNr];

            errorFolderAvg[layerNr][folderNumber] = errLayerAvg[layerNr];
            errorFolderMSE[layerNr][folderNumber] = errLayerMSE[layerNr];
            errorFolderRMSE[layerNr][folderNumber] = errLayerRMSE[layerNr];
            errorFolderPSNR[layerNr][folderNumber] = errLayerPSNR[layerNr];
            errorFolderProc[layerNr][folderNumber] = errLayerProc[layerNr];
        }
    }

    if (errorCount != 0){
        errorAvg = errorSum / errorCount;
        errorMSE = errorSquareSum / errorCount;
        errorRMSE = qSqrt(errorMSE);
        double peaksnr = pData->getBscanHeight();
        errorPSNR = 10 * qLn(peaksnr * peaksnr / errorMSE) / qLn(10);
        errorProc = errorCountProc / errorCount * 100;
    }

    foreach (Layers layer, layersList) {
        int layerNr = layersList.indexOf(layer);
        for (int scan=0; scan < pData->getBscansNumber(); scan++){
            mPoints = pData->getLayerPoints(scan,layer,0,pData->getBscanWidth()-1);
            aPoints = pData->getLayerPointsAuto(scan,layer,0,pData->getBscanWidth()-1);
            for (int px=0; px < pData->getBscanWidth(); px++){
                if (etdrsOnly){
                    int x = px-xc;
                    int y = scan-yc;
                    distance = pData->calculateDistance(QPoint(xc,yc),QPoint(px,scan),deltaX,deltaY);
                }
                if ((mPoints[px].y() != -1) && (aPoints[px].y() != -1) && (distance <= 3.0)){
                    errorVal = mPoints[px].y() - aPoints[px].y(); //  * voxelDepth * 1000 / bscanHeight;
                    errorVal = qAbs(errorVal);
                    if (errorVal <= 5)
                        errorVal = 0;

                    errorDiffSquareSum += qPow(errorVal - errorAvg,2);
                    errLayerDiffSquareSum[layerNr] += qPow(errorVal - errLayerAvg[layerNr],2);
                }
            }
        }
        if (errLayerCount[layerNr] != 0){
            errLayerDev[layerNr] = qSqrt(errLayerDiffSquareSum[layerNr] / errLayerCount[layerNr]);
        }
        pData->setErrorLayer(layer,errLayerAvg[layerNr],errLayerDev[layerNr],errLayerMSE[layerNr],errLayerRMSE[layerNr],errLayerPSNR[layerNr],errLayerProc[layerNr]);

        if (multiscan){
            errorFolderSquareDiffSum[layerNr] += errLayerDiffSquareSum[layerNr];
            errorFolderDev[layerNr][folderNumber] = errLayerDev[layerNr];
        }
    }
    if (errorCount != 0){
        errorDev = qSqrt(errorDiffSquareSum / errorCount);
    }

    pData->setErrorAll(errorAvg,errorDev,errorMSE,errorRMSE,errorPSNR,errorProc);

    if (multiscan){
        errorCountAll += errorCount;
        errorCountProcAll += errorCountProc;
    }
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
                        int x = i-xc;
                        int y = bscan-yc;
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
                        errorLayerSquareDiffSum[layer] += qPow(errVal - errorLayerAvg[layer],2);
                        squareDiffSum += qPow(errVal - avg,2);
                    }
                }
            }
        }
        errorLayerDev[layer] = qSqrt(errorLayerSquareDiffSum[layer] / errorLayerCount[layer]);
    }
    dev = qSqrt(squareDiffSum / errorCountAll);
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
        error.append(rls);
    }

    errorLayerPSNR.clear();
    errorLayerMSE.clear();
    errorLayerRMSE.clear();
    errorLayerAvg.clear();
    errorLayerDev.clear();
    errorLayerProc.clear();

    errorLayerCount.clear();
    errorLayerCountProc.clear();
    errorLayerSum.clear();
    errorLayerSquareSum.clear();
    errorLayerSquareDiffSum.clear();

    errorFolderPSNR.clear();
    errorFolderMSE.clear();
    errorFolderRMSE.clear();
    errorFolderAvg.clear();
    errorFolderDev.clear();
    errorFolderProc.clear();
    errorFolderSquareSum.clear();
    errorFolderSquareDiffSum.clear();
    errorFolderSum.clear();
    errorFolderCount.clear();
    errorFolderCountProc.clear();

    QList<double> row;
    for (int j=0; j<folderCount; j++){
        row.append(0);
    }
    for (int i=0; i<layersCount; i++){
        errorLayerPSNR.append(0);
        errorLayerMSE.append(0);
        errorLayerRMSE.append(0);
        errorLayerAvg.append(0);
        errorLayerDev.append(0);
        errorLayerProc.append(0);
        errorLayerCount.append(0);
        errorLayerCountProc.append(0);
        errorLayerSum.append(0);
        errorLayerSquareSum.append(0);
        errorLayerSquareDiffSum.append(0);

        errorFolderPSNR.append(row);
        errorFolderMSE.append(row);
        errorFolderRMSE.append(row);
        errorFolderAvg.append(row);
        errorFolderDev.append(row);
        errorFolderProc.append(row);
        errorFolderCount.append(row);
        errorFolderCountProc.append(row);
        errorFolderSum.append(row);
        errorFolderSquareSum.append(row);
        errorFolderSquareDiffSum.append(row);
    }

    // volume
    volumes.clear();
    QList<double> v;
    for (int i=0; i < 10; i++){
        v.append(0);
    }

    for (int f=0; f < folderCount; f++){
        volumes.append(v);
        contactAreas.append(0);
        retinaDepths.append(0);
        centers.append(QPoint(-1,-1));
    }
}

void Calculate::orderVolumes(int folderNr){
    QList<double> vol = volumes[folderNr];
    QList<double> newVol;
    newVol.reserve(10);

    newVol.append(vol[0]);  // CF

    if (patientData.getEye() == 1){    // right eye
        newVol.append(vol[2]);
        newVol.append(vol[3]);
        newVol.append(vol[4]);
        newVol.append(vol[1]);
        newVol.append(vol[6]);
        newVol.append(vol[7]);
        newVol.append(vol[8]);
        newVol.append(vol[5]);
//        labelLeft->setText("T <-");
//        labelRight->setText("-> N");
    } else if (patientData.getEye() == 0){ // left eye
        newVol.append(vol[4]);
        newVol.append(vol[3]);
        newVol.append(vol[2]);
        newVol.append(vol[1]);
        newVol.append(vol[8]);
        newVol.append(vol[7]);
        newVol.append(vol[6]);
        newVol.append(vol[5]);
//        labelLeft->setText("N <-");
//        labelRight->setText("-> T");
    }

    newVol.append(vol[9]);  // sum

    volumes[folderNr] = newVol;
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

    // finding most hyperreflective line
    QList<int> maxValueIndex;
    for (int c=0; c < img->width(); c++){
        int max = 0;
        maxValueIndex.append(0);
        for (int r=0; r < img->height(); r++){
            int value = QColor::fromRgb(img->pixel(c,r)).red();
            if (value > max){
                max = value;
                maxValueIndex[c] = r;
            }
        }
    }

    int half = img->width()/2;
    int quater = img->width()/4;
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
    for (int c=(half+quater); c < img->width(); c++)
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
    for (int c=(half+quater); c < img->width(); c++){
        if (qAbs(maxValueIndex[c] - mean4) <= threshold){
            x.append(c);
            y.append(maxValueIndex[c]);
        }
    }

    // calculating polynomial
    QList<int> x_full, y_full;
    for (int c=0; c < img->width(); c++)
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

    for (int c=0; c < img->width(); c++){
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
    for (int c=0; c < img->width(); c++){
        differences.append(bottom_val - y_full[c]);
    }


    return differences;
}

QImage Calculate::flattenImage(QImage *img, QList<int> flatDiff){
    QImage imgFlat = QImage(img->size(),img->format());
    imgFlat.fill(0);

    int imgHeight = img->height();
    for (int c=0; c < img->width(); c++){
        if (flatDiff[c] >= 0){
            for (int y=0; y < (imgHeight - flatDiff[c]); y++){
                int value = qRed(img->pixel(c,y));
                imgFlat.setColor(value, qRgb(value, value, value));
                imgFlat.setPixel(c, y + flatDiff[c], value);
            }
        } else {
            for (int y=(0-flatDiff[c]); y < imgHeight; y++){
                int value = qRed(img->pixel(c,y));
                imgFlat.setColor(value, qRgb(value, value, value));
                imgFlat.setPixel(c, y + flatDiff[c], img->pixel(c,y));
            }
        }
    }
    return imgFlat;
}
