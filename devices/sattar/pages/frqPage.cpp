#include "frqPage.h"
#include "ui_frqPage.h"

FrqPage::FrqPage(QWidget *parent,
                 frqSpace::frqDataStruct             * frqData,
                 modulSpace::modulDataStruct         * modulData,
                 radarSpace::radarDataStruct         * radarData,
                 priSpace::priDataStruct             * priData,
                 pwSpace::pwDataStruct               * pwData,
                 spureSpace::spureStruct             * spureData,
                 projectDataSpace::projectDataStruct * prjData) :
    QWidget(parent),
    ui(new Ui::FrqPage)
{
    ui->setupUi(this);
    this->frqData   = frqData;
    this->modulData = modulData;
    this->radarData = radarData;
    this->priData   = priData;
    this->pwData    = pwData;
    this->spureData = spureData;
    this->prjData   = prjData;


    QList<QDoubleSpinBox *> doubleSpinBoxList = findChildren<QDoubleSpinBox*>();
    QList<QSpinBox *> spinBoxList = findChildren<QSpinBox*>();
    QList<QComboBox *> comboBoxList = findChildren<QComboBox*>();

    QSpinBox *spinBox;
    QComboBox *comboBox;
    QDoubleSpinBox *doubleSpinBox;

    foreach (spinBox, spinBoxList)
    {
        connect(spinBox, SIGNAL(valueChanged(QString)),this,SLOT(btnRed()));
    }


    foreach (comboBox, comboBoxList)
    {
        connect(comboBox, SIGNAL(currentIndexChanged(QString)),this,SLOT(btnRed()));
    }

    foreach (doubleSpinBox, doubleSpinBoxList)
    {
        connect(doubleSpinBox, SIGNAL(valueChanged(QString)),this,SLOT(btnRed()));
    }
    connect(ui->tblFrq, SIGNAL(cellChanged(int,int)), this, SLOT(btnRed()));

}


void FrqPage::frqTypeSlot(frqSpace::typeEnum type)
{
    if(frqData->frqType != type || firstRun)
    {
        frqData->frqType = type;
        firstRun = false;
        on_btnClear_clicked();

        if(type == frqSpace::typeEnum::Agile || type == frqSpace::typeEnum::Constant)
        {
            ui->stkFrq->setCurrentIndex(0);
            if(type == frqSpace::typeEnum::Constant)ui->grbAgile->setEnabled(false);
            else ui->grbAgile->setEnabled(true);
        }

        else
        {
            if(type == frqSpace::typeEnum::Hopping) ui->tblFrq->setColumnHidden(0,true);
            else ui->tblFrq->setColumnHidden(0,false);
            ui->stkFrq->setCurrentIndex(1);
        }
    }

    else
    {

    }
}

void FrqPage::changeLable()
{
    QString tmp ;
    int x = utils->getTotalPulseCount(*radarData);

    if ( x == -1) return;
    else tmp = utils->getTotalPulseCount(*radarData) == 1 ? "Unlimited" : QString::number(utils->getTotalPulseCount(*radarData));

    ui->totalPulse->setText("PRI/PRF Pulse Count: " + tmp );
}

void FrqPage::setFrqCenterSlot(double frqCenter)
{
    ui->lblFrqCenter->setText("Center frq:"+QString::number(frqCenter));
    this->frqCenter= frqCenter;
    ui->spnBase_3->setMinimum(frqCenter-250);
    ui->spnBase_3->setMaximum(frqCenter+250);
    ui->lblStartFrq->setText("Band Start Frequency: "+QString::number(frqCenter-250));
    ui->lblEndFrq->setText("Band End Frequency: "+QString::number(frqCenter+250));
}

void FrqPage::on_btnOk_clicked()
{
    float frq{};

    if(frqData->frqType == frqSpace::typeEnum::Constant)
    {
        //        frqData->baseFrq = ui->spnBase_3->value() - BandA1;
        double diff = ui->spnBase_3->value() - frqCenter;
        frqData->baseFrq = diff * alpha;
    }

    else if (frqData->frqType == frqSpace::typeEnum::Agile)
    {
        if(!agileCalc(frq)) return ;
    }

    else if(frqData->frqType == frqSpace::typeEnum::Diversity)
    {
        if(!frqTableValue())
        {
            return ;
        }
    }

    else
    {
        if(!frqTableValue()) return ;
    }

    //---------------------------------------------------------------------------
    frq = frqData->baseFrq + BandA1;

    if(!(frq >= BandA1 && frq <= (BandA1 + GetBandWidth_C)))
    {
        //        utils->msgFunction("Frequency value should be between "+QString::number(BandA1)+" and "+ QString::number(BandA1 + GetBandWidth_C));
        //        return;
    }

    generateData(BandA1, GetBandWidth_C, *this->frqData);
    if(!utils->setMaxPulseCount(this->frqData->frq.count(), *this->radarData)) return;

    if(frqData->frqType == frqSpace::typeEnum::Diversity || frqData->frqType == frqSpace::typeEnum::Hopping)
    {
        if (utils->getTotalPulseCount(*radarData) == -1) return;
        else
        {
            if(static_cast<uint>(utils->getTotalPulseCount(*radarData)) != frqData->countFrq)
            {
                utils->msgFunction("PW, PRI and frquency pilse count must be equal.");
                return;
            }
        }
    }

    if (utils->getTotalPulseCount(*radarData) == -1)return;
    if (utils->regeneratePulsePRI(utils->getTotalPulseCount(*radarData), *this->priData) == -1) return;
    if (utils->regeneratePulsePW(utils->getTotalPulseCount (*radarData), *this->pwData) == -1)return;
    if (regeneratePulseFrq(utils->getTotalPulseCount(*radarData), *this->frqData) == -1) return;

    if(modulData->type == modulSpace::modeTypeEnum::Simple)
    {
        btnGreen();
        utils->spureGenerateDate(*spureData,*modulData, *radarData, *frqData, *priData, *pwData);
        utils->showSelectedRadar(*modulData,*radarData,*frqData,*priData,*pwData,*spureData,*prjData);
        emit showMenuOptionSignal(true);
    }

    else
    {
        emit enableModulPage();
        btnGreen();
    }
}

void FrqPage::on_btnAdd_clicked()
{
    btnRed();
    ui->tblFrq->insertRow(ui->tblFrq->rowCount());

    QLabel * lblUnit = new QLabel;
    lblUnit->setText("MHz");

    QPushButton * btn = new QPushButton;
    btn->setText("Del");
    btn->setStyleSheet("background-color:red");

    ui->tblFrq->setCellWidget(row,2, lblUnit );
    ui->tblFrq->setCellWidget(row,3, btn );

    row++;

    connect(btn, SIGNAL(clicked()),
            this, SLOT(btnSlot()));
}


void FrqPage::btnSlot()
{
    row--;
    ui->tblFrq->removeRow(ui->tblFrq->currentRow());
    on_tblFrq_cellChanged(0,0);
}


void FrqPage::on_tblFrq_cellChanged(int row, int column)
{
    Q_UNUSED(row);
    Q_UNUSED(column);

    btnRed();
    uint tmp{};

    if(frqData->frqType == frqSpace::typeEnum::Diversity )
    {
        for (int i {}; i < ui->tblFrq->rowCount(); i++)
        {
            QTableWidgetItem * frqIsFilled (ui->tblFrq->item(i,0));
            if(frqIsFilled)
            {
                tmp += ui->tblFrq->item(i,0)->text().toUInt() ;
            }
        }


        ui->lblCount->setText("Count: "+QString::number(tmp));
    }

    else
    {
        for (int i {}; i < ui->tblFrq->rowCount(); i++)
        {
            QTableWidgetItem * frqIsFilled (ui->tblFrq->item(i,1));
            if(frqIsFilled)
            {
                tmp += 1 ;
            }
        }
        ui->lblCount->setText("Count: "+QString::number(tmp));
    }
}


void FrqPage::on_btnClear_clicked()
{
    row =0;
    ui->tblFrq->setRowCount(0);
    on_tblFrq_cellChanged(0,0);
    btnRed();
    frqData->frq.clear();
}

bool FrqPage::frqTableValue()
{
    if(ui->tblFrq->rowCount()== 0)
    {
        utils->msgFunction("Please enter frequency value.");
        return false;
    }

    bool res{false};
    frqData->diversity.clear();

    for (int i {}; i < ui->tblFrq->rowCount(); i++)
    {
        bool levalIsFilled{true};
        QTableWidgetItem * frqIsFilled (ui->tblFrq->item(i,1));

        if(frqData->frqType  == frqSpace::typeEnum::Diversity)
        {
            QTableWidgetItem * val1(ui->tblFrq->item(i,0));
            if(!val1) levalIsFilled = false;
        }


        if(levalIsFilled && frqIsFilled)
        {
            if(frqData->frqType == frqSpace::typeEnum::Diversity) frqData->frqStr.multiplier = ui->tblFrq->item(i,0)->text().toUInt();

            double result = ui->tblFrq->item(i,1)->text().toDouble() - frqCenter;
            frqData->frqStr.frq = result * alpha;

            if(ui->tblFrq->item(i,1)->text().toDouble() > frqCenter +250  or  ui->tblFrq->item(i,1)->text().toDouble() < frqCenter - 250)
            {
                utils->msgFunction("Please insert correct value.");
                return false;
            }
//            if( !(  (frqData->frqStr.multiplier == 0) || ((frqData->frqStr.frq >= BandA1 ) && (frqData->frqStr.frq <= (BandA1 + GetBandWidth_C)))  ))
//            {
//                utils->msgFunction("Please insert correct value.");

//            }
            res =  true;
            frqData->diversity.append(frqData->frqStr);
        }

        else
        {
            utils->msgFunction("There are some empty row. Please fill that.");
            return false;
        }
    }

    return  res;
}

bool FrqPage::agileCalc(float &frq)
{
    double diff = ui->spnBase_3->value() - frqCenter;
    frqData->baseFrq = diff * alpha;

    frqData->agilePercent = ui->spnAgilePerc_3->value();
    frqData->agileCount = ui->spnAgilePulse_3->value();

    frq = ui->spnBase_3->value();
    float per = ui->spnAgilePerc_3->value();
    float f = frq * (per /200);

    if( ((frq - f) < BandA1) || ((frq + f) > BandA1 + GetBandWidth_C) )
    {
//        utils->msgFunction("Agile frequency value should be between "+QString::number(BandA1)+" and "+ QString::number(BandA1 + GetBandWidth_C));
//        return false;
    }

    return true;
}

FrqPage::~FrqPage()
{
    delete ui;
}


void FrqPage::btnRed(){
    ui->btnOk->setStyleSheet("background-color:red");
    utils->colorState(SattarUtils::colorStatus::frqPage,false);
}

void FrqPage::btnGreen(){
    ui->btnOk->setStyleSheet("background-color:#186a3b");
    utils->colorState(SattarUtils::colorStatus::frqPage,true);
}


bool FrqPage::generateData(uint startBand, uint bw, frqSpace::frqDataStruct &frqData)
{
    frqData.isError = false;
    frqData.isGenerated = false;
    float frqVal;

    if(frqData.frqType == frqSpace::typeEnum::Constant )
    {
        frqData.frq.clear();
        frqData.frq.append(frqData.baseFrq);
        frqData.minFrq = frqData.baseFrq;
        frqData.maxFrq = frqData.baseFrq;
        frqData.countFrq = 1;
    }

    else if (frqData.frqType == frqSpace::typeEnum::Agile)
    {
        frqData.countFrq = 0;
        frqData.minFrq = MaxVal_C;
        frqData.maxFrq = 0;

        int maxAgile = static_cast<int>(((startBand + frqData.baseFrq) * frqData.agilePercent) / 200);
        int minAgile = maxAgile * (-1);

        if(maxAgile > bw) maxAgile = bw;

        frqData.frq.clear();
        for(uint i {}; i < frqData.agileCount; i++){
            frqVal = static_cast<float>(QRandomGenerator::global()->bounded( minAgile, maxAgile));
            frqVal +=frqData.baseFrq;
            if(frqData.minFrq > frqVal) frqData.minFrq = frqVal;
            if(frqData.maxFrq < frqVal) frqData.maxFrq = frqVal;
            frqData.frq.append(frqVal);
            frqData.countFrq++;
        }


    }

    else if (frqData.frqType == frqSpace::typeEnum::Hopping)
    {
        frqData.countFrq = 0;
        frqData.minFrq = MaxVal_C;
        frqData.maxFrq = 0;
        frqData.frq.clear();
        for(int i{}; i < frqData.diversity.count(); i++){

            frqVal = frqData.diversity[i].frq;

            if(frqVal >= startBand && frqVal <= (startBand + bw)){
                frqVal -= startBand;
                if(frqData.minFrq > frqVal) frqData.minFrq = frqVal;
                if(frqData.maxFrq < frqVal) frqData.maxFrq = frqVal;

                frqData.frq.append(frqVal);
                frqData.countFrq ++;
            }
            else {
                frqData.isError = true;
                frqData.isGenerated = false;
                return false ;
            }
        }
    }

    else if (frqData.frqType == frqSpace::typeEnum::Diversity)
    {

        frqData.countFrq = 0;
        frqData.minFrq = MaxVal_C;
        frqData.maxFrq = 0;
        frqData.frq.clear();
        for(int i{}; i < frqData.diversity.count(); i++)
        {

            frqVal = frqData.diversity[i].frq;
            frqVal -= startBand;
            if(frqData.minFrq > frqVal) frqData.minFrq = frqVal;
            if(frqData.maxFrq < frqVal) frqData.maxFrq = frqVal;

            for(uint j{}; j < frqData.diversity[i].multiplier; j++)
            {
                frqData.frq.append(frqVal);
                frqData.countFrq ++;
            }
        }
    }

    frqData.isError = false;
    frqData.isGenerated = true;
    return true;
}


int FrqPage::regeneratePulseFrq(uint count, frqSpace::frqDataStruct & frqData)
{

    int retVal{-1};

    if(frqData.isGenerated == true)
    {
        if(frqData.countFrq == count)
        {
            return frqData.countFrq;
        }

        else if (frqData.countFrq < count && frqData.countFrq == 1 )
        {
            for(int i{}; i < static_cast<int>(count -1); i++)
            {
                frqData.frq.append(frqData.frq[0]);
            }

            frqData.countFrq = frqData.frq.count();
            return frqData.countFrq;
        }

        else
        {
            return  retVal;
        }
    }

    return  retVal;
}
