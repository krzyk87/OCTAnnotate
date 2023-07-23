#ifndef CALCULATE_H
#define CALCULATE_H

#include "scan.h"
#include <QtCore>
#include <QObject>
#include <QImage>
#include <onnxruntime_cxx_api.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui.hpp>

class Calculate : public QObject
{
    Q_OBJECT
public:
    explicit Calculate(QObject *parent = 0);

signals:
    void segmentationFinished();
    void errorOccured(QString err);
    void processingData(double, QString);

public slots:
    void setObjectData(Scan *sData);

    void layersSegmentation();
    cv::Mat composeImage(std::vector<float>& results, int64_t numInputElements);
    void findPoints(cv::Mat edges, QVector<double>& z_data, QVector<double>& x_data, LayerName layer, int imageNumber);

    void imageEnhancement(QImage *img, float contrast, int brightness);
    QImage calculateFundus(QList< QList< QList<int> > > octData);

private:
    Scan *scan;

};

#endif // CALCULATE_H
