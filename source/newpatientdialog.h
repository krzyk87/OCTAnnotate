#ifndef NEWPATIENTDIALOG_H
#define NEWPATIENTDIALOG_H

#include <QDialog>
#include <QSqlRecord>

namespace Ui {
class NewPatientDialog;
}

class NewPatientDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewPatientDialog(QWidget *parent = 0, QSqlRecord record = QSqlRecord(), int gender = 0);
    ~NewPatientDialog();

    QString getLastName() const;
    QString getFirstName() const;
    QDate getBirthDate() const;
    int getGender() const;
    QString getFileNo() const;
    QString getNotes() const;
    QString getPathologyOD() const;
    QString getPathologyOS() const;

private slots:
    void on_cancelButton_clicked();
    void on_saveButton_clicked();

    void on_odp_cavri_RButton_toggled(bool checked);
    void on_osp_cavri_RButton_toggled(bool checked);

private:
    Ui::NewPatientDialog *ui;
};

#endif // NEWPATIENTDIALOG_H
