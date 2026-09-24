#include "priPage.h"
#include "ui_priPage.h"

#include <QPropertyAnimation>

PRIPage::PRIPage(QWidget *parent,
                 priSpace::priDataStruct * priData,
                 radarSpace::radarDataStruct * radarData) :
    QWidget(parent),
    ui(new Ui::PRIPage)
{
    ui->setupUi(this);
    this->priData = priData;
    this->radarData = radarData;

    QList<QSpinBox *> spinBoxList = findChildren<QSpinBox*>();
    QList<QDoubleSpinBox *> doubleSpinBoxList = findChildren<QDoubleSpinBox*>();
    QList<QComboBox *> comboBoxList = findChildren<QComboBox*>();

    QSpinBox *spinBox;
    QDoubleSpinBox *doubleSpinBox;
    QComboBox *comboBox;

    foreach (spinBox, spinBoxList)
    {
        connect(spinBox, SIGNAL(valueChanged(QString)),this,SLOT(btnRed()));
    }


    foreach (comboBox, comboBoxList)
    {
        connect(comboBox, SIGNAL(currentIndexChanged(int)),this,SLOT(btnRed()));
    }


    foreach (doubleSpinBox, doubleSpinBoxList)
    {
        connect(doubleSpinBox, SIGNAL(valueChanged(QString)),this,SLOT(btnRed()));
    }

    connect(ui->tblPRIOne, SIGNAL(cellChanged(int,int)), this, SLOT(btnRed()));
}

//hide  all pri type
void PRIPage::disablePRI()
{
    ui->grbJitter->setEnabled(false);
    ui->grbSliding->setEnabled(false);
    ui->grbPeriodic->setEnabled(false);
    ui->grbTriangular->setEnabled(false);
}

//show selected pri type
void PRIPage::enableDisablePRIPageSlot(priSpace::priTypeEnum typeVal)
{
    if(priData->type != typeVal || firstRun)
    {
        isPriTypeChanged = true;
        priData->type = typeVal;
        disablePRI();
        on_btnClear_clicked();

        if (typeVal == priSpace::priTypeEnum::stagger)
        {
            //selecting which stack must be shown.
            ui->stkPRI->setCurrentIndex(1);
            ui->tblPRIOne->setColumnHidden(0,true);
        }

        else if (typeVal == priSpace::priTypeEnum::dwell)
        {
            ui->stkPRI->setCurrentIndex(1);
            ui->tblPRIOne->setColumnHidden(0,false);
        }

        else
        {
            ui->stkPRI->setCurrentIndex(0);

            if(typeVal == priSpace::priTypeEnum::jitter )
            {
                ui->grbJitter->setEnabled(true);
            }

            else if (typeVal == priSpace::priTypeEnum::sliding)
            {
                ui->grbSliding->setEnabled(true);
            }

            else if (typeVal == priSpace::priTypeEnum::periodic)
            {
                ui->grbPeriodic->setEnabled(true);
            }

            else if (typeVal == priSpace::priTypeEnum::triangular)
            {
                ui->grbTriangular->setEnabled(true);
            }
        }
    }

    else
    {
        isPriTypeChanged = false;
    }

}

void PRIPage::periodTypeSlot(priSpace::periodicTypeEnum periodType)
{
    if(priData->prifType != periodType || firstRun )
    {
        firstRun = false;
        isPeriodChanged = true;

        on_btnClear_clicked();
        priData->prifType = periodType;

        if(periodType == priSpace::periodicTypeEnum::PRF)
        {
            ui->lblPeriodic->setText("Periodic Frq(KHz):");
            ui->lblSliding->setText("Ramp Step(KHz):");
            ui->lblTriang->setText("Ramp Step(KHz):");

            ui->cmbMeasUnit->clear();
            ui->cmbMeasUnit->addItem("KHz");
            ui->cmbMeasUnit->addItem("MHz");
            ui->cmbMeasUnit->addItem("Hz");
            ui->tblPRIOne->horizontalHeaderItem(2)->setText("Freq. Unit");
            ui->spnPRF_PRI->setValue(10);
            btnGreen();
        }

        else if (periodType == priSpace::periodicTypeEnum::PRI)
        {
            ui->lblPeriodic->setText("Periodic Frq(uS):");
            ui->lblSliding->setText("Ramp Step(uS):");
            ui->lblTriang->setText("Ramp Step(uS):");

            ui->cmbMeasUnit->clear();
            ui->cmbMeasUnit->addItem("uS");
            ui->cmbMeasUnit->addItem("mS");
            ui->cmbMeasUnit->addItem("nS");
            ui->tblPRIOne->horizontalHeaderItem(2)->setText("Time Unit");
            ui->spnPRF_PRI->setValue(100);
            btnGreen();
        }
    }
}

void PRIPage::on_btnOk_clicked()
{
    QString tmp1;
    if(priData->prifType ==  priSpace::periodicTypeEnum::PRI)
    {
        tmp1 = QString::number( ui->spnPRF_PRI->value() / alpha);
    }
    else{
        tmp1 = QString::number( ui->spnPRF_PRI->value() * alpha);
    }

    SattarUtils::normalSample =  (ui->spnPRF_PRI->value()+1) * 449;

    QString tmp2 = ui->cmbMeasUnit->currentText();

    priData->priBase = tmp1+" "+tmp2;

    if(priData->type ==  priSpace::priTypeEnum::jitter){
        priData->jitterPercent = ui->spnJitterPercentage->value();
        priData->jitterCount = ui->spnJitterPulseCount->value();
    }
    else if (priData->type == priSpace::priTypeEnum::sliding) {
        priData->slidingStep = ui->spnSlidingRampStep->value();
        priData->slidingCount = ui->spnSlidingRampCount->value();
    }
    else if (priData->type == priSpace::priTypeEnum::periodic) {
        priData->periodicFreq = ui->spnPeriodicFrq->value();
        priData->periodicMean = ui->spnPeriodicMeanPRI->value();
        priData->periodicCount = ui->spnPeriodicCount->value();
    }
    else if (priData->type == priSpace::priTypeEnum::triangular) {
        priData->triangularStep = ui->spnTriangRampStep->value();
        priData->triangularCount = ui->spnTriangRampCount->value();
    }

    else if (priData->type == priSpace::priTypeEnum::stagger)
    {
        if (!priTableValue()) return;
    }

    else if (priData->type == priSpace::priTypeEnum::dwell)
    {
        if(! priTableValue()) return;
    }


    if (generatePRIData(*priData))
    {
        resetMaxPulseCount();
        emit enablePWPage();
        ui->btnOk->setStyleSheet("background-color:#186a3b");
        utils->colorState(SattarUtils::colorStatus::priPage,true);
    }

    else
    {
        priData->prifType == priSpace::periodicTypeEnum::PRF ?
                    utils->msgFunction("PRF range is 50Hz to 2Mhz. \nPlease correct error and continue."):
                    utils->msgFunction("PRI range is 501nS to 49mS. \nPlease correct error and continue.");
        return;
    }
}

void PRIPage::resetMaxPulseCount()
{
    radarData->maxPulseCount = priData->pri.count();
}

void PRIPage::on_btnAdd_clicked()
{

    ui->tblPRIOne->insertRow(ui->tblPRIOne->rowCount());
    QComboBox * c = new QComboBox;
    QPushButton * btn = new QPushButton;
    btn->setText("Del");
    btn->setStyleSheet("background-color:red");

    if(priData->prifType == priSpace::periodicTypeEnum::PRI){
        c->clear();
        c->addItem("nS");
        c->addItem("uS");
        c->addItem("mS");
    }
    else {
        c->clear();
        c->addItem("KHz");
        c->addItem("MHz");
        c->addItem("Hz");
    }

    ui->tblPRIOne->setCellWidget(row,2, c );
    ui->tblPRIOne->setCellWidget(row,3, btn );
    row++;

    connect(btn, SIGNAL(clicked()),this, SLOT(btnSlot()));

}

void PRIPage::btnSlot()
{
    row--;
    ui->tblPRIOne->removeRow(ui->tblPRIOne->currentRow());
    on_tblPRIOne_cellChanged(0,0);
    btnRed();
}

void PRIPage::on_btnClear_clicked()
{
    row =0;
    ui->tblPRIOne->setRowCount(0);
    on_tblPRIOne_cellChanged(0,0);
    btnRed();
    priData->pri.clear();

}

//Calculate table value when user choise stagger or dwell type
bool PRIPage::priTableValue()
{
    //check if table is empty
    if(ui->tblPRIOne->rowCount() == 0)
    {
        utils->msgFunction("Please enter PRI value.");
        return false;
    }

    bool res{false};
    priData->dwell.clear();

    for (int i{}; i < ui->tblPRIOne->rowCount(); i++)
    {
        bool levalIsFilled{true};
        QTableWidgetItem * priPrfIsFilled (ui->tblPRIOne->item(i,1));

        if(priData->type == priSpace::priTypeEnum::dwell)
        {
            QTableWidgetItem * val1(ui->tblPRIOne->item(i,0));
            if(!val1) levalIsFilled = false;
        }


        if(levalIsFilled && priPrfIsFilled)
        {
            if(priData->type == priSpace::priTypeEnum::dwell) priData->priStr.multiplier = ui->tblPRIOne->item(i,0)->text().toUInt();
            priData->priStr.pri = ui->tblPRIOne->item(i,1)->text().toUInt();
            priData->priStr.unit = qobject_cast<QComboBox*>(ui->tblPRIOne->cellWidget(i,2))->currentText();
            float pri{};

            if(priData->prifType == priSpace::periodicTypeEnum::PRF)
            {
                QString valStr = QString::number(priData->priStr.pri) + " "+priData->priStr.unit;
                pri = utils->frqToTime(valStr);
            }

            else
            {
                QString valStr = QString::number(priData->priStr.pri) + " "+priData->priStr.unit;
                pri = utils->timeToFloat(valStr);
            }

            if(priData->priStr.multiplier == 0 || priData->priStr.pri ==0 || pri > priData->maxPriValue || pri < priData->minPriValue)
            {
                if(priSpace::periodicTypeEnum::PRF)
                {
                    utils->msgFunction("PRF range is 50Hz to 2Mhz. please correct error and continue.");
                    return false;
                }
                else
                {
                    utils->msgFunction("PRI range is 501nS to 49mS. please correct error and continue.");
                    return false;
                }
            }
            priData->dwell.append(priData->priStr);
            res =  true;
        }

        else
        {
            utils->msgFunction("There are empty rows. Please fill them.");
            return  false;
        }
    }

    return  res;
}

PRIPage::~PRIPage()
{
    delete ui;
    //     delete  priData;
    //    delete radarData;
}

void PRIPage::on_tblPRIOne_cellChanged(int row, int column)
{
    Q_UNUSED(row);
    Q_UNUSED(column);

    int count = 0;

    if(priData->type == priSpace::priTypeEnum::stagger )
    {
        for (int i {}; i < ui->tblPRIOne->rowCount(); i++)
        {
            QTableWidgetItem * priPrfIsFilled (ui->tblPRIOne->item(i,1));
            if(priPrfIsFilled)
            {
                count += 1 ;
            }
        }
        ui->lblCount->setText("Count: "+QString::number(count));
    }

    else
    {
        for (int i {}; i < ui->tblPRIOne->rowCount(); i++)
        {
            QTableWidgetItem * priPrfIsFilled (ui->tblPRIOne->item(i,0));
            if(priPrfIsFilled)
            {
                count += ui->tblPRIOne->item(i,0)->text().toUInt() ;
            }
        }

        ui->lblCount->setText("Count: "+QString::number(count));
    }


}

void PRIPage::btnRed()
{
    ui->btnOk->setStyleSheet("background-color:red");
    utils->colorState(SattarUtils::colorStatus::priPage,false);
}

void PRIPage::btnGreen()
{
    if(utils->isBtnColorGreen[1] && isPeriodChanged== false && isPriTypeChanged==false)
    {
        ui->btnOk->setStyleSheet("background-color:#186a3b");
    }

    else
    {
        btnRed();
    }
}

//=======================================================================================

bool PRIPage::generatePRIData(priSpace::priDataStruct & data)
{
    data.isError = false;
    data.isGenerated = false;
    float prf{};
    float priVal{};

    //========================================================
    if(data.type == priSpace::priTypeEnum::constant )
    {
        if(data.prifType == priSpace::periodicTypeEnum::PRF)
        {
            priVal = utils->frqToTime(data.priBase);
        }

        else
        {
            priVal = utils->timeToFloat(data.priBase);
        }

        if (priVal >= data.minPriValue && priVal <= data.maxPriValue)
        {
            if(priVal < 1000)
            {
                uint p = static_cast<uint>(priVal) ;
                priVal = static_cast<float>(p) ;
            }
            data.pri.clear();
            data.pri.append(priVal);
            data.CountPri = 1;
            data.minPri = priVal;
            data.maxPri = priVal;
        }
        else

        {
            return elseFunction(data);
        }
    }

    //========================================================
    else if (data.type == priSpace::priTypeEnum::jitter)
    {
        if(data.prifType == priSpace::periodicTypeEnum::PRF)
        {
            priVal = utils->frqToTime(data.priBase );
        }
        else
        {
            priVal = utils->timeToFloat(data.priBase);
        }

        if(priVal >= data.minPriValue && priVal <= data.maxPriValue)
        {
            initPRIVal(data);
            data.pri.clear();

            int  maxJitter = static_cast<int>(priVal) + static_cast<int>((priVal * data.jitterPercent) / 100);
            int  minJitter = static_cast<int>(priVal) - static_cast<int>((priVal * data.jitterPercent) / 100);

            if(minJitter < static_cast<int>(data.minPriValue)) minJitter = static_cast<int>(data.minPriValue);
            if(maxJitter > static_cast<int>(data.maxPriValue)) maxJitter = static_cast<int>(data.maxPriValue);


            for(uint i{0} ; i < data.jitterCount; i ++)
            {
                priVal = static_cast<float>(QRandomGenerator::global()->bounded( minJitter, maxJitter));
                if (data.minPri > priVal) data.minPri = priVal;
                if (data.maxPri < priVal) data.maxPri = priVal;

                data.pri.append(priVal);
                data.CountPri ++;
            }
        }
        else
        {
            return elseFunction(data);
        }
    }

    //========================================================
    else if (data.type == priSpace::priTypeEnum::sliding)
    {
        if( data.prifType == priSpace::periodicTypeEnum::PRF)
        {
            priVal = utils->frqToTime(data.priBase);
            prf = utils->frqToULong(data.priBase);
        }
        else
        {
            priVal = utils->timeToFloat(data.priBase);
        }

        if(priVal >= data.minPriValue && priVal <= data.maxPriValue)
        {
            initPRIVal(data);
            data.pri.clear();
            float pris{};

            for(uint i{0}; i < data.slidingCount; i++)
            {
                if(data.prifType == priSpace::periodicTypeEnum::PRI)
                {
                    pris = static_cast<float>((data.slidingStep * 1000 * i) + priVal);
                }
                else
                {
                    QString priValStr = QString::number((data.slidingStep * 1000 * i) + prf)+" " + "Hz";
                    pris = utils->frqToTime(priValStr);
                }
                minMaxPRI(data,pris);
            }
        }
        else
        {
            return elseFunction(data);
        }
    }

    //========================================================
    else if (data.type == priSpace::priTypeEnum::triangular)
    {
        if(data.prifType == priSpace::periodicTypeEnum::PRF)
        {
            priVal = utils->frqToTime(data.priBase);
            prf = utils->frqToULong(data.priBase);
        }
        else {
            priVal = utils->timeToFloat(data.priBase);
        }

        if(priVal >= data.minPriValue && priVal <= data.maxPriValue)
        {
            initPRIVal(data);
            data.pri.clear();
            float pris{};

            for(uint i {0}; i < data.triangularCount; i++)
            {
                if(data.prifType == priSpace::periodicTypeEnum::PRI){
                    pris = static_cast<float>((data.slidingStep * 1000 * i) + priVal);
                }
                else
                {
                    QString priValStr = QString::number((data.triangularStep * 1000 * i) + prf)+" " + "Hz";
                    pris = utils->frqToTime(priValStr);
                }
                minMaxPRI(data, pris);
            }

            for (int i{static_cast<int>(data.triangularCount -1)}; i >= 0 ; i--  )
            {
                if(data.prifType == priSpace::periodicTypeEnum::PRI){
                    pris = static_cast<float>(data.triangularStep * 1000 * i + priVal);
                }
                else
                {
                    QString priValStr = QString::number((data.triangularStep * 1000 * i) + prf)+" " + "Hz";
                    pris = utils->frqToTime(priValStr);
                }

                minMaxPRI(data, pris);
            }
        }
        else {
            return elseFunction(data);
        }
    }

    //========================================================
    else if (data.type == priSpace::priTypeEnum::periodic)
    {
        if (data.prifType == priSpace::periodicTypeEnum::PRF)
        {
            priVal = utils->frqToTime(data.priBase);
        }

        else
        {
            priVal = utils->timeToFloat(data.priBase);
        }

        float amplitude = (priVal * data.periodicMean) / 100;
        double frq = data.periodicFreq * 1000 * 2;
        double phaseAngle{};
        double sampleRate{1000000};

        if(priVal >= data.minPriValue && priVal <= data.maxPriValue)
        {
            initPRIVal(data);
            data.pri.clear();

            for(uint i{0};i < data.periodicCount; i++)
            {
                minMaxPRI( data, priVal,amplitude,phaseAngle,frq, sampleRate);
            }
        }
        else {
            return elseFunction(data);
        }
    }

    //========================================================
    else if(data.type == priSpace::priTypeEnum::stagger)
    {
        initPRIVal(data);
        data.pri.clear();

        for(int i {0}; i < data.dwell.count(); i++ )
        {
            if(data.prifType == priSpace::periodicTypeEnum::PRF)
            {
                QString priValStr = QString::number(data.dwell[i].pri) + " "+data.dwell[i].unit;
                priVal = utils->frqToTime(priValStr);
            }

            else
            {
                QString priValStr = QString::number(data.dwell[i].pri) + " " + data.dwell[i].unit;
                priVal = utils->timeToFloat(priValStr);
            }

            if(priVal >= data.minPriValue && priVal <= data.maxPriValue)
            {
                minMaxPRI(data, priVal);
            }

            else
            {
                return elseFunction(data);
            }
        }

        if(data.pri.count() == 0 )
        {
            return elseFunction(data);
        }
    }
    //========================================================
    else if(data.type == priSpace::priTypeEnum::dwell){

        initPRIVal(data);
        data.pri.clear();

        for(uint i {}; i < static_cast<uint>(data.dwell.count()); i++){

            if(data.prifType == priSpace::periodicTypeEnum::PRF)
            {
                QString priValStr = QString::number(data.dwell[i].pri) + " "+ data.dwell[i].unit;
                priVal = utils->frqToTime(priValStr);

            }
            else {
                QString priValStr = QString::number(data.dwell[i].pri) + " "+ data.dwell[i].unit;
                priVal = utils->timeToFloat(priValStr);
            }
            if( priVal > data.minPriValue && priVal < data.maxPriValue)
            {
                minMaxPRIDwell(data,priVal);
                for(uint j{}; j <data.dwell[i].multiplier; j++)
                {
                    data.pri.append(priVal);
                    data.CountPri ++;
                }
            }
            else {
                return elseFunction(data);

            }
        }
        if(data.pri.count() == 0){
            return elseFunction(data);

        }

    }

    data.isError = false;
    data.isGenerated = true;
    return  true;
}

bool PRIPage::elseFunction(priSpace::priDataStruct &data)
{
    data.isError = true;
    data.isGenerated = false;
    return false;
}

void PRIPage::initPRIVal(priSpace::priDataStruct & data)
{
    data.CountPri = 0;
    data.minPri = MaxVal_C;
    data.maxPri = 0;
}

void PRIPage::minMaxPRIDwell(priSpace::priDataStruct &data, float& pris)
{
    if (data.minPri > pris) data.minPri = pris;
    if (data.maxPri < pris) data.maxPri = pris;
    if (data.minPri < data.minPriValue) pris = data.minPriValue;
    if (data.maxPri > data.maxPriValue) pris = data.maxPriValue;
}

void PRIPage::minMaxPRI(priSpace::priDataStruct &data, float& priVal, float&amplitude , double&phaseAngle, double&frq, double&sampleRate)
{
    float periodicVal = Gen(amplitude, phaseAngle, frq, sampleRate);
    periodicVal = priVal + periodicVal;

    if(data.minPri > periodicVal)  data.minPri = periodicVal;
    if(data.maxPri < periodicVal) data.maxPri = periodicVal;
    if(periodicVal < data.minPriValue) periodicVal = data.minPriValue;
    if(periodicVal > data.maxPriValue) periodicVal = data.maxPriValue;
    data.pri.append(periodicVal);
    data.CountPri ++;
}

void PRIPage::minMaxPRI(priSpace::priDataStruct &data, float& pris)
{
    if (data.minPri > pris) data.minPri = pris;
    if (data.maxPri < pris) data.maxPri = pris;
    if (data.minPri < data.minPriValue) pris = data.minPriValue;
    if (data.maxPri > data.maxPriValue) pris = data.maxPriValue;
    data.pri.append(pris);
    data.CountPri ++;
}

float PRIPage::Gen(float & amp, double &phaseAng, double &frq, double &sampleRate)
{
    float val = amp * static_cast<float>(sin(phaseAng));
    phaseAng += 2 * PI_C * frq / sampleRate;
    if (phaseAng > 2 * PI_C)
    {
        phaseAng -= 2* PI_C;
    }
    return val;
}
