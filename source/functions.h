#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "enums.h"
#include <QPoint>
#include <QString>
#include <QList>

Layers decodeLayer(QString layer);
QString encodeLayer(Layers layer);
QList<Layers> getAllLayers();

double calculateDistance(QPoint p1, QPoint p2, double dx = 0, double dy = 0);

#endif // FUNCTIONS_H
