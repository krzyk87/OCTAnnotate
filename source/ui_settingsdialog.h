/********************************************************************************
** Form generated from reading UI file 'settingsdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSDIALOG_H
#define UI_SETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SettingsDialog
{
public:
    QVBoxLayout *verticalLayout_3;
    QGroupBox *groupBox_6;
    QHBoxLayout *horizontalLayout_7;
    QLineEdit *pathDatabaseLEdit;
    QPushButton *selectPathDatabaseButton;
    QPushButton *resetPathDatabaseButton;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout;
    QLineEdit *pathOctDataLEdit;
    QPushButton *selectPathOctDataButton;
    QPushButton *resetPathOctDataButton;
    QGroupBox *groupBox_2;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *pathExamDataLEdit;
    QPushButton *selectPathExamDataButton;
    QPushButton *resetPathExamDataButton;
    QGroupBox *groupBox_3;
    QHBoxLayout *horizontalLayout_3;
    QLineEdit *pathAutoSegmLEdit;
    QPushButton *selectPathAutoSegmButton;
    QPushButton *resetPathAutoSegmButton;
    QHBoxLayout *horizontalLayout_6;
    QGroupBox *groupBox_5;
    QVBoxLayout *verticalLayout_4;
    QCheckBox *showETDRSGridCBox;
    QCheckBox *showCenterOnBscanCBox;
    QCheckBox *showBscanOnErrorPlotCBox;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_5;
    QLabel *dataSaveStructureLabel;
    QComboBox *dataSaveStructureCBox;
    QCheckBox *blockPCVCBox;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer;
    QPushButton *acceptButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *SettingsDialog)
    {
        if (SettingsDialog->objectName().isEmpty())
            SettingsDialog->setObjectName("SettingsDialog");
        SettingsDialog->resize(653, 420);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(SettingsDialog->sizePolicy().hasHeightForWidth());
        SettingsDialog->setSizePolicy(sizePolicy);
        SettingsDialog->setMinimumSize(QSize(653, 293));
        SettingsDialog->setMaximumSize(QSize(900, 670));
        verticalLayout_3 = new QVBoxLayout(SettingsDialog);
        verticalLayout_3->setObjectName("verticalLayout_3");
        groupBox_6 = new QGroupBox(SettingsDialog);
        groupBox_6->setObjectName("groupBox_6");
        horizontalLayout_7 = new QHBoxLayout(groupBox_6);
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        pathDatabaseLEdit = new QLineEdit(groupBox_6);
        pathDatabaseLEdit->setObjectName("pathDatabaseLEdit");
        pathDatabaseLEdit->setMinimumSize(QSize(450, 20));

        horizontalLayout_7->addWidget(pathDatabaseLEdit);

        selectPathDatabaseButton = new QPushButton(groupBox_6);
        selectPathDatabaseButton->setObjectName("selectPathDatabaseButton");
        selectPathDatabaseButton->setMinimumSize(QSize(75, 0));
        selectPathDatabaseButton->setMaximumSize(QSize(100, 16777215));

        horizontalLayout_7->addWidget(selectPathDatabaseButton);

        resetPathDatabaseButton = new QPushButton(groupBox_6);
        resetPathDatabaseButton->setObjectName("resetPathDatabaseButton");
        resetPathDatabaseButton->setMinimumSize(QSize(50, 0));
        resetPathDatabaseButton->setMaximumSize(QSize(100, 16777215));

        horizontalLayout_7->addWidget(resetPathDatabaseButton);


        verticalLayout_3->addWidget(groupBox_6);

        groupBox = new QGroupBox(SettingsDialog);
        groupBox->setObjectName("groupBox");
        horizontalLayout = new QHBoxLayout(groupBox);
        horizontalLayout->setObjectName("horizontalLayout");
        pathOctDataLEdit = new QLineEdit(groupBox);
        pathOctDataLEdit->setObjectName("pathOctDataLEdit");
        pathOctDataLEdit->setMinimumSize(QSize(450, 20));

        horizontalLayout->addWidget(pathOctDataLEdit);

        selectPathOctDataButton = new QPushButton(groupBox);
        selectPathOctDataButton->setObjectName("selectPathOctDataButton");
        selectPathOctDataButton->setMinimumSize(QSize(75, 0));
        selectPathOctDataButton->setMaximumSize(QSize(100, 16777215));

        horizontalLayout->addWidget(selectPathOctDataButton);

        resetPathOctDataButton = new QPushButton(groupBox);
        resetPathOctDataButton->setObjectName("resetPathOctDataButton");
        resetPathOctDataButton->setMinimumSize(QSize(50, 0));
        resetPathOctDataButton->setMaximumSize(QSize(100, 16777215));

        horizontalLayout->addWidget(resetPathOctDataButton);


        verticalLayout_3->addWidget(groupBox);

        groupBox_2 = new QGroupBox(SettingsDialog);
        groupBox_2->setObjectName("groupBox_2");
        horizontalLayout_2 = new QHBoxLayout(groupBox_2);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        pathExamDataLEdit = new QLineEdit(groupBox_2);
        pathExamDataLEdit->setObjectName("pathExamDataLEdit");
        pathExamDataLEdit->setMinimumSize(QSize(450, 20));

        horizontalLayout_2->addWidget(pathExamDataLEdit);

        selectPathExamDataButton = new QPushButton(groupBox_2);
        selectPathExamDataButton->setObjectName("selectPathExamDataButton");
        selectPathExamDataButton->setMinimumSize(QSize(75, 0));
        selectPathExamDataButton->setMaximumSize(QSize(100, 16777215));

        horizontalLayout_2->addWidget(selectPathExamDataButton);

        resetPathExamDataButton = new QPushButton(groupBox_2);
        resetPathExamDataButton->setObjectName("resetPathExamDataButton");
        resetPathExamDataButton->setMinimumSize(QSize(50, 0));
        resetPathExamDataButton->setMaximumSize(QSize(100, 16777215));

        horizontalLayout_2->addWidget(resetPathExamDataButton);


        verticalLayout_3->addWidget(groupBox_2);

        groupBox_3 = new QGroupBox(SettingsDialog);
        groupBox_3->setObjectName("groupBox_3");
        horizontalLayout_3 = new QHBoxLayout(groupBox_3);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        pathAutoSegmLEdit = new QLineEdit(groupBox_3);
        pathAutoSegmLEdit->setObjectName("pathAutoSegmLEdit");
        pathAutoSegmLEdit->setEnabled(false);
        pathAutoSegmLEdit->setMinimumSize(QSize(450, 20));

        horizontalLayout_3->addWidget(pathAutoSegmLEdit);

        selectPathAutoSegmButton = new QPushButton(groupBox_3);
        selectPathAutoSegmButton->setObjectName("selectPathAutoSegmButton");
        selectPathAutoSegmButton->setEnabled(false);
        selectPathAutoSegmButton->setMinimumSize(QSize(75, 0));
        selectPathAutoSegmButton->setMaximumSize(QSize(100, 16777215));

        horizontalLayout_3->addWidget(selectPathAutoSegmButton);

        resetPathAutoSegmButton = new QPushButton(groupBox_3);
        resetPathAutoSegmButton->setObjectName("resetPathAutoSegmButton");
        resetPathAutoSegmButton->setEnabled(false);
        resetPathAutoSegmButton->setMinimumSize(QSize(50, 0));
        resetPathAutoSegmButton->setMaximumSize(QSize(100, 16777215));

        horizontalLayout_3->addWidget(resetPathAutoSegmButton);


        verticalLayout_3->addWidget(groupBox_3);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        groupBox_5 = new QGroupBox(SettingsDialog);
        groupBox_5->setObjectName("groupBox_5");
        groupBox_5->setMinimumSize(QSize(211, 91));
        groupBox_5->setMaximumSize(QSize(450, 200));
        verticalLayout_4 = new QVBoxLayout(groupBox_5);
        verticalLayout_4->setObjectName("verticalLayout_4");
        showETDRSGridCBox = new QCheckBox(groupBox_5);
        showETDRSGridCBox->setObjectName("showETDRSGridCBox");

        verticalLayout_4->addWidget(showETDRSGridCBox);

        showCenterOnBscanCBox = new QCheckBox(groupBox_5);
        showCenterOnBscanCBox->setObjectName("showCenterOnBscanCBox");

        verticalLayout_4->addWidget(showCenterOnBscanCBox);

        showBscanOnErrorPlotCBox = new QCheckBox(groupBox_5);
        showBscanOnErrorPlotCBox->setObjectName("showBscanOnErrorPlotCBox");

        verticalLayout_4->addWidget(showBscanOnErrorPlotCBox);


        horizontalLayout_6->addWidget(groupBox_5);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        dataSaveStructureLabel = new QLabel(SettingsDialog);
        dataSaveStructureLabel->setObjectName("dataSaveStructureLabel");

        horizontalLayout_5->addWidget(dataSaveStructureLabel);

        dataSaveStructureCBox = new QComboBox(SettingsDialog);
        dataSaveStructureCBox->addItem(QString());
        dataSaveStructureCBox->addItem(QString());
        dataSaveStructureCBox->setObjectName("dataSaveStructureCBox");

        horizontalLayout_5->addWidget(dataSaveStructureCBox);


        verticalLayout_2->addLayout(horizontalLayout_5);

        blockPCVCBox = new QCheckBox(SettingsDialog);
        blockPCVCBox->setObjectName("blockPCVCBox");

        verticalLayout_2->addWidget(blockPCVCBox);

        verticalSpacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);

        acceptButton = new QPushButton(SettingsDialog);
        acceptButton->setObjectName("acceptButton");

        horizontalLayout_4->addWidget(acceptButton);

        cancelButton = new QPushButton(SettingsDialog);
        cancelButton->setObjectName("cancelButton");

        horizontalLayout_4->addWidget(cancelButton);


        verticalLayout_2->addLayout(horizontalLayout_4);


        horizontalLayout_6->addLayout(verticalLayout_2);


        verticalLayout_3->addLayout(horizontalLayout_6);


        retranslateUi(SettingsDialog);

        QMetaObject::connectSlotsByName(SettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *SettingsDialog)
    {
        SettingsDialog->setWindowTitle(QCoreApplication::translate("SettingsDialog", "Settings", nullptr));
        groupBox_6->setTitle(QCoreApplication::translate("SettingsDialog", "Patients database folder path (SQLite):", nullptr));
        selectPathDatabaseButton->setText(QCoreApplication::translate("SettingsDialog", "Browse", nullptr));
        resetPathDatabaseButton->setText(QCoreApplication::translate("SettingsDialog", "Reset", nullptr));
        groupBox->setTitle(QCoreApplication::translate("SettingsDialog", "Folder path to binary OCT data:", nullptr));
        selectPathOctDataButton->setText(QCoreApplication::translate("SettingsDialog", "Browse", nullptr));
        resetPathOctDataButton->setText(QCoreApplication::translate("SettingsDialog", "Reset", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("SettingsDialog", "Folder path to exam data:", nullptr));
        selectPathExamDataButton->setText(QCoreApplication::translate("SettingsDialog", "Browse", nullptr));
        resetPathExamDataButton->setText(QCoreApplication::translate("SettingsDialog", "Reset", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("SettingsDialog", "Folder path to auto segmentations:", nullptr));
        selectPathAutoSegmButton->setText(QCoreApplication::translate("SettingsDialog", "Browse", nullptr));
        resetPathAutoSegmButton->setText(QCoreApplication::translate("SettingsDialog", "Reset", nullptr));
        groupBox_5->setTitle(QCoreApplication::translate("SettingsDialog", "Display", nullptr));
        showETDRSGridCBox->setText(QCoreApplication::translate("SettingsDialog", "Show ETDRS grid", nullptr));
        showCenterOnBscanCBox->setText(QCoreApplication::translate("SettingsDialog", "Show scan center on B-scan image", nullptr));
        showBscanOnErrorPlotCBox->setText(QCoreApplication::translate("SettingsDialog", "Show B-scan image on error plot", nullptr));
        dataSaveStructureLabel->setText(QCoreApplication::translate("SettingsDialog", "Segmentation data save structure:", nullptr));
        dataSaveStructureCBox->setItemText(0, QCoreApplication::translate("SettingsDialog", "xml", nullptr));
        dataSaveStructureCBox->setItemText(1, QCoreApplication::translate("SettingsDialog", "txt", nullptr));

#if QT_CONFIG(tooltip)
        blockPCVCBox->setToolTip(QCoreApplication::translate("SettingsDialog", "Blokuj rysowanie PCV poni\305\274ej warstwy ILM", nullptr));
#endif // QT_CONFIG(tooltip)
        blockPCVCBox->setText(QCoreApplication::translate("SettingsDialog", "PCV can not be lower than ILM", nullptr));
        acceptButton->setText(QCoreApplication::translate("SettingsDialog", "OK", nullptr));
        cancelButton->setText(QCoreApplication::translate("SettingsDialog", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SettingsDialog: public Ui_SettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSDIALOG_H
