#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "enums.h"
#include <QString>
#include <QList>

Layers decodeLayer(QString layer);
QString encodeLayer(Layers layer);
QList<Layers> getAllLayers();

#endif // FUNCTIONS_H
