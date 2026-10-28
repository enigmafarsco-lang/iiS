/**
 * @file radarInfoPage.cpp
 * @brief Subsystem module for pages in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

﻿#include "radarInfoPage.h"
#include "ui_radarInfoPage.h"

RadarInfoPage::RadarInfoPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RadarInfoPage)
{
    ui->setupUi(this);
    //    init();
}

RadarInfoPage::~RadarInfoPage()
{
    delete ui;
}


/**
 * @brief Slot handler responding to on rad  s band clicked event.
 */
void RadarInfoPage::on_rad_SBand_clicked()
{
    ui->stk_radarFrequency->setCurrentIndex(0);
}

/**
 * @brief Slot handler responding to on rad  x band clicked event.
 */
void RadarInfoPage::on_rad_XBand_clicked()
{
    ui->stk_radarFrequency->setCurrentIndex(1);
}

/**
 * @brief Slot handler responding to on rad  k band clicked event.
 */
void RadarInfoPage::on_rad_KBand_clicked()
{
    ui->stk_radarFrequency->setCurrentIndex(2);
}

/**
 * @brief Slot handler responding to on rad  c band clicked event.
 */
void RadarInfoPage::on_rad_CBand_clicked()
{
    ui->stk_radarFrequency->setCurrentIndex(3);
}



/**
 * @brief Slot handler responding to on btn ok clicked event.
 */
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


















