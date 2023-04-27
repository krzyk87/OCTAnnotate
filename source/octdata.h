#ifndef OCTDATA_H
#define OCTDATA_H

#include <QObject>

class OCTData : public QObject
{
    Q_OBJECT
public:
    explicit OCTData(QObject *parent = nullptr);

    void resetData(int width, int height, int depth);
    void setData(QImage bscan, int bscanNumber);
    void setData(QList<QList<int> > bscan, int bscanNumber);
    QList<QList<QList<int> > > getData();
    QImage getDataImage(int bscanNumber);
    QImage getNormalDataImage(int normalImageNumber);

signals:

private:
    QList< QList< QList<int> > > data;
    int width;
    int height;
    int depth;
};

#endif // OCTDATA_H
