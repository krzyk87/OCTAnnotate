/********************************************************************************
** Form generated from reading UI file 'computeerrorsettingsdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COMPUTEERRORSETTINGSDIALOG_H
#define UI_COMPUTEERRORSETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>

QT_BEGIN_NAMESPACE

class Ui_ComputeErrorSettingsDialog
{
public:
    QGroupBox *groupBox;
    QTableWidget *tableWidget;
    QGroupBox *groupBox_2;
    QCheckBox *ermLayerCBox;
    QCheckBox *iplLayerCBox;
    QCheckBox *iosLayerCBox;
    QCheckBox *mezLayerCBox;
    QCheckBox *ilmLayerCBox;
    QCheckBox *inlLayerCBox;
    QCheckBox *chrLayerCBox;
    QCheckBox *oplLayerCBox;
    QCheckBox *gclLayerCBox;
    QCheckBox *onlLayerCBox;
    QCheckBox *allLayersCBox;
    QCheckBox *pcvLayerCBox;
    QCheckBox *elmLayerCBox;
    QCheckBox *rpeLayerCBox;
    QGroupBox *groupBox_3;
    QLineEdit *examDataDirLEdit;
    QLabel *label;
    QLabel *label_2;
    QLineEdit *autoDirLEdit;
    QLabel *label_3;
    QLineEdit *algorithmLEdit;
    QPushButton *cancelButton;
    QPushButton *acceptButton;

    void setupUi(QDialog *ComputeErrorSettingsDialog)
    {
        if (ComputeErrorSettingsDialog->objectName().isEmpty())
            ComputeErrorSettingsDialog->setObjectName(QStringLiteral("ComputeErrorSettingsDialog"));
        ComputeErrorSettingsDialog->resize(854, 481);
        groupBox = new QGroupBox(ComputeErrorSettingsDialog);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(140, 10, 701, 291));
        tableWidget = new QTableWidget(groupBox);
        if (tableWidget->columnCount() < 2)
            tableWidget->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        tableWidget->setObjectName(QStringLiteral("tableWidget"));
        tableWidget->setGeometry(QRect(10, 20, 681, 261));
        groupBox_2 = new QGroupBox(ComputeErrorSettingsDialog);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 10, 120, 421));
        ermLayerCBox = new QCheckBox(groupBox_2);
        ermLayerCBox->setObjectName(QStringLiteral("ermLayerCBox"));
        ermLayerCBox->setGeometry(QRect(10, 60, 51, 21));
        ermLayerCBox->setFocusPolicy(Qt::NoFocus);
        ermLayerCBox->setChecked(false);
        iplLayerCBox = new QCheckBox(groupBox_2);
        iplLayerCBox->setObjectName(QStringLiteral("iplLayerCBox"));
        iplLayerCBox->setGeometry(QRect(10, 120, 61, 21));
        iplLayerCBox->setFocusPolicy(Qt::NoFocus);
        iplLayerCBox->setChecked(false);
        iosLayerCBox = new QCheckBox(groupBox_2);
        iosLayerCBox->setObjectName(QStringLiteral("iosLayerCBox"));
        iosLayerCBox->setGeometry(QRect(10, 240, 51, 21));
        iosLayerCBox->setChecked(true);
        mezLayerCBox = new QCheckBox(groupBox_2);
        mezLayerCBox->setObjectName(QStringLiteral("mezLayerCBox"));
        mezLayerCBox->setGeometry(QRect(10, 220, 61, 21));
        mezLayerCBox->setChecked(false);
        ilmLayerCBox = new QCheckBox(groupBox_2);
        ilmLayerCBox->setObjectName(QStringLiteral("ilmLayerCBox"));
        ilmLayerCBox->setGeometry(QRect(10, 80, 51, 21));
        ilmLayerCBox->setFocusPolicy(Qt::NoFocus);
        ilmLayerCBox->setChecked(true);
        inlLayerCBox = new QCheckBox(groupBox_2);
        inlLayerCBox->setObjectName(QStringLiteral("inlLayerCBox"));
        inlLayerCBox->setGeometry(QRect(10, 140, 61, 21));
        inlLayerCBox->setFocusPolicy(Qt::NoFocus);
        inlLayerCBox->setChecked(true);
        chrLayerCBox = new QCheckBox(groupBox_2);
        chrLayerCBox->setObjectName(QStringLiteral("chrLayerCBox"));
        chrLayerCBox->setGeometry(QRect(10, 280, 91, 21));
        chrLayerCBox->setChecked(true);
        oplLayerCBox = new QCheckBox(groupBox_2);
        oplLayerCBox->setObjectName(QStringLiteral("oplLayerCBox"));
        oplLayerCBox->setGeometry(QRect(10, 160, 61, 21));
        oplLayerCBox->setFocusPolicy(Qt::NoFocus);
        oplLayerCBox->setChecked(true);
        gclLayerCBox = new QCheckBox(groupBox_2);
        gclLayerCBox->setObjectName(QStringLiteral("gclLayerCBox"));
        gclLayerCBox->setGeometry(QRect(10, 100, 71, 21));
        gclLayerCBox->setFocusPolicy(Qt::NoFocus);
        gclLayerCBox->setChecked(true);
        onlLayerCBox = new QCheckBox(groupBox_2);
        onlLayerCBox->setObjectName(QStringLiteral("onlLayerCBox"));
        onlLayerCBox->setGeometry(QRect(10, 180, 71, 21));
        onlLayerCBox->setFocusPolicy(Qt::NoFocus);
        onlLayerCBox->setChecked(true);
        allLayersCBox = new QCheckBox(groupBox_2);
        allLayersCBox->setObjectName(QStringLiteral("allLayersCBox"));
        allLayersCBox->setGeometry(QRect(10, 20, 101, 21));
        allLayersCBox->setChecked(false);
        pcvLayerCBox = new QCheckBox(groupBox_2);
        pcvLayerCBox->setObjectName(QStringLiteral("pcvLayerCBox"));
        pcvLayerCBox->setGeometry(QRect(10, 40, 51, 21));
        pcvLayerCBox->setFocusPolicy(Qt::NoFocus);
        pcvLayerCBox->setChecked(false);
        elmLayerCBox = new QCheckBox(groupBox_2);
        elmLayerCBox->setObjectName(QStringLiteral("elmLayerCBox"));
        elmLayerCBox->setGeometry(QRect(10, 200, 61, 21));
        elmLayerCBox->setChecked(false);
        rpeLayerCBox = new QCheckBox(groupBox_2);
        rpeLayerCBox->setObjectName(QStringLiteral("rpeLayerCBox"));
        rpeLayerCBox->setGeometry(QRect(10, 260, 61, 21));
        rpeLayerCBox->setChecked(false);
        groupBox_3 = new QGroupBox(ComputeErrorSettingsDialog);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        groupBox_3->setGeometry(QRect(140, 310, 701, 121));
        examDataDirLEdit = new QLineEdit(groupBox_3);
        examDataDirLEdit->setObjectName(QStringLiteral("examDataDirLEdit"));
        examDataDirLEdit->setGeometry(QRect(150, 20, 541, 20));
        label = new QLabel(groupBox_3);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 20, 101, 21));
        label_2 = new QLabel(groupBox_3);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 50, 141, 21));
        autoDirLEdit = new QLineEdit(groupBox_3);
        autoDirLEdit->setObjectName(QStringLiteral("autoDirLEdit"));
        autoDirLEdit->setGeometry(QRect(150, 50, 541, 20));
        label_3 = new QLabel(groupBox_3);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(10, 80, 131, 21));
        algorithmLEdit = new QLineEdit(groupBox_3);
        algorithmLEdit->setObjectName(QStringLiteral("algorithmLEdit"));
        algorithmLEdit->setGeometry(QRect(150, 80, 541, 20));
        cancelButton = new QPushButton(ComputeErrorSettingsDialog);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));
        cancelButton->setGeometry(QRect(754, 450, 91, 23));
        acceptButton = new QPushButton(ComputeErrorSettingsDialog);
        acceptButton->setObjectName(QStringLiteral("acceptButton"));
        acceptButton->setGeometry(QRect(654, 450, 91, 23));

        retranslateUi(ComputeErrorSettingsDialog);

        QMetaObject::connectSlotsByName(ComputeErrorSettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *ComputeErrorSettingsDialog)
    {
        ComputeErrorSettingsDialog->setWindowTitle(QApplication::translate("ComputeErrorSettingsDialog", "Dialog", 0));
        groupBox->setTitle(QApplication::translate("ComputeErrorSettingsDialog", "Lista skan\303\263w", 0));
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("ComputeErrorSettingsDialog", "Select", 0));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("ComputeErrorSettingsDialog", "Folder name", 0));
        groupBox_2->setTitle(QApplication::translate("ComputeErrorSettingsDialog", "Warstwy", 0));
        ermLayerCBox->setText(QApplication::translate("ComputeErrorSettingsDialog", "ERM", 0));
        iplLayerCBox->setText(QApplication::translate("ComputeErrorSettingsDialog", "GCL/IPL", 0));
        iosLayerCBox->setText(QApplication::translate("ComputeErrorSettingsDialog", "IS/OS", 0));
        mezLayerCBox->setText(QApplication::translate("ComputeErrorSettingsDialog", "MEZ", 0));
        ilmLayerCBox->setText(QApplication::translate("ComputeErrorSettingsDialog", "ILM", 0));
        inlLayerCBox->setText(QApplication::translate("ComputeErrorSettingsDialog", "IPL/INL", 0));
        chrLayerCBox->setText(QApplication::translate("ComputeErrorSettingsDialog", "RPE/Choroid", 0));
        oplLayerCBox->setText(QApplication::translate("ComputeErrorSettingsDialog", "INL/OPL", 0));
        gclLayerCBox->setText(QApplication::translate("ComputeErrorSettingsDialog", "NFL/GCL", 0));
        onlLayerCBox->setText(QApplication::translate("ComputeErrorSettingsDialog", "OPL/ONL", 0));
        allLayersCBox->setText(QApplication::translate("ComputeErrorSettingsDialog", "Wszystkie", 0));
        pcvLayerCBox->setText(QApplication::translate("ComputeErrorSettingsDialog", "PCV", 0));
        elmLayerCBox->setText(QApplication::translate("ComputeErrorSettingsDialog", "ELM", 0));
        rpeLayerCBox->setText(QApplication::translate("ComputeErrorSettingsDialog", "OS/RPE", 0));
        groupBox_3->setTitle(QApplication::translate("ComputeErrorSettingsDialog", "\305\232cie\305\274ki", 0));
        label->setText(QApplication::translate("ComputeErrorSettingsDialog", "R\304\231czne oznaczenia:", 0));
        label_2->setText(QApplication::translate("ComputeErrorSettingsDialog", "Automatyczne oznaczenia:", 0));
        label_3->setText(QApplication::translate("ComputeErrorSettingsDialog", "Oznaczenie algorytmu:", 0));
        cancelButton->setText(QApplication::translate("ComputeErrorSettingsDialog", "Cancel", 0));
        acceptButton->setText(QApplication::translate("ComputeErrorSettingsDialog", "OK", 0));
    } // retranslateUi

};

namespace Ui {
    class ComputeErrorSettingsDialog: public Ui_ComputeErrorSettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COMPUTEERRORSETTINGSDIALOG_H
