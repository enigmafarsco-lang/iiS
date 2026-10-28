// ============================================================================
// File: radarInfoPage.cpp
// Subsystem: eLynxSDR pages
// Description: Subsystem implementation and interface guidance for radarInfoPage.cpp.
// ============================================================================

#include "radarInfoPage.h"
#include "ui_radarInfoPage.h"

RadarInfoPage::RadarInfoPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RadarInfoPage)
{
    ui->setupUi(this);
    //    init();
}

// Destructor: Cleans up RadarInfoPage allocated resources, timers, and buffers.
RadarInfoPage::~RadarInfoPage()
{
    delete ui;
}


// Signal/Slot handler responding to on rad  s band clicked event.
void RadarInfoPage::on_rad_SBand_clicked()
{
    ui->stk_radarFrequency->setCurrentIndex(0);
}

// Signal/Slot handler responding to on rad  x band clicked event.
void RadarInfoPage::on_rad_XBand_clicked()
{
    ui->stk_radarFrequency->setCurrentIndex(1);
}

// Signal/Slot handler responding to on rad  k band clicked event.
void RadarInfoPage::on_rad_KBand_clicked()
{
    ui->stk_radarFrequency->setCurrentIndex(2);
}

// Signal/Slot handler responding to on rad  c band clicked event.
void RadarInfoPage::on_rad_CBand_clicked()
{
    ui->stk_radarFrequency->setCurrentIndex(3);
}



// Signal/Slot handler responding to on btn ok clicked event.
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


















