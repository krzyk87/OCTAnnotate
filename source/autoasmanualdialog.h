#ifndef AUTOASMANUALDIALOG_H
#define AUTOASMANUALDIALOG_H

#include <QDialog>

class QGroupBox;
class QCheckBox;
class QRadioButton;
class QPushButton;

class AutoAsManualDialog : public QDialog
{
    Q_OBJECT

public:
    AutoAsManualDialog(QWidget *parent = 0);

    QList<QString> getSelectedLayers();
    int getSelectedScans();

private:
    QGroupBox *layersGroup;
    QGroupBox *scansGroup;

    QCheckBox *pcvCBox;
    QCheckBox *ilmCBox;
    QCheckBox *gclCBox;
    QCheckBox *iplCBox;
    QCheckBox *inlCBox;
    QCheckBox *oplCBox;
    QCheckBox *onlCBox;
    QCheckBox *elmCBox;
    QCheckBox *mezCBox;
    QCheckBox *iosCBox;
    QCheckBox *rpeCBox;
    QCheckBox *chrCBox;

    QRadioButton *allLayersRButton;
    QRadioButton *selectedLayersRButton;
    QRadioButton *allScansRButton;
    QRadioButton *emptyScansRButton;
    QRadioButton *currentScanRButton;
    QRadioButton *otherScanRButton;

    QPushButton *acceptButton;
    QPushButton *cancelButton;
};

#endif // AUTOASMANUALDIALOG_H
