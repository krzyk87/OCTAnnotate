#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "enums.h"
#include <QPoint>
#include <QString>
#include <QList>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QDebug>

LayerName decodeLayer(QString layer);
QString encodeLayer(LayerName layer);
QList<LayerName> getAllLayers();

double calculateDistance(QPoint p1, QPoint p2, double dx = 0, double dy = 0);
std::vector<float> slicing(std::vector<float>& arr, int X, int Y);
extern std::vector<float> loadImageTest(cv::Mat mat, int sizeX, int sizeY);

#endif // FUNCTIONS_H
