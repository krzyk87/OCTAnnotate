#ifndef BATCHPROCESSINGDIALOG_H
#define BATCHPROCESSINGDIALOG_H

#include <QDialog>
#include <QDir>
#include "patientdata.h"

namespace Ui {
class BatchProcessingDialog;
}

class BatchProcessingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BatchProcessingDialog(QList<QString> list, QDir mDir, QWidget *parent = 0);
    ~BatchProcessingDialog();

private slots:
    void on_cancelButton_clicked();
    void on_acceptButton_clicked();

    void on_errorOccured(QString err);
    void on_processingData(double count, QString message);

    void process();
    void processingFinished();

private:
    Ui::BatchProcessingDialog *ui;

    QList<QString> folderList;
    int folderListCount;
    int currentFolder;

    bool canceled;
    bool finished;

    int tasksNumber;

    QString sufix;
    QDir manualDir;

    QTimer *myTimer;
    PatientData pData;
};

#endif // BATCHPROCESSINGDIALOG_H
