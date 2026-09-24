#include "pulsePage.h"
#include "ui_pulsePage.h"

PulsePage::PulsePage(QWidget *parent, radarSpace::radarDataStruct * radarData) :
    QWidget(parent),
    ui(new Ui::PulsePage)
{
    ui->setupUi(this);
    this->radarData = radarData;

    QList<QRadioButton *> radioBtnList = findChildren<QRadioButton*>();
    QList<QSpinBox *> spinBoxList = findChildren<QSpinBox*>();
    QList<QComboBox *> comboBoxList = findChildren<QComboBox*>();

    QRadioButton *radioBtn;
    QSpinBox *spinBox;
    QComboBox *comboBox;

    foreach (radioBtn, radioBtnList)
    {
        connect(radioBtn, SIGNAL(clicked()),this,SLOT(btnRed()));
    }

    foreach (spinBox, spinBoxList)
    {
        connect(spinBox, SIGNAL(valueChanged(QString)),this,SLOT(btnRed()));
    }

    foreach (comboBox, comboBoxList)
    {
        connect(comboBox, SIGNAL(currentIndexChanged(QString)),this,SLOT(btnRed()));
    }
}




void PulsePage::on_btnOk_clicked()
{

//    SattarUtils::count = 0;

    //setting radar paremeters
    radarData->shiftTime     =  utils->timeToULong(ui->spnShiftTime->value(), ui->cmbMeasUnit->currentText());
    radarData->outPutPower   = 100 - ui->cmbRelativePower->currentText().toUInt();
    radarData->missPulse     = ui->spnMissPulse->value();
    radarData->radarPriority = ui->spnPriority->value();


    if (ui->edtRadarName->text().isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setText("please fill radar name.");
        msgBox.exec();
        return;
    }
    radarData->radarName     = ui->edtRadarName->text();

    radarData->spurPercent   = ui->spnSpur->value();
    radarData->durationTime  = 1000;

    //====================== pw type ====================================
    if(ui->rad_PW_Random->isChecked())
    {
        emit pwTypeSignal(pwSpace::pwTypeEnum::random);
    }
    else if (ui->rad_PW_MultiLevel->isChecked())
    {
        emit pwTypeSignal(pwSpace::pwTypeEnum::multilevel);
    }
    else
    {
        emit pwTypeSignal(pwSpace::pwTypeEnum::constant);
    }

    //====================== pri/prf type =================================
    if(ui->rad_PRIPRF_Jitter->isChecked())
    {
        emit priTypeSignal(priSpace::priTypeEnum::jitter);
    }
    else if (ui->rad_PRIPRF_Sliding->isChecked())
    {
        emit priTypeSignal(priSpace::priTypeEnum::sliding);
    }
    else if (ui->rad_PRIPRF_Stagger->isChecked())
    {
        emit priTypeSignal(priSpace::priTypeEnum::stagger);
    }
    else if (ui->rad_PRIPRF_Periodic->isChecked())
    {
        emit priTypeSignal(priSpace::priTypeEnum::periodic);
    }
    else if (ui->rad_PRIPRF_DwellAndSwitch->isChecked())
    {
        emit priTypeSignal(priSpace::priTypeEnum::dwell);
    }
    else if (ui->rad_PRIPRF_Triangular->isChecked())
    {
        emit priTypeSignal(priSpace::priTypeEnum::triangular);
    }
    else
    {
        emit priTypeSignal(priSpace::priTypeEnum::constant);
    }


    //====================== frq type ========================================
    if(ui->rad_frq_Agile->isChecked())
    {
        emit frqTypeSignal(frqSpace::typeEnum::Agile);
    }
    else if (ui->rad_frq_Hopping->isChecked())
    {
        emit frqTypeSignal(frqSpace::typeEnum::Hopping);
    }
    else if (ui->rad_frq_Diversity->isChecked())
    {
        emit frqTypeSignal(frqSpace::typeEnum::Diversity);
    }
    else
    {
        emit frqTypeSignal(frqSpace::typeEnum::Constant);
    }


    //====================== modulation type =============================
    if(ui->rad_mod_FSK->isChecked())
    {
        emit modulTypeSignal(modulSpace::modeTypeEnum::FSK);//fsk
    }

    else if (ui->rad_mod_XFM->isChecked())
    {
        emit modulTypeSignal(modulSpace::modeTypeEnum::xFM);//xfm
    }

    else if (ui->rad_mod_BPSK->isChecked())
    {
        emit modulTypeSignal(modulSpace::modeTypeEnum::BPSK);//bpsk
    }

    else if (ui->rad_mod_QPSK->isChecked())
    {
        emit modulTypeSignal(modulSpace::modeTypeEnum::QPSK);//qpsk
    }

    else
    {
        emit modulTypeSignal(modulSpace::modeTypeEnum::Simple); //simple
    }

    //====================== PRF/PRI =================================
    if(ui->radPRF->isChecked()){

        emit periodTypeSignal(priSpace::periodicTypeEnum::PRF);
    }
    else
    {
        emit periodTypeSignal(priSpace::periodicTypeEnum::PRI);
    }

    btnGreen();
}



PulsePage::~PulsePage()
{
    delete  radarData;
    delete ui;
}


void PulsePage::btnRed()
{
    ui->btnOk->setStyleSheet("background-color:red");
    utils->colorState(SattarUtils::colorStatus::pulsePage,false);
}

void PulsePage::btnGreen(){
    ui->btnOk->setStyleSheet("background-color:#186a3b");
    utils->colorState(SattarUtils::colorStatus::pulsePage,true);
}
