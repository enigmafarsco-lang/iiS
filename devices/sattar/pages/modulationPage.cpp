#include "modulationPage.h"
#include "ui_modulationPage.h"

ModulationPage::ModulationPage(QWidget *parent,
                               frqSpace::frqDataStruct     * frqData,
                               modulSpace::modulDataStruct * modulData,
                               radarSpace::radarDataStruct * radarData,
                               priSpace::priDataStruct     * priData,
                               pwSpace::pwDataStruct       * pwData,
                               spureSpace::spureStruct     * spureData,
                               projectDataSpace::projectDataStruct * prjData
                               ) :
    QWidget(parent),
    ui(new Ui::ModulationPage)
{
    ui->setupUi(this);
    this->frqData   = frqData;
    this->modulData = modulData;
    this->radarData = radarData;
    this->priData   = priData;
    this->pwData    = pwData;
    this->spureData = spureData;
    this->prjData = prjData;

    ui->fsk_edtStepCoast->setValidator(new QIntValidator(0,10,this));
    ui->fsk_edtSimpleBitInfo->setValidator(new QIntValidator(0,1,this));
    ui->qpsk_edtCustomCode->setValidator(new QIntValidator(0,1,this));
    ui->bpsk_edtCustomCode->setValidator(new QIntValidator(0,1,this));

    on_bpsk_radBarkerCode_toggled(true);
    on_fsk_radFskSimple_toggled(true);
    on_qpsk_radFrank_toggled(true);
    on_xfm_radLfm_toggled(true);

    ui->fsk_edtSimpleBitInfo->setText(modulData->fskSimpleBitInfo);
    ui->fsk_edtStepCoast->setText(modulData->fskStepCostas);


    QList<QRadioButton *> radioBtnList = findChildren<QRadioButton*>();
    QList<QSpinBox *> spinBoxList = findChildren<QSpinBox*>();
    QList<QDoubleSpinBox *> doubleSpinBoxList = findChildren<QDoubleSpinBox*>();
    QList<QComboBox *> comboBoxList = findChildren<QComboBox*>();

    QSpinBox *spinBox;
    QDoubleSpinBox *doubleSpinBox;
    QComboBox *comboBox;
    QRadioButton *radioBtn;

    foreach (spinBox, spinBoxList)
    {
        connect(spinBox, SIGNAL(valueChanged(QString)),this,SLOT(btnRed()));
    }

    foreach (radioBtn, radioBtnList)
    {
        connect(radioBtn, SIGNAL(clicked()),this,SLOT(btnRed()));
    }


    foreach (comboBox, comboBoxList)
    {
        connect(comboBox, SIGNAL(currentIndexChanged(QString)),this,SLOT(btnRed()));
    }


    foreach (doubleSpinBox, doubleSpinBoxList)
    {
        connect(doubleSpinBox, SIGNAL(valueChanged(QString)),this,SLOT(btnRed()));
    }


}

ModulationPage::~ModulationPage()
{
    delete ui;
}

void ModulationPage::modulInfoSlot(modulSpace::modeTypeEnum modultype)
{
    if(modulData->type != modultype || firstRun )
    {
        btnRed();
        firstRun = false;
        modulData->type = modultype;

        ui->stkModul->setVisible(true);
        if(modultype == modulSpace::modeTypeEnum::xFM)
        {
            ui->stkModul->setCurrentIndex(0);
        }

        else if (modultype == modulSpace::modeTypeEnum::FSK)
        {
            ui->stkModul->setCurrentIndex(1);
        }

        else if (modultype == modulSpace::modeTypeEnum::QPSK)
        {
            ui->stkModul->setCurrentIndex(3);
        }

        else if (modultype == modulSpace::modeTypeEnum::BPSK)
        {
            ui->stkModul->setCurrentIndex(2);
        }

        else//simple
        {
            ui->stkModul->setVisible(false);
            ui->btnOk->setVisible(false);
        }
    }

    else
    {

    }


}

//------------------------xfm----------------------------------

bool ModulationPage::xfmAction()
{
    modulData->xfmTargetFrq = ui->xfm_spnFrq->value() - BandA1;

    if(ui->xfm_radLfm->isChecked())
    {
        modulData->xFMType = modulSpace::xfmTypeEnum::LFM;

        if(ui->xfm_radSweepUnipolar->isChecked())
        {
            modulData->LFMSweetDirection = modulSpace::lfmSweepDirectionEnum::UniPolar;
        }

        else
        {
            modulData->LFMSweetDirection = modulSpace::lfmSweepDirectionEnum::Bipolar;
        }
    }

    else
    {
        modulData->xFMType = modulSpace::xfmTypeEnum::NLFM;
        QString valStr = QString::number(ui->xfm_spnTargetTime->value()) + " "+ui->xfm_cmbMeasUnit->currentText();
        modulData->xfmTargetTime = utils->timeToFloat(valStr);
    }

    for (int i{}; i<  this->frqData->frq.count(); i++)
    {
        if(frqData->frq[i] >= modulData->xfmTargetFrq)
        {
            utils->msgFunction("xFm target frequency shouldn`t be less than start frequency. \n Please lower xFM bandwidth or correct frequency value");
            return false;
        }
    }
    utils->spureGenerateDate(*spureData, *modulData, *radarData, *frqData, *priData, *pwData);
    return true;
}

//-----------------------fsk-----------------------------------

bool ModulationPage::fskAction()
{
    if(ui->fsk_radFskSimple->isChecked())
    {
        QString modifiedBitVal = bitInfiModifier();
        if(modifiedBitVal.isNull()) return false;


        modulData->fskSimpleBitInfo = modifiedBitVal;
        modulData->fskDeltaF = ui->fsk_spnSimpleDelta->value();
        modulData->fskSimpleTB = ui->fsk_spnTb->value();
        modulData->FSKType = modulSpace::fskTypeEnum::Simple1;
        deltaCheckSimple();
    }


    else
    {
        QString modifiedCostasVal = costasModifier();
        if(modifiedCostasVal.isNull()) return false;

        modulData->fskStepCostas = modifiedCostasVal;
        modulData->fskDeltaF = ui->fsk_spnStepDelta->value();
        modulData->FSKType = modulSpace::fskTypeEnum::Step;
        deltaCheckStep();

    }

    utils->spureGenerateDate(*spureData, *modulData, *radarData, *frqData, *priData, *pwData);
    return  true;
}

QString ModulationPage::bitInfiModifier()
{
    QString bitVal = ui->fsk_edtSimpleBitInfo->text();

    if (bitVal.isEmpty() || bitVal.isNull()|| bitVal.mid(0,1) ==",")
    {
        utils->msgFunction("Please enter bintInfo sequence.");
        return nullptr;
    }

    bitVal = bitVal.simplified().replace(" ", "");

    QString temp1 = ",";
    QString modifiedBitInfo = nullptr;

    for(auto &str:bitVal)
    {
        if(str == "1" ||str == "0" ||str == "," )
        {
            if(temp1 == str)
            {
                continue;
            }
            else
            {
                temp1 = str;
                modifiedBitInfo.append(str);
            }
        }
        else
        {
            utils->msgFunction("Please enter correct bit info value, [ correct value: 1, 0 and comma(,) ].");
            return nullptr;
        }
    }

    return  modifiedBitInfo;
}

QString ModulationPage::costasModifier()
{
    QString costasVal = ui->fsk_edtStepCoast->text();


    if (costasVal.isEmpty() || costasVal.isNull()  ||costasVal.mid(0,1) ==",")
    {
        utils->msgFunction("Please enter costas sequence.");
        return nullptr;
    }

    costasVal = costasVal.simplified().replace(" ", "");

    QString temp1 = nullptr;
    QString modifiedCosta = nullptr;

    for(auto &str:costasVal)
    {
        if(str == "0" ||str == "1" ||str == "2"||str == "3"||str == "4"||str == "5"||str == "6"||str == "7"||str == "8"||str == "9"||str == "," )
        {
            if(temp1 == str)
            {
                continue;
            }
            else
            {
                temp1 = str;
                modifiedCosta.append(str);
            }
        }
        else
        {
            utils->msgFunction("Please enter correct costas info value, [ correct value: 1, 10 and comma(,) ].");
            return nullptr;
        }
    }

    QStringList t = modifiedCosta.split(",");
    for (int i{};i < t.count() ; i++)
    {
        if(t[i].toInt() > 10)
        {
            utils->msgFunction("Please enter correct costas info value, [ correct value: 1, 10 and comma(,) ].");
            return nullptr;
        }
    }

    return  modifiedCosta;
}

void ModulationPage::deltaCheckSimple()
{
    for (uint i{}; i < frqData->countFrq; i++)
    {

        if(frqData->frq[i] + modulData->fskDeltaF > GetBandWidth_C ||
                frqData->frq[i] - modulData->fskDeltaF <= 0)
        {
            utils->msgFunction("FSK Delata F should not exceed frequency bandwidth.");
        }
    }

}

void ModulationPage::deltaCheckStep()
{
    QStringList strTemp = modulData->fskStepCostas.split(",");
    int max{};

    for (int i{}; i< strTemp.length(); i++)
    {
        int numTemp= strTemp[i].toInt();
        if( numTemp> max) max = numTemp ;
    }

    for (uint i{}; i< frqData->countFrq; i++)
    {
        if(frqData->frq[i] + (modulData->fskDeltaF * max) > GetBandWidth_C)
        {
            utils->msgFunction("maximum FSK F multiply to costas values should not exceed maximum bandwidth.");
        }
    }
}

//-----------------------bpsk-----------------------------------
bool ModulationPage::bpskAction()
{

    modulData->bpskTB = ui->bpsk_spnTb->value();

    if(ui->bpsk_radBarkerCode->isChecked())
    {
        modulData->bpsKType = modulSpace::bpskTypeEnum::Barker;
        modulData->bpskBarkerCode = ui->bpsk_cmbBarkerCode->currentText().toUInt();
    }

    else
    {
        modulData->bpsKType = modulSpace::bpskTypeEnum::Custom;
        QString modifiedBarker = barkerModifier();
        if(modifiedBarker.isNull()) return false;
        modulData->bpskCustomCode = modifiedBarker;
    }

    utils->spureGenerateDate(*spureData, *modulData, *radarData, *frqData, *priData, *pwData);
    return true;
}

QString ModulationPage::barkerModifier()
{
    QString barkerVal = ui->bpsk_edtCustomCode->text();

    if (barkerVal.isEmpty() || barkerVal.isNull())
    {
        utils->msgFunction("Please enter barkerVal sequence.");
        return nullptr;
    }

    barkerVal = barkerVal.simplified().replace(" ", "");

    QString temp1 = nullptr;
    QString modifiedBarkerVal = nullptr;

    for(auto &str:barkerVal)
    {
        if(str == "1" ||str == "0" ||str == "," )
        {
            if(temp1 == str)
            {
                continue;
            }
            else
            {
                temp1 = str;
                modifiedBarkerVal.append(str);
            }
        }
        else
        {
            utils->msgFunction("Please enter correct barker value, [ correct value: 1, 0 and comma(,) ].");
            return nullptr;
        }
    }

    return  modifiedBarkerVal;
}

void ModulationPage::on_bpsk_btnRandomGenerate_clicked()
{
    int count = ui->bpsk_spnCount->value();

    QString custom;

    for (int i{}; i < count;i++)
    {
        int randNum = QRandomGenerator::global()->bounded(0, 1000);
        if (randNum >= 0 && randNum < 250) custom += "0";
        else if (randNum >= 250 && randNum < 500) custom += "1";
        else if (randNum >= 500 && randNum < 750) custom += "0";
        else if (randNum >= 750 && randNum < 1000) custom += "1";
        if (i < count -1) custom += ",";
    }
    ui->bpsk_edtCustomCode->setText(custom);
}

//-----------------------qpsk-----------------------------------

void ModulationPage::qpskAction()
{
    modulData->qpskTB = ui->qpsk_spnTb->value(); // bit duration

    if(ui->qpsk_radFrank->isChecked())
    {
        modulData->qpskType = modulSpace::QpskTypeEnum::Frank;
        modulData->qpskFrankCode = ui->qpsk_cmbFrankCode->currentText().toUInt();
    }

    else
    {
        modulData->qpskType = modulSpace::QpskTypeEnum::Custom1;
        //QString modifiedCustom = customCodeModifier(); // here we should modifie the value with related function.
        modulData->qpskCustomCode = ui->qpsk_edtCustomCode->text();
    }

    utils->spureGenerateDate(*spureData, *modulData, *radarData, *frqData, *priData, *pwData);
}

void ModulationPage::on_qpsk_btnRandomGenerate_clicked()
{
    int count = ui->qpsk_spnCustomCount->value();

    QString custom;

    for (int i{}; i < count;i++)
    {
        int randNum = QRandomGenerator::global()->bounded(0, 1000);
        if (randNum >= 0 && randNum < 250) custom += "00";
        else if (randNum >= 250 && randNum < 500) custom += "01";
        else if (randNum >= 500 && randNum < 750) custom += "01";
        else if (randNum >= 750 && randNum < 1000) custom += "11";
        if (i < count -1) custom += ",";
    }
    ui->qpsk_edtCustomCode->setText(custom);
}

QString  ModulationPage::customCodeModifier()
{
    //    QString customVal = ui->qpsk_edtCustomCode->text();

    //    if (customVal.isEmpty() || customVal.isNull())
    //    {
    //        utils->msgFunction("Please enter custom sequence.");
    //        return nullptr;
    //    }

    //    customVal = customVal.simplified().replace(" ", "");

    //    QString temp1 = nullptr;
    //    QString modifiedCustomVal = nullptr;

    //    for(auto &str:customVal)
    //    {
    //        if(str == "1" ||str == "0" ||str == "," )
    //        {
    //            if(temp1 == str && str==",")
    //            {
    //                continue;
    //            }
    //            else
    //            {
    //                temp1 = str;
    //                modifiedCustomVal.append(str);
    //            }
    //        }
    //        else
    //        {
    //            utils->msgFunction("Please enter correct custom value, [ correct value: 1, 0 and comma(,) ].");
    //            return nullptr;
    //        }
    //    }

    //    for(int i{}){

    //    }

    //    return  modifiedCustomVal;
}

//------------------------others----------------------------------

void ModulationPage::on_xfm_radLfm_toggled(bool checked)
{
    if(checked){
        ui->xfm_lfmGroup->setEnabled(checked);
        ui->xfm_nlfmGroup->setEnabled(!checked);
    }
    else
    {
        ui->xfm_lfmGroup->setEnabled(checked);
        ui->xfm_nlfmGroup->setEnabled(!checked);
    }
}

void ModulationPage::on_bpsk_radBarkerCode_toggled(bool checked)
{
    if (checked)
    {
        ui->barkerCodeGroup->setEnabled(checked);
        ui->customCodeGroup->setEnabled(!checked);
    }
    else
    {
        ui->barkerCodeGroup->setEnabled(checked);
        ui->customCodeGroup->setEnabled(!checked);
    }
}

void ModulationPage::on_qpsk_radFrank_toggled(bool checked)
{

    //    std::cout << checked << std::endl;
    if (checked)
    {
        ui->frankGroup->setEnabled(checked);
        ui->customGroup->setEnabled(!checked);
    }
    else
    {
        ui->frankGroup->setEnabled(checked);
        ui->customGroup->setEnabled(!checked);
    }
}

void ModulationPage::on_fsk_radFskSimple_toggled(bool checked)
{
    //    std::cout << checked << std::endl;
    if (checked)
    {
        ui->fskGroup->setEnabled(checked);
        ui->fskStepGroup->setEnabled(!checked);
    }
    else
    {
        ui->fskGroup->setEnabled(checked);
        ui->fskStepGroup->setEnabled(!checked);
    }
}

void ModulationPage::on_bpsk_cmbBarkerCode_activated(int index)
{
    switch (index)
    {

    case 0:
        ui->lblBarker->setText("1,0");
        break;

    case 1:
        ui->lblBarker->setText("1,1,0");
        break;

    case 2:
        ui->lblBarker->setText("1,0,1,1");
        break;

    case 3:
        ui->lblBarker->setText("1,1,1,0,1");
        break;

    case 4:
        ui->lblBarker->setText("1,1,1,0,0,1,0");
        break;

    case 5:
        ui->lblBarker->setText("1,1,1,0,0,0,1,0,0,1,0");
        break;

    case 6:
        ui->lblBarker->setText("1,1,1,1,1,0,0,1,1,0,1,0,1");
        break;

    default:
        ui->lblBarker->setText("");
        break;
    }
}

void ModulationPage::on_btnOk_clicked()
{
    if(modulData->type == modulSpace::modeTypeEnum::xFM)
    {
        if(!xfmAction()) return;
    }

    else if (modulData->type == modulSpace::modeTypeEnum::FSK)
    {
        if (!fskAction()) return;
    }

    else if (modulData->type == modulSpace::modeTypeEnum::QPSK)
    {
        qpskAction();
    }

    else if (modulData->type == modulSpace::modeTypeEnum::BPSK)
    {
        if (!bpskAction()) return;
    }

    //    utils->showSelectedRadarInfo(*modulData, *radarData, *frqData, *priData, *pwData,* spureData, *prjData);
    btnGreen();
    utils->showSelectedRadar(*modulData,
                             *radarData,
                             *frqData,
                             *priData,
                             *pwData,
                             * spureData,
                             *prjData);

    showMenuOptionSignal(true);

}



void ModulationPage::btnRed(){
    ui->btnOk->setStyleSheet("background-color:red");
    utils->colorState(utils->colorStatus::modulPage,false);
}

void ModulationPage::btnGreen(){
    ui->btnOk->setStyleSheet("background-color:#186a3b");
    utils->colorState(utils->colorStatus::modulPage,true);
}
