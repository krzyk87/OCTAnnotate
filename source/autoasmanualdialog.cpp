#include <QGroupBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QPushButton>
#include <QLayout>

#include "autoasmanualdialog.h"

AutoAsManualDialog::AutoAsManualDialog(QWidget *parent) : QDialog(parent)
{
    // layers group
    layersGroup = new QGroupBox(tr("Which layers overwrite?"));

    allLayersRButton = new QRadioButton(tr("all"));
    allLayersRButton->setChecked(true);
    selectedLayersRButton = new QRadioButton(tr("selected"));

    pcvCBox = new QCheckBox(tr("PCV"));
    ibermCBox = new QCheckBox(tr("IB_ERM"));
    obermCBox = new QCheckBox(tr("OB_ERM"));
    ilmCBox = new QCheckBox(tr("ILM"));
    gclCBox = new QCheckBox(tr("NFL/GCL"));
    iplCBox = new QCheckBox(tr("GCL/IPL"));
    inlCBox = new QCheckBox(tr("IPL/INL"));
    oplCBox = new QCheckBox(tr("INL/OPL"));
    onlCBox = new QCheckBox(tr("OPL/ONL"));
    elmCBox = new QCheckBox(tr("ELM"));
    mezCBox = new QCheckBox(tr("Myoid/Ellipsoid Zone"));
    iosCBox = new QCheckBox(tr("IS/OS"));
    iboprCBox = new QCheckBox(tr("IB_OPR"));
    rpeCBox = new QCheckBox(tr("OS/RPE"));
    chrCBox = new QCheckBox(tr("RPE/Choroid"));

    //iplCBox->setEnabled(false);
    elmCBox->setEnabled(false);
    //mezCBox->setEnabled(false);
    //rpeCBox->setEnabled(false);

    QVBoxLayout *vbox1 = new QVBoxLayout;
    vbox1->addWidget(pcvCBox);
    vbox1->addWidget(ibermCBox);
    vbox1->addWidget(obermCBox);
    vbox1->addWidget(ilmCBox);
    vbox1->addWidget(gclCBox);
    vbox1->addWidget(iplCBox);
    vbox1->addWidget(inlCBox);
    vbox1->addWidget(oplCBox);
    vbox1->addWidget(onlCBox);
    vbox1->addWidget(elmCBox);
    vbox1->addWidget(mezCBox);
    vbox1->addWidget(iosCBox);
    vbox1->addWidget(iboprCBox);
    vbox1->addWidget(rpeCBox);
    vbox1->addWidget(chrCBox);

    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addSpacing(20);
    hbox->addLayout(vbox1);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(allLayersRButton);
    vbox->addWidget(selectedLayersRButton);
    vbox->addLayout(hbox);

    layersGroup->setLayout(vbox);

    // scans group
    scansGroup = new QGroupBox(tr("For which cross-sections?"));

    allScansRButton = new QRadioButton(tr("all"));
    allScansRButton->setChecked(true);
    emptyScansRButton = new QRadioButton(tr("those not having manual segmentations"));
    currentScanRButton = new QRadioButton(tr("only current"));
    otherScanRButton = new QRadioButton(tr("other..."));
    otherScanRButton->setEnabled(false);

    QVBoxLayout *vbox2 = new QVBoxLayout;
    vbox2->addWidget(allScansRButton);
    vbox2->addWidget(emptyScansRButton);
    vbox2->addWidget(currentScanRButton);
    vbox2->addWidget(otherScanRButton);
    vbox2->addStretch();

    scansGroup->setLayout(vbox2);

    // buttons
    acceptButton = new QPushButton(tr("Select"));
    acceptButton->setDefault(true);

    cancelButton = new QPushButton(tr("Cancel"));

    // final layout
    QHBoxLayout *optionsLayout = new QHBoxLayout;
    optionsLayout->addWidget(layersGroup);
    optionsLayout->addWidget(scansGroup);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(cancelButton);
    buttonsLayout->addWidget(acceptButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(optionsLayout);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);

    // actions
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(acceptButton, SIGNAL(clicked()), this, SLOT(accept()));

    connect(pcvCBox, SIGNAL(clicked()), selectedLayersRButton, SLOT(click()));
    connect(ilmCBox, SIGNAL(clicked()), selectedLayersRButton, SLOT(click()));
    connect(gclCBox, SIGNAL(clicked()), selectedLayersRButton, SLOT(click()));
    connect(iplCBox, SIGNAL(clicked()), selectedLayersRButton, SLOT(click()));
    connect(inlCBox, SIGNAL(clicked()), selectedLayersRButton, SLOT(click()));
    connect(oplCBox, SIGNAL(clicked()), selectedLayersRButton, SLOT(click()));
    connect(onlCBox, SIGNAL(clicked()), selectedLayersRButton, SLOT(click()));
    connect(elmCBox, SIGNAL(clicked()), selectedLayersRButton, SLOT(click()));
    connect(mezCBox, SIGNAL(clicked()), selectedLayersRButton, SLOT(click()));
    connect(iosCBox, SIGNAL(clicked()), selectedLayersRButton, SLOT(click()));
    connect(rpeCBox, SIGNAL(clicked()), selectedLayersRButton, SLOT(click()));
    connect(chrCBox, SIGNAL(clicked()), selectedLayersRButton, SLOT(click()));
}

QList<LayerName> AutoAsManualDialog::getSelectedLayers(){
    QList<LayerName> result;

    if (allLayersRButton->isChecked()){
        result.append(PCV);
        result.append(IB_ERM);
        result.append(OB_ERM);
        result.append(ILM);
        result.append(RNFL_GCL);
        result.append(GCL_IPL);
        result.append(IPL_INL);
        result.append(INL_OPL);
        result.append(OPL_ONL);
        result.append(ELM);
        result.append(MEZ);
        result.append(IS_OS);\
        result.append(IB_OPR);
        result.append(IB_RPE);
        result.append(OB_RPE);
    } else if (selectedLayersRButton->isChecked()){
        if (pcvCBox->isChecked())
            result.append(PCV);
        if (ibermCBox->isChecked())
            result.append(IB_ERM);
        if (obermCBox->isChecked())
            result.append(OB_ERM);
        if (ilmCBox->isChecked())
            result.append(ILM);
        if (gclCBox->isChecked())
            result.append(RNFL_GCL);
        if (iplCBox->isChecked())
            result.append(GCL_IPL);
        if (inlCBox->isChecked())
            result.append(IPL_INL);
        if (oplCBox->isChecked())
            result.append(INL_OPL);
        if (onlCBox->isChecked())
            result.append(OPL_ONL);
        if (elmCBox->isChecked())
            result.append(ELM);
        if (mezCBox->isChecked())
            result.append(MEZ);
        if (iosCBox->isChecked())
            result.append(IS_OS);
        if (iboprCBox->isChecked())
            result.append(IB_OPR);
        if (rpeCBox->isChecked())
            result.append(IB_RPE);
        if (chrCBox->isChecked())
            result.append(OB_RPE);
    }

    return result;
}

int AutoAsManualDialog::getSelectedScans(){
    int result = 1;

    if (allScansRButton->isChecked())
        result = 0;
    else if (emptyScansRButton->isChecked())
        result = 1;
    else if (currentScanRButton->isChecked())
        result = 2;
    else if (otherScanRButton->isChecked())
        result = 3;

    return result;
}
