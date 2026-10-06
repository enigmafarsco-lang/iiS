
#include "exciter.h"
#include "ui_exciter.h"
#include "constants/tx_calibration.h"
#include <QLineEdit>
#include <QRegularExpression>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QRegExp>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <cmath>

// Phase 6: DAC file playback rate per ADRV9009 profile (MS/s) =
// 122.88 x P/100: the board consumes the DAC buffer at the profile's
// DAC sample rate - 122.88 MS/s at P100, 245.76 at P200, 491.52 at
// P400.  The spot/bridge generators use the same table
// (files/spot/generate.py, files/bridge/generate_bridge.py).
static double txFileRateMhz(int profileBw)
{
    // The txt generation rate for ALL exciter tabs is 122.88 x P/100
    // MS/s: 122.88 x 1 = 122.88 (profile 100), 122.88 x 2 = 245.76
    // (profile 200), 122.88 x 4 = 491.52 (profile 400).
    return 122.88 * profileBw / 100.0;
}

Exciter::Exciter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::exciter)
{
    ui->setupUi(this);
    // Phase 5: free-typed spot bandwidth (e.g. "40"), NOT a pull-down menu:
    // the user writes 1 .. profile BW and spot{N}mhz_{P}.txt is loaded
    // (P = active profile: 100 -> spotN mhz_100.txt, 400 -> spotN mhz_400.txt).
    ui->cmbBW->setEditable(true);
    ui->cmbBW->clear();
    if (ui->cmbBW->lineEdit())
        ui->cmbBW->lineEdit()->setPlaceholderText(
            QStringLiteral("1 - %1 MHz").arg(profileBw));
    // Default spot bandwidth: 10 (MHz).
    ui->cmbBW->setEditText(QStringLiteral("10"));
    //    QPushButton * getBtn[] = {ui->btnGetCW,ui->btnGetWB,ui->btnDisableSpot,ui->btnStopSweep,ui->btnDisImpulse};
    QPushButton * setBtn[] = {ui->btnSetCW,ui->btnSetWB, ui->btnLoadSpot, ui->btnStartSweep, ui->btnLoadImpulse};

    ui->grpDacBuffer->setVisible(false);

    for (int i{};i < 5; i++)
    {
//        setBtn[i]->setEnabled(false);
        connect(setBtn[i], SIGNAL(clicked()),this,SLOT(setDataSlot()));
    }

    //=================== Phase 6: DDS parameters on the CW tab =============
    // The CW tab drives the on-chip DDS tone engine (iio-oscilloscope
    // "DDS Mode: One CW Tone") on TX1 and TX2 instead of the DAC buffer,
    // so its DDS parameters (frequency / scale / phase - same ranges as
    // the DDS device panel) are shown right on the CW tab and applied to
    // both TXs when "On" is pressed.
    {
        QGridLayout *cwGrid = qobject_cast<QGridLayout *>(ui->cw->layout());
        if (cwGrid)
        {
            cwGrid->addWidget(new QLabel(QStringLiteral("DDS Freq (MHz):")), 2, 0);
            cwDdsFrqSpn = new QDoubleSpinBox;
            cwDdsFrqSpn->setRange(-245.76, 245.76);
            cwDdsFrqSpn->setDecimals(3);
            cwDdsFrqSpn->setValue(10.0);
            cwGrid->addWidget(cwDdsFrqSpn, 2, 1);
            cwGrid->addWidget(new QLabel(QStringLiteral("DDS Scale (dBFS):")), 3, 0);
            cwDdsScaleSpn = new QDoubleSpinBox;
            cwDdsScaleSpn->setRange(-91.0, 0.0);
            cwDdsScaleSpn->setDecimals(1);
            cwDdsScaleSpn->setValue(-10.0);
            cwGrid->addWidget(cwDdsScaleSpn, 3, 1);
            cwGrid->addWidget(new QLabel(QStringLiteral("DDS Phase (deg):")), 4, 0);
            cwDdsPhaseSpn = new QDoubleSpinBox;
            cwDdsPhaseSpn->setRange(0.0, 360.0);
            cwDdsPhaseSpn->setDecimals(1);
            cwDdsPhaseSpn->setValue(0.0);
            cwGrid->addWidget(cwDdsPhaseSpn, 4, 1);
        }
    }

    //=================== Phase 6: Bridge Noise tab ===================
    // Band-limited noise files bridge/bridge{N}mhz_{P}.txt created by
    // files/bridge/generate_bridge.py (same sharp band-limited noise
    // engine as the spot files, board-calibrated sample rate).
    {
        QWidget *bridgeTab = new QWidget;
        QVBoxLayout *bridgeVbox = new QVBoxLayout(bridgeTab);
        QHBoxLayout *bridgeRow = new QHBoxLayout;
        bridgeRow->addWidget(new QLabel(QStringLiteral("Bandwidth (MHz):")));
        bridgeSpn = new QDoubleSpinBox;
        bridgeSpn->setMinimum(1.0);
        bridgeSpn->setMaximum(400.0);
        bridgeSpn->setDecimals(0);
        bridgeSpn->setValue(10.0);
        bridgeRow->addWidget(bridgeSpn);
        QPushButton *btnSetBridge = new QPushButton(QStringLiteral("Set Bridge Noise"));
        bridgeRow->addWidget(btnSetBridge);
        QPushButton *btnDisableBridge = new QPushButton(QStringLiteral("Disable"));
        bridgeRow->addWidget(btnDisableBridge);
        bridgeRow->addStretch(1);
        bridgeVbox->addLayout(bridgeRow);
        bridgeVbox->addStretch(1);
        ui->tabWidget->addTab(bridgeTab, QStringLiteral("Bridge Noise"));
        connect(btnSetBridge, SIGNAL(clicked()), this, SLOT(setDataSlot()));
        connect(btnDisableBridge, &QPushButton::clicked, this, [this]() {
            if (isExciterOn) emit changeDacSignal("Bridge");
            setModeActive("bridge", false);
        });
    }

    //=================== Phase 6: Multi Target tab ====================
    // Up to 5 selectable targets.  Each target: enable checkbox +
    // modulation type (Spot/CW/Impulse/LFM/NLFM/Bridge) + its own
    // specification (fields relabel per type) + its own frequency shift
    // (LFM/NLFM rows have NO shift - just start frequency / BW / T like
    // the LFM/NLFM tabs).  On "Generate & Send" every selected target is
    // written to its own txt file, shifted by its own complex exponential
    // (e^{j 2 pi f t}) and summed into one I/Q stream (MultiTarget.txt)
    // that is sent to the DAC buffer.
    {
        QWidget *mtTab = new QWidget;
        QVBoxLayout *mtVbox = new QVBoxLayout(mtTab);
        for (int t = 0; t < 5; t++)
        {
            QHBoxLayout *row = new QHBoxLayout;
            mtEnable[t] = new QCheckBox(QStringLiteral("Target %1").arg(t + 1));
            row->addWidget(mtEnable[t]);
            mtType[t] = new QComboBox;
            mtType[t]->addItems({QStringLiteral("Spot"), QStringLiteral("CW"),
                                 QStringLiteral("Impulse"), QStringLiteral("LFM"),
                                 QStringLiteral("NLFM"), QStringLiteral("Bridge")});
            row->addWidget(mtType[t]);
            for (int s = 0; s < 3; s++)
            {
                mtSpecLbl[t][s] = new QLabel(QStringLiteral("--"));
                row->addWidget(mtSpecLbl[t][s]);
                mtSpec[t][s] = new QDoubleSpinBox;
                mtSpec[t][s]->setDecimals(2);
                row->addWidget(mtSpec[t][s]);
            }
            mtShiftLbl[t] = new QLabel(QStringLiteral("Shift (MHz):"));
            row->addWidget(mtShiftLbl[t]);
            mtShift[t] = new QDoubleSpinBox;
            mtShift[t]->setMinimum(-200.0);
            mtShift[t]->setMaximum(200.0);
            mtShift[t]->setDecimals(2);
            row->addWidget(mtShift[t]);
            mtVbox->addLayout(row);
            const int rowT = t;
            connect(mtType[t],
                    QOverload<int>::of(&QComboBox::currentIndexChanged),
                    this, [this, rowT]() { updateMultiTargetRow(rowT, true); });
        }
        QPushButton *btnSetMulti = new QPushButton(QStringLiteral("Generate & Send Multi Target"));
        mtVbox->addWidget(btnSetMulti);
        QPushButton *btnDisableMulti = new QPushButton(QStringLiteral("Disable"));
        mtVbox->addWidget(btnDisableMulti);
        mtVbox->addStretch(1);
        ui->tabWidget->addTab(mtTab, QStringLiteral("Multi Target"));
        connect(btnSetMulti, SIGNAL(clicked()), this, SLOT(setDataSlot()));
        connect(btnDisableMulti, &QPushButton::clicked, this, [this]() {
            if (isExciterOn) emit changeDacSignal("MultiTarget");
            setModeActive("multitarget", false);
        });
        for (int t = 0; t < 5; t++)
            updateMultiTargetRow(t, true);
    }

    //=================== Phase 6: LFM / NLFM tabs ====================
    // Same flow as the spot tab: type the specification (start frequency,
    // bandwidth, pulse duration), press Set and the chirp is synthesized
    // at the profile playback rate (P^2/800 MS/s) into Lfm.txt /
    // Nlfm.txt and sent to the DAC buffer.  No pre-made files needed -
    // the waveform is built in-app (phase 2*pi*(f0*t + B*t^2/2T), period
    // T).  NLFM = LFM with a raised-cosine amplitude coding.
    {
        auto buildChirpTab = [this](const QString &title, const QString &btnText,
                                    QDoubleSpinBox **startSpn,
                                    QDoubleSpinBox **bwSpn,
                                    QDoubleSpinBox **tSpn)
        {
            QWidget *tab = new QWidget;
            QVBoxLayout *vbox = new QVBoxLayout(tab);
            QHBoxLayout *row = new QHBoxLayout;
            *startSpn = new QDoubleSpinBox;
            (*startSpn)->setRange(-245.76, 245.76);
            (*startSpn)->setDecimals(2);
            (*startSpn)->setValue(0.0);
            *bwSpn = new QDoubleSpinBox;
            (*bwSpn)->setRange(0.1, 200.0);
            (*bwSpn)->setDecimals(2);
            (*bwSpn)->setValue(10.0);
            *tSpn = new QDoubleSpinBox;
            (*tSpn)->setRange(0.1, 10000.0);
            (*tSpn)->setDecimals(2);
            (*tSpn)->setValue(10.0);
            row->addWidget(new QLabel(QStringLiteral("Start (MHz):")));
            row->addWidget(*startSpn);
            row->addWidget(new QLabel(QStringLiteral("BW (MHz):")));
            row->addWidget(*bwSpn);
            row->addWidget(new QLabel(QStringLiteral("T (us):")));
            row->addWidget(*tSpn);
            row->addStretch(1);
            vbox->addLayout(row);
            QPushButton *btnSet = new QPushButton(btnText);
            vbox->addWidget(btnSet);
            QPushButton *btnDisable = new QPushButton(QStringLiteral("Disable"));
            vbox->addWidget(btnDisable);
            vbox->addStretch(1);
            ui->tabWidget->addTab(tab, title);
            connect(btnSet, SIGNAL(clicked()), this, SLOT(setDataSlot()));
            return btnDisable;
        };
        QPushButton *btnDisableLfm =
            buildChirpTab(QStringLiteral("LFM"), QStringLiteral("Set LFM"),
                          &lfmStartSpn, &lfmBwSpn, &lfmTSpn);
        connect(btnDisableLfm, &QPushButton::clicked, this, [this]() {
            if (isExciterOn) emit changeDacSignal("Lfm");
            setModeActive("lfm", false);
        });
        QPushButton *btnDisableNlfm =
            buildChirpTab(QStringLiteral("NLFM"), QStringLiteral("Set NLFM"),
                          &nlfmStartSpn, &nlfmBwSpn, &nlfmTSpn);
        connect(btnDisableNlfm, &QPushButton::clicked, this, [this]() {
            if (isExciterOn) emit changeDacSignal("Nlfm");
            setModeActive("nlfm", false);
        });
    }

    //    ui->tabWidget->setStyleSheet("QTabBar::tab:selected {background-color:" +QString(DARK_OLIVE_GREEN)+ ";}");

    //    connect(&exciterConnection, &QTimer::timeout, this, &Exciter::connectToDevice);

    //public some elements in order to access them and set their value with cart data
    this->spnCWPower =   ui->spnCWPower;
    spnSweepPower    =   ui->spnSweepPower;
    spnimpulsePower  =   ui->spnImpulsePower;
    spnWidePower     =   ui->spnWBPower;
    spnSpotPower     =   ui->spnSpotPower;

    spnCWFrq      = ui->spnCWFrq;
    spnSweepStep  = ui->spnSweepStep;
    spnSweepStop  = ui->spnSweepStopFrq;
    spnSweepStart = ui->spnSweepStartFrq;

    spnimpulseFrq  = ui->spnImpulseFrq;
    spnWideFrq     = ui->spnCWFrq;
    spnSpotFrq     = ui->spnSpotFrq;
    powerOn        = ui->chbPower;
    spnDelay       = ui->spnDelay;

    spnImpPw = ui->spnImpulsePulseWidth;
    spnImpPri = ui-> spnImpulsePRI;

    startBtn       = ui->btnStartSweep;
    stopBtn        = ui->btnStopSweep;

    btnDisableSpot  = ui->btnDisableSpot;
    btnSetCW        = ui->btnSetCW;
    btnSetWB        = ui->btnSetWB;
    btnLoadSpot     = ui->btnLoadSpot;
    btnDisableCW    = ui->btnDisableCW;
    btnDisableWB    = ui->btnDisableWB;
    btnStopSweep    = ui->btnStopSweep;
    btnDisImpulse   = ui->btnDisImpulse;
    btnStartSweep   = ui->btnStartSweep;
    btnLoadImpulse  = ui->btnLoadImpulse;


    connect(ui->spnCWFrq, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double val){ ui->lblCurrentFrq->setNum(ui->spnCWFrq->value());});
    // Phase 6: "Current P (attenuation)" shows the calibrated TX power
    // (P + TX calibration, see constants/tx_calibration.h).
    connect(ui->spnCWPower, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double val){ refreshTxPowerDisplay();});

    connect(ui->chbPower, &QCheckBox::stateChanged, [&](int val){

        ui->chbPower->setText(val ? "Power On":"Power Off");
        ui->chbPower->setStyleSheet(val? "color: #9acd32":"color: #ff0000");
    });



    //-------------------------------------------------------------------
    //spin box limitation
    if(DC_6_UPTO_8_12 == 0)
    {
        ui->spnCWFrq->setRange         (200,6000);
        ui->spnSpotFrq->setRange       (200,6000);
        ui->spnImpulseFrq->setRange    (200,6000);
    }

    else
    {
        ui->spnCWFrq->setRange         (8000,12000);
        ui->spnSpotFrq->setRange       (8000,12000);
        ui->spnImpulseFrq->setRange    (8000,12000);
    }

    // Phase 6: the Sweep tab synthesizes BASEBAND stepped-sine I/Q like the
    // LFM tab (the LO does not move), so its start/stop/step live in the
    // playback band of the DAC file, not in the RF band of the cart.
    ui->spnSweepStartFrq->setRange (-245.76, 245.76);
    ui->spnSweepStopFrq->setRange  (-122.878, 122.878);
    ui->spnSweepStep->setRange     (0.01, 491.52);
    ui->spnSweepStartFrq->setValue (0.0);
    ui->spnSweepStopFrq->setValue  (10.0);
    ui->spnSweepStep->setValue     (2.0);

    //-------------------------------------------------------------------
    ui->chbPower->setVisible(false);
    refreshTxPowerDisplay();
}

Exciter::~Exciter()
{
    delete ui;
}



//void Exciter::receiveIpAddressSlot(bool isConnect)
//{
////    ipAddress = ip;
////    connectToDevice();
//    if(isConnect && firstRun)
//    {
//        initConnection(isConnect);
//        getDataSlot();
//        firstRun = false;
//    }

//    else if(! isConnect)
//    {
//        initConnection(isConnect);
//        firstRun = true;
//    }
//}


void Exciter::connectToDevice()
{
    //    isConnect = adrvObj->connect(ipAddress);
    //globals::ctx=adrvObj->ctx;
}



void Exciter::joshanFuncDataSlot()
{
    QJsonObject exciterData;

    exciterData["exciter_enable"] = isConnect? "on" : "off";
    exciterData["exciter_frequency"] = ui->lblCurrentFrq->text().toInt();
    exciterData["exciter_power"] = ui->lblCurrentPower->text().toInt();
    QStringList p =  ui->cmbBW->currentText().split(" ");
    exciterData["exciter_band_width"] = p[0];
    exciterData["exciter_start_frequency"] = ui->spnSweepStartFrq->value();
    exciterData["exciter_stop_frequency"] = ui->spnSweepStopFrq->value();
    exciterData["exciter_sweep_time"] = ui->spnSweepStep->value();
    exciterData["exciter_pulse_width"] = ui->spnImpulsePulseWidth->value();
    exciterData["exciter_pri"] = ui->spnImpulsePRI->value();

    switch (currentTabState) {
    case CW:
        exciterData["exciter_mode"] = "cw";
        break;

    case Spot:
        exciterData["exciter_mode"] = "spot";
        break;

    case Sweep:
        exciterData["exciter_mode"] = "sweep";
        break;

    case Impulse:
        exciterData["exciter_mode"] = "impulse";
        break;

    case WB:
        exciterData["exciter_mode"] = "wideband";
        break;

    default:
        exciterData["exciter_mode"] = "null";
    }

    //    exciterData["mode"] = val;
    exciterData["device"] = EXCITER_DEVICE_NAME;

    emit sendFuncDataToJoshanSignal(exciterData);
}

void Exciter::joshanStatusDataSlot()
{

}


void Exciter::initConnection(bool val)
{

    isConnect = val;
    ui->btnConnect->setStyleSheet(val?"background-color:"+QString(YELLOW_GREEN) :"background-color:"+QString(RED));
    ui->btnConnect->setText(val?"Connected":"Disconnect");
    ui->btnConnect->setVisible(false);
    //    emit connectionStatusSignal(val);
    //    ui->tabWidget->setEnabled(val);
}

void Exciter::addingCartElementsToExciter()
{
    //    QGroupBox *grbRX1 = ui->groupBox_2;
    //    QGridLayout *gridRX2 = grbRX1->findChild<QGridLayout *>();

    //    gridRX2->addWidget( , 0 , 1)             ;
}

void Exciter::changingDacMsg(QString dacMsg,QString mode)
{
    ui->lblDacMsg->setText(mode  + " " + dacMsg);
}

void Exciter::joshanControlDataSlot(QJsonObject val)
{
    QJsonObject value = val["parameters"].toObject();
    QString type = val["type"].toString();
    //    bool power = val["power"].toBool();

    if(type == "control_exciter_set_mode")
    {

        if(!value["mode"].isNull())
        {


            if(value["mode"].toString() == "cw_dis")
            {
                on_btnDisableCW_clicked(false);
            }

            else if(value["mode"].toString() == "spot_dis")
            {
                on_btnDisableSpot_clicked();
            }

            else if(value["mode"].toString() == "sweep_dis")
            {
                on_btnStopSweep_clicked();
            }

            else if(value["mode"].toString() == "impulse_dis")
            {
                on_btnDisImpulse_clicked();
            }

            else if(value["mode"].toString() == "wideband_dis")
            {
                on_btnDisableWB_clicked(false);
            }




            if(!value["power_on"].isNull())
            {
                //                ui->chbPower->stateChanged(val["power_on"].toInt());
            }


            if(value["mode"].toString() == "cw")
            {
                ui->tabWidget->setCurrentIndex(0);

                if(!value["frequency"].isNull())
                    ui->spnCWFrq->setValue(value["frequency"].toDouble());

                if(!value["power"].isNull())
                    ui->spnCWPower->setValue(value["power"].toDouble());

                ui->btnSetCW->clicked();
            }



            else if(value["mode"].toString() == "spot")
            {
                ui->tabWidget->setCurrentIndex(1);

                if(!value["frequency"].isNull())
                    ui->spnSpotFrq->setValue(value["frequency"].toDouble());

                if(!value["power"].isNull())
                    ui->spnSpotPower->setValue(value["power"].toDouble());

                if(!value["band_width"].isNull())
                {
                    int num = value["band_width"].toInt();

                    if(     num == 10  ||
                            num == 30  ||
                            num == 50  ||
                            num == 70  ||
                            num == 100 ||
                            num == 200 ||
                            num == 300 ||
                            num == 400 ||
                            num == 500 ||
                            num == 600 ||
                            num == 700 ||
                            num == 800 ||
                            num == 900 ||
                            num == 1000 )
                    {
                        ui->cmbBW->setCurrentText(QString::number(num)+" MHz");
                    }
                }

                ui->btnLoadSpot->clicked();
            }




            else if(value["mode"].toString() == "sweep")
            {
                ui->tabWidget->setCurrentIndex(2);

                if(!value["start_frequency"].isNull())
                    ui->spnSweepStartFrq->setValue(value["start_frequency"].toDouble());

                if(!value["stop_frequency"].isNull())
                    ui->spnSweepStopFrq->setValue(value["stop_frequency"].toDouble());

                if(!value["step_frequency"].isNull())
                    ui->spnSweepStep->setValue(value["step_frequency"].toDouble());

                if(!value["power"].isNull())
                    ui->spnSweepPower->setValue(value["power"].toDouble());

                ui->btnStartSweep->clicked();
            }



            else if(value["mode"].toString() == "impulse")
            {
                ui->tabWidget->setCurrentIndex(3);

                if(!value["frequency"].isNull())
                    ui->spnImpulseFrq->setValue(value["frequency"].toDouble());

                if(!value["power"].isNull())
                    ui->spnImpulsePower->setValue(value["power"].toDouble());

                if(!value["pri"].isNull())
                    ui->spnImpulsePRI->setValue(value["pri"].toDouble());

                if(!value["pulse_width"].isNull())
                    ui->spnImpulsePulseWidth->setValue(value["pulse_width"].toDouble());

                ui->btnLoadImpulse->clicked();
            }




            else if(value["mode"].toString() == "wideband")
            {
                ui->tabWidget->setCurrentIndex(4);

                if(!value["power"].isNull())
                    ui->spnWBPower->setValue(value["power"].toDouble());
                ui->btnSetWB->clicked();
            }
        }
    }

    else
    {


    }
}

void Exciter::smartNoiseIsActiveSlot()
{
    ui->lblDacMsg->setText("Smart noise is active.");
}

void Exciter::isUserLoggedInSlot(double state)
{
    isUserLoggedIn = state;
    for(int i{}; i < 5; i++)
    {
//         setBtn[i]->setEnabled(true ? state==true: false);
        ui->btnSetCW->setEnabled(state);
        ui->btnSetWB->setEnabled(state);
        ui->btnLoadSpot->setEnabled(state);
        ui->btnLoadImpulse->setEnabled(state);
        ui->btnStartSweep->setEnabled(state);
    }
}

//void Exciter::limitationValueSlot(double minFrq, double maxFrq)
//{
//    minFrqLimit = minFrq;
//    maxFrqLimit = maxFrq;
//}

// The exciter reads and writes its waveform txt files ONLY in the
// project's files/ folder (PROJECT_FILES_DIR defined by eLynxSDR.pro =
// <project>/files, e.g. /home/joshua/Documents/NIMA_USB/
// iiS-arena-01a0e308-iis/files).  No other folder is searched - a
// waveform can never come from another directory or another checkout.
static QString projectFilesDir()
{
#ifdef PROJECT_FILES_DIR
    return QStringLiteral(PROJECT_FILES_DIR);
#else
    return QCoreApplication::applicationDirPath() + QStringLiteral("/files");
#endif
}

bool Exciter::returnfilePath(QString &fileName)
{
    fullPath = QDir(projectFilesDir()).filePath(fileName);

    if(!existsFile(fullPath.toStdString()))
    {
        QMessageBox msgBox;
        msgBox.setText(QString("File doesn't exist.\n%1").arg(fullPath));
        msgBox.exec();
        return false;
    }

    // Hand the resolved absolute path back to the caller so the DAC
    // loader gets the full path into the project files/ folder.
    fileName = fullPath;

    // The one folder the exciter plays waveform files from.
    qInfo() << "Exciter waveform file:" << fullPath;

    return  true;
}



void Exciter::getDataSlot()
{
    //set frq
    //    ui->spnCWFrq->setValue(adrvObj->GetFrequency());
    //    ui->spnSpotFrq->setValue(adrvObj->GetFrequency());
    //    ui->spnImpulseFrq->setValue(adrvObj->GetFrequency());
    //    ui->lblCurrentFrq->setText("Current Frq: " + QString::number(adrvObj->GetFrequency()));

    //    //set power
    //    ui->spnCWPower->setValue(adrvObj->GetPower());
    //    ui->spnWBPower->setValue(adrvObj->GetPower());
    //    ui->spnSpotPower->setValue(adrvObj->GetPower());
    //    ui->spnSweepPower->setValue(adrvObj->GetPower());
    //    ui->spnImpulsePower->setValue(adrvObj->GetPower());
    //    ui->lblCurrentPower->setText("Current Power: " + QString::number(adrvObj->GetPower()));
}


bool Exciter::existsFile (const std::string& name)
{
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}

QString Exciter::resolveFileInAppFolders(const QString &relPath)
{
    // ONLY the project's files/ folder is searched (see
    // projectFilesDir() above) - nothing else, so a waveform file can
    // never be picked up from another folder or another checkout.
    const QString full = QDir(projectFilesDir()).absoluteFilePath(relPath);
    if (existsFile(full.toStdString()))
        return full;
    return QString();
}

bool Exciter::voltage01Selected() const
{
    return ui->chbVoltage01 && ui->chbVoltage01->isChecked();
}

bool Exciter::voltage23Selected() const
{
    return ui->chbVoltage23 && ui->chbVoltage23->isChecked();
}

void Exciter::setProfileBw(int bwMHz)
{
    // Phase 5: active ADRV9009 profile from the receiver Profile tab.
    // The spot BW box is free-typed 1 .. profileBw (100/200/400 MHz),
    // so keep the placeholder in sync with the active profile.
    if (bwMHz > 0)
    {
        profileBw = bwMHz;
        if (ui->cmbBW->lineEdit())
            ui->cmbBW->lineEdit()->setPlaceholderText(
                QStringLiteral("1 - %1 MHz").arg(profileBw));
    }
}


void Exciter::setModeActive(const QString &mode, bool on)
{
    if (on)
        activeModes.insert(mode);
    else
        activeModes.remove(mode);

    emit modeActivitySignal(!activeModes.isEmpty());
}

void Exciter::setDataSlot()
{


    //    if(!ui->chbPower->isChecked())
    //    {
    //        QMessageBox msgBox;
    //        msgBox.setText("Power is off, please turn it on. ");
    //        msgBox.exec();
    //    }


    //    else
    //    {




    //    ui->lblDacMsg->setText("Loading...");

    switch (ui->tabWidget->currentIndex())
    {

    //==============================CW=========================================
    case 0:
    {
        if ( ui->spnCWFrq->value() >= minFrqLimit and  ui->spnCWFrq->value() < maxFrqLimit ) return;

        emit turnOffSmartNoiseSignal();
        currentTabState = CW;
        setModeActive("cw", true);
        // Phase 6: the CW tab drives the on-chip DDS (not the DAC buffer):
        // hand over this tab's DDS tone parameters, then switch TX1+TX2
        // into the DDS "One CW Tone" mode.
        if (cwDdsFrqSpn && cwDdsScaleSpn && cwDdsPhaseSpn)
            emit cwDdsParamsSignal(cwDdsFrqSpn->value(),
                                   cwDdsScaleSpn->value(),
                                   cwDdsPhaseSpn->value());
        changeDacSignal("set-cw");
        QTimer::singleShot(500,  [&]{emit sendFrqDataSignal(QString::number(ui->spnCWFrq->value()));});
        QTimer::singleShot(500,  [&]{emit sendPowerToCart(ui->spnCWPower->value());});
        isExciterOn = true;
        break;
    }


        //===========================Spot=======================================
    case 1:
    {
        if ( ui->spnSpotFrq->value() >= minFrqLimit and  ui->spnSpotFrq->value() < maxFrqLimit ) return;
        emit turnOffSmartNoiseSignal();
        currentTabState = Spot;
        setModeActive("spot", true);

        //            adrvObj->SetPower(ui->spnSpotPower->value());
        //            adrvObj->SetFrequency(ui->spnSpotFrq->value());
        //            getDataSlot();
        emit sendFrqDataSignal(QString::number(ui->spnSpotFrq->value()));

        // Phase 5: prefer the profile-specific band-limited noise file
        // spot{N}mhz_{P}.txt (P = active ADRV9009 profile bandwidth, set
        // from the receiver Profile tab via setProfileBw()). N is the
        // numeric MHz value typed/selected in cmbBW. Falls back to the
        // legacy spot{N}mhz.txt when the profile file is not present.
        // Only the file name selection changes here; the DAC load path
        // (returnfilePath / sendFileToCardSignal) below is untouched.
        double spotBwMhz = 0.0;
        // Integer or decimal MHz, optionally suffixed with "MHz" (the
        // remote control injects "N MHz"); decimals round to the nearest
        // generated integer file.  Anything non-numeric keeps the
        // legacy plain file-name behaviour below.
        const QRegularExpression spotBwRx(
            "^\\s*([0-9]+(?:\\.[0-9]+)?)\\s*(?:MHz)?\\s*$",
            QRegularExpression::CaseInsensitiveOption);
        const QRegularExpressionMatch spotBwM = spotBwRx.match(ui->cmbBW->currentText());
        if (spotBwM.hasMatch())
            spotBwMhz = qRound(spotBwM.captured(1).toDouble());

        if (spotBwMhz > 0.0)
        {
            const QString profileFile =
                QString("spot/spot%1mhz_%2.txt")
                    .arg(spotBwMhz, 0, 'f', 0).arg(profileBw);
            const QString legacyFile =
                QString("spot/spot%1mhz.txt")
                    .arg(spotBwMhz, 0, 'f', 0);
            if (!resolveFileInAppFolders(profileFile).isEmpty())
                fileName = profileFile;
            else if (!resolveFileInAppFolders(legacyFile).isEmpty())
                fileName = legacyFile;
            else
                fileName = profileFile;
        }
        else
        {
            fileName = ("spot/spot" + ui->cmbBW->currentText().toLower().replace(" ", "")+ ".txt");
        }
        //            QString fileName = ("spot/iio/msk_20M.txt");

        const QString spotFull = resolveFileInAppFolders(fileName);
        if (spotFull.isEmpty())
        {
            QMessageBox msgBox;
            msgBox.setText(tr("Spot noise file not found:\n%1\n\n"
                              "The file name is spot<N>mhz_<P>.txt: <N> is "
                              "the spot bandwidth in MHz (the box on this "
                              "tab), <P> is the ACTIVE profile bandwidth "
                              "(now %2). <P> changes only when Set is "
                              "pressed in the Profile tab.\n\n"
                              "Generate the file with files/spot/generate.py "
                              "(run in the files/spot folder), e.g.\n"
                              "  python3 generate.py --profiles %2 --bw <N>")
                               .arg(QDir(projectFilesDir()).filePath(fileName))
                               .arg(profileBw));
            msgBox.exec();
            return;
        }
        fileName = spotFull;
        emit sendFileToCardSignal(fileName, 0,"spot");

        //            try
        //            {
        //            QString mess = adrvObj->SetFile(fullPath,0);
        //                ui->lblStatus->setText("Status: "+mess);

        //                if(mess.contains("successfully")){
        //                    ui->btnLoadSpot->setStyleSheet("background-color:#186a3b");
        //                    ui->btnDisableSpot->setStyleSheet("background-color:red");
        //                }
        //                else {
        //                    ui->btnLoadSpot->setStyleSheet("background-color:red");
        //                    ui->btnDisableSpot->setStyleSheet("background-color:red");
        //                }
        //            }
        //            catch (...)
        //            {
        //                QMessageBox msgBox;
        //                msgBox.setText("Something went wrong! Try again");
        //                msgBox.exec();
        //                return;
        //            }
        isExciterOn = true;
        break;
    }

        //===========================Sweep=====================================
    case 2:
    {
        // Phase 6: the Sweep tab synthesizes a stepped-sine I/Q file like
        // the LFM tab (baseband tones at start, start+step, ... stop - e.g.
        // 0/2/4/6/8/10 MHz for start 0, stop 10, step 2).  The LO does not
        // change: the whole sweep lives inside the DAC file, no cart
        // hopping.
        emit turnOffSmartNoiseSignal();
        currentTabState = Sweep;
        setModeActive("sweep", true);
        if (!buildSweepFile("Sweep.txt", ui->spnSweepStartFrq->value(),
                            ui->spnSweepStopFrq->value(),
                            ui->spnSweepStep->value()))
            return;
        fileName = "Sweep.txt";
        if (!returnfilePath(fileName)) return;
        emit sendFileToCardSignal(fileName, 0, "sweep");
        isExciterOn = true;
        break;
    }


        //==============================Impulse===================================
    case 3:
    {
        if ( ui->spnImpulseFrq->value() >= minFrqLimit and  ui->spnImpulseFrq->value() < maxFrqLimit ) return;
        emit turnOffSmartNoiseSignal();
        //            changeDacSignal("impulse");
        currentTabState = Impulse;
        setModeActive("impulse", true);
        //            adrvObj->SetPower(ui->spnImpulsePower->value());
        //            adrvObj->SetFrequency(ui->spnImpulseFrq->value());
        //            getDataSlot();

        emit sendFrqDataSignal(QString::number(ui->spnImpulseFrq->value()));
        // PRI and pulse width spinboxes are in microseconds ("us" labels,
        // see exciter.ui).  createImpulseFile converts us -> samples at the
        // profile's I/Q playback rate (0.5 x profile BW, see
        // files/spot/generate.py), so the pulse on air is exactly the typed
        // width at exactly the typed PRI.  (The old formula
        // value*2000/4.1 multiplied the typed us by 487.8, so pulses came
        // out 2.4-9.8x too wide on air.)
        double pri = ui->spnImpulsePRI->value();
        double pw = ui->spnImpulsePulseWidth->value();

        QString impulseFileName = "Impulse.txt";
        // Same per-profile I/Q playback rate as the spot files
        // (files/spot/generate.py): 61.44/122.88/491.52 MS/s for the
        // 100/200/400 profiles (measured on the analyser, see
        // txFileRateMhz()).
        const double impulseFs = txFileRateMhz(profileBw);
        createImpulseFile(pri, pw, impulseFileName, impulseFs);

        if (!returnfilePath(impulseFileName)) return;
        fileName = impulseFileName;
        emit sendFileToCardSignal(fileName, 0,"impulse");
        isExciterOn = true;
        //            fileName = "pls.txt";
        //            emit sendFileToCardSignal(fileName, 0,"impulse");
        try
        {

            //                QString mess = adrvObj->SetFile(fullPath,0);
            //                ui->lblImpulseStatus->setText("Status: "+mess);

            //                if(mess.contains("successfully")){
            //                    ui->btnLoadImpulse->setStyleSheet("background-color:#186a3b");
            //                    ui->btnDisImpulse->setStyleSheet("background-color:red");
            //                }
            //                else {
            //                    ui->btnLoadImpulse->setStyleSheet("background-color:red");
            //                    ui->btnDisImpulse->setStyleSheet("background-color:red");
            //                }
        }
        catch (...)
        {
            QMessageBox msgBox;
            msgBox.setText("Something went wrong! Try again");
            msgBox.exec();
            return;
        }


        break;
    }


        //=============================WB==========================================
    case 4:
    {
        emit turnOffSmartNoiseSignal();
        //            changeDacSignal("we");
        currentTabState = WB;
        setModeActive("wb", true);
        //            adrvObj->SetPower(ui->spnWBPower->value());
        //            getDataSlot();
        QString fileName = ("spot/widebandnoise.txt");
        const QString wbFull = resolveFileInAppFolders(fileName);
        if (wbFull.isEmpty())
        {
            QMessageBox msgBox;
            msgBox.setText(tr("Wideband noise file not found:\n%1\n\n"
                              "This tab plays the fixed file "
                              "files/spot/widebandnoise.txt - the generator "
                              "does not create it. Copy the full-band spot "
                              "file of your profile over it, e.g. for the "
                              "current profile (%2):\n"
                              "  cp files/spot/spot%2mhz_%2.txt "
                              "files/spot/widebandnoise.txt")
                               .arg(QDir(projectFilesDir()).filePath(fileName))
                               .arg(profileBw));
            msgBox.exec();
            return;
        }
        fileName = wbFull;
        emit sendFileToCardSignal(fileName, 0,"wb");
        break;
    }

    //===========================Bridge Noise===============================
    case 5:
    {
        emit turnOffSmartNoiseSignal();
        setModeActive("bridge", true);
        const int bridgeN = int(bridgeSpn->value());
        fileName = QString("bridge/bridge%1mhz_%2.txt")
                       .arg(bridgeN).arg(profileBw);
        const QString bridgeFull = resolveFileInAppFolders(fileName);
        if (bridgeFull.isEmpty())
        {
            QMessageBox msgBox;
            msgBox.setText(tr("Bridge noise file not found:\n%1\n\n"
                              "The exciter looks only in the project "
                              "files/ folder. Run files/bridge/generate_bridge.py "
                              "with '--out bridge' in the files/ folder of the "
                              "main project folder (next to the spot/ "
                              "folder), then press Set again.")
                               .arg(QDir(projectFilesDir()).filePath(fileName)));
            msgBox.exec();
            return;
        }
        emit sendFileToCardSignal(bridgeFull, 0, "bridge");
        isExciterOn = true;
        break;
    }

    //===========================Multi Target===============================
    case 6:
    {
        bool any = false;
        for (int t = 0; t < 5; t++)
            if (mtEnable[t]->isChecked())
                any = true;
        if (!any)
        {
            QMessageBox msgBox;
            msgBox.setText(tr("Select at least one target."));
            msgBox.exec();
            return;
        }
        emit turnOffSmartNoiseSignal();
        setModeActive("multitarget", true);
        if (!buildMultiTargetWaveform())
            return;
        fileName = "MultiTarget.txt";
        if (!returnfilePath(fileName)) return;
        emit sendFileToCardSignal(fileName, 0, "multitarget");
        isExciterOn = true;
        break;
    }

    //===========================LFM=====================================
    case 7:
    {
        emit turnOffSmartNoiseSignal();
        setModeActive("lfm", true);
        if (!buildChirpFile("Lfm.txt", false,
                            lfmStartSpn->value(), lfmBwSpn->value(),
                            lfmTSpn->value()))
            return;
        fileName = "Lfm.txt";
        if (!returnfilePath(fileName)) return;
        emit sendFileToCardSignal(fileName, 0, "lfm");
        isExciterOn = true;
        break;
    }

    //===========================NLFM====================================
    case 8:
    {
        emit turnOffSmartNoiseSignal();
        setModeActive("nlfm", true);
        if (!buildChirpFile("Nlfm.txt", true,
                            nlfmStartSpn->value(), nlfmBwSpn->value(),
                            nlfmTSpn->value()))
            return;
        fileName = "Nlfm.txt";
        if (!returnfilePath(fileName)) return;
        emit sendFileToCardSignal(fileName, 0, "nlfm");
        isExciterOn = true;
        break;
    }

    default:
        break;
    }

    //    ui->lblDacMsg->setText("...");
    //    }



}


void Exciter::createImpulseFile(double pri, double pw, QString &impulseFileName, double fs_msps)
{
    uint iValue{}, qValue{};

    const QString impPath = QDir(projectFilesDir()).filePath(impulseFileName);
    QFile file(impPath);

    if(file.exists())
    {
        QFile::remove(impPath);
    }

    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out<< "TEXT\n";

        // The board plays file samples at fs_msps = 122.88 x P/100
        // (the profile's DAC sample rate, see files/spot/generate.py).  PRI and
        // pulse width arrive in microseconds, so:
        const uint priSamp = uint(pri * fs_msps);
        uint pwSamp  = uint(pw  * fs_msps);
        // A pulse wider than its own PRI would be a continuous tone; keep
        // at least one sample off so the waveform stays a pulse train.
        if (priSamp > 1 && pwSamp >= priSamp)
            pwSamp = priSamp - 1;
        for(uint i{}; i < priSamp; i++)
        {
            if(i < pwSamp)
            {
                iValue = 1;
                qValue = 1;
                out<<iValue <<" "<<qValue <<"\n";
            }

            else
            {
                iValue = 0;
                qValue = 0;
                out<<iValue <<" "<<qValue<<"\n";
            }
        }

        file.close();
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Somthing went wrong, Try again.");
        msgBox.exec();
        return;
    }
}


void Exciter::on_btnStopSweep_clicked()
{
    if (isExciterOn) emit changeDacSignal("Sweep");
    setModeActive("sweep", false);
}


void Exciter::on_btnDisableSpot_clicked()
{

    //    adrvObj->DisableDac();
    if (isExciterOn) emit changeDacSignal("Spot");
    setModeActive("spot", false);
    //    ui->lblSpotStatus->setText("Status: disable");
    //    ui->btnDisableSpot->setStyleSheet("background-color:#186a3b");
    //    ui->btnLoadSpot->setStyleSheet("background-color:#d30000");
    //    ui->btnLoadSpot->setStyleSheet("background-color:");
}

void Exciter::on_btnDisImpulse_clicked()
{    if (isExciterOn) emit changeDacSignal("Impulse");
    setModeActive("impulse", false);
    //     ui->lblImpulseStatus->setText("Status: disable");
    //     ui->btnDisImpulse->setStyleSheet("background-color:#186a3b");
    //     ui->btnLoadImpulse->setStyleSheet("background-color:red");
}


void Exciter::on_btnDisableCW_clicked(bool checked)
{
    if (isExciterOn) emit changeDacSignal("CW");
    setModeActive("cw", false);
}

void Exciter::on_btnDisableWB_clicked(bool checked)
{
    if (isExciterOn) emit changeDacSignal("WB");
    setModeActive("wb", false);
}

// ---------------------------------------------------------------------------
// Phase 6: Multi Target tab
// ---------------------------------------------------------------------------

void Exciter::refreshTxPowerDisplay()
{
    // Phase 6: the "Current P (attenuation)" readout shows Pb = Pa + Pc -
    // the calibrated power sent to the board and used everywhere in the
    // software (constants/tx_calibration.h, default Pc = 0 dB).
    ui->lblCurrentPower->setNum(TxCalibration::boardP(ui->spnCWPower->value()));
}

void Exciter::updateMultiTargetRow(int row, bool applyDefaults)
{
    // Per-type specification fields: label / min / max / decimals / default.
    struct SpecDef {
        const char *lbl[3];
        double min[3];
        double max[3];
        int dec[3];
        double dflt[3];
    };
    static const SpecDef defs[6] = {
        // Spot: bandwidth of the pre-generated spot{N}mhz_{P}.txt file
        // (default 10 MHz).
        {{"BW (MHz)", "--", "--"}, {1.0, -1.0, -1.0}, {400.0, 1.0, 1.0}, {0, 0, 0}, {10.0, 0.0, 0.0}},
        // CW: baseband tone frequency (MHz)
        {{"Frq (MHz)", "--", "--"}, {-120.0, -1.0, -1.0}, {120.0, 1.0, 1.0}, {2, 0, 0}, {0.0, 0.0, 0.0}},
        // Impulse: PRI and pulse width (us)
        {{"PRI (us)", "PW (us)", "--"}, {1.0, 0.05, -1.0}, {50000.0, 10000.0, 1.0}, {1, 2, 0}, {1.0, 0.05, 0.0}},
        // LFM: start frequency (MHz), bandwidth (MHz), pulse duration (us)
        // - exactly the LFM/NLFM tab parameters (no shift on these rows)
        {{"Start (MHz)", "BW (MHz)", "T (us)"}, {-120.0, 0.1, 0.1}, {120.0, 200.0, 10000.0}, {2, 2, 2}, {0.0, 10.0, 10.0}},
        // NLFM: same fields as LFM (raised-cosine amplitude coding)
        {{"Start (MHz)", "BW (MHz)", "T (us)"}, {-120.0, 0.1, 0.1}, {120.0, 200.0, 10000.0}, {2, 2, 2}, {0.0, 10.0, 10.0}},
        // Bridge: bandwidth of the generated bridge{N}mhz_{P}.txt noise
        // (default 10 MHz, like the Bridge Noise tab)
        {{"BW (MHz)", "--", "--"}, {1.0, -1.0, -1.0}, {400.0, 1.0, 1.0}, {0, 0, 0}, {10.0, 0.0, 0.0}},
    };
    if (row < 0 || row > 4 || !mtType[row])
        return;
    const int typeIdx = mtType[row]->currentIndex();
    const SpecDef &d = defs[typeIdx];
    for (int s2 = 0; s2 < 3; s2++)
    {
        const bool used = (typeIdx == 0) ? (s2 == 0)
                     : (typeIdx == 1) ? (s2 == 0)
                     : (typeIdx == 2) ? (s2 < 2)
                     : (typeIdx == 5) ? (s2 == 0)
                     : true;
        mtSpecLbl[row][s2]->setText(used ? d.lbl[s2] : QStringLiteral("--"));
        mtSpec[row][s2]->setEnabled(used);
        mtSpec[row][s2]->setVisible(used);
        mtSpecLbl[row][s2]->setVisible(used);
        if (used)
        {
            mtSpec[row][s2]->setMinimum(d.min[s2]);
            mtSpec[row][s2]->setMaximum(d.max[s2]);
            mtSpec[row][s2]->setDecimals(d.dec[s2]);
            if (applyDefaults)
                mtSpec[row][s2]->setValue(d.dflt[s2]);
        }
    }
    // LFM / NLFM rows carry just start frequency / BW / T like the
    // LFM/NLFM tabs - NO frequency shift.  The other types keep their
    // shift field.
    const bool hasShift = (typeIdx != 3 && typeIdx != 4);
    mtShift[row]->setVisible(hasShift);
    mtShiftLbl[row]->setVisible(hasShift);
}

int Exciter::loadIqTextSamples(const QString &path,
                               QVector<double> &ti, QVector<double> &tq,
                               int nMax)
{
    QFile in(path);
    if (!in.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "MultiTarget: cannot open" << path;
        return 0;
    }
    QTextStream r(&in);
    QString header;
    r >> header; // "TEXT" header token
    int n2 = 0;
    // Whitespace-agnostic I/Q extraction (spaces, tabs, CR/LF all work).
    // The old line splitting used a C string literal whose "\s+" escape
    // was broken (the regex was really "s+"), so it never matched a data
    // line and every spot row read zero samples.
    while (n2 < nMax && !r.atEnd())
    {
        double a, b;
        r >> a >> b;
        if (r.status() != QTextStream::Ok)
            break;
        ti[n2] = a;
        tq[n2] = b;
        n2++;
    }
    in.close();
    return n2;
}

bool Exciter::buildMultiTargetWaveform()
{
    const int N = 262144;
    // Multi-target playback rate = the profile's TX input / playback rate
    // (the spectrum width): 122.88 x P/100 -> 122.88 / 245.76 / 491.52
    // MS/s for the 100/200/400 profiles.  The +/- 61.44 MHz band belongs
    // to the 122.88/100 profile - at profile 200 the playback band is
    // +/- 122.88 MHz (a 70 MHz shift is valid there), at 400 +/- 245.76.
    const double fsMhz = 122.88 * profileBw / 100.0;
    const double twoPi = 2.0 * M_PI;

    QVector<double> sumI(N, 0.0);
    QVector<double> sumQ(N, 0.0);
    int written = 0;

    for (int t = 0; t < 5; t++)
    {
        if (!mtEnable[t]->isChecked())
            continue;
        const int type = mtType[t]->currentIndex();
        QVector<double> ti(N, 0.0);
        QVector<double> tq(N, 0.0);

        if (type == 0 || type == 5)
        {
            // Spot / Bridge: load the generated band-limited noise file
            // (262144 samples) - spot/spot{N}mhz_{P}.txt or
            // bridge/bridge{N}mhz_{P}.txt.
            const int n = int(mtSpec[t][0]->value());
            const bool isBridge = (type == 5);
            const QString f = isBridge
                ? QString("bridge/bridge%1mhz_%2.txt").arg(n).arg(profileBw)
                : QString("spot/spot%1mhz_%2.txt").arg(n).arg(profileBw);
            const QString path = resolveFileInAppFolders(f);
            const QString what = isBridge ? tr("Bridge noise") : tr("Spot");
            const QString genHint = isBridge
                ? tr("Run files/bridge/generate_bridge.py first.")
                : tr("Run files/spot/generate.py first.");
            const QString regenHint = isBridge
                ? tr("Regenerate it with files/bridge/generate_bridge.py.")
                : tr("Regenerate it with files/spot/generate.py.");
            if (path.isEmpty())
            {
                QMessageBox msgBox;
                msgBox.setText(tr("%1 file for target %2 not found:\n%3\n\n%4")
                                   .arg(what).arg(t + 1)
                                   .arg(QDir(projectFilesDir()).filePath(f))
                                   .arg(genHint));
                msgBox.exec();
                return false;
            }
            const int n2 = loadIqTextSamples(path, ti, tq, N);
            if (n2 == 0)
            {
                // Never transmit a silent file: an empty/unparsable noise
                // file used to produce an all-zero waveform and the user
                // just saw "nothing" on the spectrum.
                QMessageBox msgBox;
                msgBox.setText(tr("%1 file for target %2 contains no samples:\n%3\n\n%4")
                                   .arg(what).arg(t + 1).arg(path).arg(regenHint));
                msgBox.exec();
                return false;
            }
        }
        else if (type == 1)
        {
            // CW: complex tone at baseband frequency f (MHz)
            const double f = mtSpec[t][0]->value();
            if (std::abs(f) > fsMhz / 2.0)
            {
                QMessageBox msgBox;
                msgBox.setText(tr("Target %1 tone %2 MHz exceeds the playback "
                                  "band (+/- %3 MHz on this profile).")
                                   .arg(t + 1).arg(f).arg(fsMhz / 2.0));
                msgBox.exec();
                return false;
            }
            // Whole cycles in the N-sample buffer (the buffer loops):
            // the exact nearest frequency - no step at the wrap, no spurs.
            const double fSnap = std::floor(f * N / fsMhz + 0.5) * fsMhz / double(N);
            const double w = twoPi * fSnap / fsMhz;
            for (int i = 0; i < N; i++)
            {
                const double ph = w * i;
                ti[i] = std::cos(ph);
                tq[i] = std::sin(ph);
            }
        }
        else if (type == 2)
        {
            // Impulse: rectangular PRF pulse train (1/0 on I and Q)
            const double priUs = mtSpec[t][0]->value();
            const double pwUs = mtSpec[t][1]->value();
            const int priSamp = qMax(2, int(priUs * fsMhz));
            int pwSamp = int(pwUs * fsMhz);
            if (pwSamp <= 0)
                pwSamp = 1;
            if (pwSamp >= priSamp)
                pwSamp = priSamp - 1;
            for (int i = 0; i < N; i++)
            {
                const double v = ((i % priSamp) < pwSamp) ? 1.0 : 0.0;
                ti[i] = v;
                tq[i] = v;
            }
        }
        else
        {
            // LFM / NLFM: the exact waveform of the LFM/NLFM tabs - just
            // start frequency / BW / T (no frequency shift on these rows).
            // phase(tau) = 2 pi (f0 tau + B tau^2 / (2 T)), period T.
            const double f0 = mtSpec[t][0]->value();
            const double B = mtSpec[t][1]->value();
            const double T = mtSpec[t][2]->value();
            // The sweep must fit inside the playback band or it folds.
            const double lo = qMin(f0, f0 + B);
            const double hi = qMax(f0, f0 + B);
            if (lo < -fsMhz / 2.0 || hi > fsMhz / 2.0)
            {
                QMessageBox msgBox;
                msgBox.setText(tr("Target %1 sweep %2 .. %3 MHz exceeds the "
                                  "playback band (+/- %4 MHz on this profile).")
                                   .arg(t + 1).arg(lo).arg(hi).arg(fsMhz / 2.0));
                msgBox.exec();
                return false;
            }
            fillChirpSamples(ti, tq, N, fsMhz, type == 4, f0, B, T);
        }

        // Frequency shift = complex exponential multiplier ("DDS sine"):
        //   s'(n) = s(n) * e^{j 2 pi f_shift n / fs}
        // LFM/NLFM rows carry no shift (start frequency / BW / T only, the
        // exact LFM/NLFM tab formula) - their placement comes from f0.
        const double fsh = (type == 3 || type == 4) ? 0.0 : mtShift[t]->value();
        if (std::abs(fsh) > fsMhz / 2.0)
        {
            QMessageBox msgBox;
            msgBox.setText(tr("Target %1 shift %2 MHz exceeds the playback "
                              "band (+/- %3 MHz on this profile).")
                               .arg(t + 1).arg(fsh).arg(fsMhz / 2.0));
            msgBox.exec();
            return false;
        }
        if (fsh != 0.0)
        {
            // The shift is a complex exponential multiplier ("DDS sine")
            // s'(n) = s(n) * e^{j 2 pi fsh n / fs} - used by the Spot, CW,
            // Impulse and Bridge rows.  The shift frequency snaps to whole
            // cycles in the looping buffer like the tone above: a
            // non-integer cycle count makes a step at the wrap and the
            // step puts the "some other" spurs around the shifted signal.
            const double fshSnap = std::floor(fsh * N / fsMhz + 0.5) * fsMhz / double(N);
            const double wsh = twoPi * fshSnap / fsMhz;
            for (int i = 0; i < N; i++)
            {
                const double ph = wsh * i;
                const double c = std::cos(ph);
                const double sn = std::sin(ph);
                const double a = ti[i];
                const double b = tq[i];
                ti[i] = a * c - b * sn;
                tq[i] = a * sn + b * c;
            }
        }

        // One txt file per selected object (inspectable individually)
        const QString tf = QDir(projectFilesDir()).filePath(
            QString("MultiTarget_t%1.txt").arg(t + 1));
        QFile out(tf);
        if (!out.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qWarning() << "MultiTarget: cannot write" << tf;
            return false;
        }
        QTextStream w(&out);
        w << "TEXT\n";
        for (int i = 0; i < N; i++)
            w << ti[i] << " " << tq[i] << "\n";
        out.close();
        written++;

        for (int i = 0; i < N; i++)
        {
            sumI[i] += ti[i];
            sumQ[i] += tq[i];
        }
    }

    if (written == 0)
        return false;

    // Sum of all shifted targets -> one I/Q stream, peak-normalized.
    double peak = 0.0;
    for (int i = 0; i < N; i++)
        peak = qMax(peak, qMax(qAbs(sumI[i]), qAbs(sumQ[i])));
    QFile out(QDir(projectFilesDir()).filePath(QStringLiteral("MultiTarget.txt")));
    if (!out.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qWarning() << "MultiTarget: cannot write MultiTarget.txt";
        return false;
    }
    QTextStream w(&out);
    w << "TEXT\n";
    const double g = (peak > 0.0) ? 1.0 / peak : 1.0;
    for (int i = 0; i < N; i++)
        w << (sumI[i] * g) << " " << (sumQ[i] * g) << "\n";
    out.close();
    return true;
}

// ---------------------------------------------------------------------------
// Phase 6: LFM / NLFM tabs
// ---------------------------------------------------------------------------

void Exciter::fillChirpSamples(QVector<double> &ti, QVector<double> &tq, int n,
                               double fsMhz, bool nlfm,
                               double f0Mhz, double bwMhz, double tUs)
{
    // THE chirp formula, shared by the LFM/NLFM tabs and the Multi Target
    // LFM/NLFM rows so both build the exact same waveform: a repeating
    // pulse of duration T with phase(tau) = 2*pi*(f0*tau + B*tau^2/(2*T)),
    // NLFM adds raised-cosine amplitude coding over the pulse.
    const double twoPi = 2.0 * M_PI;
    const int Tsamp = qMax(2, int(tUs * fsMhz));
    const double Tm = (double)Tsamp / fsMhz;
    for (int i = 0; i < n; i++)
    {
        const int m = i % Tsamp;
        const double tau = (double)m / fsMhz;
        const double ph = twoPi * (f0Mhz * tau + bwMhz * tau * tau / (2.0 * Tm));
        double amp = 1.0;
        if (nlfm)
        {
            // NLFM: raised-cosine amplitude coding over the pulse
            amp = 0.5 * (1.0 - std::cos(twoPi * m / Tsamp));
        }
        ti[i] = amp * std::cos(ph);
        tq[i] = amp * std::sin(ph);
    }
}

bool Exciter::buildChirpFile(const QString &fileName, bool nlfm,
                             double f0Mhz, double bwMhz, double tUs)
{
    const int N = 262144;
    // Board playback rate, same calibration as the spot/impulse files:
    // 61.44/122.88/491.52 MS/s for the 100/200/400 profiles (measured on
    // the analyser, see txFileRateMhz()).
    const double fsMhz = txFileRateMhz(profileBw);
    // The whole sweep must fit inside the file's Nyquist band
    // (-fs/2 .. +fs/2) or it folds and does not look like an LFM.
    {
        const double lo = qMin(f0Mhz, f0Mhz + bwMhz);
        const double hi = qMax(f0Mhz, f0Mhz + bwMhz);
        if (lo < -fsMhz / 2.0 || hi > fsMhz / 2.0)
        {
            QMessageBox msgBox;
            msgBox.setText(tr("LFM sweep %1 .. %2 MHz exceeds the playback "
                              "band (+/- %3 MHz on this profile). "
                              "Reduce the start frequency or bandwidth.")
                               .arg(lo).arg(hi).arg(fsMhz / 2.0));
            msgBox.exec();
            return false;
        }
    }
    QVector<double> ti(N, 0.0);
    QVector<double> tq(N, 0.0);
    fillChirpSamples(ti, tq, N, fsMhz, nlfm, f0Mhz, bwMhz, tUs);

    QFile out(QDir(projectFilesDir()).filePath(fileName));
    if (!out.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qWarning() << "Chirp: cannot write" << fileName;
        return false;
    }
    QTextStream w(&out);
    w << "TEXT\n";
    for (int i = 0; i < N; i++)
        w << ti[i] << " " << tq[i] << "\n";
    out.close();
    return true;
}

// ---------------------------------------------------------------------------
// Phase 6: Sweep tab - stepped-sine I/Q synthesis
// ---------------------------------------------------------------------------

bool Exciter::buildSweepFile(const QString &fileName,
                             double fStartMhz, double fStopMhz, double fStepMhz)
{
    // The Sweep tab builds its I/Q in-app with a sine function like the LFM
    // tab (baseband, fixed LO - no cart hopping): the tone steps through
    // f = start, start+step, ... toward stop (start 0, stop 10, step 2
    // gives exactly the tones 0, 2, 4, 6, 8, 10 MHz).  The time spent on
    // each tone is not critical - the tones share the looping 262144-sample
    // file equally - and the phase runs continuously across tone changes.
    const int N = 262144;
    // Board playback rate, same calibration as the spot/impulse files:
    // 61.44/122.88/491.52 MS/s for the 100/200/400 profiles (measured on
    // the analyser, see txFileRateMhz()).
    const double fsMhz = txFileRateMhz(profileBw);
    const double twoPi = 2.0 * M_PI;

    if (fStepMhz <= 0.0)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("The sweep step must be positive."));
        msgBox.exec();
        return false;
    }
    // Every tone must sit inside the playback band (-fs/2 .. +fs/2) or it
    // folds; the tones all lie between start and stop.
    const double lo = qMin(fStartMhz, fStopMhz);
    const double hi = qMax(fStartMhz, fStopMhz);
    if (lo < -fsMhz / 2.0 || hi > fsMhz / 2.0)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Sweep %1 .. %2 MHz exceeds the playback band "
                          "(+/- %3 MHz on this profile). Reduce the start or "
                          "stop frequency.")
                           .arg(lo).arg(hi).arg(fsMhz / 2.0));
        msgBox.exec();
        return false;
    }
    const double dir = (fStopMhz >= fStartMhz) ? 1.0 : -1.0;
    const int nTones = int(std::floor(std::abs(fStopMhz - fStartMhz) /
                                      fStepMhz + 1e-9)) + 1;
    if (nTones > N)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("The sweep step %1 MHz is too small: %2 tones do "
                          "not fit in the waveform file.")
                           .arg(fStepMhz).arg(nTones));
        msgBox.exec();
        return false;
    }

    QVector<double> ti(N, 0.0);
    QVector<double> tq(N, 0.0);
    double ph = 0.0;
    int i = 0;
    for (int k = 0; k < nTones; k++)
    {
        const double f = fStartMhz + dir * fStepMhz * k;
        const int iEnd = (k + 1) * N / nTones;
        // Whole cycles in the tone slice (the buffer loops - the phase
        // must end where the slice started or the wrap makes spurs).
        const int nSlice = iEnd - i;
        const double fSnap = (nSlice > 0)
            ? std::floor(f * nSlice / fsMhz + 0.5) * fsMhz / double(nSlice)
            : f;
        const double w = twoPi * fSnap / fsMhz;
        for (; i < iEnd; i++)
        {
            ti[i] = std::cos(ph);
            tq[i] = std::sin(ph);
            ph += w;
            if (ph >= twoPi)
                ph -= twoPi;
            else if (ph < 0.0)
                ph += twoPi;
        }
    }

    QFile out(QDir(projectFilesDir()).filePath(fileName));
    if (!out.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qWarning() << "Sweep: cannot write" << fileName;
        return false;
    }
    QTextStream w2(&out);
    w2 << "TEXT\n";
    for (int j = 0; j < N; j++)
        w2 << ti[j] << " " << tq[j] << "\n";
    out.close();
    return true;
}
