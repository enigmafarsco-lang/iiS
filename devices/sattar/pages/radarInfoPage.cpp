// ============================================================================
// File: radarInfoPage.cpp
// Subsystem: eLynxSDR pages
// Description: Subsystem implementation and interface guidance for radarInfoPage.cpp.
// ============================================================================

﻿#include "radarInfoPage.h"
#include "ui_radarInfoPage.h"

RadarInfoPage::RadarInfoPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RadarInfoPage)
{
    ui->setupUi(this);
    //    init();
}

// Destructor: Cleans up RadarInfoPage resources, timers, and allocated memory.
RadarInfoPage::~RadarInfoPage()
{
    delete ui;
}


// Event/Signal handler for on rad  s band clicked.
void RadarInfoPage::on_rad_SBand_clicked()
{
    ui->stk_radarFrequency->setCurrentIndex(0);
}

// Event/Signal handler for on rad  x band clicked.
void RadarInfoPage::on_rad_XBand_clicked()
{
    ui->stk_radarFrequency->setCurrentIndex(1);
}

// Event/Signal handler for on rad  k band clicked.
void RadarInfoPage::on_rad_KBand_clicked()
{
    ui->stk_radarFrequency->setCurrentIndex(2);
}

// Event/Signal handler for on rad  c band clicked.
void RadarInfoPage::on_rad_CBand_clicked()
{
    ui->stk_radarFrequency->setCurrentIndex(3);
}



// Event/Signal handler for on btn ok clicked.
void RadarInfoPage::on_btnOk_clicked()
{
    if(ui->rad_PulseMode->isChecked())
    {
        radarOperation=0;
    }
    else if (ui->rad_ScanMode->isChecked())
    {
        radarOperation=1;
    }

    else
    {
        radarOperation=2;
    }

    emit btnClickedSignal(radarOperation);
}


















