#include "pwPage.h"
#include "ui_pwPage.h"

PWPage::PWPage(QWidget *parent,
               pwSpace::pwDataStruct * pwData,
               radarSpace::radarDataStruct * radarData,
               priSpace::priDataStruct * priData) :
    QWidget(parent),
    ui(new Ui::PWPage)
{
    ui->setupUi(this);
    this->pwData = pwData;
    this->radarData = radarData;
    this->priData = priData;


    QList<QSpinBox *> spinBoxList = findChildren<QSpinBox*>();
    QList<QComboBox *> comboBoxList = findChildren<QComboBox*>();

    QSpinBox *spinBox;
    QComboBox *comboBox;

    foreach (spinBox, spinBoxList)
    {
        connect(spinBox, SIGNAL(valueChanged(QString)),this,SLOT(btnRed()));
    }


    foreach (comboBox, comboBoxList)
    {
        connect(comboBox, SIGNAL(currentIndexChanged(QString)),this,SLOT(btnRed()));
    }

    connect(ui->tblPW, SIGNAL(cellChanged(int,int)), this, SLOT(btnRed()));

}

void PWPage::pwInfoSlot(pwSpace::pwTypeEnum pwType)
{
    if(pwData->Type != pwType || firstRun)
    {
        firstRun = false;
        pwData->Type = pwType;
        on_btnClear_clicked();

        if(pwType == pwSpace::pwTypeEnum::multilevel)
        {
            ui->grbConstant->setEnabled(false);
            ui->stackMulitiPW->setVisible(true);
            ui->stackMulitiPW->setCurrentIndex(0);
        }

        else if (pwType == pwSpace::pwTypeEnum::random)
        {
            ui->grbConstant->setEnabled(true);
            ui->stackMulitiPW->setVisible(true);
            ui->stackMulitiPW->setCurrentIndex(1);
        }

        else
        {
            ui->grbConstant->setEnabled(true);
            ui->stackMulitiPW->setVisible(false);
        }
    }
}

void PWPage::changeLable()
{
    float maxVal = priData->maxPri;
    float minVal = priData->minPri;

    ui->lblMaxTime->setText("PRI/PRF Max Time: " + utils->floatToTime(maxVal));
    ui->lblMinTime->setText("PRI/PRF Min Time: " + utils->floatToTime(minVal));

    QString tmp ;
    int x = utils->getTotalPulseCount(*radarData);

    if ( x == -1) return;
    else tmp = utils->getTotalPulseCount(*radarData) == 1 ? "Unlimited" : QString::number(utils->getTotalPulseCount(*radarData));

    ui->lblPulseCount->setText("PRI/PRF Pulse Count: " + tmp );
}



void PWPage::on_btnAdd_clicked()
{
    ui->tblPW->insertRow(ui->tblPW->rowCount());

    QPushButton * btn = new QPushButton;
    btn->setText("Del");
    btn->setStyleSheet("background-color:red");

    QComboBox * cmbTime = new QComboBox;
    cmbTime->addItem("nS");
    cmbTime->addItem("uS");
    cmbTime->addItem("mS");
    ui->tblPW->setCellWidget(row,2, cmbTime );
    ui->tblPW->setCellWidget(row,3, btn );

    row++;

    connect(btn, SIGNAL(clicked()),this, SLOT(btnSlot()));
}

void PWPage::btnSlot()
{
    row--;
    ui->tblPW->removeRow(ui->tblPW->currentRow());
    on_tblPW_cellChanged(0,0);
    btnRed();
}

void PWPage::on_btnClear_clicked()
{
    row =0;
    ui->tblPW->setRowCount(0);
    pwData->pw.clear();
    on_tblPW_cellChanged(0,0);
    btnRed();
}

void PWPage::on_btnOk_clicked()
{

    if(pwData->Type == pwSpace::pwTypeEnum::constant)
    {
        pwData->pwConstant = utils->timeToULong(ui->spnConstantPW->value(), ui->cmbMeasUnit_ConstantPW->currentText() ) / alpha;
    }

    else if (pwData->Type == pwSpace::pwTypeEnum::random)
    {
        pwData->randomPwPercent = ui->spnRandomPercent->value();
        pwData->randomPwCount = ui->spnRandomPulseCount->value();
        pwData->pwConstant = utils->timeToULong(ui->spnConstantPW->value(), ui->cmbMeasUnit_ConstantPW->currentText() ) / alpha;
    }

    else if (pwData->Type == pwSpace::pwTypeEnum::multilevel)
    {
        pwData->multiLevel.clear();
        if (!pwTableValue("pwRange")) return;
    }


    if (!generatePWData(*pwData) || pwData->isError)
    {
//        utils->msgFunction("PW range is 50nS to 2mS.\n Please correct error and continue");
//        return;
    }

    if (!utils->setMaxPulseCount(pwData->countPw,*radarData)) return;

    if(utils->getTotalPulseCount(*radarData) == -1)return;
    if(utils->regeneratePulsePRI(utils->getTotalPulseCount(*radarData),*priData) == -1) return;
    if (utils->regeneratePulsePW(utils->getTotalPulseCount(*radarData),*pwData) == -1) return;

    bool res = utils->checkError(*priData, *pwData );

    if(!res)
    {
        utils->msgFunction("PW pulse must be less than PRI/PRF time.");
        return;
    }

    else
    {

    }

    emit enableFrqPage();
    btnGreen();
}

bool PWPage::pwTableValue(QString status)
{

    if(ui->tblPW->rowCount() == 0)
    {
        utils->msgFunction("Please insert PW value");
        return false;
    }

    for (int i {}; i < ui->tblPW->rowCount(); i++)
    {
        QTableWidgetItem * val1 (ui->tblPW->item(i,0));
        QTableWidgetItem * val2 (ui->tblPW->item(i,1));

        if(val1 && val2)
        {
            pwData->pwStr.multiplier = ui->tblPW->item(i,0)->text().toUInt();
            pwData->pwStr.pw = ui->tblPW->item(i,1)->text().toUInt();
            pwData->pwStr.unint = qobject_cast<QComboBox*>(ui->tblPW->cellWidget(i,2))->currentText();

            QString valStr = QString::number(pwData->pwStr.pw) + " "+pwData->pwStr.unint;
            float pw = utils->timeToFloat(valStr );

            if(status == "pwRange")
            {
                if(!pwRange(pw)) return false ;
            }
            else
            {
                if(!pwCompare(pw)) return false;
            }

        }

        else
        {
            utils->msgFunction("There are some empty row. Please fill that.");
            return  false;
        }
    }


    return  true;
}

bool PWPage::pwRange(float &pw){
    if(pwData->pwStr.multiplier == 0 || pwData->pwStr.pw == 0 || pw < 50 || pw > 2000000)
    {
        //here we have to change background color of cells
        utils->msgFunction("PW range is 50nS to 2mS.\n Please correct error and continue");
        return false;
    }
    else
    {
        pwData->multiLevel.append(pwData->pwStr);
    }
    return true;
}

bool PWPage::pwCompare(float&pw)
{
    if(pw >= priData->minPri)
    {
        utils->msgFunction("Index out of range!");
        return false;
    }
    return true;
}

PWPage::~PWPage()
{
    delete ui;
    //    delete  pwData;
    //    delete  radarData;
    //    delete  priData;

}

void PWPage::on_tblPW_cellChanged(int row, int column)
{

    Q_UNUSED(row);
    Q_UNUSED(column);

    uint tmp{};
    for (int i {}; i < ui->tblPW->rowCount(); i++)
    {
        QTableWidgetItem * pwFilled (ui->tblPW->item(i,0));
        if(pwFilled)
        {
            tmp += ui->tblPW->item(i,0)->text().toUInt() ;
        }
    }

    ui->totalPulse->setText("Count: "+QString::number(tmp));
}

void PWPage::btnRed(){
    ui->btnOk->setStyleSheet("background-color:red");
    utils->colorState(SattarUtils::colorStatus::pwPage,false);
}

void PWPage::btnGreen(){
    ui->btnOk->setStyleSheet("background-color:#186a3b");
    utils->colorState(SattarUtils::colorStatus::pwPage,true);
}


//====================================================================

bool PWPage::generatePWData(pwSpace::pwDataStruct &data )
{
    if(data.Type == pwSpace::pwTypeEnum::constant)
    {
        if(data.pwConstant < data.MinPwValue || data.pwConstant > data.MaxPwValue)
        {
            data.isError = true;
            data.isGenerated = false;
            return false;
        }

        data.pw.clear();
        data.pw.append(data.pwConstant);
        data.countPw = 1;
        data.maxPw = data.pwConstant;
        data.minPw = data.pwConstant;
        data.isGenerated = true;
        data.isError = false;
    }

    //---------------------------------------------------
    else if (data.Type == pwSpace::pwTypeEnum::multilevel)
    {
        data.pw.clear();
        data.minPw = static_cast<uint>(MaxValPW_C);
        data.maxPw = 0;
        data.countPw = 0;
        for(int i {}; i < data.multiLevel.count(); i++)
        {
            uint pwVal = utils->timeToInt(data.multiLevel[i].pw, data.multiLevel[i].unint);

            if(pwVal >= 50 && pwVal <= 2000000)
            {
                data.minPw > pwVal ? data.minPw = pwVal : data.minPw ;
                data.maxPw < pwVal ? data.maxPw = pwVal : data.minPw ;

                for(uint j {0}; j < data.multiLevel[i].multiplier;j++ )
                {
                    data.pw.append(pwVal);
                    data.countPw++;
                }
            }
            else {
                data.countPw = 0;
                data.isError = true;
                data.isGenerated = false;
                return false;
            }
        }

        data.isGenerated = true;
        data.isError = false;
    }
    //---------------------------------------------------

    else if (data.Type == pwSpace::pwTypeEnum::random)
    {
        if (data.pwConstant >= data.MinPwValue || data.pwConstant <= data.MaxPwValue){
            data.countPw = 0;
            data.minPw = MaxValPW_C;
            data.maxPw = 0;
            //random

            int maxJitter = data.pwConstant + ((data.pwConstant * data.randomPwPercent) / 100);
            int minJitter = data.pwConstant - ((data.pwConstant * data.randomPwPercent) / 100);

            if(minJitter < MinValPW_C) minJitter = MinValPW_C;
            if(maxJitter > MaxValPW_C) maxJitter = MaxValPW_C;

            data.pw.clear();

            for(uint i{}; i < data.randomPwCount; i ++)
            {
                if(minJitter > maxJitter)
                {
                    int tmp = maxJitter;
                    maxJitter = minJitter;
                    minJitter = tmp;
                }
                uint pwVal = static_cast<uint>(QRandomGenerator::global()->bounded( minJitter, maxJitter));
                if (data.minPw > pwVal) data.minPw = pwVal;
                if(data.maxPw < pwVal) data.maxPw = pwVal;
                data.pw.append(pwVal);
                data.countPw ++;
            }
            data.isError = false;
            data.isGenerated = true;
        }
        else
        {

            data.isError = true;
            data.isGenerated = false;
            return false;
        }
    }

    return true;
}

