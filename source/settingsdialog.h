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
    explicit SettingsDialog(QString configFilePath, QWidget *parent = 0);
    ~SettingsDialog();

    QString getDatabasePath();
    QString getPathOctData();
    QString getPathExamData();

    QString getDataSaveStructure();

    bool getShowETDRSGrid();
    bool getShowCenterOnBscan();
    bool getShowBscanOnErrorPlot();
    bool getBlockPCV();

private slots:
    void on_cancelButton_clicked();
    void on_acceptButton_clicked();

    void on_resetPathDatabaseButton_clicked();
    void on_resetPathOctDataButton_clicked();

    void on_selectPathDatabaseButton_clicked();
    void on_selectPathOctDataButton_clicked();
    void on_dataSaveStructureCBox_currentIndexChanged(int index);

    void on_selectPathExamDataButton_clicked();

    void on_resetPathExamDataButton_clicked();

private:
    Ui::SettingsDialog *ui;
    QString configFileName;

    void readSettingsFromFile();
    bool saveSettingsToFile();

    QString pathDatabase;
    QString pathOctData;
    QString pathExamData;

    QString dataSaveStructure;

    bool showETDRSGrid;
    bool showCenterOnBscan;
    bool showBscanOnErrorPlot;
    bool blockPCV;

    static const QString pathDatabaseDef;
    static const QString pathOctDataDef;
    static const QString pathExamDataDef;

    static const QString dataSaveStructureDef;

    static const bool showETDRSGridDef;
    static const bool showCenterOnBscanDef;
    static const bool showBscanOnErrorPlotDef;
    static const bool blockPCVDef;
};

#endif // SETTINGSDIALOG_H
