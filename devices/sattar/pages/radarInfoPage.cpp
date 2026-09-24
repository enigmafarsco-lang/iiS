#include "radarInfoPage.h"
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


void RadarInfoPage::on_rad_SBand_clicked()
{
    ui->stk_radarFrequency->setCurrentIndex(0);
}

void RadarInfoPage::on_rad_XBand_clicked()
{
    ui->stk_radarFrequency->setCurrentIndex(1);
}

void RadarInfoPage::on_rad_KBand_clicked()
{
    ui->stk_radarFrequency->setCurrentIndex(2);
}

void RadarInfoPage::on_rad_CBand_clicked()
{
    ui->stk_radarFrequency->setCurrentIndex(3);
}



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


















