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
    QAction *actionSaveOCTExam;
    QAction *actionLoadOCTSequence;
    QAction *actionChangeLayerColor;
    QAction *actionSetScanCenter;
    QAction *actionClose;
    QAction *actionSaveAutoAnnotationsAs;
    QAction *actionConvertAutoAnnotations;
    QAction *actionReadManualAnnotations;
    QAction *actionReadAutoSegmentation;
    QAction *actionClose_auto_segmentation;
    QAction *actionSetAutoSegmentationAsManual;
    QAction *actionShowETDRSGrid;
    QAction *actionSettings;
    QAction *actionShowCenterOnBscan;
    QAction *actionImageFlattening;
    QAction *actionEditAnnotations;
    QAction *actionLoadOCTFile;
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
    QRadioButton *pcvLayerRButton;
    QRadioButton *ilmLayerRButton;
    QLabel *pcvColorLabel;
    QLabel *ilmColorLabel;
    QRadioButton *inlLayerRButton;
    QRadioButton *oplLayerRButton;
    QRadioButton *onlLayerRButton;
    QRadioButton *elmLayerRButton;
    QLabel *inlColorLabel;
    QLabel *oplColorLabel;
    QLabel *onlColorLabel;
    QLabel *elmColorLabel;
    QCheckBox *pcvLayerCBox;
    QLabel *pcvAnnotCountLabel;
    QCheckBox *ilmLayerCBox;
    QLabel *ilmAnnotCountLabel;
    QCheckBox *inlLayerCBox;
    QLabel *inlAnnotCountLabel;
    QCheckBox *oplLayerCBox;
    QLabel *oplAnnotCountLabel;
    QCheckBox *onlLayerCBox;
    QLabel *onlAnnotCountLabel;
    QCheckBox *elmLayerCBox;
    QLabel *elmAnnotCountLabel;
    QCheckBox *gclLayerCBox;
    QRadioButton *gclLayerRButton;
    QLabel *gclColorLabel;
    QLabel *gclAnnotCountLabel;
    QCheckBox *iplLayerCBox;
    QRadioButton *iplLayerRButton;
    QLabel *iplColorLabel;
    QLabel *iplAnnotCountLabel;
    QCheckBox *mezLayerCBox;
    QRadioButton *mezLayerRButton;
    QLabel *mezAnnotCountLabel;
    QCheckBox *iosLayerCBox;
    QRadioButton *iosLayerRButton;
    QLabel *iosAnnotCountLabel;
    QCheckBox *rpeLayerCBox;
    QRadioButton *rpeLayerRButton;
    QLabel *rpeAnnotCountLabel;
    QCheckBox *chrLayerCBox;
    QRadioButton *chrLayerRButton;
    QLabel *chrAnnotCountLabel;
    QLabel *mezColorLabel;
    QLabel *iosColorLabel;
    QLabel *rpeColorLabel;
    QLabel *chrColorLabel;
    QCheckBox *ermLayerCBox;
    QRadioButton *ermLayerRButton;
    QLabel *ermColorLabel;
    QLabel *ermAnnotCountLabel;
    QCheckBox *allLayersCBox;
    QFormLayout *formLayout_4;
    QPushButton *contrastResetButton;
    QSlider *contrastSlider;
    QPushButton *brightnessResetButton;
    QSlider *brightnessSlider;
    QSpacerItem *verticalSpacer_2;
    QLabel *fundusImageLabel;
    QMenuBar *menuBar;
    QMenu *menuPatient;
    QMenu *menuAnnotations;
    QMenu *menuProgram;
    QStatusBar *statusBar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *OCTAnnotate)
    {
        if (OCTAnnotate->objectName().isEmpty())
            OCTAnnotate->setObjectName("OCTAnnotate");
        OCTAnnotate->resize(1260, 900);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(OCTAnnotate->sizePolicy().hasHeightForWidth());
        OCTAnnotate->setSizePolicy(sizePolicy);
        OCTAnnotate->setMinimumSize(QSize(1260, 900));
        actionSaveOCTExam = new QAction(OCTAnnotate);
        actionSaveOCTExam->setObjectName("actionSaveOCTExam");
        actionSaveOCTExam->setEnabled(true);
        actionLoadOCTSequence = new QAction(OCTAnnotate);
        actionLoadOCTSequence->setObjectName("actionLoadOCTSequence");
        actionChangeLayerColor = new QAction(OCTAnnotate);
        actionChangeLayerColor->setObjectName("actionChangeLayerColor");
        actionSetScanCenter = new QAction(OCTAnnotate);
        actionSetScanCenter->setObjectName("actionSetScanCenter");
        actionSetScanCenter->setCheckable(true);
        actionSetScanCenter->setEnabled(false);
        actionClose = new QAction(OCTAnnotate);
        actionClose->setObjectName("actionClose");
        actionSaveAutoAnnotationsAs = new QAction(OCTAnnotate);
        actionSaveAutoAnnotationsAs->setObjectName("actionSaveAutoAnnotationsAs");
        actionConvertAutoAnnotations = new QAction(OCTAnnotate);
        actionConvertAutoAnnotations->setObjectName("actionConvertAutoAnnotations");
        actionReadManualAnnotations = new QAction(OCTAnnotate);
        actionReadManualAnnotations->setObjectName("actionReadManualAnnotations");
        actionReadAutoSegmentation = new QAction(OCTAnnotate);
        actionReadAutoSegmentation->setObjectName("actionReadAutoSegmentation");
        actionClose_auto_segmentation = new QAction(OCTAnnotate);
        actionClose_auto_segmentation->setObjectName("actionClose_auto_segmentation");
        actionSetAutoSegmentationAsManual = new QAction(OCTAnnotate);
        actionSetAutoSegmentationAsManual->setObjectName("actionSetAutoSegmentationAsManual");
        actionShowETDRSGrid = new QAction(OCTAnnotate);
        actionShowETDRSGrid->setObjectName("actionShowETDRSGrid");
        actionShowETDRSGrid->setCheckable(true);
        actionShowETDRSGrid->setChecked(true);
        actionSettings = new QAction(OCTAnnotate);
        actionSettings->setObjectName("actionSettings");
        actionShowCenterOnBscan = new QAction(OCTAnnotate);
        actionShowCenterOnBscan->setObjectName("actionShowCenterOnBscan");
        actionShowCenterOnBscan->setCheckable(true);
        actionShowCenterOnBscan->setChecked(true);
        actionImageFlattening = new QAction(OCTAnnotate);
        actionImageFlattening->setObjectName("actionImageFlattening");
        actionImageFlattening->setCheckable(true);
        actionEditAnnotations = new QAction(OCTAnnotate);
        actionEditAnnotations->setObjectName("actionEditAnnotations");
        actionEditAnnotations->setCheckable(true);
        actionLoadOCTFile = new QAction(OCTAnnotate);
        actionLoadOCTFile->setObjectName("actionLoadOCTFile");
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
        editLayerGBox->setMinimumSize(QSize(221, 311));
        editLayerGBox->setMaximumSize(QSize(221, 1000));
        pcvLayerRButton = new QRadioButton(editLayerGBox);
        pcvLayerRButton->setObjectName("pcvLayerRButton");
        pcvLayerRButton->setGeometry(QRect(30, 40, 51, 21));
        pcvLayerRButton->setFocusPolicy(Qt::NoFocus);
        pcvLayerRButton->setChecked(true);
        ilmLayerRButton = new QRadioButton(editLayerGBox);
        ilmLayerRButton->setObjectName("ilmLayerRButton");
        ilmLayerRButton->setGeometry(QRect(30, 80, 51, 21));
        ilmLayerRButton->setFocusPolicy(Qt::NoFocus);
        pcvColorLabel = new QLabel(editLayerGBox);
        pcvColorLabel->setObjectName("pcvColorLabel");
        pcvColorLabel->setGeometry(QRect(120, 40, 20, 20));
        ilmColorLabel = new QLabel(editLayerGBox);
        ilmColorLabel->setObjectName("ilmColorLabel");
        ilmColorLabel->setGeometry(QRect(120, 80, 20, 20));
        inlLayerRButton = new QRadioButton(editLayerGBox);
        inlLayerRButton->setObjectName("inlLayerRButton");
        inlLayerRButton->setGeometry(QRect(30, 140, 61, 21));
        inlLayerRButton->setFocusPolicy(Qt::NoFocus);
        inlLayerRButton->setChecked(false);
        oplLayerRButton = new QRadioButton(editLayerGBox);
        oplLayerRButton->setObjectName("oplLayerRButton");
        oplLayerRButton->setGeometry(QRect(30, 160, 61, 21));
        oplLayerRButton->setFocusPolicy(Qt::NoFocus);
        oplLayerRButton->setChecked(false);
        onlLayerRButton = new QRadioButton(editLayerGBox);
        onlLayerRButton->setObjectName("onlLayerRButton");
        onlLayerRButton->setGeometry(QRect(30, 180, 61, 21));
        onlLayerRButton->setFocusPolicy(Qt::NoFocus);
        onlLayerRButton->setChecked(false);
        elmLayerRButton = new QRadioButton(editLayerGBox);
        elmLayerRButton->setObjectName("elmLayerRButton");
        elmLayerRButton->setEnabled(true);
        elmLayerRButton->setGeometry(QRect(30, 200, 51, 21));
        elmLayerRButton->setChecked(false);
        inlColorLabel = new QLabel(editLayerGBox);
        inlColorLabel->setObjectName("inlColorLabel");
        inlColorLabel->setGeometry(QRect(120, 140, 20, 20));
        oplColorLabel = new QLabel(editLayerGBox);
        oplColorLabel->setObjectName("oplColorLabel");
        oplColorLabel->setGeometry(QRect(120, 160, 20, 20));
        onlColorLabel = new QLabel(editLayerGBox);
        onlColorLabel->setObjectName("onlColorLabel");
        onlColorLabel->setGeometry(QRect(120, 180, 20, 20));
        elmColorLabel = new QLabel(editLayerGBox);
        elmColorLabel->setObjectName("elmColorLabel");
        elmColorLabel->setGeometry(QRect(120, 200, 20, 20));
        pcvLayerCBox = new QCheckBox(editLayerGBox);
        pcvLayerCBox->setObjectName("pcvLayerCBox");
        pcvLayerCBox->setGeometry(QRect(10, 40, 16, 21));
        pcvLayerCBox->setFocusPolicy(Qt::NoFocus);
        pcvLayerCBox->setChecked(true);
        pcvAnnotCountLabel = new QLabel(editLayerGBox);
        pcvAnnotCountLabel->setObjectName("pcvAnnotCountLabel");
        pcvAnnotCountLabel->setGeometry(QRect(150, 40, 71, 21));
        ilmLayerCBox = new QCheckBox(editLayerGBox);
        ilmLayerCBox->setObjectName("ilmLayerCBox");
        ilmLayerCBox->setGeometry(QRect(10, 80, 16, 21));
        ilmLayerCBox->setFocusPolicy(Qt::NoFocus);
        ilmLayerCBox->setChecked(true);
        ilmAnnotCountLabel = new QLabel(editLayerGBox);
        ilmAnnotCountLabel->setObjectName("ilmAnnotCountLabel");
        ilmAnnotCountLabel->setGeometry(QRect(150, 80, 71, 21));
        inlLayerCBox = new QCheckBox(editLayerGBox);
        inlLayerCBox->setObjectName("inlLayerCBox");
        inlLayerCBox->setGeometry(QRect(10, 140, 16, 21));
        inlLayerCBox->setFocusPolicy(Qt::NoFocus);
        inlLayerCBox->setChecked(true);
        inlAnnotCountLabel = new QLabel(editLayerGBox);
        inlAnnotCountLabel->setObjectName("inlAnnotCountLabel");
        inlAnnotCountLabel->setGeometry(QRect(150, 140, 71, 21));
        oplLayerCBox = new QCheckBox(editLayerGBox);
        oplLayerCBox->setObjectName("oplLayerCBox");
        oplLayerCBox->setGeometry(QRect(10, 160, 16, 21));
        oplLayerCBox->setFocusPolicy(Qt::NoFocus);
        oplLayerCBox->setChecked(true);
        oplAnnotCountLabel = new QLabel(editLayerGBox);
        oplAnnotCountLabel->setObjectName("oplAnnotCountLabel");
        oplAnnotCountLabel->setGeometry(QRect(150, 160, 71, 21));
        onlLayerCBox = new QCheckBox(editLayerGBox);
        onlLayerCBox->setObjectName("onlLayerCBox");
        onlLayerCBox->setGeometry(QRect(10, 180, 16, 21));
        onlLayerCBox->setFocusPolicy(Qt::NoFocus);
        onlLayerCBox->setChecked(true);
        onlAnnotCountLabel = new QLabel(editLayerGBox);
        onlAnnotCountLabel->setObjectName("onlAnnotCountLabel");
        onlAnnotCountLabel->setGeometry(QRect(150, 180, 71, 21));
        elmLayerCBox = new QCheckBox(editLayerGBox);
        elmLayerCBox->setObjectName("elmLayerCBox");
        elmLayerCBox->setGeometry(QRect(10, 200, 16, 21));
        elmLayerCBox->setChecked(true);
        elmAnnotCountLabel = new QLabel(editLayerGBox);
        elmAnnotCountLabel->setObjectName("elmAnnotCountLabel");
        elmAnnotCountLabel->setGeometry(QRect(150, 200, 71, 21));
        gclLayerCBox = new QCheckBox(editLayerGBox);
        gclLayerCBox->setObjectName("gclLayerCBox");
        gclLayerCBox->setGeometry(QRect(10, 100, 16, 21));
        gclLayerCBox->setFocusPolicy(Qt::NoFocus);
        gclLayerCBox->setChecked(true);
        gclLayerRButton = new QRadioButton(editLayerGBox);
        gclLayerRButton->setObjectName("gclLayerRButton");
        gclLayerRButton->setGeometry(QRect(30, 100, 61, 21));
        gclLayerRButton->setFocusPolicy(Qt::NoFocus);
        gclColorLabel = new QLabel(editLayerGBox);
        gclColorLabel->setObjectName("gclColorLabel");
        gclColorLabel->setGeometry(QRect(120, 100, 20, 20));
        gclAnnotCountLabel = new QLabel(editLayerGBox);
        gclAnnotCountLabel->setObjectName("gclAnnotCountLabel");
        gclAnnotCountLabel->setGeometry(QRect(150, 100, 71, 21));
        iplLayerCBox = new QCheckBox(editLayerGBox);
        iplLayerCBox->setObjectName("iplLayerCBox");
        iplLayerCBox->setGeometry(QRect(10, 120, 16, 21));
        iplLayerCBox->setFocusPolicy(Qt::NoFocus);
        iplLayerCBox->setChecked(true);
        iplLayerRButton = new QRadioButton(editLayerGBox);
        iplLayerRButton->setObjectName("iplLayerRButton");
        iplLayerRButton->setGeometry(QRect(30, 120, 61, 21));
        iplLayerRButton->setFocusPolicy(Qt::NoFocus);
        iplColorLabel = new QLabel(editLayerGBox);
        iplColorLabel->setObjectName("iplColorLabel");
        iplColorLabel->setGeometry(QRect(120, 120, 20, 20));
        iplAnnotCountLabel = new QLabel(editLayerGBox);
        iplAnnotCountLabel->setObjectName("iplAnnotCountLabel");
        iplAnnotCountLabel->setGeometry(QRect(150, 120, 71, 21));
        mezLayerCBox = new QCheckBox(editLayerGBox);
        mezLayerCBox->setObjectName("mezLayerCBox");
        mezLayerCBox->setGeometry(QRect(10, 220, 16, 21));
        mezLayerCBox->setChecked(true);
        mezLayerRButton = new QRadioButton(editLayerGBox);
        mezLayerRButton->setObjectName("mezLayerRButton");
        mezLayerRButton->setGeometry(QRect(30, 220, 91, 21));
        mezAnnotCountLabel = new QLabel(editLayerGBox);
        mezAnnotCountLabel->setObjectName("mezAnnotCountLabel");
        mezAnnotCountLabel->setGeometry(QRect(150, 220, 71, 21));
        iosLayerCBox = new QCheckBox(editLayerGBox);
        iosLayerCBox->setObjectName("iosLayerCBox");
        iosLayerCBox->setGeometry(QRect(10, 240, 16, 21));
        iosLayerCBox->setChecked(true);
        iosLayerRButton = new QRadioButton(editLayerGBox);
        iosLayerRButton->setObjectName("iosLayerRButton");
        iosLayerRButton->setGeometry(QRect(30, 240, 61, 21));
        iosAnnotCountLabel = new QLabel(editLayerGBox);
        iosAnnotCountLabel->setObjectName("iosAnnotCountLabel");
        iosAnnotCountLabel->setGeometry(QRect(150, 240, 71, 21));
        rpeLayerCBox = new QCheckBox(editLayerGBox);
        rpeLayerCBox->setObjectName("rpeLayerCBox");
        rpeLayerCBox->setGeometry(QRect(10, 260, 16, 21));
        rpeLayerCBox->setChecked(true);
        rpeLayerRButton = new QRadioButton(editLayerGBox);
        rpeLayerRButton->setObjectName("rpeLayerRButton");
        rpeLayerRButton->setGeometry(QRect(30, 260, 61, 21));
        rpeAnnotCountLabel = new QLabel(editLayerGBox);
        rpeAnnotCountLabel->setObjectName("rpeAnnotCountLabel");
        rpeAnnotCountLabel->setGeometry(QRect(150, 260, 71, 21));
        chrLayerCBox = new QCheckBox(editLayerGBox);
        chrLayerCBox->setObjectName("chrLayerCBox");
        chrLayerCBox->setGeometry(QRect(10, 280, 16, 21));
        chrLayerCBox->setChecked(true);
        chrLayerRButton = new QRadioButton(editLayerGBox);
        chrLayerRButton->setObjectName("chrLayerRButton");
        chrLayerRButton->setGeometry(QRect(30, 280, 81, 21));
        chrAnnotCountLabel = new QLabel(editLayerGBox);
        chrAnnotCountLabel->setObjectName("chrAnnotCountLabel");
        chrAnnotCountLabel->setGeometry(QRect(150, 280, 71, 21));
        mezColorLabel = new QLabel(editLayerGBox);
        mezColorLabel->setObjectName("mezColorLabel");
        mezColorLabel->setGeometry(QRect(120, 220, 20, 20));
        iosColorLabel = new QLabel(editLayerGBox);
        iosColorLabel->setObjectName("iosColorLabel");
        iosColorLabel->setGeometry(QRect(120, 240, 20, 20));
        rpeColorLabel = new QLabel(editLayerGBox);
        rpeColorLabel->setObjectName("rpeColorLabel");
        rpeColorLabel->setGeometry(QRect(120, 260, 20, 20));
        chrColorLabel = new QLabel(editLayerGBox);
        chrColorLabel->setObjectName("chrColorLabel");
        chrColorLabel->setGeometry(QRect(120, 280, 20, 20));
        ermLayerCBox = new QCheckBox(editLayerGBox);
        ermLayerCBox->setObjectName("ermLayerCBox");
        ermLayerCBox->setGeometry(QRect(10, 60, 16, 21));
        ermLayerCBox->setFocusPolicy(Qt::NoFocus);
        ermLayerCBox->setChecked(true);
        ermLayerRButton = new QRadioButton(editLayerGBox);
        ermLayerRButton->setObjectName("ermLayerRButton");
        ermLayerRButton->setGeometry(QRect(30, 60, 51, 21));
        ermLayerRButton->setFocusPolicy(Qt::NoFocus);
        ermLayerRButton->setChecked(false);
        ermColorLabel = new QLabel(editLayerGBox);
        ermColorLabel->setObjectName("ermColorLabel");
        ermColorLabel->setGeometry(QRect(120, 60, 20, 20));
        ermAnnotCountLabel = new QLabel(editLayerGBox);
        ermAnnotCountLabel->setObjectName("ermAnnotCountLabel");
        ermAnnotCountLabel->setGeometry(QRect(150, 60, 71, 21));
        allLayersCBox = new QCheckBox(editLayerGBox);
        allLayersCBox->setObjectName("allLayersCBox");
        allLayersCBox->setGeometry(QRect(10, 20, 151, 21));
        allLayersCBox->setChecked(true);

        verticalLayout_6->addWidget(editLayerGBox);

        formLayout_4 = new QFormLayout();
        formLayout_4->setSpacing(6);
        formLayout_4->setObjectName("formLayout_4");
        formLayout_4->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
        contrastResetButton = new QPushButton(tabOCTExam);
        contrastResetButton->setObjectName("contrastResetButton");
        contrastResetButton->setMaximumSize(QSize(75, 23));

        formLayout_4->setWidget(0, QFormLayout::LabelRole, contrastResetButton);

        contrastSlider = new QSlider(tabOCTExam);
        contrastSlider->setObjectName("contrastSlider");
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
        brightnessResetButton->setMaximumSize(QSize(75, 23));

        formLayout_4->setWidget(1, QFormLayout::LabelRole, brightnessResetButton);

        brightnessSlider = new QSlider(tabOCTExam);
        brightnessSlider->setObjectName("brightnessSlider");
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
        menuAnnotations = new QMenu(menuPatient);
        menuAnnotations->setObjectName("menuAnnotations");
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
        menuPatient->addAction(actionSaveOCTExam);
        menuPatient->addSeparator();
        menuPatient->addAction(menuAnnotations->menuAction());
        menuAnnotations->addAction(actionReadAutoSegmentation);
        menuAnnotations->addAction(actionClose_auto_segmentation);
        menuAnnotations->addAction(actionSaveAutoAnnotationsAs);
        menuAnnotations->addAction(actionConvertAutoAnnotations);
        menuAnnotations->addSeparator();
        menuAnnotations->addAction(actionReadManualAnnotations);
        menuAnnotations->addAction(actionSetAutoSegmentationAsManual);
        menuProgram->addAction(actionChangeLayerColor);
        menuProgram->addAction(actionSetScanCenter);
        menuProgram->addSeparator();
        menuProgram->addAction(actionSettings);
        menuProgram->addAction(actionShowCenterOnBscan);
        menuProgram->addSeparator();
        menuProgram->addAction(actionClose);
        toolBar->addAction(actionImageFlattening);
        toolBar->addAction(actionEditAnnotations);
        toolBar->addAction(actionShowCenterOnBscan);

        retranslateUi(OCTAnnotate);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(OCTAnnotate);
    } // setupUi

    void retranslateUi(QMainWindow *OCTAnnotate)
    {
        OCTAnnotate->setWindowTitle(QCoreApplication::translate("OCTAnnotate", "OCTAnnotate", nullptr));
        actionSaveOCTExam->setText(QCoreApplication::translate("OCTAnnotate", "Save OCT scan segmentations", nullptr));
        actionLoadOCTSequence->setText(QCoreApplication::translate("OCTAnnotate", "Load OCT scan folder", nullptr));
        actionChangeLayerColor->setText(QCoreApplication::translate("OCTAnnotate", "Change layer color", nullptr));
        actionSetScanCenter->setText(QCoreApplication::translate("OCTAnnotate", "Set scan center", nullptr));
        actionClose->setText(QCoreApplication::translate("OCTAnnotate", "Close", nullptr));
        actionSaveAutoAnnotationsAs->setText(QCoreApplication::translate("OCTAnnotate", "Save automatic segmentation as...", nullptr));
        actionConvertAutoAnnotations->setText(QCoreApplication::translate("OCTAnnotate", "Convert automatic segmentation from XML to TXT", nullptr));
        actionReadManualAnnotations->setText(QCoreApplication::translate("OCTAnnotate", "Load manual segmentation", nullptr));
        actionReadAutoSegmentation->setText(QCoreApplication::translate("OCTAnnotate", "Load auto segmentation", nullptr));
        actionClose_auto_segmentation->setText(QCoreApplication::translate("OCTAnnotate", "Close auto segmentation", nullptr));
        actionSetAutoSegmentationAsManual->setText(QCoreApplication::translate("OCTAnnotate", "Copy auto segmentation as manual", nullptr));
        actionShowETDRSGrid->setText(QCoreApplication::translate("OCTAnnotate", "Show ETDRS grid", nullptr));
        actionSettings->setText(QCoreApplication::translate("OCTAnnotate", "Settings", nullptr));
        actionShowCenterOnBscan->setText(QCoreApplication::translate("OCTAnnotate", "Show scan center on cross-section", nullptr));
        actionImageFlattening->setText(QCoreApplication::translate("OCTAnnotate", "Image flattening", nullptr));
#if QT_CONFIG(tooltip)
        actionImageFlattening->setToolTip(QCoreApplication::translate("OCTAnnotate", "Image flattening", nullptr));
#endif // QT_CONFIG(tooltip)
        actionEditAnnotations->setText(QCoreApplication::translate("OCTAnnotate", "Edit segmentations", nullptr));
#if QT_CONFIG(tooltip)
        actionEditAnnotations->setToolTip(QCoreApplication::translate("OCTAnnotate", "Edit Annotations", nullptr));
#endif // QT_CONFIG(tooltip)
        actionLoadOCTFile->setText(QCoreApplication::translate("OCTAnnotate", "Load OCT scan file (.oct)", nullptr));
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
        pcvLayerRButton->setText(QCoreApplication::translate("OCTAnnotate", "PCV", nullptr));
        ilmLayerRButton->setText(QCoreApplication::translate("OCTAnnotate", "ILM", nullptr));
        pcvColorLabel->setText(QString());
        ilmColorLabel->setText(QString());
        inlLayerRButton->setText(QCoreApplication::translate("OCTAnnotate", "IPL/INL", nullptr));
        oplLayerRButton->setText(QCoreApplication::translate("OCTAnnotate", "INL/OPL", nullptr));
        onlLayerRButton->setText(QCoreApplication::translate("OCTAnnotate", "OPL/ONL", nullptr));
        elmLayerRButton->setText(QCoreApplication::translate("OCTAnnotate", "ELM", nullptr));
        inlColorLabel->setText(QString());
        oplColorLabel->setText(QString());
        onlColorLabel->setText(QString());
        elmColorLabel->setText(QString());
        pcvLayerCBox->setText(QString());
        pcvAnnotCountLabel->setText(QCoreApplication::translate("OCTAnnotate", "( --- / --- )", nullptr));
        ilmLayerCBox->setText(QString());
        ilmAnnotCountLabel->setText(QCoreApplication::translate("OCTAnnotate", "( --- / --- )", nullptr));
        inlLayerCBox->setText(QString());
        inlAnnotCountLabel->setText(QCoreApplication::translate("OCTAnnotate", "( --- / --- )", nullptr));
        oplLayerCBox->setText(QString());
        oplAnnotCountLabel->setText(QCoreApplication::translate("OCTAnnotate", "( --- / --- )", nullptr));
        onlLayerCBox->setText(QString());
        onlAnnotCountLabel->setText(QCoreApplication::translate("OCTAnnotate", "( --- / --- )", nullptr));
        elmLayerCBox->setText(QString());
        elmAnnotCountLabel->setText(QCoreApplication::translate("OCTAnnotate", "( --- / --- )", nullptr));
        gclLayerCBox->setText(QString());
        gclLayerRButton->setText(QCoreApplication::translate("OCTAnnotate", "NFL/GCL", nullptr));
        gclColorLabel->setText(QString());
        gclAnnotCountLabel->setText(QCoreApplication::translate("OCTAnnotate", "( --- / --- )", nullptr));
        iplLayerCBox->setText(QString());
        iplLayerRButton->setText(QCoreApplication::translate("OCTAnnotate", "GCL/IPL", nullptr));
        iplColorLabel->setText(QString());
        iplAnnotCountLabel->setText(QCoreApplication::translate("OCTAnnotate", "( --- / --- )", nullptr));
        mezLayerCBox->setText(QString());
        mezLayerRButton->setText(QCoreApplication::translate("OCTAnnotate", "Myoid/Ellipsoid", nullptr));
        mezAnnotCountLabel->setText(QCoreApplication::translate("OCTAnnotate", "( --- / --- )", nullptr));
        iosLayerCBox->setText(QString());
        iosLayerRButton->setText(QCoreApplication::translate("OCTAnnotate", "IS/OS", nullptr));
        iosAnnotCountLabel->setText(QCoreApplication::translate("OCTAnnotate", "( --- / --- )", nullptr));
        rpeLayerCBox->setText(QString());
        rpeLayerRButton->setText(QCoreApplication::translate("OCTAnnotate", "OS/RPE", nullptr));
        rpeAnnotCountLabel->setText(QCoreApplication::translate("OCTAnnotate", "( --- / --- )", nullptr));
        chrLayerCBox->setText(QString());
        chrLayerRButton->setText(QCoreApplication::translate("OCTAnnotate", "RPE/Choroid", nullptr));
        chrAnnotCountLabel->setText(QCoreApplication::translate("OCTAnnotate", "( --- / --- )", nullptr));
        mezColorLabel->setText(QString());
        iosColorLabel->setText(QString());
        rpeColorLabel->setText(QString());
        chrColorLabel->setText(QString());
        ermLayerCBox->setText(QString());
        ermLayerRButton->setText(QCoreApplication::translate("OCTAnnotate", "ERM", nullptr));
        ermColorLabel->setText(QString());
        ermAnnotCountLabel->setText(QCoreApplication::translate("OCTAnnotate", "( --- / --- )", nullptr));
        allLayersCBox->setText(QCoreApplication::translate("OCTAnnotate", "Show all layers", nullptr));
        contrastResetButton->setText(QCoreApplication::translate("OCTAnnotate", "Contrast", nullptr));
        brightnessResetButton->setText(QCoreApplication::translate("OCTAnnotate", "Brightness", nullptr));
        fundusImageLabel->setText(QString());
        tabWidget->setTabText(tabWidget->indexOf(tabOCTExam), QCoreApplication::translate("OCTAnnotate", "OCT cross-sections", nullptr));
        menuPatient->setTitle(QCoreApplication::translate("OCTAnnotate", "Patient", nullptr));
        menuAnnotations->setTitle(QCoreApplication::translate("OCTAnnotate", "Segmentations", nullptr));
        menuProgram->setTitle(QCoreApplication::translate("OCTAnnotate", "Program", nullptr));
        toolBar->setWindowTitle(QCoreApplication::translate("OCTAnnotate", "toolBar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class OCTAnnotate: public Ui_OCTAnnotate {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OCTANNOTATE_H
