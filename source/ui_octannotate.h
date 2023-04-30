/********************************************************************************
** Form generated from reading UI file 'octannotate.ui'
**
** Created by: Qt User Interface Compiler version 6.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OCTANNOTATE_H
#define UI_OCTANNOTATE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_OCTAnnotate
{
public:
    QAction *actionLoadOCTSequence;
    QAction *actionClose;
    QAction *actionSettings;
    QAction *actionImageFlattening;
    QAction *actionLoadOCTFile;
    QAction *actionReadManualAnnotations;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout_11;
    QTabWidget *tabWidget;
    QWidget *tabOCTExam;
    QHBoxLayout *horizontalLayout_16;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_2;
    QFormLayout *formLayout_5;
    QLabel *label_2;
    QLabel *imageNumberLabel;
    QGridLayout *gridLayout_5;
    QPushButton *zoomOutButton;
    QPushButton *zoomInButton;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout_2;
    QCustomPlot *bScanHCPlot;
    QHBoxLayout *horizontalLayout;
    QPushButton *prevImageButton;
    QSpacerItem *horizontalSpacer;
    QLineEdit *currImageNumberLEdit;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *nextImageButton;
    QVBoxLayout *verticalLayout_3;
    QCustomPlot *bScanVCPlot;
    QHBoxLayout *horizontalLayout_9;
    QPushButton *prevNormalImageButton;
    QSpacerItem *horizontalSpacer_3;
    QLineEdit *currNormalImageNumberLEdit;
    QSpacerItem *horizontalSpacer_10;
    QPushButton *nextNormalImageButton;
    QVBoxLayout *verticalLayout_6;
    QGroupBox *editLayerGBox;
    QGridLayout *gridLayout;
    QCheckBox *allLayersCBox;
    QLabel *opl_onlAnnotCountLabel;
    QCheckBox *mezLayerCBox;
    QLabel *ipl_inlAnnotCountLabel;
    QLabel *obrpeAnnotCountLabel;
    QCheckBox *gcl_iplLayerCBox;
    QLabel *obermColorLabel;
    QLabel *mezAnnotCountLabel;
    QCheckBox *pcvLayerCBox;
    QLabel *ilmAnnotCountLabel;
    QLabel *ibermColorLabel;
    QRadioButton *opl_onlLayerRButton;
    QRadioButton *mezLayerRButton;
    QLabel *iboprAnnotCountLabel;
    QRadioButton *pcvLayerRButton;
    QLabel *inl_oplAnnotCountLabel;
    QCheckBox *iosLayerCBox;
    QLabel *pcvColorLabel;
    QRadioButton *ipl_inlLayerRButton;
    QRadioButton *rnfl_gclLayerRButton;
    QLabel *ibrpeColorLabel;
    QLabel *iosColorLabel;
    QRadioButton *obrpeLayerRButton;
    QRadioButton *ibrpeLayerRButton;
    QCheckBox *ilmLayerCBox;
    QLabel *rnfl_gclAnnotCountLabel;
    QLabel *ibrpeAnnotCountLabel;
    QCheckBox *elmLayerCBox;
    QRadioButton *iosLayerRButton;
    QRadioButton *iboprLayerRButton;
    QLabel *gcl_iplAnnotCountLabel;
    QLabel *ilmColorLabel;
    QRadioButton *gcl_iplLayerRButton;
    QLabel *rnfl_gclColorLabel;
    QLabel *elmAnnotCountLabel;
    QCheckBox *ipl_inlLayerCBox;
    QCheckBox *ibermLayerCBox;
    QLabel *elmColorLabel;
    QCheckBox *iboprLayerCBox;
    QRadioButton *obermLayerRButton;
    QCheckBox *obermLayerCBox;
    QRadioButton *elmLayerRButton;
    QCheckBox *inl_oplLayerCBox;
    QLabel *pcvAnnotCountLabel;
    QLabel *opl_onlColorLabel;
    QCheckBox *ibrpeLayerCBox;
    QLabel *ipl_inlColorLabel;
    QLabel *ibermAnnotCountLabel;
    QLabel *iosAnnotCountLabel;
    QCheckBox *obrpeLayerCBox;
    QLabel *obrpeColorLabel;
    QLabel *ibermAnnotCountLabel_2;
    QCheckBox *opl_onlLayerCBox;
    QLabel *inl_oplColorLabel;
    QRadioButton *ilmLayerRButton;
    QLabel *gcl_iplColorLabel;
    QCheckBox *rnfl_gclLayerCBox;
    QRadioButton *ibermLayerRButton;
    QRadioButton *inl_oplLayerRButton;
    QLabel *mezColorLabel;
    QLabel *iboprColorLabel;
    QFormLayout *formLayout_4;
    QPushButton *contrastResetButton;
    QSlider *contrastSlider;
    QPushButton *brightnessResetButton;
    QSlider *brightnessSlider;
    QSpacerItem *verticalSpacer_2;
    QLabel *fundusImageLabel;
    QMenuBar *menuBar;
    QMenu *menuPatient;
    QMenu *menuSegmentations_2;
    QMenu *menuProgram;
    QStatusBar *statusBar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *OCTAnnotate)
    {
        if (OCTAnnotate->objectName().isEmpty())
            OCTAnnotate->setObjectName("OCTAnnotate");
        OCTAnnotate->resize(1260, 850);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(OCTAnnotate->sizePolicy().hasHeightForWidth());
        OCTAnnotate->setSizePolicy(sizePolicy);
        OCTAnnotate->setMinimumSize(QSize(1260, 850));
        actionLoadOCTSequence = new QAction(OCTAnnotate);
        actionLoadOCTSequence->setObjectName("actionLoadOCTSequence");
        actionClose = new QAction(OCTAnnotate);
        actionClose->setObjectName("actionClose");
        actionSettings = new QAction(OCTAnnotate);
        actionSettings->setObjectName("actionSettings");
        actionImageFlattening = new QAction(OCTAnnotate);
        actionImageFlattening->setObjectName("actionImageFlattening");
        actionImageFlattening->setCheckable(true);
        actionLoadOCTFile = new QAction(OCTAnnotate);
        actionLoadOCTFile->setObjectName("actionLoadOCTFile");
        actionReadManualAnnotations = new QAction(OCTAnnotate);
        actionReadManualAnnotations->setObjectName("actionReadManualAnnotations");
        centralWidget = new QWidget(OCTAnnotate);
        centralWidget->setObjectName("centralWidget");
        centralWidget->setMaximumSize(QSize(6000, 3000));
        horizontalLayout_11 = new QHBoxLayout(centralWidget);
        horizontalLayout_11->setSpacing(6);
        horizontalLayout_11->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_11->setObjectName("horizontalLayout_11");
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName("tabWidget");
        tabWidget->setMinimumSize(QSize(1076, 619));
        tabWidget->setFocusPolicy(Qt::NoFocus);
        tabOCTExam = new QWidget();
        tabOCTExam->setObjectName("tabOCTExam");
        horizontalLayout_16 = new QHBoxLayout(tabOCTExam);
        horizontalLayout_16->setSpacing(6);
        horizontalLayout_16->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_16->setObjectName("horizontalLayout_16");
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setObjectName("verticalLayout_4");
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        formLayout_5 = new QFormLayout();
        formLayout_5->setSpacing(6);
        formLayout_5->setObjectName("formLayout_5");
        formLayout_5->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
        label_2 = new QLabel(tabOCTExam);
        label_2->setObjectName("label_2");
        label_2->setMinimumSize(QSize(40, 23));
        label_2->setMaximumSize(QSize(40, 23));

        formLayout_5->setWidget(0, QFormLayout::LabelRole, label_2);

        imageNumberLabel = new QLabel(tabOCTExam);
        imageNumberLabel->setObjectName("imageNumberLabel");
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(imageNumberLabel->sizePolicy().hasHeightForWidth());
        imageNumberLabel->setSizePolicy(sizePolicy1);
        imageNumberLabel->setMinimumSize(QSize(100, 23));
        imageNumberLabel->setMaximumSize(QSize(16777215, 23));

        formLayout_5->setWidget(0, QFormLayout::FieldRole, imageNumberLabel);


        horizontalLayout_2->addLayout(formLayout_5);

        gridLayout_5 = new QGridLayout();
        gridLayout_5->setSpacing(6);
        gridLayout_5->setObjectName("gridLayout_5");
        gridLayout_5->setSizeConstraint(QLayout::SetMaximumSize);
        zoomOutButton = new QPushButton(tabOCTExam);
        zoomOutButton->setObjectName("zoomOutButton");
        zoomOutButton->setEnabled(false);
        zoomOutButton->setMaximumSize(QSize(37, 23));
        zoomOutButton->setFocusPolicy(Qt::NoFocus);

        gridLayout_5->addWidget(zoomOutButton, 0, 1, 1, 1);

        zoomInButton = new QPushButton(tabOCTExam);
        zoomInButton->setObjectName("zoomInButton");
        zoomInButton->setEnabled(false);
        zoomInButton->setMaximumSize(QSize(36, 23));
        zoomInButton->setFocusPolicy(Qt::NoFocus);

        gridLayout_5->addWidget(zoomInButton, 0, 0, 1, 1);


        horizontalLayout_2->addLayout(gridLayout_5);

        horizontalLayout_2->setStretch(0, 3);

        verticalLayout_4->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName("verticalLayout_2");
        bScanHCPlot = new QCustomPlot(tabOCTExam);
        bScanHCPlot->setObjectName("bScanHCPlot");
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(bScanHCPlot->sizePolicy().hasHeightForWidth());
        bScanHCPlot->setSizePolicy(sizePolicy2);
        bScanHCPlot->setMinimumSize(QSize(400, 600));
        bScanHCPlot->setMaximumSize(QSize(1500, 16777215));

        verticalLayout_2->addWidget(bScanHCPlot);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName("horizontalLayout");
        prevImageButton = new QPushButton(tabOCTExam);
        prevImageButton->setObjectName("prevImageButton");
        prevImageButton->setEnabled(false);
        prevImageButton->setMinimumSize(QSize(100, 0));

        horizontalLayout->addWidget(prevImageButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        currImageNumberLEdit = new QLineEdit(tabOCTExam);
        currImageNumberLEdit->setObjectName("currImageNumberLEdit");
        currImageNumberLEdit->setMinimumSize(QSize(50, 0));
        currImageNumberLEdit->setMaximumSize(QSize(100, 16777215));
        currImageNumberLEdit->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(currImageNumberLEdit);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        nextImageButton = new QPushButton(tabOCTExam);
        nextImageButton->setObjectName("nextImageButton");
        nextImageButton->setEnabled(false);
        nextImageButton->setMinimumSize(QSize(100, 0));

        horizontalLayout->addWidget(nextImageButton);


        verticalLayout_2->addLayout(horizontalLayout);


        horizontalLayout_3->addLayout(verticalLayout_2);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setObjectName("verticalLayout_3");
        bScanVCPlot = new QCustomPlot(tabOCTExam);
        bScanVCPlot->setObjectName("bScanVCPlot");
        sizePolicy2.setHeightForWidth(bScanVCPlot->sizePolicy().hasHeightForWidth());
        bScanVCPlot->setSizePolicy(sizePolicy2);
        bScanVCPlot->setMinimumSize(QSize(400, 600));
        bScanVCPlot->setMaximumSize(QSize(1500, 16777215));

        verticalLayout_3->addWidget(bScanVCPlot);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setObjectName("horizontalLayout_9");
        prevNormalImageButton = new QPushButton(tabOCTExam);
        prevNormalImageButton->setObjectName("prevNormalImageButton");
        prevNormalImageButton->setEnabled(false);
        prevNormalImageButton->setMinimumSize(QSize(100, 0));

        horizontalLayout_9->addWidget(prevNormalImageButton);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer_3);

        currNormalImageNumberLEdit = new QLineEdit(tabOCTExam);
        currNormalImageNumberLEdit->setObjectName("currNormalImageNumberLEdit");
        currNormalImageNumberLEdit->setMinimumSize(QSize(50, 0));
        currNormalImageNumberLEdit->setMaximumSize(QSize(100, 16777215));
        currNormalImageNumberLEdit->setAlignment(Qt::AlignCenter);

        horizontalLayout_9->addWidget(currNormalImageNumberLEdit);

        horizontalSpacer_10 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer_10);

        nextNormalImageButton = new QPushButton(tabOCTExam);
        nextNormalImageButton->setObjectName("nextNormalImageButton");
        nextNormalImageButton->setEnabled(false);
        nextNormalImageButton->setMinimumSize(QSize(100, 0));

        horizontalLayout_9->addWidget(nextNormalImageButton);


        verticalLayout_3->addLayout(horizontalLayout_9);


        horizontalLayout_3->addLayout(verticalLayout_3);


        verticalLayout_4->addLayout(horizontalLayout_3);


        horizontalLayout_16->addLayout(verticalLayout_4);

        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setObjectName("verticalLayout_6");
        editLayerGBox = new QGroupBox(tabOCTExam);
        editLayerGBox->setObjectName("editLayerGBox");
        editLayerGBox->setEnabled(true);
        editLayerGBox->setMinimumSize(QSize(221, 311));
        editLayerGBox->setMaximumSize(QSize(221, 1000));
        gridLayout = new QGridLayout(editLayerGBox);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName("gridLayout");
        allLayersCBox = new QCheckBox(editLayerGBox);
        allLayersCBox->setObjectName("allLayersCBox");
        allLayersCBox->setChecked(true);

        gridLayout->addWidget(allLayersCBox, 0, 0, 1, 4);

        opl_onlAnnotCountLabel = new QLabel(editLayerGBox);
        opl_onlAnnotCountLabel->setObjectName("opl_onlAnnotCountLabel");

        gridLayout->addWidget(opl_onlAnnotCountLabel, 9, 3, 1, 1);

        mezLayerCBox = new QCheckBox(editLayerGBox);
        mezLayerCBox->setObjectName("mezLayerCBox");
        mezLayerCBox->setChecked(true);

        gridLayout->addWidget(mezLayerCBox, 11, 0, 1, 1);

        ipl_inlAnnotCountLabel = new QLabel(editLayerGBox);
        ipl_inlAnnotCountLabel->setObjectName("ipl_inlAnnotCountLabel");

        gridLayout->addWidget(ipl_inlAnnotCountLabel, 7, 3, 1, 1);

        obrpeAnnotCountLabel = new QLabel(editLayerGBox);
        obrpeAnnotCountLabel->setObjectName("obrpeAnnotCountLabel");

        gridLayout->addWidget(obrpeAnnotCountLabel, 15, 3, 1, 1);

        gcl_iplLayerCBox = new QCheckBox(editLayerGBox);
        gcl_iplLayerCBox->setObjectName("gcl_iplLayerCBox");
        gcl_iplLayerCBox->setFocusPolicy(Qt::NoFocus);
        gcl_iplLayerCBox->setChecked(true);

        gridLayout->addWidget(gcl_iplLayerCBox, 6, 0, 1, 1);

        obermColorLabel = new QLabel(editLayerGBox);
        obermColorLabel->setObjectName("obermColorLabel");
        obermColorLabel->setFrameShape(QFrame::Box);

        gridLayout->addWidget(obermColorLabel, 3, 2, 1, 1);

        mezAnnotCountLabel = new QLabel(editLayerGBox);
        mezAnnotCountLabel->setObjectName("mezAnnotCountLabel");

        gridLayout->addWidget(mezAnnotCountLabel, 11, 3, 1, 1);

        pcvLayerCBox = new QCheckBox(editLayerGBox);
        pcvLayerCBox->setObjectName("pcvLayerCBox");
        pcvLayerCBox->setFocusPolicy(Qt::NoFocus);
        pcvLayerCBox->setChecked(true);

        gridLayout->addWidget(pcvLayerCBox, 1, 0, 1, 1);

        ilmAnnotCountLabel = new QLabel(editLayerGBox);
        ilmAnnotCountLabel->setObjectName("ilmAnnotCountLabel");

        gridLayout->addWidget(ilmAnnotCountLabel, 4, 3, 1, 1);

        ibermColorLabel = new QLabel(editLayerGBox);
        ibermColorLabel->setObjectName("ibermColorLabel");
        ibermColorLabel->setFrameShape(QFrame::Box);

        gridLayout->addWidget(ibermColorLabel, 2, 2, 1, 1);

        opl_onlLayerRButton = new QRadioButton(editLayerGBox);
        opl_onlLayerRButton->setObjectName("opl_onlLayerRButton");
        opl_onlLayerRButton->setFocusPolicy(Qt::NoFocus);
        opl_onlLayerRButton->setChecked(false);

        gridLayout->addWidget(opl_onlLayerRButton, 9, 1, 1, 1);

        mezLayerRButton = new QRadioButton(editLayerGBox);
        mezLayerRButton->setObjectName("mezLayerRButton");

        gridLayout->addWidget(mezLayerRButton, 11, 1, 1, 1);

        iboprAnnotCountLabel = new QLabel(editLayerGBox);
        iboprAnnotCountLabel->setObjectName("iboprAnnotCountLabel");

        gridLayout->addWidget(iboprAnnotCountLabel, 13, 3, 1, 1);

        pcvLayerRButton = new QRadioButton(editLayerGBox);
        pcvLayerRButton->setObjectName("pcvLayerRButton");
        pcvLayerRButton->setFocusPolicy(Qt::NoFocus);
        pcvLayerRButton->setChecked(true);

        gridLayout->addWidget(pcvLayerRButton, 1, 1, 1, 1);

        inl_oplAnnotCountLabel = new QLabel(editLayerGBox);
        inl_oplAnnotCountLabel->setObjectName("inl_oplAnnotCountLabel");

        gridLayout->addWidget(inl_oplAnnotCountLabel, 8, 3, 1, 1);

        iosLayerCBox = new QCheckBox(editLayerGBox);
        iosLayerCBox->setObjectName("iosLayerCBox");
        iosLayerCBox->setChecked(true);

        gridLayout->addWidget(iosLayerCBox, 12, 0, 1, 1);

        pcvColorLabel = new QLabel(editLayerGBox);
        pcvColorLabel->setObjectName("pcvColorLabel");
        pcvColorLabel->setFrameShape(QFrame::Box);

        gridLayout->addWidget(pcvColorLabel, 1, 2, 1, 1);

        ipl_inlLayerRButton = new QRadioButton(editLayerGBox);
        ipl_inlLayerRButton->setObjectName("ipl_inlLayerRButton");
        ipl_inlLayerRButton->setFocusPolicy(Qt::NoFocus);
        ipl_inlLayerRButton->setChecked(false);

        gridLayout->addWidget(ipl_inlLayerRButton, 7, 1, 1, 1);

        rnfl_gclLayerRButton = new QRadioButton(editLayerGBox);
        rnfl_gclLayerRButton->setObjectName("rnfl_gclLayerRButton");
        rnfl_gclLayerRButton->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(rnfl_gclLayerRButton, 5, 1, 1, 1);

        ibrpeColorLabel = new QLabel(editLayerGBox);
        ibrpeColorLabel->setObjectName("ibrpeColorLabel");
        ibrpeColorLabel->setFrameShape(QFrame::Box);

        gridLayout->addWidget(ibrpeColorLabel, 14, 2, 1, 1);

        iosColorLabel = new QLabel(editLayerGBox);
        iosColorLabel->setObjectName("iosColorLabel");
        iosColorLabel->setFrameShape(QFrame::Box);

        gridLayout->addWidget(iosColorLabel, 12, 2, 1, 1);

        obrpeLayerRButton = new QRadioButton(editLayerGBox);
        obrpeLayerRButton->setObjectName("obrpeLayerRButton");

        gridLayout->addWidget(obrpeLayerRButton, 15, 1, 1, 1);

        ibrpeLayerRButton = new QRadioButton(editLayerGBox);
        ibrpeLayerRButton->setObjectName("ibrpeLayerRButton");

        gridLayout->addWidget(ibrpeLayerRButton, 14, 1, 1, 1);

        ilmLayerCBox = new QCheckBox(editLayerGBox);
        ilmLayerCBox->setObjectName("ilmLayerCBox");
        ilmLayerCBox->setFocusPolicy(Qt::NoFocus);
        ilmLayerCBox->setChecked(true);

        gridLayout->addWidget(ilmLayerCBox, 4, 0, 1, 1);

        rnfl_gclAnnotCountLabel = new QLabel(editLayerGBox);
        rnfl_gclAnnotCountLabel->setObjectName("rnfl_gclAnnotCountLabel");

        gridLayout->addWidget(rnfl_gclAnnotCountLabel, 5, 3, 1, 1);

        ibrpeAnnotCountLabel = new QLabel(editLayerGBox);
        ibrpeAnnotCountLabel->setObjectName("ibrpeAnnotCountLabel");

        gridLayout->addWidget(ibrpeAnnotCountLabel, 14, 3, 1, 1);

        elmLayerCBox = new QCheckBox(editLayerGBox);
        elmLayerCBox->setObjectName("elmLayerCBox");
        elmLayerCBox->setChecked(true);

        gridLayout->addWidget(elmLayerCBox, 10, 0, 1, 1);

        iosLayerRButton = new QRadioButton(editLayerGBox);
        iosLayerRButton->setObjectName("iosLayerRButton");

        gridLayout->addWidget(iosLayerRButton, 12, 1, 1, 1);

        iboprLayerRButton = new QRadioButton(editLayerGBox);
        iboprLayerRButton->setObjectName("iboprLayerRButton");

        gridLayout->addWidget(iboprLayerRButton, 13, 1, 1, 1);

        gcl_iplAnnotCountLabel = new QLabel(editLayerGBox);
        gcl_iplAnnotCountLabel->setObjectName("gcl_iplAnnotCountLabel");

        gridLayout->addWidget(gcl_iplAnnotCountLabel, 6, 3, 1, 1);

        ilmColorLabel = new QLabel(editLayerGBox);
        ilmColorLabel->setObjectName("ilmColorLabel");
        ilmColorLabel->setFrameShape(QFrame::Box);

        gridLayout->addWidget(ilmColorLabel, 4, 2, 1, 1);

        gcl_iplLayerRButton = new QRadioButton(editLayerGBox);
        gcl_iplLayerRButton->setObjectName("gcl_iplLayerRButton");
        gcl_iplLayerRButton->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(gcl_iplLayerRButton, 6, 1, 1, 1);

        rnfl_gclColorLabel = new QLabel(editLayerGBox);
        rnfl_gclColorLabel->setObjectName("rnfl_gclColorLabel");
        rnfl_gclColorLabel->setFrameShape(QFrame::Box);

        gridLayout->addWidget(rnfl_gclColorLabel, 5, 2, 1, 1);

        elmAnnotCountLabel = new QLabel(editLayerGBox);
        elmAnnotCountLabel->setObjectName("elmAnnotCountLabel");

        gridLayout->addWidget(elmAnnotCountLabel, 10, 3, 1, 1);

        ipl_inlLayerCBox = new QCheckBox(editLayerGBox);
        ipl_inlLayerCBox->setObjectName("ipl_inlLayerCBox");
        ipl_inlLayerCBox->setFocusPolicy(Qt::NoFocus);
        ipl_inlLayerCBox->setChecked(true);

        gridLayout->addWidget(ipl_inlLayerCBox, 7, 0, 1, 1);

        ibermLayerCBox = new QCheckBox(editLayerGBox);
        ibermLayerCBox->setObjectName("ibermLayerCBox");
        ibermLayerCBox->setFocusPolicy(Qt::NoFocus);
        ibermLayerCBox->setChecked(true);

        gridLayout->addWidget(ibermLayerCBox, 2, 0, 1, 1);

        elmColorLabel = new QLabel(editLayerGBox);
        elmColorLabel->setObjectName("elmColorLabel");
        elmColorLabel->setFrameShape(QFrame::Box);

        gridLayout->addWidget(elmColorLabel, 10, 2, 1, 1);

        iboprLayerCBox = new QCheckBox(editLayerGBox);
        iboprLayerCBox->setObjectName("iboprLayerCBox");
        iboprLayerCBox->setChecked(true);

        gridLayout->addWidget(iboprLayerCBox, 13, 0, 1, 1);

        obermLayerRButton = new QRadioButton(editLayerGBox);
        obermLayerRButton->setObjectName("obermLayerRButton");

        gridLayout->addWidget(obermLayerRButton, 3, 1, 1, 1);

        obermLayerCBox = new QCheckBox(editLayerGBox);
        obermLayerCBox->setObjectName("obermLayerCBox");
        obermLayerCBox->setChecked(true);

        gridLayout->addWidget(obermLayerCBox, 3, 0, 1, 1);

        elmLayerRButton = new QRadioButton(editLayerGBox);
        elmLayerRButton->setObjectName("elmLayerRButton");
        elmLayerRButton->setEnabled(true);
        elmLayerRButton->setChecked(false);

        gridLayout->addWidget(elmLayerRButton, 10, 1, 1, 1);

        inl_oplLayerCBox = new QCheckBox(editLayerGBox);
        inl_oplLayerCBox->setObjectName("inl_oplLayerCBox");
        inl_oplLayerCBox->setFocusPolicy(Qt::NoFocus);
        inl_oplLayerCBox->setChecked(true);

        gridLayout->addWidget(inl_oplLayerCBox, 8, 0, 1, 1);

        pcvAnnotCountLabel = new QLabel(editLayerGBox);
        pcvAnnotCountLabel->setObjectName("pcvAnnotCountLabel");

        gridLayout->addWidget(pcvAnnotCountLabel, 1, 3, 1, 1);

        opl_onlColorLabel = new QLabel(editLayerGBox);
        opl_onlColorLabel->setObjectName("opl_onlColorLabel");
        opl_onlColorLabel->setFrameShape(QFrame::Box);

        gridLayout->addWidget(opl_onlColorLabel, 9, 2, 1, 1);

        ibrpeLayerCBox = new QCheckBox(editLayerGBox);
        ibrpeLayerCBox->setObjectName("ibrpeLayerCBox");
        ibrpeLayerCBox->setChecked(true);

        gridLayout->addWidget(ibrpeLayerCBox, 14, 0, 1, 1);

        ipl_inlColorLabel = new QLabel(editLayerGBox);
        ipl_inlColorLabel->setObjectName("ipl_inlColorLabel");
        ipl_inlColorLabel->setFrameShape(QFrame::Box);

        gridLayout->addWidget(ipl_inlColorLabel, 7, 2, 1, 1);

        ibermAnnotCountLabel = new QLabel(editLayerGBox);
        ibermAnnotCountLabel->setObjectName("ibermAnnotCountLabel");

        gridLayout->addWidget(ibermAnnotCountLabel, 2, 3, 1, 1);

        iosAnnotCountLabel = new QLabel(editLayerGBox);
        iosAnnotCountLabel->setObjectName("iosAnnotCountLabel");

        gridLayout->addWidget(iosAnnotCountLabel, 12, 3, 1, 1);

        obrpeLayerCBox = new QCheckBox(editLayerGBox);
        obrpeLayerCBox->setObjectName("obrpeLayerCBox");
        obrpeLayerCBox->setChecked(true);

        gridLayout->addWidget(obrpeLayerCBox, 15, 0, 1, 1);

        obrpeColorLabel = new QLabel(editLayerGBox);
        obrpeColorLabel->setObjectName("obrpeColorLabel");
        obrpeColorLabel->setFrameShape(QFrame::Box);

        gridLayout->addWidget(obrpeColorLabel, 15, 2, 1, 1);

        ibermAnnotCountLabel_2 = new QLabel(editLayerGBox);
        ibermAnnotCountLabel_2->setObjectName("ibermAnnotCountLabel_2");

        gridLayout->addWidget(ibermAnnotCountLabel_2, 3, 3, 1, 1);

        opl_onlLayerCBox = new QCheckBox(editLayerGBox);
        opl_onlLayerCBox->setObjectName("opl_onlLayerCBox");
        opl_onlLayerCBox->setFocusPolicy(Qt::NoFocus);
        opl_onlLayerCBox->setChecked(true);

        gridLayout->addWidget(opl_onlLayerCBox, 9, 0, 1, 1);

        inl_oplColorLabel = new QLabel(editLayerGBox);
        inl_oplColorLabel->setObjectName("inl_oplColorLabel");
        inl_oplColorLabel->setFrameShape(QFrame::Box);

        gridLayout->addWidget(inl_oplColorLabel, 8, 2, 1, 1);

        ilmLayerRButton = new QRadioButton(editLayerGBox);
        ilmLayerRButton->setObjectName("ilmLayerRButton");
        ilmLayerRButton->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(ilmLayerRButton, 4, 1, 1, 1);

        gcl_iplColorLabel = new QLabel(editLayerGBox);
        gcl_iplColorLabel->setObjectName("gcl_iplColorLabel");
        gcl_iplColorLabel->setFrameShape(QFrame::Box);

        gridLayout->addWidget(gcl_iplColorLabel, 6, 2, 1, 1);

        rnfl_gclLayerCBox = new QCheckBox(editLayerGBox);
        rnfl_gclLayerCBox->setObjectName("rnfl_gclLayerCBox");
        rnfl_gclLayerCBox->setFocusPolicy(Qt::NoFocus);
        rnfl_gclLayerCBox->setChecked(true);

        gridLayout->addWidget(rnfl_gclLayerCBox, 5, 0, 1, 1);

        ibermLayerRButton = new QRadioButton(editLayerGBox);
        ibermLayerRButton->setObjectName("ibermLayerRButton");
        ibermLayerRButton->setFocusPolicy(Qt::NoFocus);
        ibermLayerRButton->setChecked(false);

        gridLayout->addWidget(ibermLayerRButton, 2, 1, 1, 1);

        inl_oplLayerRButton = new QRadioButton(editLayerGBox);
        inl_oplLayerRButton->setObjectName("inl_oplLayerRButton");
        inl_oplLayerRButton->setFocusPolicy(Qt::NoFocus);
        inl_oplLayerRButton->setChecked(false);

        gridLayout->addWidget(inl_oplLayerRButton, 8, 1, 1, 1);

        mezColorLabel = new QLabel(editLayerGBox);
        mezColorLabel->setObjectName("mezColorLabel");
        mezColorLabel->setFrameShape(QFrame::Box);

        gridLayout->addWidget(mezColorLabel, 11, 2, 1, 1);

        iboprColorLabel = new QLabel(editLayerGBox);
        iboprColorLabel->setObjectName("iboprColorLabel");
        iboprColorLabel->setFrameShape(QFrame::Box);

        gridLayout->addWidget(iboprColorLabel, 13, 2, 1, 1);


        verticalLayout_6->addWidget(editLayerGBox);

        formLayout_4 = new QFormLayout();
        formLayout_4->setSpacing(6);
        formLayout_4->setObjectName("formLayout_4");
        formLayout_4->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
        contrastResetButton = new QPushButton(tabOCTExam);
        contrastResetButton->setObjectName("contrastResetButton");
        contrastResetButton->setEnabled(false);
        contrastResetButton->setMaximumSize(QSize(75, 23));

        formLayout_4->setWidget(0, QFormLayout::LabelRole, contrastResetButton);

        contrastSlider = new QSlider(tabOCTExam);
        contrastSlider->setObjectName("contrastSlider");
        contrastSlider->setEnabled(false);
        QSizePolicy sizePolicy3(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(contrastSlider->sizePolicy().hasHeightForWidth());
        contrastSlider->setSizePolicy(sizePolicy3);
        contrastSlider->setMinimumSize(QSize(140, 19));
        contrastSlider->setMaximumSize(QSize(140, 16777215));
        contrastSlider->setMinimum(1);
        contrastSlider->setMaximum(30);
        contrastSlider->setPageStep(2);
        contrastSlider->setOrientation(Qt::Horizontal);

        formLayout_4->setWidget(0, QFormLayout::FieldRole, contrastSlider);

        brightnessResetButton = new QPushButton(tabOCTExam);
        brightnessResetButton->setObjectName("brightnessResetButton");
        brightnessResetButton->setEnabled(false);
        brightnessResetButton->setMaximumSize(QSize(75, 23));

        formLayout_4->setWidget(1, QFormLayout::LabelRole, brightnessResetButton);

        brightnessSlider = new QSlider(tabOCTExam);
        brightnessSlider->setObjectName("brightnessSlider");
        brightnessSlider->setEnabled(false);
        sizePolicy3.setHeightForWidth(brightnessSlider->sizePolicy().hasHeightForWidth());
        brightnessSlider->setSizePolicy(sizePolicy3);
        brightnessSlider->setMinimumSize(QSize(140, 19));
        brightnessSlider->setMaximumSize(QSize(140, 16777215));
        brightnessSlider->setMinimum(-100);
        brightnessSlider->setMaximum(100);
        brightnessSlider->setOrientation(Qt::Horizontal);

        formLayout_4->setWidget(1, QFormLayout::FieldRole, brightnessSlider);


        verticalLayout_6->addLayout(formLayout_4);

        verticalSpacer_2 = new QSpacerItem(20, 30, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_6->addItem(verticalSpacer_2);

        fundusImageLabel = new QLabel(tabOCTExam);
        fundusImageLabel->setObjectName("fundusImageLabel");
        fundusImageLabel->setMinimumSize(QSize(222, 222));
        fundusImageLabel->setMaximumSize(QSize(222, 222));
        fundusImageLabel->setFrameShape(QFrame::Box);
        fundusImageLabel->setScaledContents(true);
        fundusImageLabel->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        verticalLayout_6->addWidget(fundusImageLabel);


        horizontalLayout_16->addLayout(verticalLayout_6);

        tabWidget->addTab(tabOCTExam, QString());

        horizontalLayout_11->addWidget(tabWidget);

        OCTAnnotate->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(OCTAnnotate);
        menuBar->setObjectName("menuBar");
        menuBar->setGeometry(QRect(0, 0, 1260, 21));
        menuPatient = new QMenu(menuBar);
        menuPatient->setObjectName("menuPatient");
        menuSegmentations_2 = new QMenu(menuPatient);
        menuSegmentations_2->setObjectName("menuSegmentations_2");
        menuProgram = new QMenu(menuBar);
        menuProgram->setObjectName("menuProgram");
        OCTAnnotate->setMenuBar(menuBar);
        statusBar = new QStatusBar(OCTAnnotate);
        statusBar->setObjectName("statusBar");
        OCTAnnotate->setStatusBar(statusBar);
        toolBar = new QToolBar(OCTAnnotate);
        toolBar->setObjectName("toolBar");
        OCTAnnotate->addToolBar(Qt::TopToolBarArea, toolBar);

        menuBar->addAction(menuProgram->menuAction());
        menuBar->addAction(menuPatient->menuAction());
        menuPatient->addAction(actionLoadOCTFile);
        menuPatient->addAction(actionLoadOCTSequence);
        menuPatient->addSeparator();
        menuPatient->addAction(menuSegmentations_2->menuAction());
        menuSegmentations_2->addAction(actionReadManualAnnotations);
        menuProgram->addSeparator();
        menuProgram->addAction(actionSettings);
        menuProgram->addSeparator();
        menuProgram->addAction(actionClose);

        retranslateUi(OCTAnnotate);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(OCTAnnotate);
    } // setupUi

    void retranslateUi(QMainWindow *OCTAnnotate)
    {
        OCTAnnotate->setWindowTitle(QCoreApplication::translate("OCTAnnotate", "OCTAnnotate", nullptr));
        actionLoadOCTSequence->setText(QCoreApplication::translate("OCTAnnotate", "Za\305\202aduj skan OCT (katalog obraz\303\263w)", nullptr));
        actionClose->setText(QCoreApplication::translate("OCTAnnotate", "Zamknij", nullptr));
        actionSettings->setText(QCoreApplication::translate("OCTAnnotate", "Ustawienia", nullptr));
        actionImageFlattening->setText(QCoreApplication::translate("OCTAnnotate", "Image flattening", nullptr));
#if QT_CONFIG(tooltip)
        actionImageFlattening->setToolTip(QCoreApplication::translate("OCTAnnotate", "Image flattening", nullptr));
#endif // QT_CONFIG(tooltip)
        actionLoadOCTFile->setText(QCoreApplication::translate("OCTAnnotate", "Za\305\202aduj skan OCT (plik .oct)", nullptr));
        actionReadManualAnnotations->setText(QCoreApplication::translate("OCTAnnotate", "Wczytaj referencyjne segmentacje", nullptr));
        label_2->setText(QCoreApplication::translate("OCTAnnotate", "Image:", nullptr));
        imageNumberLabel->setText(QString());
        zoomOutButton->setText(QCoreApplication::translate("OCTAnnotate", "-", nullptr));
        zoomInButton->setText(QCoreApplication::translate("OCTAnnotate", "+", nullptr));
        prevImageButton->setText(QCoreApplication::translate("OCTAnnotate", "Upper", nullptr));
        currImageNumberLEdit->setText(QCoreApplication::translate("OCTAnnotate", "0", nullptr));
        nextImageButton->setText(QCoreApplication::translate("OCTAnnotate", "Lower", nullptr));
        prevNormalImageButton->setText(QCoreApplication::translate("OCTAnnotate", "Left", nullptr));
        currNormalImageNumberLEdit->setText(QCoreApplication::translate("OCTAnnotate", "0", nullptr));
        nextNormalImageButton->setText(QCoreApplication::translate("OCTAnnotate", "Right", nullptr));
        editLayerGBox->setTitle(QCoreApplication::translate("OCTAnnotate", "Edit layers", nullptr));
        allLayersCBox->setText(QCoreApplication::translate("OCTAnnotate", "Show all layers", nullptr));
        opl_onlAnnotCountLabel->setText(QCoreApplication::translate("OCTAnnotate", "( --- / --- )", nullptr));
        mezLayerCBox->setText(QString());
        ipl_inlAnnotCountLabel->setText(QCoreApplication::translate("OCTAnnotate", "( --- / --- )", nullptr));
        obrpeAnnotCountLabel->setText(QCoreApplication::translate("OCTAnnotate", "( --- / --- )", nullptr));
        gcl_iplLayerCBox->setText(QString());
        obermColorLabel->setText(QString());
        mezAnnotCountLabel->setText(QCoreApplication::translate("OCTAnnotate", "( --- / --- )", nullptr));
        pcvLayerCBox->setText(QString());
        ilmAnnotCountLabel->setText(QCoreApplication::translate("OCTAnnotate", "( --- / --- )", nullptr));
        ibermColorLabel->setText(QString());
        opl_onlLayerRButton->setText(QCoreApplication::translate("OCTAnnotate", "OPL/ONL", nullptr));
        mezLayerRButton->setText(QCoreApplication::translate("OCTAnnotate", "Myoid/Ellipsoid", nullptr));
        iboprAnnotCountLabel->setText(QCoreApplication::translate("OCTAnnotate", "( --- / --- )", nullptr));
        pcvLayerRButton->setText(QCoreApplication::translate("OCTAnnotate", "PCV", nullptr));
        inl_oplAnnotCountLabel->setText(QCoreApplication::translate("OCTAnnotate", "( --- / --- )", nullptr));
        iosLayerCBox->setText(QString());
        pcvColorLabel->setText(QString());
        ipl_inlLayerRButton->setText(QCoreApplication::translate("OCTAnnotate", "IPL/INL", nullptr));
        rnfl_gclLayerRButton->setText(QCoreApplication::translate("OCTAnnotate", "NFL/GCL", nullptr));
        ibrpeColorLabel->setText(QString());
        iosColorLabel->setText(QString());
        obrpeLayerRButton->setText(QCoreApplication::translate("OCTAnnotate", "RPE/Choroid", nullptr));
        ibrpeLayerRButton->setText(QCoreApplication::translate("OCTAnnotate", "OS/RPE", nullptr));
        ilmLayerCBox->setText(QString());
        rnfl_gclAnnotCountLabel->setText(QCoreApplication::translate("OCTAnnotate", "( --- / --- )", nullptr));
        ibrpeAnnotCountLabel->setText(QCoreApplication::translate("OCTAnnotate", "( --- / --- )", nullptr));
        elmLayerCBox->setText(QString());
        iosLayerRButton->setText(QCoreApplication::translate("OCTAnnotate", "IS/OS", nullptr));
        iboprLayerRButton->setText(QCoreApplication::translate("OCTAnnotate", "IB OPR", nullptr));
        gcl_iplAnnotCountLabel->setText(QCoreApplication::translate("OCTAnnotate", "( --- / --- )", nullptr));
        ilmColorLabel->setText(QString());
        gcl_iplLayerRButton->setText(QCoreApplication::translate("OCTAnnotate", "GCL/IPL", nullptr));
        rnfl_gclColorLabel->setText(QString());
        elmAnnotCountLabel->setText(QCoreApplication::translate("OCTAnnotate", "( --- / --- )", nullptr));
        ipl_inlLayerCBox->setText(QString());
        ibermLayerCBox->setText(QString());
        elmColorLabel->setText(QString());
        iboprLayerCBox->setText(QString());
        obermLayerRButton->setText(QCoreApplication::translate("OCTAnnotate", "OB ERM", nullptr));
        obermLayerCBox->setText(QString());
        elmLayerRButton->setText(QCoreApplication::translate("OCTAnnotate", "ELM", nullptr));
        inl_oplLayerCBox->setText(QString());
        pcvAnnotCountLabel->setText(QCoreApplication::translate("OCTAnnotate", "( --- / --- )", nullptr));
        opl_onlColorLabel->setText(QString());
        ibrpeLayerCBox->setText(QString());
        ipl_inlColorLabel->setText(QString());
        ibermAnnotCountLabel->setText(QCoreApplication::translate("OCTAnnotate", "( --- / --- )", nullptr));
        iosAnnotCountLabel->setText(QCoreApplication::translate("OCTAnnotate", "( --- / --- )", nullptr));
        obrpeLayerCBox->setText(QString());
        obrpeColorLabel->setText(QString());
        ibermAnnotCountLabel_2->setText(QCoreApplication::translate("OCTAnnotate", "( --- / --- )", nullptr));
        opl_onlLayerCBox->setText(QString());
        inl_oplColorLabel->setText(QString());
        ilmLayerRButton->setText(QCoreApplication::translate("OCTAnnotate", "ILM", nullptr));
        gcl_iplColorLabel->setText(QString());
        rnfl_gclLayerCBox->setText(QString());
        ibermLayerRButton->setText(QCoreApplication::translate("OCTAnnotate", "IB ERM", nullptr));
        inl_oplLayerRButton->setText(QCoreApplication::translate("OCTAnnotate", "INL/OPL", nullptr));
        mezColorLabel->setText(QString());
        iboprColorLabel->setText(QString());
        contrastResetButton->setText(QCoreApplication::translate("OCTAnnotate", "Contrast", nullptr));
        brightnessResetButton->setText(QCoreApplication::translate("OCTAnnotate", "Brightness", nullptr));
        fundusImageLabel->setText(QString());
        tabWidget->setTabText(tabWidget->indexOf(tabOCTExam), QCoreApplication::translate("OCTAnnotate", "OCT cross-sections", nullptr));
        menuPatient->setTitle(QCoreApplication::translate("OCTAnnotate", "Skan", nullptr));
        menuSegmentations_2->setTitle(QCoreApplication::translate("OCTAnnotate", "Segmentacje", nullptr));
        menuProgram->setTitle(QCoreApplication::translate("OCTAnnotate", "Program", nullptr));
        toolBar->setWindowTitle(QCoreApplication::translate("OCTAnnotate", "toolBar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class OCTAnnotate: public Ui_OCTAnnotate {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OCTANNOTATE_H
