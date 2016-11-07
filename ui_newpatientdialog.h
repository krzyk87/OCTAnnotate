/********************************************************************************
** Form generated from reading UI file 'newpatientdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NEWPATIENTDIALOG_H
#define UI_NEWPATIENTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_NewPatientDialog
{
public:
    QLabel *label_5;
    QLineEdit *firstNameLEdit;
    QComboBox *genderCBox;
    QLabel *label_3;
    QDateEdit *birthDateEdit;
    QLineEdit *lastNameLEdit;
    QLabel *label_4;
    QLabel *label_6;
    QLabel *label;
    QPushButton *cancelButton;
    QPushButton *saveButton;
    QLabel *label_7;
    QLineEdit *fileNoLEdit;
    QLabel *label_8;
    QTextEdit *notesTEdit;
    QGroupBox *ODPathologyGroupBox;
    QCheckBox *odp_vmt_CBox;
    QRadioButton *odp_undef_RButton;
    QRadioButton *odp_none_RButton;
    QRadioButton *odp_cavri_RButton;
    QRadioButton *odp_other_RButton;
    QCheckBox *odp_erm_CBox;
    QCheckBox *odp_mh_CBox;
    QCheckBox *odp_cnv_CBox;
    QCheckBox *odp_vma_CBox;
    QGroupBox *OSPathologyGroupBox;
    QRadioButton *osp_cavri_RButton;
    QCheckBox *osp_erm_CBox;
    QCheckBox *osp_vmt_CBox;
    QRadioButton *osp_none_RButton;
    QCheckBox *osp_mh_CBox;
    QRadioButton *osp_undef_RButton;
    QCheckBox *osp_cnv_CBox;
    QRadioButton *osp_other_RButton;
    QCheckBox *osp_vma_CBox;

    void setupUi(QDialog *NewPatientDialog)
    {
        if (NewPatientDialog->objectName().isEmpty())
            NewPatientDialog->setObjectName(QStringLiteral("NewPatientDialog"));
        NewPatientDialog->resize(381, 561);
        label_5 = new QLabel(NewPatientDialog);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(20, 120, 91, 21));
        firstNameLEdit = new QLineEdit(NewPatientDialog);
        firstNameLEdit->setObjectName(QStringLiteral("firstNameLEdit"));
        firstNameLEdit->setEnabled(true);
        firstNameLEdit->setGeometry(QRect(120, 90, 241, 20));
        genderCBox = new QComboBox(NewPatientDialog);
        genderCBox->setObjectName(QStringLiteral("genderCBox"));
        genderCBox->setEnabled(true);
        genderCBox->setGeometry(QRect(120, 150, 241, 22));
        label_3 = new QLabel(NewPatientDialog);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(20, 60, 81, 21));
        birthDateEdit = new QDateEdit(NewPatientDialog);
        birthDateEdit->setObjectName(QStringLiteral("birthDateEdit"));
        birthDateEdit->setEnabled(true);
        birthDateEdit->setGeometry(QRect(120, 120, 121, 22));
        lastNameLEdit = new QLineEdit(NewPatientDialog);
        lastNameLEdit->setObjectName(QStringLiteral("lastNameLEdit"));
        lastNameLEdit->setEnabled(true);
        lastNameLEdit->setGeometry(QRect(120, 60, 241, 20));
        label_4 = new QLabel(NewPatientDialog);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(20, 90, 31, 16));
        label_6 = new QLabel(NewPatientDialog);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(20, 150, 46, 16));
        label = new QLabel(NewPatientDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 20, 341, 16));
        QFont font;
        font.setPointSize(11);
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);
        cancelButton = new QPushButton(NewPatientDialog);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));
        cancelButton->setGeometry(QRect(50, 530, 111, 23));
        saveButton = new QPushButton(NewPatientDialog);
        saveButton->setObjectName(QStringLiteral("saveButton"));
        saveButton->setGeometry(QRect(230, 530, 111, 23));
        label_7 = new QLabel(NewPatientDialog);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(20, 180, 91, 21));
        fileNoLEdit = new QLineEdit(NewPatientDialog);
        fileNoLEdit->setObjectName(QStringLiteral("fileNoLEdit"));
        fileNoLEdit->setEnabled(true);
        fileNoLEdit->setGeometry(QRect(120, 180, 241, 20));
        label_8 = new QLabel(NewPatientDialog);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(20, 210, 91, 21));
        notesTEdit = new QTextEdit(NewPatientDialog);
        notesTEdit->setObjectName(QStringLiteral("notesTEdit"));
        notesTEdit->setGeometry(QRect(120, 210, 241, 91));
        ODPathologyGroupBox = new QGroupBox(NewPatientDialog);
        ODPathologyGroupBox->setObjectName(QStringLiteral("ODPathologyGroupBox"));
        ODPathologyGroupBox->setGeometry(QRect(20, 310, 171, 211));
        odp_vmt_CBox = new QCheckBox(ODPathologyGroupBox);
        odp_vmt_CBox->setObjectName(QStringLiteral("odp_vmt_CBox"));
        odp_vmt_CBox->setGeometry(QRect(30, 100, 70, 17));
        odp_undef_RButton = new QRadioButton(ODPathologyGroupBox);
        odp_undef_RButton->setObjectName(QStringLiteral("odp_undef_RButton"));
        odp_undef_RButton->setGeometry(QRect(10, 20, 82, 17));
        odp_none_RButton = new QRadioButton(ODPathologyGroupBox);
        odp_none_RButton->setObjectName(QStringLiteral("odp_none_RButton"));
        odp_none_RButton->setGeometry(QRect(10, 40, 82, 17));
        odp_cavri_RButton = new QRadioButton(ODPathologyGroupBox);
        odp_cavri_RButton->setObjectName(QStringLiteral("odp_cavri_RButton"));
        odp_cavri_RButton->setGeometry(QRect(10, 60, 101, 17));
        odp_other_RButton = new QRadioButton(ODPathologyGroupBox);
        odp_other_RButton->setObjectName(QStringLiteral("odp_other_RButton"));
        odp_other_RButton->setGeometry(QRect(10, 180, 82, 17));
        odp_erm_CBox = new QCheckBox(ODPathologyGroupBox);
        odp_erm_CBox->setObjectName(QStringLiteral("odp_erm_CBox"));
        odp_erm_CBox->setGeometry(QRect(30, 120, 70, 17));
        odp_mh_CBox = new QCheckBox(ODPathologyGroupBox);
        odp_mh_CBox->setObjectName(QStringLiteral("odp_mh_CBox"));
        odp_mh_CBox->setGeometry(QRect(30, 140, 70, 17));
        odp_cnv_CBox = new QCheckBox(ODPathologyGroupBox);
        odp_cnv_CBox->setObjectName(QStringLiteral("odp_cnv_CBox"));
        odp_cnv_CBox->setGeometry(QRect(30, 160, 70, 17));
        odp_vma_CBox = new QCheckBox(ODPathologyGroupBox);
        odp_vma_CBox->setObjectName(QStringLiteral("odp_vma_CBox"));
        odp_vma_CBox->setGeometry(QRect(30, 80, 70, 17));
        OSPathologyGroupBox = new QGroupBox(NewPatientDialog);
        OSPathologyGroupBox->setObjectName(QStringLiteral("OSPathologyGroupBox"));
        OSPathologyGroupBox->setGeometry(QRect(200, 310, 171, 211));
        osp_cavri_RButton = new QRadioButton(OSPathologyGroupBox);
        osp_cavri_RButton->setObjectName(QStringLiteral("osp_cavri_RButton"));
        osp_cavri_RButton->setGeometry(QRect(10, 60, 101, 17));
        osp_erm_CBox = new QCheckBox(OSPathologyGroupBox);
        osp_erm_CBox->setObjectName(QStringLiteral("osp_erm_CBox"));
        osp_erm_CBox->setGeometry(QRect(30, 120, 70, 17));
        osp_vmt_CBox = new QCheckBox(OSPathologyGroupBox);
        osp_vmt_CBox->setObjectName(QStringLiteral("osp_vmt_CBox"));
        osp_vmt_CBox->setGeometry(QRect(30, 100, 70, 17));
        osp_none_RButton = new QRadioButton(OSPathologyGroupBox);
        osp_none_RButton->setObjectName(QStringLiteral("osp_none_RButton"));
        osp_none_RButton->setGeometry(QRect(10, 40, 82, 17));
        osp_mh_CBox = new QCheckBox(OSPathologyGroupBox);
        osp_mh_CBox->setObjectName(QStringLiteral("osp_mh_CBox"));
        osp_mh_CBox->setGeometry(QRect(30, 140, 70, 17));
        osp_undef_RButton = new QRadioButton(OSPathologyGroupBox);
        osp_undef_RButton->setObjectName(QStringLiteral("osp_undef_RButton"));
        osp_undef_RButton->setGeometry(QRect(10, 20, 82, 17));
        osp_cnv_CBox = new QCheckBox(OSPathologyGroupBox);
        osp_cnv_CBox->setObjectName(QStringLiteral("osp_cnv_CBox"));
        osp_cnv_CBox->setGeometry(QRect(30, 160, 70, 17));
        osp_other_RButton = new QRadioButton(OSPathologyGroupBox);
        osp_other_RButton->setObjectName(QStringLiteral("osp_other_RButton"));
        osp_other_RButton->setGeometry(QRect(10, 180, 82, 17));
        osp_vma_CBox = new QCheckBox(OSPathologyGroupBox);
        osp_vma_CBox->setObjectName(QStringLiteral("osp_vma_CBox"));
        osp_vma_CBox->setGeometry(QRect(30, 80, 70, 17));

        retranslateUi(NewPatientDialog);

        genderCBox->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(NewPatientDialog);
    } // setupUi

    void retranslateUi(QDialog *NewPatientDialog)
    {
        NewPatientDialog->setWindowTitle(QApplication::translate("NewPatientDialog", "Nowy Pacjent", 0));
        label_5->setText(QApplication::translate("NewPatientDialog", "Data urodzenia:", 0));
        genderCBox->clear();
        genderCBox->insertItems(0, QStringList()
         << QApplication::translate("NewPatientDialog", "Kobieta", 0)
         << QApplication::translate("NewPatientDialog", "M\304\231\305\274czyzna", 0)
         << QApplication::translate("NewPatientDialog", "p\305\202e\304\207", 0)
        );
        label_3->setText(QApplication::translate("NewPatientDialog", "Nazwisko:", 0));
        label_4->setText(QApplication::translate("NewPatientDialog", "Imi\304\231:", 0));
        label_6->setText(QApplication::translate("NewPatientDialog", "P\305\202e\304\207:", 0));
        label->setText(QApplication::translate("NewPatientDialog", "Prosz\304\231 poda\304\207 dane nowego pacjenta:", 0));
        cancelButton->setText(QApplication::translate("NewPatientDialog", "Anuluj", 0));
        saveButton->setText(QApplication::translate("NewPatientDialog", "Zapisz", 0));
        label_7->setText(QApplication::translate("NewPatientDialog", "Numer kartoteki:", 0));
        label_8->setText(QApplication::translate("NewPatientDialog", "Uwagi:", 0));
        ODPathologyGroupBox->setTitle(QApplication::translate("NewPatientDialog", "Rozpoznanie OD", 0));
        odp_vmt_CBox->setText(QApplication::translate("NewPatientDialog", "VMT", 0));
        odp_undef_RButton->setText(QApplication::translate("NewPatientDialog", "undefined", 0));
        odp_none_RButton->setText(QApplication::translate("NewPatientDialog", "brak", 0));
        odp_cavri_RButton->setText(QApplication::translate("NewPatientDialog", "in CAVRI group", 0));
        odp_other_RButton->setText(QApplication::translate("NewPatientDialog", "other", 0));
        odp_erm_CBox->setText(QApplication::translate("NewPatientDialog", "ERM", 0));
        odp_mh_CBox->setText(QApplication::translate("NewPatientDialog", "MH", 0));
        odp_cnv_CBox->setText(QApplication::translate("NewPatientDialog", "CNV", 0));
        odp_vma_CBox->setText(QApplication::translate("NewPatientDialog", "VMA", 0));
        OSPathologyGroupBox->setTitle(QApplication::translate("NewPatientDialog", "Rozpoznanie OS", 0));
        osp_cavri_RButton->setText(QApplication::translate("NewPatientDialog", "in CAVRI group", 0));
        osp_erm_CBox->setText(QApplication::translate("NewPatientDialog", "ERM", 0));
        osp_vmt_CBox->setText(QApplication::translate("NewPatientDialog", "VMT", 0));
        osp_none_RButton->setText(QApplication::translate("NewPatientDialog", "brak", 0));
        osp_mh_CBox->setText(QApplication::translate("NewPatientDialog", "MH", 0));
        osp_undef_RButton->setText(QApplication::translate("NewPatientDialog", "undefined", 0));
        osp_cnv_CBox->setText(QApplication::translate("NewPatientDialog", "CNV", 0));
        osp_other_RButton->setText(QApplication::translate("NewPatientDialog", "other", 0));
        osp_vma_CBox->setText(QApplication::translate("NewPatientDialog", "VMA", 0));
    } // retranslateUi

};

namespace Ui {
    class NewPatientDialog: public Ui_NewPatientDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NEWPATIENTDIALOG_H
