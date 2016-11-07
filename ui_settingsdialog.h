/********************************************************************************
** Form generated from reading UI file 'settingsdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSDIALOG_H
#define UI_SETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
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
    QLineEdit *pathOctExamLEdit;
    QPushButton *selectPathOctExamButton;
    QPushButton *resetPathOctExamButton;
    QGroupBox *groupBox_2;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *pathManualSegmLEdit;
    QPushButton *selectPathManualSegmButton;
    QPushButton *resetPathManualSegmButton;
    QGroupBox *groupBox_3;
    QHBoxLayout *horizontalLayout_3;
    QLineEdit *pathAutoSegmLEdit;
    QPushButton *selectPathAutoSegmButton;
    QPushButton *resetPathAutoSegmButton;
    QHBoxLayout *horizontalLayout_6;
    QGroupBox *groupBox_5;
    QCheckBox *showCenterOnBscanCBox;
    QCheckBox *showETDRSGridCBox;
    QCheckBox *showBscanOnErrorPlotCBox;
    QGroupBox *groupBox_4;
    QVBoxLayout *verticalLayout;
    QRadioButton *openBskanFirstRButton;
    QRadioButton *openBskanMiddleRButton;
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
            SettingsDialog->setObjectName(QStringLiteral("SettingsDialog"));
        SettingsDialog->resize(653, 363);
        SettingsDialog->setMinimumSize(QSize(653, 293));
        SettingsDialog->setMaximumSize(QSize(653, 363));
        verticalLayout_3 = new QVBoxLayout(SettingsDialog);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        groupBox_6 = new QGroupBox(SettingsDialog);
        groupBox_6->setObjectName(QStringLiteral("groupBox_6"));
        horizontalLayout_7 = new QHBoxLayout(groupBox_6);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        pathDatabaseLEdit = new QLineEdit(groupBox_6);
        pathDatabaseLEdit->setObjectName(QStringLiteral("pathDatabaseLEdit"));
        pathDatabaseLEdit->setMinimumSize(QSize(450, 20));

        horizontalLayout_7->addWidget(pathDatabaseLEdit);

        selectPathDatabaseButton = new QPushButton(groupBox_6);
        selectPathDatabaseButton->setObjectName(QStringLiteral("selectPathDatabaseButton"));
        selectPathDatabaseButton->setMinimumSize(QSize(75, 0));
        selectPathDatabaseButton->setMaximumSize(QSize(75, 16777215));

        horizontalLayout_7->addWidget(selectPathDatabaseButton);

        resetPathDatabaseButton = new QPushButton(groupBox_6);
        resetPathDatabaseButton->setObjectName(QStringLiteral("resetPathDatabaseButton"));
        resetPathDatabaseButton->setMinimumSize(QSize(50, 0));
        resetPathDatabaseButton->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_7->addWidget(resetPathDatabaseButton);


        verticalLayout_3->addWidget(groupBox_6);

        groupBox = new QGroupBox(SettingsDialog);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        horizontalLayout = new QHBoxLayout(groupBox);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        pathOctExamLEdit = new QLineEdit(groupBox);
        pathOctExamLEdit->setObjectName(QStringLiteral("pathOctExamLEdit"));
        pathOctExamLEdit->setMinimumSize(QSize(450, 20));

        horizontalLayout->addWidget(pathOctExamLEdit);

        selectPathOctExamButton = new QPushButton(groupBox);
        selectPathOctExamButton->setObjectName(QStringLiteral("selectPathOctExamButton"));
        selectPathOctExamButton->setMinimumSize(QSize(75, 0));
        selectPathOctExamButton->setMaximumSize(QSize(75, 16777215));

        horizontalLayout->addWidget(selectPathOctExamButton);

        resetPathOctExamButton = new QPushButton(groupBox);
        resetPathOctExamButton->setObjectName(QStringLiteral("resetPathOctExamButton"));
        resetPathOctExamButton->setMinimumSize(QSize(50, 0));
        resetPathOctExamButton->setMaximumSize(QSize(50, 16777215));

        horizontalLayout->addWidget(resetPathOctExamButton);


        verticalLayout_3->addWidget(groupBox);

        groupBox_2 = new QGroupBox(SettingsDialog);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        horizontalLayout_2 = new QHBoxLayout(groupBox_2);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        pathManualSegmLEdit = new QLineEdit(groupBox_2);
        pathManualSegmLEdit->setObjectName(QStringLiteral("pathManualSegmLEdit"));
        pathManualSegmLEdit->setMinimumSize(QSize(450, 20));

        horizontalLayout_2->addWidget(pathManualSegmLEdit);

        selectPathManualSegmButton = new QPushButton(groupBox_2);
        selectPathManualSegmButton->setObjectName(QStringLiteral("selectPathManualSegmButton"));
        selectPathManualSegmButton->setMinimumSize(QSize(75, 0));
        selectPathManualSegmButton->setMaximumSize(QSize(75, 16777215));

        horizontalLayout_2->addWidget(selectPathManualSegmButton);

        resetPathManualSegmButton = new QPushButton(groupBox_2);
        resetPathManualSegmButton->setObjectName(QStringLiteral("resetPathManualSegmButton"));
        resetPathManualSegmButton->setMinimumSize(QSize(50, 0));
        resetPathManualSegmButton->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_2->addWidget(resetPathManualSegmButton);


        verticalLayout_3->addWidget(groupBox_2);

        groupBox_3 = new QGroupBox(SettingsDialog);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        horizontalLayout_3 = new QHBoxLayout(groupBox_3);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        pathAutoSegmLEdit = new QLineEdit(groupBox_3);
        pathAutoSegmLEdit->setObjectName(QStringLiteral("pathAutoSegmLEdit"));
        pathAutoSegmLEdit->setMinimumSize(QSize(450, 20));

        horizontalLayout_3->addWidget(pathAutoSegmLEdit);

        selectPathAutoSegmButton = new QPushButton(groupBox_3);
        selectPathAutoSegmButton->setObjectName(QStringLiteral("selectPathAutoSegmButton"));
        selectPathAutoSegmButton->setMinimumSize(QSize(75, 0));
        selectPathAutoSegmButton->setMaximumSize(QSize(75, 16777215));

        horizontalLayout_3->addWidget(selectPathAutoSegmButton);

        resetPathAutoSegmButton = new QPushButton(groupBox_3);
        resetPathAutoSegmButton->setObjectName(QStringLiteral("resetPathAutoSegmButton"));
        resetPathAutoSegmButton->setMinimumSize(QSize(50, 0));
        resetPathAutoSegmButton->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_3->addWidget(resetPathAutoSegmButton);


        verticalLayout_3->addWidget(groupBox_3);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        groupBox_5 = new QGroupBox(SettingsDialog);
        groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
        groupBox_5->setMinimumSize(QSize(211, 91));
        groupBox_5->setMaximumSize(QSize(211, 91));
        showCenterOnBscanCBox = new QCheckBox(groupBox_5);
        showCenterOnBscanCBox->setObjectName(QStringLiteral("showCenterOnBscanCBox"));
        showCenterOnBscanCBox->setGeometry(QRect(10, 40, 181, 17));
        showETDRSGridCBox = new QCheckBox(groupBox_5);
        showETDRSGridCBox->setObjectName(QStringLiteral("showETDRSGridCBox"));
        showETDRSGridCBox->setGeometry(QRect(10, 20, 131, 21));
        showBscanOnErrorPlotCBox = new QCheckBox(groupBox_5);
        showBscanOnErrorPlotCBox->setObjectName(QStringLiteral("showBscanOnErrorPlotCBox"));
        showBscanOnErrorPlotCBox->setGeometry(QRect(10, 60, 191, 17));

        horizontalLayout_6->addWidget(groupBox_5);

        groupBox_4 = new QGroupBox(SettingsDialog);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        groupBox_4->setMinimumSize(QSize(161, 91));
        groupBox_4->setMaximumSize(QSize(161, 91));
        verticalLayout = new QVBoxLayout(groupBox_4);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        openBskanFirstRButton = new QRadioButton(groupBox_4);
        openBskanFirstRButton->setObjectName(QStringLiteral("openBskanFirstRButton"));
        openBskanFirstRButton->setMinimumSize(QSize(100, 0));

        verticalLayout->addWidget(openBskanFirstRButton);

        openBskanMiddleRButton = new QRadioButton(groupBox_4);
        openBskanMiddleRButton->setObjectName(QStringLiteral("openBskanMiddleRButton"));

        verticalLayout->addWidget(openBskanMiddleRButton);


        horizontalLayout_6->addWidget(groupBox_4);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        dataSaveStructureLabel = new QLabel(SettingsDialog);
        dataSaveStructureLabel->setObjectName(QStringLiteral("dataSaveStructureLabel"));

        horizontalLayout_5->addWidget(dataSaveStructureLabel);

        dataSaveStructureCBox = new QComboBox(SettingsDialog);
        dataSaveStructureCBox->setObjectName(QStringLiteral("dataSaveStructureCBox"));

        horizontalLayout_5->addWidget(dataSaveStructureCBox);


        verticalLayout_2->addLayout(horizontalLayout_5);

        blockPCVCBox = new QCheckBox(SettingsDialog);
        blockPCVCBox->setObjectName(QStringLiteral("blockPCVCBox"));

        verticalLayout_2->addWidget(blockPCVCBox);

        verticalSpacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);

        acceptButton = new QPushButton(SettingsDialog);
        acceptButton->setObjectName(QStringLiteral("acceptButton"));

        horizontalLayout_4->addWidget(acceptButton);

        cancelButton = new QPushButton(SettingsDialog);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));

        horizontalLayout_4->addWidget(cancelButton);


        verticalLayout_2->addLayout(horizontalLayout_4);


        horizontalLayout_6->addLayout(verticalLayout_2);


        verticalLayout_3->addLayout(horizontalLayout_6);


        retranslateUi(SettingsDialog);

        QMetaObject::connectSlotsByName(SettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *SettingsDialog)
    {
        SettingsDialog->setWindowTitle(QApplication::translate("SettingsDialog", "Dialog", 0));
        groupBox_6->setTitle(QApplication::translate("SettingsDialog", "Folder zawieraj\304\205cy baz\304\231 danych pacjent\303\263w (SQLite):", 0));
        selectPathDatabaseButton->setText(QApplication::translate("SettingsDialog", "Wybierz", 0));
        resetPathDatabaseButton->setText(QApplication::translate("SettingsDialog", "Reset", 0));
        groupBox->setTitle(QApplication::translate("SettingsDialog", "Folder zawieraj\304\205cy badania OCT:", 0));
        selectPathOctExamButton->setText(QApplication::translate("SettingsDialog", "Wybierz", 0));
        resetPathOctExamButton->setText(QApplication::translate("SettingsDialog", "Reset", 0));
        groupBox_2->setTitle(QApplication::translate("SettingsDialog", "Folder zawieraj\304\205cy r\304\231czn\304\205 segmentacj\304\231:", 0));
        selectPathManualSegmButton->setText(QApplication::translate("SettingsDialog", "Wybierz", 0));
        resetPathManualSegmButton->setText(QApplication::translate("SettingsDialog", "Reset", 0));
        groupBox_3->setTitle(QApplication::translate("SettingsDialog", "Folder zawieraj\304\205cy automatyczn\304\205 segmentacj\304\231:", 0));
        selectPathAutoSegmButton->setText(QApplication::translate("SettingsDialog", "Wybierz", 0));
        resetPathAutoSegmButton->setText(QApplication::translate("SettingsDialog", "Reset", 0));
        groupBox_5->setTitle(QApplication::translate("SettingsDialog", "Wy\305\233wietlanie", 0));
        showCenterOnBscanCBox->setText(QApplication::translate("SettingsDialog", "Poka\305\274 \305\233rodek skanu na B-skanie", 0));
        showETDRSGridCBox->setText(QApplication::translate("SettingsDialog", "Poka\305\274 siatk\304\231 ETDRS", 0));
        showBscanOnErrorPlotCBox->setText(QApplication::translate("SettingsDialog", "Poka\305\274 B-skan na wykresie b\305\202\304\231du", 0));
        groupBox_4->setTitle(QApplication::translate("SettingsDialog", "Otwieraj badania na skanie:", 0));
        openBskanFirstRButton->setText(QApplication::translate("SettingsDialog", "pierwszym", 0));
        openBskanMiddleRButton->setText(QApplication::translate("SettingsDialog", "\305\233rodkowym", 0));
        dataSaveStructureLabel->setText(QApplication::translate("SettingsDialog", "Struktura zapisu danych segmentacji:", 0));
        dataSaveStructureCBox->clear();
        dataSaveStructureCBox->insertItems(0, QStringList()
         << QApplication::translate("SettingsDialog", "xml", 0)
         << QApplication::translate("SettingsDialog", "txt", 0)
        );
#ifndef QT_NO_TOOLTIP
        blockPCVCBox->setToolTip(QApplication::translate("SettingsDialog", "Blokuj rysowanie PCV poni\305\274ej warstwy ILM", 0));
#endif // QT_NO_TOOLTIP
        blockPCVCBox->setText(QApplication::translate("SettingsDialog", "Blokada PCV poni\305\274ej ILM", 0));
        acceptButton->setText(QApplication::translate("SettingsDialog", "OK", 0));
        cancelButton->setText(QApplication::translate("SettingsDialog", "Cancel", 0));
    } // retranslateUi

};

namespace Ui {
    class SettingsDialog: public Ui_SettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSDIALOG_H
