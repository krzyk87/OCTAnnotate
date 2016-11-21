#ifndef COMPUTEERRORSETTINGSDIALOG_H
#define COMPUTEERRORSETTINGSDIALOG_H

#include <QDialog>
#include "patientdata.h"

namespace Ui {
class ComputeErrorSettingsDialog;
}

class ComputeErrorSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ComputeErrorSettingsDialog(QWidget *parent = 0);
    ~ComputeErrorSettingsDialog();

    QList<Layers> getLayers();
    QString getExamDataDir();
    QString getAutoDir();
    QString getAlgorithm();

private slots:
    void on_acceptButton_clicked();
    void on_cancelButton_clicked();

    void on_allLayersCBox_toggled(bool checked);
    QList<Layers> fillLayerList();

private:
    Ui::ComputeErrorSettingsDialog *ui;

    void setDefaultValues();

    QList<Layers> layers;

    QString examDataDir;
    QString autoDir;
    QString algorithm;

    static const QString examDataDirDef;
    static const QString autoDirDef;
    static const QString algorithmDef;
};

#endif // COMPUTEERRORSETTINGSDIALOG_H
