#ifndef OCTANNOTATE_H
#define OCTANNOTATE_H

#include <QMainWindow>
#include <QDir>
#include <QLabel>
#include <QScrollArea>
#include <QScrollBar>
#include <QListWidgetItem>
#include <QPen>
#include "patientdata.h"
#include "qcustomplot.h"
#include <QtSql>
#include "scan.h"
#include "settingsdialog.h"

namespace Ui {
class OCTAnnotate;
}

class OCTAnnotate : public QMainWindow
{
    Q_OBJECT

public:
    explicit OCTAnnotate(QWidget *parent = 0);
    ~OCTAnnotate();

private slots:
    void on_actionClose_triggered();
    void on_actionSettings_triggered();

// Display
    void on_tabWidget_currentChanged();
    void on_nextImageButton_clicked();
    void on_prevImageButton_clicked();
    void on_nextNormalImageButton_clicked();
    void on_prevNormalImageButton_clicked();
    void on_nextLayersButton_clicked();
    void on_prevLayersButton_clicked();
    void on_currImageNumberLEdit_returnPressed();
    void on_currNormalImageNumberLEdit_returnPressed();
    void on_currLayersNumberLEdit_returnPressed();
    void on_contrastSlider_valueChanged(int value);
    void on_brightnessSlider_valueChanged(int value);
    void on_contrastResetButton_clicked();
    void on_brightnessResetButton_clicked();

// Data
    void on_actionLoadOCTSequence_triggered();
    void on_actionLoadOCTFile_triggered();
    void loadOCT(bool isBinary);

// General exam

// OCT exam
    void setLayerVisibility(bool st, QRadioButton* button, LayerName layer);
    void on_allLayersCBox_stateChanged(int state);
    void on_pcvLayerCBox_stateChanged(int state);
    void on_ibermLayerCBox_stateChanged(int state);
    void on_obermLayerCBox_stateChanged(int state);
    void on_ilmLayerCBox_stateChanged(int state);
    void on_rnfl_gclLayerCBox_stateChanged(int state);
    void on_gcl_iplLayerCBox_stateChanged(int state);
    void on_ipl_inlLayerCBox_stateChanged(int state);
    void on_inl_oplLayerCBox_stateChanged(int state);
    void on_opl_onlLayerCBox_stateChanged(int state);
    void on_elmLayerCBox_stateChanged(int state);
    void on_mezLayerCBox_stateChanged(int state);
    void on_iosLayerCBox_stateChanged(int state);
    void on_iboprLayerCBox_stateChanged(int state);
    void on_ibrpeLayerCBox_stateChanged(int state);
    void on_obrpeLayerCBox_stateChanged(int state);
    void on_pcvLayerRButton_clicked();
    void on_ibermLayerRButton_clicked();
    void on_obermLayerRButton_clicked();
    void on_ilmLayerRButton_clicked();
    void on_rnfl_gclLayerRButton_clicked();
    void on_gcl_iplLayerRButton_clicked();
    void on_ipl_inlLayerRButton_clicked();
    void on_inl_oplLayerRButton_clicked();
    void on_opl_onlLayerRButton_clicked();
    void on_elmLayerRButton_clicked();
    void on_mezLayerRButton_clicked();
    void on_iosLayerRButton_clicked();
    void on_iboprLayerRButton_clicked();
    void on_ibrpeLayerRButton_clicked();
    void on_obrpeLayerRButton_clicked();

    void on_zoomInButton_clicked();
    void on_zoomOutButton_clicked();

// Virtual map

// Annotations
    void on_actionReadManualAnnotations_triggered();
    void on_actionSaveManualAnnotations_triggered();
    void on_actionReadAutoAnnotations_triggered();
    void on_actionSaveAutoAnnotations_triggered();
    void on_actionCopyAutoSegmentationsAsManual_triggered();
    void on_actionPerformAutomaticSegmentation_triggered();

// Database

public slots:
    void on_errorOccured(QString);
    void on_processingData(double, QString msg = "");

    void on_readingDataFinished(QString data);
    void on_savingDataFinished(QString data);
    void on_segmentationFinished();

protected:
    bool eventFilter(QObject *target, QEvent *event);
    void paintEvent(QPaintEvent * /*pEvent*/);
    void closeEvent(QCloseEvent * cEvent);
    void keyPressEvent(QKeyEvent *keyEvent);

private:
    Ui::OCTAnnotate *ui;
    void loadConfigurations(SettingsDialog *sDialog);
    void enableNavigationButtons();

// display OCT exam
    void loadImage(int imageNumber);
    void loadNormalImage(int normalImageNumber);
    void loadImageLayers(int imageNumber);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);
    void rescaleImage();
    void changeImageRange(int dir);

// perform automatic segmentation
//    void layersSegmentation();
//    cv::Mat composeImage(std::vector<float>& results, int64_t numInputElements);
//    void findPoints(cv::Mat edges, QVector<double>& z_data, QVector<double>& x_data, LayerName layer, int imageNumber);

// Data
//    bool readPatientData(QDir *dir);
//    void readOctExamData();
    void readAnnotations(bool isManual);
    void saveAnnotations(bool isManual);

// paint on OCT exam
    QList<LayerName> getLayersToDisplay();

    void displayAnnotations(QList<int> flatDiff = QList<int>());
    void displayNormalAnnotations(QList<int> flatDiff = QList<int>());
    void displayLayers(QCustomPlot *plot, bool isAuto);

// draw Virtual map
    void setupBScanPlots();

    void delay( int secondsToWait );

// variables
    bool quit;
    QString scanName;
    QDir octDir;
    QFile octFile;
    QDir examDir;
    static const QString settingsFilePath;

    PatientData patientData;
    Scan *scan;

    int currentImageNumber;
    int currentNormalImageNumber;
    bool showCenterOnBscan;
    QString dataSaveFormat;

    QScrollArea *scrollArea;
    QScrollArea *scrollArea2;
    QSize orgImageSize;
    int scaleFactor;
    QList<double> scales;
    QCPRange bscanRange;
    float contrast;
    int brightness;

    bool fundusAnnotate;
    QPalette myPalette;

    QString appVersion;
    QProgressBar *progressBar;
};

#endif // OCTANNOTATE_H
