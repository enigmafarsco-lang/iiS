#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
    connections();
    this->setWindowTitle("Transmitter");


    this->setFixedWidth(1270);
    this->setFixedHeight(980);

    if(receiverIsConnected) emit exciterConnectionStatus(receiverWindow->isConnect);
    //    isAdrvConnect = receiverWindow->isConnect;

    ui->groupBox->setObjectName("1");
    ui->groupBox->setStyleSheet("QGroupBox#1 {border: 2px solid "+QString(GE)+"}");

    ui->groupBox_2->setObjectName("2");
    ui->groupBox_2->setStyleSheet("QGroupBox#2 {border: 2px solid "+QString(GE)+"}");

    ui->groupBox_3->setObjectName("3");
    ui->groupBox_3->setStyleSheet("QGroupBox#3 {border: 2px solid "+QString(GE)+"}");

    ui->groupBox_4->setObjectName("4");
    ui->groupBox_4->setStyleSheet("QGroupBox#4 {border: 2px solid "+QString(GE)+"}");

    ui->groupBox_5->setObjectName("5");
    ui->groupBox_5->setStyleSheet("QGroupBox#5 {border: 2px solid "+QString(GE)+"}");


}


class delay:public QThread
{
public:
    static void msleep(unsigned long msc)
    {
        QThread::msleep(msc);
    }
};

void MainWindow::init()
{

    //creating and showing /*receiver widget
    receiverWindow = new ReceiverMain;
    receiverWindow->show();
    //    receiverWindow->show();



    //    ui->tabWidget->setStyleSheet("QTabBar::tab:selected {background-color:   #186a3b  }");
    //==========================================================================

    solidDialog = new SolidDialog;
    mAppConfig = new AppConfiguration;

    mCalib = new BTcpLan(mAppConfig,COLIBR_DEVICE_NAME);
    mCalib->setTimerCheckingInterval(15000);

    mRffLan = new BTcpLan( mAppConfig,RFF_DEVICE_NAME);
    mRffLan->setTimerCheckingInterval(15000);

    mTunerLan = new BTcpLan( mAppConfig,TUNER_DEVICE_NAME);
    mTunerLan->setTimerCheckingInterval(-1);

    mServoLan = new BTcpLan( mAppConfig,SERVO_DEVICE_NAME);
    mServoLan->setTimerCheckingInterval(15000);

    mSolidLan = new UdpLan( mAppConfig,SOLID_DEVICE_NAME);

    mCoolLan = new BTcpLan(mAppConfig, COOLING_DEVICE_NAME);
    mCoolLan->setTimerCheckingInterval(-1);

    mJoshan = new BTcpLan(mAppConfig, JOSHAN_DEVICE_NAME);
    mJoshan->setTimerCheckingInterval(-1);

    joshan = new Joshan();

    //#ifdef QT_DEPRECATED_WARNINGS

    //    if(mAppConfig->m_setting->includeRadar.compare("yes",Qt::CaseInsensitive) == 0)
    //    {
    //        mUDP_OuterNode = new UdpLan(mAppConfig,OUTER_NODE);
    //    }

    //    else
    //    {
    //        mTCP_OuterNode = new BTcpLan( mAppConfig,OUTER_NODE);
    //        mTCP_OuterNode->setTimerCheckingInterval(-1);
    //    }
    //#endif
}


void MainWindow::connections()
{
    //    connect(this,        &MainWindow::sendIpAddressToExciterSignal,ui->exiter, &Exciter::receiveIpAddressSlot);

    //    connect(receiverWindow, &ReceiverMain::isConnect,               ui->exiter,     &Exciter::receiveIpAddressSlot,Qt::QueuedConnection);
    //    QObject::connect(receiverWindow, &ReceiverMain::isConnect,               this,           &MainWindow::test,Qt::QueuedConnection);
    //    connect(ui->exiter,     &Exciter::sayHi,                        receiverWindow, &ReceiverMain::getSayHi);
    //    connect(receiverWindow, &ReceiverMain::isConnectSignal,         ui->exiter,     &Exciter::receiveIpAddressSlot);

    connect(mRffLan,        &BTcpLan::DeviceResponseSignal,          ui->Rff,        &RFF::on_DeviceResponseSlot);
    connect(ui->Rff,        &RFF::sendCommandToDeviceSignal,         mRffLan,        &BTcpLan::on_sendCommandToDeviceSlot);
    connect(ui->Rff,        &RFF::sendStartSignal,                   mRffLan,        &BTcpLan::on_sendCommandToDeviceSlot );

    connect(mTunerLan,      &BTcpLan::DeviceResponseSignal,          ui->tuner,      &Tuner::on_DeviceResponseSlot);
    connect(ui->tuner,      &Tuner::sendCommandToDeviceSignal,       mTunerLan,      &BTcpLan::on_sendCommandToDeviceSlot);

    connect(mServoLan,      &BTcpLan::DeviceResponseSignal,          ui->servo,      &Servo::on_DeviceResponseSlot);
    connect(ui->servo,      &Servo::sendCommandToDeviceSignal,       mServoLan,      &BTcpLan::on_sendCommandToDeviceSlot);

    connect(ui->solid,      &Solid::sendCommandToDeviceSignal,       mSolidLan,      &UdpLan::on_sendCommandToDeviceSlot);
    connect(ui->solid,      &Solid::sendStatusTempSignal,            ui->cooling,      &Cooling3::getSolidTempSlot);

    connect(solidDialog,    &SolidDialog::sendCommandToDeviceSignal, mSolidLan,      &UdpLan::on_sendCommandToDeviceSlot);
    connect(mSolidLan,      &UdpLan::DeviceResponseSignal,           solidDialog,    &SolidDialog::on_DeviceResponseSlot);

    connect(mCoolLan,       &BTcpLan::DeviceResponseSignal,          ui->cooling,    &Cooling3::newPacketReceivedSignal);
    connect(ui->cooling,    &Cooling3::sendCommandToDeviceSignal,    mCoolLan,       &BTcpLan::on_sendCommandToDeviceSlot );


    connect(this,           &MainWindow::exciterConnectionStatus,    ui->exiter,     &Exciter::initConnection);
    connect(mJoshan,        &BTcpLan::DeviceResponseSignal,          joshan,         &Joshan::on_DeviceResponseSlot );

    connect(joshan,         &Joshan::sendCommandToDeviceSignal,      mJoshan,        &BTcpLan::on_sendCommandToDeviceSlot);
    //    connect(mJoshan,   &BTcpLan::connectionStatusSignal,  joshan, &Joshan::joshanIsConnected);
    joshan->joshanIsConnected(true);

    connect(receiverWindow,   &ReceiverMain::sendCommandToDeviceSignal,  mCalib,         &BTcpLan::on_sendCommandToDeviceSlot);
    connect(mCalib,           &BTcpLan::DeviceResponseSignal,            receiverWindow,  &ReceiverMain::on_DeviceResponseSlot);

    //----------------------------- [status bar] --------------------------------
    connect(mRffLan,   &BTcpLan::connectionStatusSignal, ui->status, &StatusWidget::connectionStatusSlot);
    connect(mRffLan,   &BTcpLan::connectionStatusSignal, ui->Rff,    &RFF::isDeviceConnectedSlot);

    connect(mTunerLan, &BTcpLan::connectionStatusSignal, ui->status, &StatusWidget::connectionStatusSlot);
    connect(mTunerLan, &BTcpLan::connectionStatusSignal, ui->tuner, &Tuner::isDeviceConnectedSlot);

    connect(mCoolLan,  &BTcpLan::connectionStatusSignal, ui->status, &StatusWidget::connectionStatusSlot);
    connect(mCoolLan,  &BTcpLan::connectionStatusSignal, ui->cooling,&Cooling3::isDeviceConnectedSlot);

    connect(mServoLan, &BTcpLan::connectionStatusSignal, ui->status, &StatusWidget::connectionStatusSlot);
    connect(mServoLan, &BTcpLan::connectionStatusSignal, ui->servo,  &Servo::isDeviceConnectedSlot);

    connect(mSolidLan, &UdpLan::connectionStatusSignal,  ui->status,   &StatusWidget::connectionStatusSlot);
    connect(mSolidLan, &UdpLan::connectionStatusSignal,  solidDialog,  &SolidDialog::isDeviceConnectedSlot);

    connect(this,      &MainWindow::exciterConnectionStatus,  ui->status, &StatusWidget::exciterConnectionSlot);

    connect(mJoshan,   &BTcpLan::connectionStatusSignal,  ui->status, &StatusWidget::connectionStatusSlot);
    connect(mCalib,   &BTcpLan::connectionStatusSignal, ui->status, &StatusWidget::connectionStatusSlot);
    connect(ui->status, &StatusWidget::isJoshanStop, [=](bool val){joshan->isJoshanStopSetter(val);});


    //====================================----------------------=======================================
    //=================================== [ joshan connection ] =======================================
    //====================================----------------------=======================================

    //joshan functional report =============================================
    connect(joshan,      &Joshan::sendFuncDataToJoshanSignal,   ui->Rff,     &RFF::joshanFuncDataSlot);
    connect(ui->Rff,     &RFF::sendFuncDataToJoshanSignal,      joshan,      &Joshan::getFuncDataJoshanSlot);

    connect(joshan,      &Joshan::sendFuncDataToJoshanSignal,   ui->servo,   &Servo::joshanFuncDataSlot    );
    connect(ui->servo,   &Servo::sendFuncDataToJoshanSignal,    joshan,      &Joshan::getFuncDataJoshanSlot);

    connect(joshan,      &Joshan::sendFuncDataToJoshanSignal,   ui->solid,   &Solid::joshanFuncDataSlot    );
    connect(ui->solid,   &Solid::sendFuncDataToJoshanSignal,    joshan,      &Joshan::getFuncDataJoshanSlot);

    connect(joshan,      &Joshan::sendFuncDataToJoshanSignal,   ui->cooling, &Cooling3::joshanFuncDataSlot );
    connect(ui->cooling, &Cooling3::sendFuncDataToJoshanSignal, joshan,      &Joshan::getFuncDataJoshanSlot);

    connect(joshan,      &Joshan::sendFuncDataToJoshanSignal,   ui->exiter,  &Exciter::joshanFuncDataSlot  );
    connect(ui->exiter,  &Exciter::sendFuncDataToJoshanSignal,  joshan,      &Joshan::getFuncDataJoshanSlot);

    //joshan status report =============================================
    connect(joshan,      &Joshan::sendStatusDataToJoshanSignal,      ui->Rff,     &RFF::joshanStatusDataSlot);
    connect(ui->Rff,     &RFF::sendStatusDataToJoshanSignal,         joshan,      &Joshan::getStatusDataJoshanSlot);

    connect(joshan,      &Joshan::sendStatusDataToJoshanSignal,      ui->servo,   &Servo::joshanStatusDataSlot    );
    connect(ui->servo,   &Servo::sendStatusDataToJoshanSignal,       joshan,      &Joshan::getStatusDataJoshanSlot);

    connect(joshan,      &Joshan::sendStatusDataToJoshanSignal,      solidDialog, &SolidDialog::joshanStatusDataSlot);
    connect(solidDialog, &SolidDialog::sendStatusDataToJoshanSignal, joshan,      &Joshan::getStatusDataJoshanSlot);

    connect(joshan,      &Joshan::sendStatusDataToJoshanSignal,      ui->cooling, &Cooling3::joshanStatusDataSlot );
    connect(ui->cooling, &Cooling3::sendStatusDataToJoshanSignal,    joshan,      &Joshan::getStatusDataJoshanSlot);

    connect(joshan,      &Joshan::sendStatusDataToJoshanSignal,      ui->exiter,  &Exciter::joshanStatusDataSlot  );
    connect(ui->exiter,  &Exciter::sendStatusDataToJoshanSignal,     joshan,      &Joshan::getStatusDataJoshanSlot);


    //joshan control data =============================================
    connect(joshan, &Joshan::joshanDataToRffSignal          ,   ui->Rff,      &RFF::joshanControlDataSlot);
    connect(joshan, &Joshan::joshanDataToSolidSignal        ,   ui->solid,    &Solid::joshanControlDataSlot  );
    connect(joshan, &Joshan::joshanDataToServoSignal        ,   ui->servo,    &Servo::joshanControlDataSlot  );
    connect(joshan, &Joshan::joshanDataToCoolingSignal      ,   ui->cooling,  &Cooling3::joshanControlDataSlot  );
    connect(joshan, &Joshan::joshanDataToExciterSetSignal   ,   ui->exiter,   &Exciter::joshanControlDataSlot  );
    //====================================----------------------=======================================
    //====================================----------------------=======================================
    //====================================----------------------=======================================
    //====================================----------------------=======================================
    //====================================----------------------=======================================
    //====================================----------------------=======================================

    //--------------------------- [Internal connection] ---------------------------
    connect(ui->Rff,     &RFF::sendCommandToObjectSignal,        ui->servo,      &Servo::receiveCommandFromObjectSlot);
    connect(ui->solid,   &Solid::sendCommandToOtherObjectSignal, solidDialog,    &SolidDialog::receiveCommandFromOtherObjectSlot );
    connect(solidDialog, &SolidDialog::sendCommandToOtherObjectSignal,ui->solid, &Solid::receiveCommandFromOtherObjectSlot );


    //----------------------[ connecting excitet and cart ]---------------------------
    if(receiverWindow->receiverIsConnected)
    {



        //sending limitation value to exciter
//        connect(receiverWindow,&ReceiverMain::limitationValueSignal, ui->exiter,&Exciter::limitationValueSlot);
//        connect(this,&MainWindow::getLimitationValueSignal, ui->exiter,&Exciter::limitationValueSlot);
        ui->exiter->minFrqLimit = receiverWindow->minFrqLimit;
        ui->exiter->maxFrqLimit = receiverWindow->maxFrqLimit;
        connect(receiverWindow, &ReceiverMain::userLoggedInSignal, ui->exiter, &Exciter::isUserLoggedInSlot);



        //these connections are for connecting exciter panel in receiver to exciter panel in transmmiter
        connect(receiverWindow, &ReceiverMain::cwFrqSignal ,        [&](double val){ ui->exiter->spnCWFrq->setValue(val)        ;});
        connect(receiverWindow, &ReceiverMain::cwPowerSignal ,      [&](double val){ ui->exiter->spnCWPower->setValue(val)      ;});
        connect(receiverWindow, &ReceiverMain::spotFrqSignal ,      [&](double val){ ui->exiter->spnSpotFrq->setValue(val)      ;});
        connect(receiverWindow, &ReceiverMain::spotPowerSignal ,    [&](double val){ ui->exiter->spnSpotPower->setValue(val)    ;});
        connect(receiverWindow, &ReceiverMain::sweepStartFrqSignal ,[&](double val){ ui->exiter->spnSweepStart->setValue(val)   ;});
        connect(receiverWindow, &ReceiverMain::sweepStopFrqSignal , [&](double val){ ui->exiter->spnSweepStop->setValue(val)    ;});
        connect(receiverWindow, &ReceiverMain::sweepStepSignal ,    [&](double val){ ui->exiter->spnSweepStep->setValue(val)    ;});
        connect(receiverWindow, &ReceiverMain::sweepPowerSignal ,   [&](double val){ ui->exiter->spnSweepPower->setValue(val)   ;});
        connect(receiverWindow, &ReceiverMain::sweepDelaySignal ,   [&](double val){ ui->exiter->spnDelay->setValue(val)        ;});
        connect(receiverWindow, &ReceiverMain::impFrqSignal ,       [&](double val){ ui->exiter->spnimpulseFrq->setValue(val)   ;});
        connect(receiverWindow, &ReceiverMain::impPowerSignal ,     [&](double val){ ui->exiter->spnimpulsePower->setValue(val) ;});
        connect(receiverWindow, &ReceiverMain::impPwSignal ,        [&](double val){ ui->exiter->spnImpPw->setValue(val)        ;});
        connect(receiverWindow, &ReceiverMain::impPriSignal ,       [&](double val){ ui->exiter->spnImpPri->setValue(val)       ;});
        connect(receiverWindow, &ReceiverMain::wbPowerSignal ,      [&](double val){ ui->exiter->spnWidePower->setValue(val)    ;});

        connect(receiverWindow, &ReceiverMain::cwOnBtnSignal ,      [&]{ ui->exiter->btnSetCW->clicked() ;});
        connect(receiverWindow, &ReceiverMain::cwOffBtnSignal ,     [&]{ ui->exiter->btnDisableCW->clicked();});
        connect(receiverWindow, &ReceiverMain::spotOnBtnSignal ,    [&]{ ui->exiter->btnLoadSpot->clicked();});
        connect(receiverWindow, &ReceiverMain::spotOffBtnSignal ,   [&]{ ui->exiter->btnDisableSpot->clicked();});
        connect(receiverWindow, &ReceiverMain::sweepOnBtnSignal ,   [&]{ ui->exiter->btnStartSweep->clicked();});
        connect(receiverWindow, &ReceiverMain::sweepOffBtnSignal ,  [&]{ ui->exiter->btnStopSweep->clicked();});
        connect(receiverWindow, &ReceiverMain::impOnBtnSignal ,     [&]{ ui->exiter->btnLoadImpulse->clicked();});
        connect(receiverWindow, &ReceiverMain::impOffBtnSignal ,    [&]{ ui->exiter->btnDisImpulse->clicked();});
        connect(receiverWindow, &ReceiverMain::wbOnBtnSignal ,      [&]{ ui->exiter->btnSetWB->clicked();});
        connect(receiverWindow, &ReceiverMain::wbOffBtnSignal ,     [&]{ ui->exiter->btnDisableWB->clicked();});



        //----------- seeting according cart
        //setting exciter value with cart value during initialization
        //seeting power value according to power value of cart
        ui->exiter->spnCWPower->setValue(receiverWindow->att_TX1_Spn->value());
        ui->exiter->spnSweepPower->setValue(receiverWindow->att_TX1_Spn->value());
        ui->exiter->spnimpulsePower->setValue(receiverWindow->att_TX1_Spn->value());
        ui->exiter->spnWidePower->setValue(receiverWindow->att_TX1_Spn->value());
        ui->exiter->spnSpotPower->setValue(receiverWindow->att_TX1_Spn->value());
        ui->exiter->spnDelay->setValue(receiverWindow->frqDomainPlot->txt_freq_delay->value());


        //setting sweep
        ui->exiter->spnSweepStart->setValue(receiverWindow->frqDomainPlot->txt_start_freq->value());
        ui->exiter->spnSweepStop->setValue(receiverWindow->frqDomainPlot->txt_stop_freq->value());
        ui->exiter->spnSweepStep->setValue(receiverWindow->frqDomainPlot->txt_freq_step->value());

        //seeting frq value according to frq value of cart
        ui->exiter->spnCWFrq->setValue(abs(DC_6_UPTO_8_12 - receiverWindow->frqSpn->value()));
        ui->exiter->spnimpulseFrq->setValue(abs(DC_6_UPTO_8_12 - receiverWindow->frqSpn->value()));
        ui->exiter->spnWideFrq->setValue(abs(DC_6_UPTO_8_12 - receiverWindow->frqSpn->value()));
        ui->exiter->spnSpotFrq->setValue(abs(DC_6_UPTO_8_12 - receiverWindow->frqSpn->value()));
        ui->sattar->frqPage->setFrqCenterSlot(abs(DC_6_UPTO_8_12 - receiverWindow->frqSpn->value()));



        receiverWindow->spnCWFrq->setValue(abs(DC_6_UPTO_8_12 - receiverWindow->frqSpn->value()));
        receiverWindow->spnSpotFrq->setValue(abs(DC_6_UPTO_8_12 - receiverWindow->frqSpn->value()));
        receiverWindow->spnSweepStartFrq->setValue(receiverWindow->frqDomainPlot->txt_start_freq->value());
        receiverWindow->spnSweepStopFrq->setValue(receiverWindow->frqDomainPlot->txt_stop_freq->value());
        receiverWindow->spnImpulseFrq->setValue(abs(DC_6_UPTO_8_12 - receiverWindow->frqSpn->value()));

        receiverWindow->spnCWPower->setValue(receiverWindow->att_TX1_Spn->value());
        receiverWindow->spnSpotPower->setValue(receiverWindow->att_TX1_Spn->value());
        receiverWindow->spnSweepPower->setValue(receiverWindow->att_TX1_Spn->value());
        receiverWindow->spnImpulsePower->setValue(receiverWindow->att_TX1_Spn->value());
        receiverWindow->spnWBPower->setValue(receiverWindow->att_TX1_Spn->value());

        receiverWindow->spnSweepStep->setValue(receiverWindow->frqDomainPlot->txt_freq_step->value());
        receiverWindow->spnDelay->setValue(receiverWindow->frqDomainPlot->txt_freq_delay->value());
//        ui->exiter->spnImpPw->setValue(val);
//        ui->exiter->spnImpPri->setValue(val);


        //---------- changing cart value
        //when cart value is  changed, exciter values are changed too
        connect(receiverWindow->att_TX1_Spn, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double val){ ui->exiter->spnCWPower->setValue(receiverWindow->att_TX1_Spn->value());});
        connect(receiverWindow->att_TX1_Spn, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double val){ ui->exiter->spnSweepPower->setValue(receiverWindow->att_TX1_Spn->value());});
        connect(receiverWindow->att_TX1_Spn, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double val){ ui->exiter->spnimpulsePower->setValue(receiverWindow->att_TX1_Spn->value());});
        connect(receiverWindow->att_TX1_Spn, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double val){ ui->exiter->spnWidePower->setValue(receiverWindow->att_TX1_Spn->value());});
        connect(receiverWindow->att_TX1_Spn, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double val){ ui->exiter->spnSpotPower->setValue(receiverWindow->att_TX1_Spn->value());});

        connect(ui->exiter->spnCWPower ,      QOverload<double>::of(&QDoubleSpinBox::valueChanged),  [&](double val){ receiverWindow->att_TX1_Spn->setValue(ui->exiter->spnCWPower->value()); });
        connect(ui->exiter->spnSweepPower ,   QOverload<double>::of(&QDoubleSpinBox::valueChanged),  [&](double val){ receiverWindow->att_TX1_Spn->setValue(ui->exiter->spnSweepPower->value()); });
        connect(ui->exiter->spnimpulsePower , QOverload<double>::of(&QDoubleSpinBox::valueChanged),  [&](double val){ receiverWindow->att_TX1_Spn->setValue(ui->exiter->spnimpulsePower->value()); });
        connect(ui->exiter->spnWidePower ,    QOverload<double>::of(&QDoubleSpinBox::valueChanged),  [&](double val){ receiverWindow->att_TX1_Spn->setValue(ui->exiter->spnWidePower->value()); });
        connect(ui->exiter->spnSpotPower,     QOverload<double>::of(&QDoubleSpinBox::valueChanged),  [&](double val){ receiverWindow->att_TX1_Spn->setValue(ui->exiter->spnSpotPower->value()); });
        connect(ui->exiter->spnDelay,         QOverload<int>::of(&QSpinBox::valueChanged),           [&](int    val){ receiverWindow->frqDomainPlot->txt_freq_delay->setValue(val); });



        connect(receiverWindow->frqSpn, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double val){ ui->exiter->spnCWFrq->setValue(abs(DC_6_UPTO_8_12 - receiverWindow->frqSpn->value()));});
        connect(receiverWindow->frqSpn, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double val){ emit ui->sattar->sendFrqValueToFrqPageSig(abs(DC_6_UPTO_8_12 - val));});
        connect(receiverWindow->frqSpn, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double val){ ui->exiter->spnimpulseFrq->setValue(abs(DC_6_UPTO_8_12 - receiverWindow->frqSpn->value()));});
        connect(receiverWindow->frqSpn, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double val){ ui->exiter->spnWideFrq->setValue(abs(DC_6_UPTO_8_12 - receiverWindow->frqSpn->value()));});
        connect(receiverWindow->frqSpn, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double val){ ui->exiter->spnSpotFrq->setValue(abs(DC_6_UPTO_8_12 - receiverWindow->frqSpn->value()));});
        //    connect(receiverWindow, &ReceiverMain::seekingIsOn,[&]{ui->exiter->startBtn->setStyleSheet("background-color:#186a3b");});
        connect(receiverWindow, &ReceiverMain::seekingStatusSignal,[&](QString msg){emit dacMsgSignal(msg,"");});
        connect(receiverWindow, &ReceiverMain::seekingStatusSignal,[&](QString msg){emit dacMsgSignal(msg,"");});

        connect(receiverWindow->frqDomainPlot->txt_start_freq,QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double val){ ui->exiter->spnSweepStart->setValue(val);});
        connect(receiverWindow->frqDomainPlot->txt_stop_freq,QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double val){ ui->exiter->spnSweepStop->setValue(val);});
        connect(receiverWindow->frqDomainPlot->txt_freq_step, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double val){ ui->exiter->spnSweepStep->setValue(val);});


        //--------- sending to cart
        //these connections are for changing cart values with exciter panel
        connect(ui->exiter, &Exciter::sendFrqDataSignal,  receiverWindow,&ReceiverMain::frqValueChenged );
        connect(ui->exiter, &Exciter::sendPowerToCart,    [&](double val){receiverWindow->att_TX1_Spn->setValue(val);});
        connect(ui->exiter, &Exciter::sendStartFrqToCart, [&](double val){receiverWindow->frqDomainPlot->txt_start_freq->setValue(val);});
        connect(ui->exiter, &Exciter::sendStoptFrqToCart, [&](double val){receiverWindow->frqDomainPlot->txt_stop_freq->setValue(val);});
        connect(ui->exiter, &Exciter::sendStepFrqToCart,  [&](double val){receiverWindow->frqDomainPlot->txt_freq_step->setValue(val);});

        connect(ui->exiter, &Exciter::startHopp, [&](){receiverWindow->frqAndSeekStopCapturing(true);});
        connect(ui->exiter, &Exciter::stoptHopp, [&](){receiverWindow->frqAndSeekStopCapturing(false);});

        //    connect(ui->exiter->spnSweepStart,QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double val){ receiverWindow->frqDomainPlot->txt_start_freq->setValue(val);});
        //    connect(ui->exiter->spnSweepStop,QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double val){ receiverWindow->frqDomainPlot->txt_stop_freq->setValue(val);});
        //    connect(ui->exiter->spnSweepStep, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double val){ receiverWindow->frqDomainPlot->txt_freq_step->setValue(val);});

        connect(ui->exiter,&Exciter::sendFileToCardSignal,this,[&](QString path, double sc,QString mode)
        {
            receiverWindow->att_TX1_Spn->setValue(0);
            receiverWindow->power_TX1_DownChk->stateChanged(0);
            receiverWindow->power_TX1_DownChk->setChecked(false);
            QString msg = receiverWindow->oscMain->_adrv9009->setFile(path, sc);
            emit dacMsgSignal(msg,mode);
        });


        connect(ui->exiter,&Exciter::changeDacSignal,this,[&](QString mode)
        {
            QString msg = receiverWindow->oscMain->_adrv9009->changingDac(mode);
            emit dacMsgSignal(msg,"");
            receiverWindow->stopExciterSlot();

        });


        //smart noise and exciter
        connect(ui->exiter,&Exciter::turnOffSmartNoiseSignal,receiverWindow,&ReceiverMain::stopSmartNoiseSlot);

        // TX1 follows the exciter modes: all modes off -> TX1 off,
        // any mode on -> TX1 on
        connect(ui->exiter, &Exciter::modeActivitySignal, this, [&](bool anyOn)
        {
            if (receiverWindow && receiverWindow->power_TX1_DownChk)
                receiverWindow->power_TX1_DownChk->setChecked(!anyOn);
        });
        connect(receiverWindow,&ReceiverMain::smartNoiseIsActiveSignal,ui->exiter,&Exciter::smartNoiseIsActiveSlot);

        //for sending sattar file on processing cart
        connect(ui->sattar, &MainPage::loadSattarFileSignal, [&](QString path, double sc,QString mode)
        {
            receiverWindow->att_TX1_Spn->setValue(0);
            receiverWindow->power_TX1_DownChk->stateChanged(0);
            receiverWindow->power_TX1_DownChk->setChecked(false);
            QString msg = receiverWindow->oscMain->_adrv9009->setFile(path, sc);
            emit dacMsgSignal(msg,mode);
        });



        //        //sending time data to card
        //                connect(receiverWindow,&Exciter::exciterModeFileSignal,this,[&](QString path, double sc,QString mode)
        //                {
        //                    QString msg = receiverWindow->oscMain->_adrv9009->setFile(path, sc);
        //                    emit dacMsgSignal(msg,mode);
        //                });

        connect(this,&MainWindow::dacMsgSignal, ui->exiter, &Exciter::changingDacMsg);

        //    connect(ui->exiter->btnDisableSpot, &QPushButton::clicked, [&](bool val){receiverWindow->oscMain->_adrv9009->DisableDac();});


        //    txt_start_freq
        connect(ui->exiter->powerOn, &QCheckBox::stateChanged, [&](int newState){

            if(newState)
            {
                receiverWindow->power_TX1_DownChk->setChecked(false);
            }

            else
            {
                receiverWindow->power_TX1_DownChk->setChecked(true);
            }
        });


        //    connect(ui->exiter->spnWBPower,QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double val){ receiverWindow->frqDomainPlot->txt_start_freq->setValue(val);});
    }
}


void MainWindow::receiveDataFromSubSystemSlot(QString dataReceive)
{
    QStringList frame;
    frame = dataReceive.split(',');
    if (frame[0]==("SOLID_STOP_REPORT") && frame.length()==2)
    {
        if(frame[1] == "TRUE"){

            disconnect(mSolidLan, &UdpLan::DeviceResponseSignal, solidDialog, &SolidDialog::on_DeviceResponseSlot);
        }
        else
        {
            connect(mSolidLan, &UdpLan::DeviceResponseSignal, solidDialog, &SolidDialog::on_DeviceResponseSlot);
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mRffLan;
    delete mServoLan;
    delete mSolidLan;
    delete mAppConfig;
    delete mCoolLan;
    delete mJoshan;
    //    delete mSattar;
    //start cheraghi
    //    if(mAppConfig->m_setting->includeRadar.compare("yes",Qt::CaseInsensitive) == 0){delete mUDP_OuterNode;}
    //    else {delete mTCP_OuterNode;}
    //end cheraghi
}

