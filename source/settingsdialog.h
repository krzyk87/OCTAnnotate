#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

    QString getPathOctExam();
    QString getPathManualSegm();
    QString getPathAutoSegm();
    QString getOpenBskan();
    QString getDataSaveStructure();
    QString getDatabasePath();
    bool getShowETDRSGrid();
    bool getShowCenterOnBscan();
    bool getShowBscanOnErrorPlot();
    bool getBlockPCV();

private slots:
    void on_cancelButton_clicked();
    void on_acceptButton_clicked();

    void on_resetPathOctExamButton_clicked();
    void on_resetPathManualSegmButton_clicked();
    void on_resetPathAutoSegmButton_clicked();

    void on_selectPathOctExamButton_clicked();
    void on_selectPathManualSegmButton_clicked();
    void on_selectPathAutoSegmButton_clicked();

    void on_dataSaveStructureCBox_currentIndexChanged(int index);

    void on_selectPathDatabaseButton_clicked();

    void on_resetPathDatabaseButton_clicked();

private:
    Ui::SettingsDialog *ui;
    QString configFileName;

    void readSettingsFromFile();
    bool saveSettingsToFile();
    void setDefaultValues();

    QString pathOctExam;
    QString pathManualSegm;
    QString pathAutoSegm;
    QString openBskan;
    QString dataSaveStructure;
    QString databasePath;
    bool showETDRSGrid;
    bool showCenterOnBscan;
    bool showBscanOnErrorPlot;
    bool blockPCV;

    static const QString pathOctExamDef;
    static const QString pathManualSegmDef;
    static const QString pathAutoSegmDef;
    static const QString openBskanDef;
    static const QString dataSaveStructureDef;
    static const QString databasePathDef;
    static const bool showETDRSGridDef;
    static const bool showCenterOnBscanDef;
    static const bool showBscanOnErrorPlotDef;
    static const bool blockPCVDef;
};

#endif // SETTINGSDIALOG_H
