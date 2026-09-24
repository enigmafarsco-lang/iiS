#include "receivermain.h"
#include "ui_receivermain.h"
//178.22.122.100, 185.51.200.2
#include <receiver/config.h>
#include <receiver/backtrace.h>
#include <receiver/connectdialog.h>
#include "receiver/globals.h"
#include <QFileInfo>
#include <limits>
#include <QButtonGroup>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QRadioButton>
#include <QScrollBar>
#include <QVBoxLayout>


ReceiverMain::ReceiverMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReceiverMain)
{
    ui->setupUi(this);
    setupDrfmControlTab();

    vec.setX(0);
    vec.setY(0);
    a.addPoints(vec);


    cwTableTimer = new QTimer(this);
    cwTableTimer->setInterval(5000);



    calibInitial();


    calibPowerTimer = new QTimer(this);
    calibPowerTimer->setInterval(4000);
    connect(calibPowerTimer, &QTimer::timeout,[&]
    {
        //        sendPowerToCalibDevice();
    });



    //    frqTimeVec={0,0,0,0,0};
    //    frqValVec={0,0,0,0,0};
    //    powerValVec  ={0,0,0,0,0};

    qRegisterMetaType<QCustomSeries>("QCustomSeries");

    //    ui->tableWidget_2.set

    //for test

    //    connect(ui->tabWidget,&QTableWidget::currentChanged,[]{});

    mTimer.setInterval(1);
    connect(&mTimer, &QTimer::timeout,[&]()
    {
        //        qInfo() << "main thread: " << QThread::currentThreadId();
        //                if(currentTabState !=1 ) return ;

        int rttand = QRandomGenerator::global()->bounded(0,10);

        //        int rttand = 20 * log10f(abs( 1.0 / ((RAND_MAX) * qrand()) )) + 100;

        //        emit frqValueSignal      (rttand , cnt);
        //        emit PAPeakValueSignal   (rttand , cnt);
        //        emit PASumMaxValueSignal (rttand , cnt);
        //        emit priValueSignal      (rttand , cnt);
        //        emit pwOneValueSignal    (rttand , cnt);
        //        emit pwTwoValueSignal    (rttand , cnt);
        //        emit toaValueSignal      (rttand , cnt);
        //        emit ChannelValueSignal  (rttand , cnt);
        cnt++;

    });


    mTimer.start();


    t1 = new plotfunction;
    t2 = new plotfunction;
    t3 = new plotfunction;
    t4 = new plotfunction;
    t5 = new plotfunction;
    t6 = new plotfunction;
    t7 = new plotfunction;
    t8 = new plotfunction;

    connect(this, &ReceiverMain::frqValueSignal      , t1, &plotfunction::pltGetDataSlot);
    connect(this, &ReceiverMain::PAPeakValueSignal   , t2, &plotfunction::pltGetDataSlot);
    connect(this, &ReceiverMain::PASumMaxValueSignal , t3, &plotfunction::pltGetDataSlot);
    connect(this, &ReceiverMain::priValueSignal      , t4, &plotfunction::pltGetDataSlot);
    connect(this, &ReceiverMain::pwOneValueSignal    , t5, &plotfunction::pltGetDataSlot);
    connect(this, &ReceiverMain::pwTwoValueSignal    , t6, &plotfunction::pltGetDataSlot);
    connect(this, &ReceiverMain::toaValueSignal      , t7, &plotfunction::pltGetDataSlot);
    connect(this, &ReceiverMain::ChannelValueSignal  , t8, &plotfunction::pltGetDataSlot);





    //======== frq ==================================================================================
    {
        QGLchart * plt1 = new QGLchart();
        plt1->setChartType(ChartType::ScatterPlot);
        initGLPlot(plt1, ChartType::ScatterPlot);

        QVBoxLayout * ver1 = new QVBoxLayout;
        ver1->addWidget(plt1);

        QGroupBox * grp1 = new QGroupBox;
        grp1->setLayout(ver1);
        grp1->setTitle("frequency");

        ui->verFrq->addWidget(grp1);

        t1->hasHistogram = true;

        connect(t1, &plotfunction::pltUpdateSignal, plt1,QOverload<QCustomSeries>::of(&QGLchart::updateSeriesData));

        //---------------------------------- histo ----------------------------------
        ui->widgFRQ->xAxis->setRange(5,100);
        ui->widgFRQ->xAxis->setBasePen(QPen(Qt::white));
        ui->widgFRQ->xAxis->setTickLabelColor(QColor(Qt::white));


        ui->widgFRQ->yAxis->setBasePen(QPen(Qt::white));
        ui->widgFRQ->yAxis->setTickLabelColor(QColor(Qt::white));
        ui->widgFRQ->setBackground(QColor(Qt::black));

        QCPBars * bars = new QCPBars(ui->widgFRQ->yAxis, ui->widgFRQ->xAxis);
        bars->setPen(QPen(Qt::yellow));
        bars->setBrush(QColor(10, 140, 70));
        bars->setWidth(0.5);
        //        bars1->


        connect(t1, &plotfunction::histoBarPlotSignal,this,[=](QVector<double> yAxisVal, QVector<double> xAxisVal)
        {
            if(yAxisVal.isEmpty() || xAxisVal.isEmpty()) return ;
            bars->setData(yAxisVal, xAxisVal);
            //            ui->widgFRQ->rescaleAxes();
            ui->widgFRQ->replot();
        });
        //---------------------------------- end histo ----------------------------------
    }








    //======== PAPeak ===============================================================================
    {
        QGLchart * plt2 = new QGLchart();
        plt2->setChartType(ChartType::ScatterPlot);
        initGLPlot(plt2, ChartType::ScatterPlot);

        QVBoxLayout * ver2 = new QVBoxLayout;
        ver2->addWidget(plt2);

        QGroupBox * grp2 = new QGroupBox;
        grp2->setLayout(ver2);
        grp2->setTitle("PAPeak");

        t2->hasHistogram = false;

        ui->verPaPeak->addWidget(grp2);
        connect(t2, &plotfunction::pltUpdateSignal, plt2,QOverload<QCustomSeries>::of(&QGLchart::updateSeriesData));


    }

    //======== PASumMax =============================================================================
    {
        QGLchart * plt3 = new QGLchart();
        plt3->setChartType(ChartType::ScatterPlot);
        initGLPlot(plt3, ChartType::ScatterPlot);


        QVBoxLayout * ver3 = new QVBoxLayout;
        ver3->addWidget(plt3);

        QGroupBox * grp3 = new QGroupBox;
        grp3->setLayout(ver3);
        grp3->setTitle("PASumMax");

        t3->hasHistogram = true;


        ui->verPASum->addWidget(grp3);
        connect(t3, &plotfunction::pltUpdateSignal, plt3,QOverload<QCustomSeries>::of(&QGLchart::updateSeriesData));





        //---------------------------------- histo ----------------------------------
        ui->widgPSUM->xAxis->setRange(5,100);
        ui->widgPSUM->xAxis->setBasePen(QPen(Qt::white));
        ui->widgPSUM->xAxis->setTickLabelColor(QColor(Qt::white));


        ui->widgPSUM->yAxis->setBasePen(QPen(Qt::white));
        ui->widgPSUM->yAxis->setTickLabelColor(QColor(Qt::white));
        ui->widgPSUM->setBackground(QColor(Qt::black));

        QCPBars *bars = new QCPBars(ui->widgPSUM->yAxis, ui->widgPSUM->xAxis);
        bars->setPen(QPen(Qt::yellow));
        bars->setBrush(QColor(10, 140, 70));
        bars->setWidth(0.5);
        //        bars1->


        connect(t3, &plotfunction::histoBarPlotSignal,this,[=](QVector<double> yAxisVal, QVector<double> xAxisVal)
        {
            if(yAxisVal.isEmpty() || xAxisVal.isEmpty()) return ;
            bars->setData(yAxisVal, xAxisVal);
            //            ui->widgFRQ->rescaleAxes();
            ui->widgPSUM->replot();
        });
        //---------------------------------- end histo ----------------------------------
    }

    //======== pri ==================================================================================
    {
        QGLchart * plt4 = new QGLchart();
        plt4->setChartType(ChartType::ScatterPlot);
        initGLPlot(plt4, ChartType::ScatterPlot);


        QVBoxLayout * ver4 = new QVBoxLayout;
        ver4->addWidget(plt4);

        QGroupBox * grp4 = new QGroupBox;
        grp4->setLayout(ver4);
        grp4->setTitle("pri");

        t4->hasHistogram = true;

        ui->verPRI->addWidget(grp4);
        connect(t4, &plotfunction::pltUpdateSignal, plt4,QOverload<QCustomSeries>::of(&QGLchart::updateSeriesData));


        //---------------------------------- histo ----------------------------------
        ui->widgPRI->xAxis->setRange(5,100);
        ui->widgPRI->xAxis->setBasePen(QPen(Qt::white));
        ui->widgPRI->xAxis->setTickLabelColor(QColor(Qt::white));


        ui->widgPRI->yAxis->setBasePen(QPen(Qt::white));
        ui->widgPRI->yAxis->setTickLabelColor(QColor(Qt::white));
        ui->widgPRI->setBackground(QColor(Qt::black));

        QCPBars *bars = new QCPBars(ui->widgPRI->yAxis, ui->widgPRI->xAxis);
        bars->setPen(QPen(Qt::yellow));
        bars->setBrush(QColor(10, 140, 70));
        bars->setWidth(0.5);
        //        bars1->


        connect(t4, &plotfunction::histoBarPlotSignal,this,[=](QVector<double> yAxisVal, QVector<double> xAxisVal)
        {
            if(yAxisVal.isEmpty() || xAxisVal.isEmpty()) return ;
            bars->setData(yAxisVal, xAxisVal);
            //            ui->widgFRQ->rescaleAxes();
            ui->widgPRI->replot();
        });
        //---------------------------------- end histo ----------------------------------
    }

    //======== pwOne ================================================================================
    {
        QGLchart * plt5 = new QGLchart();
        plt5->setChartType(ChartType::ScatterPlot);
        initGLPlot(plt5, ChartType::ScatterPlot);


        QVBoxLayout * ver5 = new QVBoxLayout;
        ver5->addWidget(plt5);

        QGroupBox * grp5 = new QGroupBox;
        grp5->setLayout(ver5);
        grp5->setTitle("pwOne");

        t5->hasHistogram = true;

        ui->verPWOne->addWidget(grp5);
        connect(t5, &plotfunction::pltUpdateSignal, plt5,QOverload<QCustomSeries>::of(&QGLchart::updateSeriesData));



        //---------------------------------- histo ----------------------------------
        ui->widgPW->xAxis->setRange(5,100);
        ui->widgPW->xAxis->setBasePen(QPen(Qt::white));
        ui->widgPW->xAxis->setTickLabelColor(QColor(Qt::white));


        ui->widgPW->yAxis->setBasePen(QPen(Qt::white));
        ui->widgPW->yAxis->setTickLabelColor(QColor(Qt::white));
        ui->widgPW->setBackground(QColor(Qt::black));

        QCPBars *bars = new QCPBars(ui->widgPW->yAxis, ui->widgPW->xAxis);
        bars->setPen(QPen(Qt::yellow));
        bars->setBrush(QColor(10, 140, 70));
        bars->setWidth(0.5);
        //        bars1->


        connect(t5, &plotfunction::histoBarPlotSignal,this,[=](QVector<double> yAxisVal, QVector<double> xAxisVal)
        {
            if(yAxisVal.isEmpty() || xAxisVal.isEmpty()) return ;
            bars->setData(yAxisVal, xAxisVal);
            //            ui->widgFRQ->rescaleAxes();
            ui->widgPW->replot();
        });
        //---------------------------------- end histo ----------------------------------


    }

    //======== pwTwo ================================================================================
    {
        QGLchart * plt6 = new QGLchart();
        plt6->setChartType(ChartType::ScatterPlot);
        initGLPlot(plt6, ChartType::ScatterPlot);


        QVBoxLayout * ver6 = new QVBoxLayout;
        ver6->addWidget(plt6);

        QGroupBox * grp6 = new QGroupBox;
        grp6->setLayout(ver6);
        grp6->setTitle("pwTwo");

        t6->hasHistogram = false;

        ui->verPWTwo->addWidget(grp6);
        connect(t6, &plotfunction::pltUpdateSignal, plt6,QOverload<QCustomSeries>::of(&QGLchart::updateSeriesData));
    }

    //======== toa ==================================================================================
    {
        QGLchart * plt7 = new QGLchart();
        plt7->setChartType(ChartType::ScatterPlot);
        initGLPlot(plt7, ChartType::ScatterPlot);


        QVBoxLayout * ver7 = new QVBoxLayout;
        ver7->addWidget(plt7);

        QGroupBox * grp7 = new QGroupBox;
        grp7->setLayout(ver7);
        grp7->setTitle("toa");

        t7->hasHistogram = false;

        ui->verToa->addWidget(grp7);
        connect(t7, &plotfunction::pltUpdateSignal, plt7,QOverload<QCustomSeries>::of(&QGLchart::updateSeriesData));
    }

    //======== Channel ==============================================================================
    {
        QGLchart * plt8 = new QGLchart();
        plt8->setChartType(ChartType::ScatterPlot);
        initGLPlot(plt8, ChartType::ScatterPlot);


        QVBoxLayout * ver8 = new QVBoxLayout;
        ver8->addWidget(plt8);

        QGroupBox * grp8 = new QGroupBox;
        grp8->setLayout(ver8);
        grp8->setTitle("Channel");

        t8->hasHistogram = false;

        ui->verChannel->addWidget(grp8);
        connect(t8, &plotfunction::pltUpdateSignal, plt8,QOverload<QCustomSeries>::of(&QGLchart::updateSeriesData));
    }

    //==========================================================
    //----------------------------------------------------------
    //==========================================================

    init();
    initPlot();


    //    ui->tab_2->setVisible(false);
    ////    ui->tab_3->setVisible(false);
    //    ui->tab_13->setVisible(false);
    //    ui->tab_4->setVisible(false);
    //    ui->tab->setVisible(false);
    //    ui->tabHopping->setVisible(false);
    //    ui->tabSpectrum->setVisible(false);
    //    ui->tabAtt->setVisible(false);



    // ----------------- [limitation for frq values] -----------------


    //    ui->btnSet->setEnabled(false);
    QFile file(fileAuth);
    if(file.exists())
    {
        if(file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream str(&file);
            QStringList frqLimit= str.readLine().split(",");
            if(frqLimit.size()==2)
            {
                minFrqLimit = frqLimit[0].toDouble();
                maxFrqLimit = frqLimit[1].toDouble();
            }

            //            emit limitationValueSignal(minFrqLimit, maxFrqLimit);
        }
    }
    else
    {

    }

    //-------------------------------------------------------------------

    if (receiverIsConnected)
    {
        smartNoise();
        qInfo() <<"spnFrq"<<frqSpn->value();
        ui->spnFrq->setValue(abs(DC_6_UPTO_8_12 - frqSpn->value()));
    }

    //    QRect screenGeometry = QApplication::desktop()->screenGeometry(this);

    //    int targetWidth = static_cast<int>(screenGeometry.width() );
    //    int targetHeight = static_cast<int>(screenGeometry.height());

    //    this->setFixedSize(targetWidth, targetHeight);

    this->setFixedWidth(1270);
    this->setFixedHeight(980);
    connection();



    //spin box limitation
    if(DC_6_UPTO_8_12 == 0)
    {
        ui->spnFrq->setRange         (1000,6000);
    }

    else
    {
        ui->spnFrq->setRange         (8000,12000);
    }

    //    ui->tabWidgetSetting->removeTab(0);
    //    ui->tabWidgetSetting->removeTab(1);
    //    ui->tabWidgetSetting->removeTab(2);
    //    ui->tabWidgetSetting->removeTab(3);
    //    ui->tabWidgetSetting->removeTab(4);
    //    ui->tabWidgetSetting->removeTab(5);
    //    ui->tabWidgetSetting->removeTab(6);
    //    ui->tabWidgetSetting->removeTab(7);
    //    ui->tabWidgetSetting->removeTab(8);
    //    ui->tabWidgetSetting->removeTab(9);
    //    ui->tabWidgetSetting->removeTab(10);
    //    ui->tabWidgetSetting->removeTab(11);


    spnCWFrq          = ui->spnCWFrq;
    spnCWPower        = ui->spnCWPower;
    spnSpotFrq        = ui->spnSpotFrq;
    spnSpotPower      = ui->spnSpotPower;
    spnSweepStartFrq  = ui->spnSweepStartFrq;
    spnSweepStopFrq   = ui->spnSweepStopFrq;
    spnSweepStep      = ui->spnSweepStep;
    spnSweepPower     = ui->spnSweepPower;
    spnDelay          = ui->spnDelay;
    spnImpulseFrq     = ui->spnImpulseFrq;
    spnImpulsePower   = ui->spnImpulsePower;
    spnWBPower        = ui->spnWBPower;

    //    QTableWidget:item{background-color:#6e661a}
    //    ui->tableWidget_2->setStyleSheet("QTableWidget:item:selected{background-color:red}");


    //


    //show and hide elememts

    //    ui->tabWidgetSetting->removeTab(11);
    //    ui->tabWidgetSetting->removeTab(10);
    //    ui->tabWidgetSetting->removeTab(9);
    //    ui->tabWidgetSetting->removeTab(0);
    //    ui->tabWidgetSetting->removeTab(8);
    //    ui->tabWidgetSetting->removeTab(7);
    //    ui->tabWidgetSetting->removeTab(6);
    //    ui->tabWidgetSetting->removeTab(5);
    //    ui->tabWidgetSetting->removeTab(4);
    //    ui->tabWidgetSetting->removeTab(3);
    //    ui->tabWidgetSetting->removeTab(2);
    //    ui->tabWidgetSetting->removeTab(1);


    //    ui->linQuestion->setVisible(false);




    wigRecover = ui->tabRecovery;

    for(int i{}; i < ui->tabWidget->count(); i++)
    {
        if (ui->tabWidget->tabText(i)== "Recovery Mode")
        {
            int indexTab = ui->tabWidget->indexOf(wigRecover);
            ui->tabWidget->removeTab(indexTab);
        }
    }


    hiddenUiElements(false);
    //    ui->tblUsers->setVisible(false);
    ui->grbUser->setVisible(false);
    ui->btnShowUser->setVisible(false);
}


void ReceiverMain::calibInitial()
{
    //-------------- calibratio -----------------------
    checkIfValuesSetOnSG = new QTimer;
    checkIfValuesSetOnSG->setInterval(500);
    connect(checkIfValuesSetOnSG, &QTimer::timeout,[&]
    {
        if(isFrqSetOnCalib and isPowerSetOnCalib)
        {
            powerCalibVec[cntVecCalib]   = powNw;
            frqCalibVec[cntVecCalib]     = calibVal;

            frqDomainPlot->powerCalib = powNw;
            frqDomainPlot->frqCalib   = calibVal;

            frqDomainPlot->isCalibAllowed = true;

            isFrqSetOnCalib = false;
            isPowerSetOnCalib = false;

        }
    });
    checkIfValuesSetOnSG->start();



    QFile openCalibFile(calibFile);


    //    if ( !calibFile.exists())
    //    {
    //        messageForAuth("There is no user file. Create new user in order to create file.");
    //    }

    if(openCalibFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&openCalibFile);


        while (!in.atEnd())
        {
            QString line = in.readLine();
            QStringList lineParts = line.split(",");
            sgFrqInCalibFile.append(lineParts[0].toDouble());
            sgPowerInCalibFile.append(lineParts[1].toDouble());
            cardPowerInCalibFile.append(lineParts[3].toDouble());
        }
        openCalibFile.close();
    }


    //-------------- calibratio -----------------------
}



void ReceiverMain::setupDrfmControlTab()
{
    // Hardware mapping is taken from the supplied Vivado/HDL sources:
    //   tcl07020.tcl + AddressSegments.csv:
    //       led_count_ip_0 AXI4-Lite base = 0x43C30000
    //   led_count_ip_addr_decoder.v:
    //       TH0=0x100, PDW=0x110, INchann=0x118, thcw=0x11C, DACseles=0x138
    //   tcl07020.tcl wiring:
    //       THo_0    -> MUXDACS amplify
    //       PDWO     -> MUXDACS doppler_enable
    //       Inchanno -> MUXDACS doppler_phase_offset
    //       thcwo    -> MUXDACS doppler_phase_step
    //       DACseleso-> MUXDACS DacSel
    //   MUXDACS_ip.v:
    //       DacSel=1 -> DRFM/processed path, DacSel=0 -> DMA/noise path.
    //
    // The new controls intentionally use the same IIO/mwipcore register-write
    // transaction as the original TH1/TH2 Write buttons.

    QWidget *page = new QWidget(ui->tabWidgetSetting);
    page->setObjectName(QStringLiteral("tabDrfmControl"));

    QVBoxLayout *pageLayout = new QVBoxLayout(page);
    pageLayout->setContentsMargins(10, 10, 10, 10);
    pageLayout->setSpacing(10);

    const auto physicalAddressText = [](quint32 offset) {
        return QStringLiteral("0x%1")
                .arg(QString::number(ControlUnitADRV9009::LED_COUNT_AXI_BASE + offset, 16)
                     .toUpper(), 8, QLatin1Char('0'));
    };

    // -------------------- DRFM / Noise source select --------------------
    QGroupBox *drfmGroup = new QGroupBox(tr("DRFM"), page);
    QGridLayout *drfmLayout = new QGridLayout(drfmGroup);
    QLabel *drfmAddress = new QLabel(
                tr("DACSEL register: %1   (1 = DRFM, 0 = Noise/DMA)")
                .arg(physicalAddressText(ControlUnitADRV9009::REG_DRFM_SELECT)), drfmGroup);
    QRadioButton *drfmOn = new QRadioButton(tr("On (DRFM = 1)"), drfmGroup);
    QRadioButton *drfmOff = new QRadioButton(tr("Off (Noise = 0)"), drfmGroup);
    QPushButton *drfmSet = new QPushButton(tr("Set"), drfmGroup);
    QButtonGroup *drfmButtons = new QButtonGroup(drfmGroup);
    drfmButtons->setExclusive(true);
    drfmButtons->addButton(drfmOn);
    drfmButtons->addButton(drfmOff);
    drfmOff->setChecked(true); // no write is performed during startup
    drfmSet->setObjectName(QStringLiteral("btnSetDrfm"));
    drfmLayout->addWidget(drfmAddress, 0, 0, 1, 4);
    drfmLayout->addWidget(new QLabel(tr("State"), drfmGroup), 1, 0);
    drfmLayout->addWidget(drfmOn, 1, 1);
    drfmLayout->addWidget(drfmOff, 1, 2);
    drfmLayout->addWidget(drfmSet, 1, 3);
    pageLayout->addWidget(drfmGroup);

    // -------------------- Amply / TH0 --------------------
    QGroupBox *ampGroup = new QGroupBox(tr("Amply"), page);
    QGridLayout *ampLayout = new QGridLayout(ampGroup);
    QLabel *ampAddress = new QLabel(
                tr("TH0 / amplify register: %1")
                .arg(physicalAddressText(ControlUnitADRV9009::REG_AMPLY)), ampGroup);
    QRadioButton *ampOn = new QRadioButton(tr("On"), ampGroup);
    QRadioButton *ampOff = new QRadioButton(tr("Off (unity = 1)"), ampGroup);
    QButtonGroup *ampButtons = new QButtonGroup(ampGroup);
    ampButtons->setExclusive(true);
    ampButtons->addButton(ampOn);
    ampButtons->addButton(ampOff);
    ampOff->setChecked(true);

    QLineEdit *ampEdit = new QLineEdit(ampGroup);
    QPushButton *ampSet = new QPushButton(tr("Set"), ampGroup);
    ampEdit->setObjectName(QStringLiteral("txtDrfmAmply"));
    ampSet->setObjectName(QStringLiteral("btnSetDrfmAmply"));
    ampEdit->setValidator(new QIntValidator(0, 65535, ampEdit));
    ampEdit->setText(QStringLiteral("1"));
    ampEdit->setPlaceholderText(QStringLiteral("0 ... 65535"));
    ampEdit->setToolTip(tr("Exact raw TH0/amplify value. OFF restores unity gain (1)."));
    ampLayout->addWidget(ampAddress, 0, 0, 1, 5);
    ampLayout->addWidget(new QLabel(tr("State"), ampGroup), 1, 0);
    ampLayout->addWidget(ampOn, 1, 1);
    ampLayout->addWidget(ampOff, 1, 2);
    ampLayout->addWidget(new QLabel(tr("Value"), ampGroup), 2, 0);
    ampLayout->addWidget(ampEdit, 2, 1, 1, 3);
    ampLayout->addWidget(ampSet, 2, 4);
    pageLayout->addWidget(ampGroup);

    // -------------------- VGPO / Doppler --------------------
    QGroupBox *vgpoGroup = new QGroupBox(tr("VGPO / Doppler"), page);
    QGridLayout *vgpoLayout = new QGridLayout(vgpoGroup);
    QLabel *vgpoAddress = new QLabel(
                tr("PDW Enable: %1   |   Offset (INchann): %2   |   Phase Step (thcw): %3")
                .arg(physicalAddressText(ControlUnitADRV9009::REG_VGPO_ENABLE))
                .arg(physicalAddressText(ControlUnitADRV9009::REG_VGPO_OFFSET))
                .arg(physicalAddressText(ControlUnitADRV9009::REG_VGPO_STEP)),
                vgpoGroup);
    vgpoAddress->setWordWrap(true);

    QRadioButton *vgpoOn = new QRadioButton(tr("On (PDW = 1)"), vgpoGroup);
    QRadioButton *vgpoOff = new QRadioButton(tr("Off (PDW = 0)"), vgpoGroup);
    QButtonGroup *vgpoButtons = new QButtonGroup(vgpoGroup);
    vgpoButtons->setExclusive(true);
    vgpoButtons->addButton(vgpoOn);
    vgpoButtons->addButton(vgpoOff);
    vgpoOff->setChecked(true);

    QLineEdit *vgpoOffsetEdit = new QLineEdit(vgpoGroup);
    QLineEdit *vgpoStepEdit = new QLineEdit(vgpoGroup);
    QPushButton *vgpoSet = new QPushButton(tr("Set"), vgpoGroup);
    vgpoOffsetEdit->setObjectName(QStringLiteral("txtVgpoOffset"));
    vgpoStepEdit->setObjectName(QStringLiteral("txtVgpoPhaseStep"));
    vgpoSet->setObjectName(QStringLiteral("btnSetVgpo"));
    vgpoOffsetEdit->setText(QStringLiteral("0"));
    vgpoOffsetEdit->setPlaceholderText(QStringLiteral("0 ... 4294967295"));
    vgpoOffsetEdit->setToolTip(tr("Raw 32-bit doppler_phase_offset written to INchann / 0x43C30118."));
    vgpoStepEdit->setValidator(new QIntValidator(std::numeric_limits<int>::min(),
                                                 std::numeric_limits<int>::max(),
                                                 vgpoStepEdit));
    vgpoStepEdit->setText(QStringLiteral("0"));
    vgpoStepEdit->setPlaceholderText(QStringLiteral("signed 32-bit phase step"));
    vgpoStepEdit->setToolTip(tr("Raw signed doppler_phase_step. Example: 1200 writes 1200 to 0x43C3011C."));

    vgpoLayout->addWidget(vgpoAddress, 0, 0, 1, 5);
    vgpoLayout->addWidget(new QLabel(tr("State"), vgpoGroup), 1, 0);
    vgpoLayout->addWidget(vgpoOn, 1, 1);
    vgpoLayout->addWidget(vgpoOff, 1, 2);
    vgpoLayout->addWidget(new QLabel(tr("Offset"), vgpoGroup), 2, 0);
    vgpoLayout->addWidget(vgpoOffsetEdit, 2, 1, 1, 3);
    vgpoLayout->addWidget(new QLabel(tr("Phase Step"), vgpoGroup), 3, 0);
    vgpoLayout->addWidget(vgpoStepEdit, 3, 1, 1, 3);
    vgpoLayout->addWidget(vgpoSet, 3, 4);
    pageLayout->addWidget(vgpoGroup);

    // -------------------- Hardware transaction output --------------------
    QLabel *backendLabel = new QLabel(tr("Register backend: waiting for board connection."), page);
    backendLabel->setObjectName(QStringLiteral("lblDrfmRegisterBackend"));
    backendLabel->setWordWrap(true);
    pageLayout->addWidget(backendLabel);

    QLabel *statusLabel = new QLabel(tr("No command has been sent yet."), page);
    statusLabel->setObjectName(QStringLiteral("lblDrfmControlStatus"));
    statusLabel->setWordWrap(true);
    pageLayout->addWidget(statusLabel);

    QPlainTextEdit *registerOutput = new QPlainTextEdit(page);
    registerOutput->setObjectName(QStringLiteral("txtDrfmRegisterOutput"));
    registerOutput->setReadOnly(true);
    registerOutput->setMaximumBlockCount(200);
    registerOutput->setMinimumHeight(150);
    registerOutput->setPlainText(
                tr("Vivado map: led_count_ip_0 @ 0x43C30000.\n"
                   "The controls below use the same mwipcore/IIO register-write path as TH1 and TH2.\n"
                   "Every successful Set/On/Off operation prints the exact AXI address and raw value here."));
    pageLayout->addWidget(registerOutput);
    pageLayout->addStretch(1);

    ui->tabWidgetSetting->addTab(page, tr("DRFM"));

    auto controlUnit = [this]() -> ControlUnitADRV9009 * {
        return (oscMain && oscMain->_adrv9009controlUnit)
                ? oscMain->_adrv9009controlUnit : nullptr;
    };

    auto showResult = [backendLabel, statusLabel, registerOutput](
            ControlUnitADRV9009 *unit, bool ok, const QString &details) {
        if (unit)
            backendLabel->setText(unit->registerBackendInfo());
        statusLabel->setText(ok ? QObject::tr("Applied to board.")
                                : QObject::tr("Register operation failed."));
        registerOutput->appendPlainText(QStringLiteral("\n%1\n%2")
                                        .arg(ok ? QStringLiteral("[OK]")
                                                : QStringLiteral("[FAILED]"), details));
        registerOutput->verticalScrollBar()->setValue(registerOutput->verticalScrollBar()->maximum());
    };

    auto noBoard = [statusLabel, registerOutput]() {
        const QString msg = QObject::tr("Board control is not connected yet.");
        statusLabel->setText(msg);
        registerOutput->appendPlainText(QStringLiteral("\n[FAILED]\n") + msg);
    };

    auto applyDrfm = [controlUnit, drfmOn, showResult, noBoard]() {
        ControlUnitADRV9009 *unit = controlUnit();
        if (!unit) { noBoard(); return; }
        QString details;
        const bool ok = unit->setDrfmEnabled(drfmOn->isChecked(), &details);
        showResult(unit, ok, details);
    };

    auto applyAmply = [controlUnit, ampEdit, ampOn, statusLabel, showResult, noBoard]() {
        bool valueOk = false;
        const uint value = ampEdit->text().toUInt(&valueOk);
        if (!valueOk || value > 65535u) {
            statusLabel->setText(QObject::tr("Amply must be between 0 and 65535."));
            return;
        }
        ControlUnitADRV9009 *unit = controlUnit();
        if (!unit) { noBoard(); return; }
        QString details;
        const bool ok = unit->setAmplifyValue(static_cast<quint16>(value), ampOn->isChecked(), &details);
        showResult(unit, ok, details);
    };

    auto applyVgpoEnable = [controlUnit, vgpoOn, showResult, noBoard]() {
        ControlUnitADRV9009 *unit = controlUnit();
        if (!unit) { noBoard(); return; }
        QString details;
        const bool ok = unit->setVgpoEnabled(vgpoOn->isChecked(), &details);
        showResult(unit, ok, details);
    };

    auto applyVgpoSet = [controlUnit, vgpoOffsetEdit, vgpoStepEdit, vgpoOn,
                         statusLabel, showResult, noBoard]() {
        bool offsetOk = false;
        const qulonglong offset64 = vgpoOffsetEdit->text().toULongLong(&offsetOk);
        if (!offsetOk || offset64 > std::numeric_limits<quint32>::max()) {
            statusLabel->setText(QObject::tr("VGPO Offset must be 0 ... 4294967295."));
            return;
        }

        bool stepOk = false;
        const qlonglong step64 = vgpoStepEdit->text().toLongLong(&stepOk);
        if (!stepOk || step64 < std::numeric_limits<qint32>::min() ||
                       step64 > std::numeric_limits<qint32>::max()) {
            statusLabel->setText(QObject::tr("VGPO Phase Step must fit a signed 32-bit integer."));
            return;
        }

        ControlUnitADRV9009 *unit = controlUnit();
        if (!unit) { noBoard(); return; }
        QString details;
        const bool ok = unit->setVgpoValue(static_cast<quint32>(offset64),
                                           static_cast<qint32>(step64),
                                           vgpoOn->isChecked(), &details);
        showResult(unit, ok, details);
    };

    // ON/OFF buttons are real hardware commands, not just UI state changes.
    // Set re-applies the selected state/value, matching the original TH1/TH2
    // model where a button click performs the actual register transaction.
    connect(drfmOn,  &QRadioButton::clicked, this, [applyDrfm](bool){ applyDrfm(); });
    connect(drfmOff, &QRadioButton::clicked, this, [applyDrfm](bool){ applyDrfm(); });
    connect(drfmSet, &QPushButton::clicked, this, applyDrfm);

    connect(ampOn,   &QRadioButton::clicked, this, [applyAmply](bool){ applyAmply(); });
    connect(ampOff,  &QRadioButton::clicked, this, [applyAmply](bool){ applyAmply(); });
    connect(ampSet,  &QPushButton::clicked, this, applyAmply);

    connect(vgpoOn,  &QRadioButton::clicked, this, [applyVgpoEnable](bool){ applyVgpoEnable(); });
    connect(vgpoOff, &QRadioButton::clicked, this, [applyVgpoEnable](bool){ applyVgpoEnable(); });
    connect(vgpoSet, &QPushButton::clicked, this, applyVgpoSet);
}

void ReceiverMain::showNoiseStatus(QString activeMode= "",QString msg="")
{

    whichNoiseModeIsActive = activeMode;
    ui->lblActiveNoise->setText(msg);
}
void ReceiverMain::smartNoise()
{
    smartInit();

    ui->chbFullBand->setEnabled(true);
    connect(frqSpn, QOverload<double>::of( &QDoubleSpinBox::valueChanged),  [&](double val){
        val = abs(DC_6_UPTO_8_12 - val);
        ui->lblFrqBound->setText(QString::number(val-250)+"-"+QString::number(val+250));
    });

    connect(ui->chbAutoAmp,     &QCheckBox::stateChanged,                     [&](int val){ ui->spnAutoAmp->setEnabled(val == 0 ? true :false);});
    connect(ui->spnAutoAmp,     QOverload<int>::of(&QSpinBox::valueChanged),  [&](int val){ oscMain->_adrv9009controlUnit->setAutoAmpSignal(val);});


    connect(ui->cmbTransmit,    QOverload<int>::of(&QComboBox::activated),

            oscMain->_adrv9009controlUnit,&ControlUnitADRV9009::sendTransmitSignal);


    connect(ui->cmbTransmit, QOverload<int>::of(&QComboBox::activated),[&](int val)
    {

        val== 1 ? ui->chbSmartNoise->setChecked(true):ui->chbSmartNoise->setChecked(false);

    });


    //send channel to card
    connect(ui->spnSmartNoise, QOverload<double>::of(&QDoubleSpinBox::valueChanged),this, [&](double val)
    {
        if(ui->spnSmartNoise->value() < 1000) return ;
        int channelNumber = selectChannelWithFrq(val);
        oscMain->_adrv9009controlUnit->setChannelSignal(channelNumber);
        ui->label_9->setText("channel: "+QString::number(channelNumber));
    });



    connect(ui->cmbSmartNoise,  QOverload<int>::of(&QComboBox::currentIndexChanged) ,[&](int val)
    {
        ui->spnSmartNoise->setEnabled(val == 1 ? true :false);
        ui->lblFrqBound->setVisible(val   == 1 ? true :false);
        if(val == 0) stopSmartNoise("table"); //if pw is selected, full band must be stop
    });



    //smart noise check box
    connect(ui->chbSmartNoise,  &QCheckBox::stateChanged, [&](int val)
    {

        if(val == 0)
        {
            frqDomainPlot->isSmartNoisetOn = false;
            power_TX1_DownChk->setChecked(true);
            ui->cmbTransmit->setCurrentIndex(0);
            ui->cmbTransmit->currentIndexChanged(0);



            oscMain->_adrv9009controlUnit->sendTransmitSignal(ui->cmbTransmit->currentIndex());
            stopSmartNoise("All");

            showNoiseStatus("","There is no active smart noise.");
        }

        else
        {
            frqDomainPlot->on_btn_select_fft_clicked();

            frqDomainPlot->isSmartNoisetOn = true ;

            ui->chbSelectChart->setChecked(false);
            ui->chbSelectChart->stateChanged(0);

            ui->btnSendTimeDataToCard->setEnabled(val==0?false : true);
            ui->chbFullBand->setEnabled(val==0?false : true);

            ui->cmbTransmit->setCurrentIndex(1);
            ui->cmbTransmit->currentIndexChanged(1);

            oscMain->_adrv9009controlUnit->sendTransmitSignal(ui->cmbTransmit->currentIndex());

            //            oscMain->_adrv9009->powerTX2DownChk->setChecked(true);
            //            oscMain->_adrv9009->power_TX1_DownChk->setChecked(false);

            if(!ui->chbAutoAmp->isChecked()) oscMain->_adrv9009controlUnit->setAutoAmpSignal(ui->spnAutoAmp->value());

            QString msg = oscMain->_adrv9009->setFile(spotPath,0);

            oscMain->_adrv9009->att_TX1_Spn->setValue(0);
            emit smartNoiseIsActiveSignal();

        }

    });


    connect(ui->spnSmartNoise, QOverload<double>::of( &QDoubleSpinBox::valueChanged), [&](double val)
    {
        oscMain->_adrv9009controlUnit->setAutoAmpSignal(ui->spnAutoAmp->value());
    });



    //this amp comes from table
    connect(timeDomainPlot,&Plot::ampValueSignal, [&](double ampVal){setAutoAmpValue(ampVal);});

    //this amp comes from fft chart
    connect(frqDomainPlot,&Plot::ampValueSignal, [&](double ampVal){setAutoAmpValue(ampVal);});




    // Cleaning table periodically.  These four vectors represent one logical
    // record and must always be indexed with the same valid range.
    connect(timeDomainPlot, &Plot::cleanTimeSignal, this, [this](double val){
        if (cwTableTimer && val > 0)
            cwTableTimer->setInterval(static_cast<int>(val));
    });

    connect(cwTableTimer, &QTimer::timeout, this, [this]{
        int commonSize = qMin(qMin(frqTimeVec.size(), frqValVec.size()),
                              qMin(powerValVec.size(), modValVec.size()));

        if (frqTimeVec.size() != commonSize ||
            frqValVec.size() != commonSize ||
            powerValVec.size() != commonSize ||
            modValVec.size() != commonSize)
        {
            qWarning() << "CW table vectors were out of sync; normalizing sizes:"
                       << frqTimeVec.size() << frqValVec.size()
                       << powerValVec.size() << modValVec.size()
                       << "->" << commonSize;
            frqTimeVec.resize(commonSize);
            frqValVec.resize(commonSize);
            powerValVec.resize(commonSize);
            modValVec.resize(commonSize);
        }

        const long long timeNow = QDateTime::currentMSecsSinceEpoch();
        /* Remove backwards so removing one entry never invalidates the next
         * index that still has to be inspected. */
        for (int i = commonSize - 1; i >= 0; --i)
        {
            if (timeNow - frqTimeVec.at(i) > 3000)
            {
                frqTimeVec.remove(i);
                frqValVec.remove(i);
                powerValVec.remove(i);
                modValVec.remove(i);
            }
        }
    });

    if (cwTableTimer && !cwTableTimer->isActive())
        cwTableTimer->start();


    //    connect(frqDomainPlot, &Plot::getMaxValueSignal, [&](double frqV, double PowerV)
    //    {

    //        if (frqCalibVec[cntFrqCalib] = 0) return ;

    //        if(int(frqV) == int(frqCalibVec[cntFrqCalib-1]) )
    //        {
    //            qInfo() <<"--------------------------------------------------> "<<frqV;
    //            frqCardVec[cntFrqCalib-1]   = frqV;
    //            powerCardVec[cntFrqCalib-1] = PowerV;
    //        }
    //    });


    connect(frqDomainPlot, &Plot::getMaxValueSignal, this,&ReceiverMain::checkingCalibResponseInSpectromSlot );
    smartTimeDomainNoise();
    smartTableNoise();
    smartSelectedNoise();
}

void ReceiverMain::setAutoAmpValue(double ampVal)
{
    if (!ui->chbAutoAmp->isChecked()) return;
    double amp = abs(ampVal);

    if(amp > 15)
    {
        ui->spnAutoAmp->setValue(amp - 15);
    }

    else
    {
        ui->spnAutoAmp->setValue(15);
    }
}

void ReceiverMain::stopSmartNoise(QString mode)
{

    if(mode == "table")
    {
        if(isSending) ui->btnSendTimeDataToCard->clicked(true);
        frqDomainPlot->on_btn_select_fft_clicked();
    }

    else if(mode == "full")
    {
        timeDomainPlot->checkBoxUnched();
        frqDomainPlot->on_btn_select_fft_clicked();
    }

    else if (mode == "select")
    {
        if(isSending) ui->btnSendTimeDataToCard->clicked(true);
        timeDomainPlot->checkBoxUnched();
    }

    else
    {
        if(isSending) ui->btnSendTimeDataToCard->clicked(true);
        frqDomainPlot->on_btn_select_fft_clicked();
        timeDomainPlot->checkBoxUnched();
    }
}



void ReceiverMain::smartTableNoise()
{

    //check box in table
    connect(timeDomainPlot,&Plot::sendChannelToCardSignal, [&](double val)
    {
        if(val < 0)
        {
            ui->cmbTransmit->setCurrentIndex(0);
            ui->cmbTransmit->currentIndexChanged(0);
            ui->label_9->setText("channel: not selected!");
            showNoiseStatus("","There is no active smart noise.");
            power_TX1_DownChk->setChecked(true);
        }

        else
        {

            ui->chbSmartNoise->setChecked(true);
            ui->chbSmartNoise->stateChanged(1);



            stopSmartNoise("table");

            ui->cmbSmartNoise->setCurrentText("pw");
            oscMain->_adrv9009controlUnit->setChannelFromTable(val);
            ui->label_9->setText("channel: "+QString::number(val));

            showNoiseStatus("tableNoise","Table smart noise is activated.");
            power_TX1_DownChk->setChecked(false);
        }
    });
}



void ReceiverMain::smartSelectedNoise()
{
    //get max frq and send channel to card
    connect(frqDomainPlot, &Plot::sendMaxFrqToCardSignal,this, [&](double val)
    {
        ui->chbSmartNoise->setChecked(true);
        ui->chbSmartNoise->stateChanged(1);

        stopSmartNoise("select");

        ui->cmbSmartNoise->setCurrentText("frequency band(20 Mhz variation)");

        showNoiseStatus("selectedNoise","Selected smart noise is activated.");

        power_TX1_DownChk->setChecked(false);

        int channelNumber = selectChannelWithFrq(val);
        if(channelNumber == -1) return ;
        oscMain->_adrv9009controlUnit->setChannelSignal(channelNumber);
        ui->label_9->setText("channel: "+QString::number(channelNumber));
    });


    connect(frqDomainPlot, &Plot::selectedSmartNoiseStatusSignal,this, [&](bool val)
    {
        power_TX1_DownChk->setChecked(true);
        showNoiseStatus("","There is no active smart noise.");
    });
}




void ReceiverMain::smartTimeDomainNoise()
{
    //smart time domain
    smartTimeDomainPlot = oscMain->createMultiPlot(SMART_TIME_DOMAIN) ;

    connect(ui->btnSendTimeDataToCard, &QPushButton::clicked, [&]
    {
        isSending = !isSending;

        if(isSending)
        {
            ui->chbSmartNoise->setChecked(true);
            ui->chbSmartNoise->stateChanged(1);

            stopSmartNoise("full");

            smartTimeDomainPlot->samplTime->setValue(15000);

            att_TX1_Spn->setValue(0);
            power_TX1_DownChk->setChecked(false);

            smartTimeDomainPlot->btn_capt->clicked();
            oscMain->_adrv9009controlUnit->sendTransmitSignal(0);
            ui->btnSendTimeDataToCard->setText("Is Sending ...");

            showNoiseStatus("fullNoise","Full noise is activated.");
        }

        else
        {
            att_TX1_Spn->setValue(0);
            power_TX1_DownChk->setChecked(true);

            smartTimeDomainPlot->btn_capt->clicked();
            ui->btnSendTimeDataToCard->setText("Full Band");

            showNoiseStatus("","There is no active smart noise.");

        }
    });

    connect(smartTimeDomainPlot, &Plot::sendTimeDomainToCardSignal, [&](QString fileAdress)
    {
        ui->cmbSmartNoise->setCurrentText("frequency band(20 Mhz variation)");
        //        ui->chbSmartNoise->setChecked(false);
        oscMain->_adrv9009->setFile(fileAdress,0);
    });

    connect(smartTimeDomainPlot, &Plot::THSwitchSignal, [&](bool txStatus){power_TX1_DownChk->setChecked(txStatus);att_TX1_Spn->setValue(0);});
}




void ReceiverMain::smartInit()
{
    ui->spnSmartNoise->setEnabled(false);
    ui->btnSendTimeDataToCard->setEnabled(false);
    ui->spnAutoAmp->setEnabled(false);

    ui->lblFrqBound->setVisible(false);
    ui->lblFrqBound->setText(QString::number(abs(DC_6_UPTO_8_12 - frqSpn->value()-250))+"-"+QString::number(frqSpn->value()+250));


    QTimer configTimer;
    configTimer.setInterval(1000);
    connect(&configTimer, &QTimer::timeout,[&]{

        if(!isSending) //if full band is on , it isnt allowed to chande;
        {
            att_TX1_Spn->setValue(0);
            power_TX1_DownChk->setChecked(false);
        }
    });

}



int ReceiverMain::selectChannelWithFrq(double val)
{
    double step{15};
    double  df{};
    int channel{0};



    if (val > abs(DC_6_UPTO_8_12 -frqSpn->value()) )
    {

        df  = val - abs(DC_6_UPTO_8_12 -frqSpn->value());

        if(df <= (step/2))
        {
            channel =0;
        }

        else if(df > (step/2) and df < (step +step/2))
        {

            channel = 1;
        }

        else if(df >= (step + step/2) and df < (2*step +step/2))
        {

            channel = 2;
        }

        else if(df >= (2*step + step/2) and df < (3*step +step/2) )
        {

            channel = 3;
        }

        else if(df >= (3*step + step/2) and df < (4*step +step/2))
        {

            channel = 4;
        }

        else if(df >= (4*step + step/2) and df < (5*step +step/2))
        {

            channel = 5;
        }

        else if(df >= (5*step + step/2) and df < (6*step +step/2))
        {

            channel = 6;
        }

        else if(df >= (6*step + step/2) and df < (7*step +step/2))
        {

            channel = 7;
        }

        else if(df >= (7*step + step/2) and df < (8*step +step/2))
        {

            channel = 8;
        }

        else if(df >= (8*step + step/2) and df < (9*step +step/2))
        {

            channel = 9;
        }

        else if(df >= (9*step + step/2) and df < (10*step +step/2))
        {

            channel = 10;
        }

        else if(df >= (10*step + step/2) and df < (11*step +step/2))
        {

            channel = 11;
        }

        else if(df >= (11*step + step/2) and df < (12*step +step/2))
        {

            channel = 12;
        }

        else if(df >= (12*step + step/2) and df < (13*step +step/2))
        {

            channel = 13;
        }

        else if(df >= (13*step + step/2) and df < (14*step +step/2))
        {

            channel = 14;
        }

        else if(df >= (14*step + step/2) and df < (15*step +step/2))
        {

            channel = 15;
        }


    }

    else
    {
        df =  abs(DC_6_UPTO_8_12 - frqSpn->value() - val);

        if(df <= (step/2))
        {
            channel =0;
        }

        else if(df > (step/2) and df < (step +step/2))
        {

            channel = 31;

        }



        else if(df >= (step + step/2) and df < (2*step +step/2))
        {

            channel = 30;
        }

        else if(df >= (2*step + step/2) and df < (3*step +step/2) )
        {

            channel = 29;
        }

        else if(df >= (3*step + step/2) and df < (4*step +step/2))
        {

            channel = 28;
        }

        else if(df >= (4*step + step/2) and df < (5*step +step/2))
        {

            channel = 27;
        }

        else if(df >= (5*step + step/2) and df < (6*step +step/2))
        {

            channel = 26;
        }

        else if(df >= (6*step + step/2) and df < (7*step +step/2))
        {

            channel = 25;
        }

        else if(df >= (7*step + step/2) and df < (8*step +step/2))
        {

            channel = 24;
        }

        else if(df >= (8*step + step/2) and df < (9*step +step/2))
        {

            channel = 23;
        }

        else if(df >= (9*step + step/2) and df < (10*step +step/2))
        {

            channel = 22;
        }

        else if(df >= (10*step + step/2) and df < (11*step +step/2))
        {

            channel = 21;
        }

        else if(df >= (11*step + step/2) and df < (12*step +step/2))
        {

            channel = 20;
        }

        else if(df >= (12*step + step/2) and df < (13*step +step/2))
        {
            channel = 19;
        }

        else if(df >= (13*step + step/2) and df < (14*step +step/2))
        {
            channel = 18;
        }

        else if(df >= (14*step + step/2) and df < (15*step +step/2))
        {
            channel = 17;
        }


        else if(df >= (14*step + step/2) and df < (15*step +step/2))
        {
            channel = 16;
        }


    }


    return channel;

}



void ReceiverMain::ValueSlot( Pulse val)
{

    //    QVector2D  vec;
    //        qDebug() << "val.TOA: " << static_cast<double>(val.TOA);
    //        qDebug() << "val.Freq: " << static_cast<double>(val.Freq);
    //        vec.setX(val.TOA);
    //        vec.setY(val.Freq);
    //        a.addPoints(vec);

    //        for(int i = 0; i < 100; i++)
    //        {
    //            vec.setX(8000 + i);
    //            vec.setY(20 * log10f(abs( 1.0 / ((RAND_MAX) * qrand()) )) + 100);
    //            a.addPoints(vec);

    //        }


    //        emit frqValues(a);
    //        a.removeAllPoints();
    //        cnt++;

    emit frqValueSignal      (val.Freq, val.TOA);
    emit PAPeakValueSignal   (val.PAPeak, val.TOA);
    emit PASumMaxValueSignal (val.PASum, val.TOA);
    emit priValueSignal      (val.PRI, val.TOA);
    emit pwOneValueSignal    (val.PW1, val.TOA);
    emit pwTwoValueSignal    (val.PW2, val.TOA);
    emit toaValueSignal      (val.TOA, val.TOA);
    emit ChannelValueSignal  (val.Channel, val.TOA);
}


void ReceiverMain::creatingTimePlot(QCustomPlot * customPlot)
{
    if(!customPlot) return;
    customPlot->addGraph(); // blue line
    customPlot->graph(0)->setPen(QPen(QColor(40, 110, 0)));
    customPlot->graph(0)->setLineStyle((QCPGraph::LineStyle)0);
    customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 3));
    //    customPlot->addGraph(); // red line
    //    customPlot->graph(1)->setPen(QPen(QColor(255, 110, 40)));

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    customPlot->xAxis->setTicker(timeTicker);
    customPlot->axisRect()->setupFullAxesBox();
    customPlot->yAxis->setRange(-1,1);




    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));
}

void ReceiverMain::creatingHistogramPlot(QCustomPlot *histo , QCPAxisRect *wideAxisRect, QCPBars *bars1,  QCPGraph *mainGraph)
{
    histo->plotLayout()->clear();
    wideAxisRect = new QCPAxisRect(histo);
    histo->plotLayout()->addElement(0, 0, wideAxisRect);
    histo->setBackground(QBrush(Qt::blue));

    //            mainGraph->setData(yAxisVal,xAxisVal);
    mainGraph = histo->addGraph(wideAxisRect->axis(QCPAxis::atLeft),wideAxisRect->axis(QCPAxis::atBottom));
    mainGraph->valueAxis()->setRange(-1, 1);
    mainGraph->rescaleKeyAxis();
    mainGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black), QBrush(Qt::white), 6));
    mainGraph->setPen(QPen(QColor(50, 255, 150), 2));


    bars1 = new QCPBars(histo->yAxis,histo->xAxis);
    //    bars1->setWidth(9/(double)x1.size());
    //        bars1->setData(yAxisVal, xAxisVal);
    bars1->setPen(Qt::NoPen);
    bars1->setBrush(QColor(10, 140, 70, 160));

    //===================================================================================================================
}

void ReceiverMain::initGLPlot(QGLchart *scatterChart, ChartType glChartType)
{
    //    QCustomAxis *verticalAxis = new QCustomAxis(this);
    //    verticalAxis->setAlignment(Qt::AlignLeft);
    //    verticalAxis->setOrientation(Qt::Vertical);
    //    verticalAxis->setGridLineColor(Qt::green);
    //    verticalAxis->setTitleText("Amplitude");
    //    verticalAxis->setLabelsText("        (dBm)");
    //    verticalAxis->setVisible(true);
    //    glChart->addAxis(verticalAxis, Qt::AlignLeft);

    //    QCustomAxis *horizantalAxis = new QCustomAxis(this);
    //    horizantalAxis->setAlignment(Qt::AlignBottom);
    //    horizantalAxis->setOrientation(Qt::Horizontal);
    //    horizantalAxis->setGridLineColor(Qt::green);
    //    //    horizantalAxis->setTitleText("Frequency");
    //    //    horizantalAxis->setLabelsText("(MHz)");
    //    glChart->addAxis(horizantalAxis, Qt::AlignBottom);
    //    glChart->enableMasking(true);
    //    glChart->setMaskingModifiable(true);
    //    glChart->setMouseTracking(true);
    //    glChart->setVerticalUpperRangeMargin(1000);
    //    glChart->setVerticalLowerRangeMargin(3000);
    //    glChart->enableToolTip(true);
    //    glChart->setChartType(glChartType);
    //    glChart->enableSyncedZoom(false);

    QCustomAxis *verticalAxis = new QCustomAxis(this);
    verticalAxis->setAlignment(Qt::AlignLeft);
    verticalAxis->setOrientation(Qt::Vertical);
    verticalAxis->setGridLineColor(Qt::white);
    scatterChart->addAxis(verticalAxis, Qt::AlignLeft);
    verticalAxis->setVisible(true);
    QCustomAxis *horizantalAxis = new QCustomAxis(this);
    horizantalAxis->setAlignment(Qt::AlignBottom);
    horizantalAxis->setOrientation(Qt::Horizontal);
    horizantalAxis->setGridLineColor(Qt::white);
    //    horizantalAxis->setLabelsText("[s]");
    /*if (chartTitle)
                horizantalAxis->setRange(0, 10000);*/
    scatterChart->addAxis(horizantalAxis, Qt::AlignBottom);

    QMarginsF scatterMargins;
    scatterMargins.setLeft(0.065);
    scatterMargins.setBottom(0.12);
    scatterMargins.setRight(0.02);
    scatterMargins.setTop(0.085);
    scatterChart->setContentsMargins(scatterMargins);

    scatterChart->setHorizantalNumberPrecision(0);
    scatterChart->setHorizontalNumberCount(5);
    scatterChart->enableToolTip(true);
    scatterChart->setVerticalLabelRelPos(QVector2D(0.025, 0.3));
    scatterChart->enableVerticalLabelFineTuning(false);
    scatterChart->setManualHorzScaleRelPos(QVector2D(0.01, 0.01));

    //    verticalAxis->setTitleText("Center Freq [MHz]");
    scatterChart->setVerticalNumberPrecision(1);
    scatterChart->setScatterUpperRangeMargin(10);
    scatterChart->setScatterLowerRangeMargin(10);
    scatterChart->setVerticalLabelRelPos(QVector2D(0.025, 0.35));
}


void ReceiverMain::connection()
{




    if(!receiverIsConnected) return;

    connect(frqSpn, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double val){ ui->spnFrq->setValue(abs(DC_6_UPTO_8_12 - frqSpn->value()));});

    oscMain->_adrv9009controlUnit->setInchannelSignal(4294967295);
    connect(frqDomainPlot->txtSelectedFreq,QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double value)
    {
        double f = abs(DC_6_UPTO_8_12 - value);
        frqSpn->setValue(f);

    });
    connect(frqSpn,SIGNAL(valueChanged(QString)), this,SLOT(frqValueChenged(QString)));


    connect(timeDomainPlot, &Plot::ValueSignal,  this, &ReceiverMain::ValueSlot);
    connect(timeDomainPlot, &Plot::settingHideShowSignal, this,[&](bool val){ ui->grpSetting->setVisible(val);});


    connect(frqDomainPlot,&Plot::selectedFrqMinMaxSignal,[&](double minFrq, double maxFrq){
        ui->spnMin->setValue(minFrq);
        ui->spnMax->setValue(maxFrq);
        //        on_btnSetBandSelect_clicked();

    });


    // this connection is for awaring this class when frq value  is changed in adrv9009
    connect(ui->btnCurrentFrq,&QPushButton::clicked, this, &ReceiverMain::settingMinMaxBand);
    connect(ui->btnSpecConfig, &QPushButton::clicked,[=]{changingPlotMode(1);});
    connect(ui->chbSelectChart,&QCheckBox::stateChanged,[&](int status)
    {
        if(status) frqDomainPlot->on_btn_select_fft_clicked();
        status == 0 ? frqDomainPlot->isBandRejectOn = false  :  frqDomainPlot->isBandRejectOn = true ;

        if(status != 0)
        {
            ui->chbSmartNoise->setChecked(false);
            ui->chbSmartNoise->stateChanged(0);
        }
    });


    //----------scan connections---------------------
    connect(ui->spnPdiv,      QOverload<int>::of(&QSpinBox::valueChanged), [&](int spnVal){oscMain->_adrv9009controlUnit->setPdivSignal(spnVal);});
    connect(ui->spnteTa_az,   QOverload<int>::of(&QSpinBox::valueChanged), [&](int spnVal){oscMain->_adrv9009controlUnit->setteTa_azSignal(spnVal);});
    connect(ui->spnNum_Side,  QOverload<int>::of(&QSpinBox::valueChanged), [&](int spnVal){oscMain->_adrv9009controlUnit->setNum_SideSignal(spnVal);});
    connect(ui->spnNum_Back,  QOverload<int>::of(&QSpinBox::valueChanged), [&](int spnVal){oscMain->_adrv9009controlUnit->setNum_BackSignal(spnVal);});
    connect(ui->spnP_to_B,    QOverload<int>::of(&QSpinBox::valueChanged), [&](int spnVal){oscMain->_adrv9009controlUnit->setP_to_BSignal(spnVal);});
    connect(ui->spnNPS,       QOverload<int>::of(&QSpinBox::valueChanged), [&](int spnVal)
    {
        spnVal = spnVal *122.88e5;
        oscMain->_adrv9009controlUnit->setNPSSignal(spnVal);
    });

    connect(ui->spnP_to_S,    QOverload<int>::of(&QSpinBox::valueChanged), [&](int spnVal){oscMain->_adrv9009controlUnit->setP_to_SSignal(spnVal);});


    //this code is for updating cw table. the data is received from spectrom plot which is located in plot calss.
    connect(frqDomainPlot, &Plot::cwTableValueSignal,this,&ReceiverMain::calculatingCwTableSlot);


    connect(ui->spnCWFrq,             QOverload<double>::of(&QDoubleSpinBox::valueChanged),[&](double val){emit cwFrqSignal(val);});
    connect(ui->spnCWPower,           QOverload<double>::of(&QDoubleSpinBox::valueChanged),[&](double val){emit cwPowerSignal(val);});
    connect(ui->spnSpotFrq,           QOverload<double>::of(&QDoubleSpinBox::valueChanged),[&](double val){emit spotFrqSignal(val);});
    connect(ui->spnSpotPower,         QOverload<double>::of(&QDoubleSpinBox::valueChanged),[&](double val){emit spotPowerSignal(val);});
    connect(ui->spnSweepStartFrq,     QOverload<double>::of(&QDoubleSpinBox::valueChanged),[&](double val){emit sweepStartFrqSignal(val);});
    connect(ui->spnSweepStopFrq,      QOverload<double>::of(&QDoubleSpinBox::valueChanged),[&](double val){emit sweepStopFrqSignal(val);});
    connect(ui->spnSweepStep,         QOverload<double>::of(&QDoubleSpinBox::valueChanged),[&](double val){emit sweepStepSignal(val);});
    connect(ui->spnSweepPower,        QOverload<double>::of(&QDoubleSpinBox::valueChanged),[&](double val){emit sweepPowerSignal(val);});
    connect(ui->spnDelay,             QOverload<double>::of(&QDoubleSpinBox::valueChanged),[&](double val){emit sweepDelaySignal(val);});
    connect(ui->spnImpulseFrq,        QOverload<double>::of(&QDoubleSpinBox::valueChanged),[&](double val){emit impFrqSignal(val);});
    connect(ui->spnImpulsePower,      QOverload<double>::of(&QDoubleSpinBox::valueChanged),[&](double val){emit impPowerSignal(val);});
    connect(ui->spnImpulsePulseWidth, QOverload<double>::of(&QDoubleSpinBox::valueChanged),[&](double val){emit impPwSignal(val);});
    connect(ui->spnImpulsePRI,        QOverload<double>::of(&QDoubleSpinBox::valueChanged),[&](double val){emit impPriSignal(val);});
    connect(ui->spnWBPower,           QOverload<double>::of(&QDoubleSpinBox::valueChanged),[&](double val){emit wbPowerSignal(val);});

    connect(ui->btnSetCW ,      &QPushButton::clicked,[&]{emit cwOnBtnSignal    ();});
    connect(ui->btnDisableCW ,  &QPushButton::clicked,[&]{emit cwOffBtnSignal   ();});
    connect(ui->btnLoadSpot ,   &QPushButton::clicked,[&]{emit spotOnBtnSignal  ();});
    connect(ui->btnDisableSpot ,&QPushButton::clicked,[&]{emit spotOffBtnSignal ();});
    connect(ui->btnStartSweep , &QPushButton::clicked,[&]{emit sweepOnBtnSignal ();});
    connect(ui->btnStopSweep ,  &QPushButton::clicked,[&]{emit sweepOffBtnSignal();});
    connect(ui->btnLoadImpulse ,&QPushButton::clicked,[&]{emit impOnBtnSignal   ();});
    connect(ui->btnDisImpulse , &QPushButton::clicked,[&]{emit impOffBtnSignal  ();});
    connect(ui->btnSetWB ,      &QPushButton::clicked,[&]{emit wbOnBtnSignal    ();});
    connect(ui->btnDisableWB ,  &QPushButton::clicked,[&]{emit wbOffBtnSignal   ();});

    //    connect(frqPlt,     &plotfunction::pltShow, this, [&](QCustomPlot * plt){ui->wigFRQ = plt;});
    //    connect(paPeackPlt, &plotfunction::pltShow, this, &ReceiverMain::PAPeakValueSignal   );
    //    connect(pSumPlt,    &plotfunction::pltShow, this, &ReceiverMain::PASumMaxValueSignal );
    //    connect(priPlt,     &plotfunction::pltShow, this, &ReceiverMain::priValueSignal      );
    //    connect(pwOnePlt,   &plotfunction::pltShow, this, &ReceiverMain::pwOneValueSignal    );
    //    connect(pwTwoPlt,   &plotfunction::pltShow, this, &ReceiverMain::pwTwoValueSignal    );
    //    connect(toaPlt,     &plotfunction::pltShow, this, &ReceiverMain::toaValueSignal      );
    //    connect(channelPlt, &plotfunction::pltShow, this, &ReceiverMain::ChannelValueSignal  );


    //when we change mode to sweep and then back to the spectrom mode, the frq is changed, this way we hold the previous frq in order to set that when we back to the spectrom mode
    connect(ui->spnFrq, QOverload<double>::of(&QDoubleSpinBox::valueChanged),this, &ReceiverMain::setPrvFrq);
}


void ReceiverMain::setPrvFrq(double val)
{
    QTimer::singleShot(800, [&]{

        if(frqChanged)
        {
            frqChanged = false;
            ui->spnFrq->setValue(frqHold);
            on_btnSet_clicked();
            frqDomainPlot->frqValueIsChanged(false);
        }
    });
}

void ReceiverMain::setupRealtimeDataDemo(QCustomPlot *customPlot)
{


}
void ReceiverMain::realtimeDataSlot()
{

}

void ReceiverMain::plotHistogram(QCustomPlot * plot, std::unordered_map<double, double> * histogramData)
{
}

void ReceiverMain::realTimePlot(QCustomPlot *customPlot)
{

}





void ReceiverMain::fillHistoramArray(int type, QCustomPlot * plot, double * val,int cnt)
{

}


void ReceiverMain::init()
{

    //    ui->groupBox_7->setVisible(false);
    //                ui->groupBox_7->setVisible(false);
    //                ui->groupBox_24->setVisible(false);

    //    ui->tabWidgetSetting->setStyleSheet("QTabBar::tab:selected {background-color:"+QString(DARK_OLIVE_GREEN)+"}");

    init_signal_handlers();
    opterr = 0;
    globals::ctx=NULL;
    oscMain=new OSCMain();
    ui->tabWidget->addTab(oscMain,"configs");

    //saeid raziani
    //    oscMain->show();

    //    connect(oscMain, &OSCMain::adrv9009Signal,this,[&]{defaultSettings();});


    if (!oscMain->ctx_destroyed_by_do_quit) {

        if (!globals::ctx)
        {
            connectDialog *connect_dialog=new connectDialog();
            QObject::connect(connect_dialog,&connectDialog::connectSignal,[=](){
                globals::status=true;
                oscMain->do_init();
                oscMain->create_default_plot();
            });

            bool connectionReady = connect_dialog->Initialize();
            if(!connectionReady)
            {
                receiverIsConnected = false;
                if(connect_dialog->exec() == QDialog::Accepted && globals::ctx)
                    connectionReady = true;
            }

            if(connectionReady)
            {
                if(!globals::ctx)
                {
                    qCritical() << "Receiver initialization aborted: IIO context is null";
                    receiverIsConnected = false;
                    return;
                }

                if(!iio_context_find_device(globals::ctx, "adrv9009-phy"))
                {
                    qCritical() << "Receiver initialization aborted: adrv9009-phy was not found in IIO context";
                    receiverIsConnected = false;
                    return;
                }

                globals::status=true;
                oscMain->do_init();
                //                oscMain->create_default_plot();

                //saeid raziani

                //frq domain (second plot)
                frqDomainPlot = oscMain->createMultiPlot(FRQ_DOMAIN) ;
                QVBoxLayout * vLayfrqDomainPlot = new QVBoxLayout    ;
                vLayfrqDomainPlot->setContentsMargins(0,0,0,0)       ;
                vLayfrqDomainPlot->addWidget(frqDomainPlot)          ;
                ui->wigFrqDomain->setLayout(vLayfrqDomainPlot)       ;


                //time domain plot (first plot)
                timeDomainPlot = oscMain->createMultiPlot(TIME_DOMAIN) ;
                QVBoxLayout * vLaytimeDomainPlot = new QVBoxLayout     ;
                vLaytimeDomainPlot->setContentsMargins(0,0,0,0)        ;
                vLaytimeDomainPlot->addWidget(timeDomainPlot)          ;
                ui->wigTimeDomain->setLayout(vLaytimeDomainPlot)       ;

                // Make both embedded plot pages explicitly visible before any
                // optional board/profile configuration is executed.
                ui->wigFrqDomain->show();
                ui->wigTimeDomain->show();
                frqDomainPlot->show();
                timeDomainPlot->show();
                vLayfrqDomainPlot->activate();
                vLaytimeDomainPlot->activate();

                qInfo() << "Receiver: Time/Frequency plot widgets created and attached";

                //connections
                connect(oscMain->_adrv9009adv, &adrv9009_adv::saveSettingSignal,    this, [&]{defaultParameters();});
                connect(oscMain,               &OSCMain::frqDomainIsSelectedSignal, this, [&]
                {
                    //                    frqDomainPlot->holdPreFrq;
                    changingPlotMode(1);
                });


                connect(oscMain,               &OSCMain::seekIsSelectedSignal,      this, [&]{changingPlotMode(2);});




                //setting items
                //--- Global ---------------------------------------------
                ensmCmb = oscMain->_adrv9009->ensmCmb ;
                frqSpn = oscMain->_adrv9009->frqSpn   ;


                settingMinMaxBand();
                ui->hrzGlobal_1->addWidget(ensmCmb)   ;
                //                ui->hrzGlobal_2->addWidget(frqSpn)    ;

                ensm_mode_available = oscMain->_adrv9009->ensm_mode_available;
                QString status =ensm_mode_available->currentText();

                if(status != "radio_on" )
                {
                    reboutWithSSN();
                }

                //--- Hopping --------------------------------------------
                chk_trigger_mode   = oscMain->_adrv9009adv->chk_trigger_mode     ;
                chk_exit_mode      = oscMain->_adrv9009adv->chk_exit_mode        ;
                chk_msc_sync       = oscMain->_adrv9009adv->chk_msc_sync         ;
                chk_enable         = oscMain->_adrv9009adv->chk_enable           ;
                txt_init_frequency = oscMain->_adrv9009adv->txt_init_frequency   ;
                txt_gpio_pin       = oscMain->_adrv9009adv->txt_gpio_pin         ;
                txt_min_freq       = oscMain->_adrv9009adv->txt_min_freq         ;
                txt_max_freq       = oscMain->_adrv9009adv->txt_max_freq         ;
                save_settings      = oscMain->_adrv9009adv->save_settings        ;



                //setting
                ui->hrzHopp_34->addWidget(chk_trigger_mode     );
                ui->hrzHopp_34->addWidget(chk_exit_mode        );
                ui->hrzHopp_34->addWidget(chk_msc_sync         );
                ui->hrzHopp_34->addWidget(chk_enable           );
                ui->hrzHopp_34->addWidget(txt_init_frequency   );
                ui->hrzHopp_34->addWidget(txt_gpio_pin         );
                ui->hrzHopp_34->addWidget(txt_min_freq         );
                ui->hrzHopp_34->addWidget(txt_max_freq         );
                ui->hrzHopp_35->addWidget(save_settings        );
                ui->hrzInitFrq->addWidget(txt_init_frequency, 1);

                //config
                QGroupBox *grbHopp = ui->grbHopp_1;
                QGridLayout *gridHopp = grbHopp->findChild<QGridLayout *>();
                gridHopp->addWidget(txt_gpio_pin, 0, 1)                    ;
                gridHopp->addWidget(txt_min_freq, 1, 1)                    ;
                gridHopp->addWidget(txt_max_freq, 2, 1)                    ;


                // below code is hidden in screen
                nextFrqSpn = oscMain->_adrv9009->nextFrqSpn ;
                minSpn     = oscMain->_adrv9009->minSpn     ;
                maxSpn     = oscMain->_adrv9009->maxSpn     ;
                stepSpn    = oscMain->_adrv9009->stepSpn    ;
                delaySpn   = oscMain->_adrv9009->delaySpn   ;
                startBtn   = oscMain->_adrv9009->startBtn   ;
                modeChk    = oscMain->_adrv9009->modeChk    ;

                ui->hrzHopp_1->addWidget(modeChk)           ;
                ui->hrzHopp_2->addWidget(nextFrqSpn)        ;
                ui->hrzHopp_3->addWidget(minSpn)            ;
                ui->hrzHopp_4->addWidget(maxSpn)            ;
                ui->hrzHopp_5->addWidget(stepSpn)           ;
                ui->hrzHopp_6->addWidget(delaySpn)          ;
                ui->verHopp->addWidget(startBtn)            ;

                //--- spectrum chart -------------------------------------
                ui->hrz_fft_size->addWidget(frqDomainPlot->fftCmb)         ;
                ui->hrz_window->addWidget(frqDomainPlot->windowCmb)        ;
                ui->hrz_avg->addWidget(frqDomainPlot->avgSpn)              ;
                ui->hrz_pwr->addWidget(frqDomainPlot->pwrSpn)              ;
                ui->hrz_start_frq->addWidget(frqDomainPlot->txt_start_freq);
                ui->hrz_frq_stop->addWidget(frqDomainPlot->txt_stop_freq ) ;
                ui->hrz_frq_step->addWidget(frqDomainPlot->txt_freq_step ) ;
                ui->hrz_delay->addWidget(frqDomainPlot->txt_freq_delay)    ;

                //--- TH1 ------------------------------------------------
                ui->hrz_1->addWidget(oscMain->_adrv9009controlUnit->TH1) ;
                ui->hrz_2->addWidget(oscMain->_adrv9009controlUnit->TH2) ;
                //--- TX1 ------------------------------------------------
                pinn_TX1_Chk      = oscMain->_adrv9009->pinn_TX1_Chk     ;
                track_TX1_Chk     = oscMain->_adrv9009->track_TX1_Chk    ;
                power_TX1_DownChk = oscMain->_adrv9009->power_TX1_DownChk;
                lo_TX1_Chk        = oscMain->_adrv9009->lo_TX1_Chk       ;
                att_TX1_Spn       = oscMain->_adrv9009->att_TX1_Spn      ;
                rfBandlbl         = oscMain->_adrv9009->rfBandlbl        ;
                sampleRatelbl     = oscMain->_adrv9009->sampleRatelbl    ;

                QGroupBox   * grbTX1  = ui->grbTX_1;
                QGridLayout * gridTX1 = grbTX1->findChild<QGridLayout *>();

                gridTX1->addWidget(att_TX1_Spn        , 0 , 1)         ;
                gridTX1->addWidget(pinn_TX1_Chk       , 1 , 1)         ;
                gridTX1->addWidget(track_TX1_Chk      , 2 , 1)         ;
                gridTX1->addWidget(lo_TX1_Chk         , 3 , 1)         ;
                gridTX1->addWidget(power_TX1_DownChk  , 4 , 1)         ;
                gridTX1->addWidget(rfBandlbl          , 5 , 1)         ;
                gridTX1->addWidget(sampleRatelbl      , 6 , 1)         ;

                //--- TX2 ------------------------------------------------
                pinn_TX2_Chk    = oscMain->_adrv9009->pinn_TX2_Chk     ;
                track_TX2_Chk   = oscMain->_adrv9009->track_TX2_Chk    ;
                powerTX2DownChk = oscMain->_adrv9009->powerTX2DownChk  ;
                lo_TX2_Chk      = oscMain->_adrv9009->lo_TX2_Chk       ;
                att_TX2_Spn     = oscMain->_adrv9009->att_TX2_Spn      ;

                QGroupBox *grbTX2 = ui->grbTX_2;
                QGridLayout *gridTX2 = grbTX2->findChild<QGridLayout *>();

                gridTX2->addWidget(att_TX2_Spn        , 0 , 1)           ;
                gridTX2->addWidget(pinn_TX2_Chk       , 1 , 1)           ;
                gridTX2->addWidget(track_TX2_Chk      , 2 , 1)           ;
                gridTX2->addWidget(lo_TX2_Chk         , 3 , 1)           ;
                gridTX2->addWidget(powerTX2DownChk    , 4 , 1)           ;

                //--- OBSRX ----------------------------------------------
                power_OBSRX_Spn   = oscMain->_adrv9009->power_OBSRX_Spn  ;
                track_OBSRX_Chk   = oscMain->_adrv9009->track_OBSRX_Chk  ;
                hardwareGain      = oscMain->_adrv9009->hardwareGain     ;
                rfBandOBSlbl      = oscMain->_adrv9009->rfBandOBSlbl;
                sampleRateOBSlbl  = oscMain->_adrv9009->sampleRateOBSlbl ;

                QGroupBox *grbRX1 = ui->grbRX1;
                QGridLayout *gridRX2 = grbRX1->findChild<QGridLayout *>();

                gridRX2->addWidget(hardwareGain     , 0 , 1)             ;
                gridRX2->addWidget(track_OBSRX_Chk  , 1 , 1)             ;
                gridRX2->addWidget(power_OBSRX_Spn  , 2 , 1)             ;
                gridRX2->addWidget(rfBandOBSlbl     , 3 , 1)             ;
                gridRX2->addWidget(sampleRateOBSlbl , 4 , 1)             ;

                //--- THCW -----------------------------------------------
                QGroupBox *grbTHCW = ui->grbTHCW;
                QGridLayout *gridTHCW = grbTHCW->findChild<QGridLayout *>();

                THCW1 = oscMain->_adrv9009controlUnit->THCW1               ;
                THCW2 = oscMain->_adrv9009controlUnit->THCW2               ;
                THCW3 = oscMain->_adrv9009controlUnit->THCW3               ;
                gridTHCW->addWidget(THCW1  , 0 , 1)                        ;
                gridTHCW->addWidget(THCW2  , 1 , 1)                        ;
                gridTHCW->addWidget(THCW3  , 2 , 1)                        ;
                //                gridTHCW->addWidget(chk_trigger_mode  , 3 , 1)             ;

                //======================== Default Configuration ==================================================
                isConnect = true;
                receiverIsConnected = true;

                /*
                 * Finish construction first so MainWindow and ReceiverMain can
                 * become visible immediately after the IP dialog. Channel setup
                 * and board defaults are then performed by the Qt event loop.
                 */
                QTimer::singleShot(0, this, [this](){
                    if(frqDomainPlot)
                        frqDomainPlot->onItemClicked();
                    if(timeDomainPlot)
                        timeDomainPlot->onItemClicked();

                    if(frqDomainPlot) frqDomainPlot->show();
                    if(timeDomainPlot) timeDomainPlot->show();
                    ui->wigFrqDomain->show();
                    ui->wigTimeDomain->show();

                    qInfo() << "Receiver: plot channel setup completed";
                });

                QTimer::singleShot(0, this, [this](){
                    if(receiverIsConnected)
                        defaultSettings();
                });
            }
        }
        else
        {
            // TODO: Show Plot window

        }
    }
    else
    {
        //        emit receiverIsConnected(false);
        qInfo()<<Q_FUNC_INFO<<"else";
    }
}

ReceiverMain::~ReceiverMain()
{

    power_TX1_DownChk->setChecked(true);

    delete[] rangeXChart;
    delete[] rangeYChart;
    delete timeDomainPlot;
    delete frqDomainPlot;
    delete smartTimeDomainPlot;
    delete oscMain;
    delete t1;
    delete t2;
    delete t3;
    delete t4;
    delete t5;
    delete t6;
    delete t7;
    delete t8;

    delete ui;


}

//this function is responsible for setting some configuration on the board in order to get correct value
void ReceiverMain::defaultSettings()
{
    if (!receiverIsConnected || !oscMain || !oscMain->_adrv9009 ||
        !frqDomainPlot || !rfBandlbl)
    {
        qWarning() << "Receiver: default settings skipped because startup objects are not ready";
        return;
    }

    // The original project contains an absolute /home/seraj3/... profile path.
    // Do not start profile loading with a non-existent file: that path used to
    // enter an asynchronous UI update path and could abort the Qt application.
    if (rfBandlbl->text().split(" ").value(0).toDouble() != 400)
    {
        if (QFileInfo::exists(fileAddress))
        {
            frqDomainPlot->setEnabled(false);
            oscMain->_adrv9009->on_profile_config_clicked(fileAddress);

            QTimer::singleShot(10000, this, [this]{
                if (!frqDomainPlot)
                    return;
                frqDomainPlot->setEnabled(true);
                defaultParameters();
            });
        }
        else
        {
            qWarning() << "Receiver: startup ADRV9009 profile does not exist; skipping profile load:"
                       << fileAddress;
            defaultParameters();
        }
    }
    else
    {
        defaultParameters();
    }
}



void ReceiverMain::defaultParameters()
{

    //rx and power
    oscMain->_adrv9009->rx1Powerdown->setChecked(true);
    oscMain->_adrv9009->rx2Powerdown->setChecked(true);
    oscMain->_adrv9009->obs2Powerdown->setChecked(true);//OBS RX
    oscMain->_adrv9009->power_OBSRX_Spn->setChecked(false);//OBS RX

    oscMain->_adrv9009->powerTX2DownChk->setChecked(true);
    oscMain->_adrv9009->power_TX1_DownChk->setChecked(true);
    //    oscMain->_adrv9009->power_OBSRX_Spn->setChecked(false);


    //Control unit adrv
    THCW1->setValue(8000000);
    THCW2->setValue(30);
    THCW3->setValue(25);

    oscMain->_adrv9009controlUnit->TH1->setValue(30);
    oscMain->_adrv9009controlUnit->TH2->setValue(40);


    //seeking parameters
    frqDomainPlot->txt_start_freq->setValue(1700);
    frqDomainPlot->txt_stop_freq->setValue(5700);
    frqDomainPlot->txt_freq_step->setValue(492.5);
    //    frqDomainPlot->txt_freq_delay->setValue(5);


    //Hopping
    //in order to run with these values
    chk_enable->setChecked(true);
    chk_exit_mode->setChecked(true);

    chk_msc_sync->setChecked(true);

    txt_gpio_pin->setValue(0);
    txt_min_freq->setValue(1700);
    txt_max_freq->setValue(5700);
    timeDomainPlot->samplTime->setValue(40000);


    //        oscMain->_adrv9009adv->chk_trigger_mode->setChecked(true);
    //    chk_trigger_mode->setChecked(true);
    //    QTimer::singleShot(1000,[&]{
    //        chk_trigger_mode->setChecked(false);
    //    });
    //    QTimer::singleShot(1000,[&]{
    //        chk_trigger_mode->setChecked(true);
    //    });

    //    ui->tabWidgetSetting->removeTab(1);
    //    ui->tabWidgetSetting->removeTab(1);
    //    ui->tabHopping->setVisible(true);
    //    ui->tabSpectrum->setVisible(false);


    //    QTimer::singleShot(10,[&]{chk_trigger_mode->setChecked(false);});
    //    //    QTimer::singleShot(1000,[&]{chk_trigger_mode->setChecked(false);});


    QTimer::singleShot(500, this, [this]{ if(oscMain && oscMain->_adrv9009controlUnit && oscMain->_adrv9009controlUnit->btnThCw1) oscMain->_adrv9009controlUnit->btnThCw1->click(); });
    QTimer::singleShot(500, this, [this]{ if(oscMain && oscMain->_adrv9009controlUnit && oscMain->_adrv9009controlUnit->btnThCw2) oscMain->_adrv9009controlUnit->btnThCw2->click(); });
    QTimer::singleShot(500, this, [this]{ if(oscMain && oscMain->_adrv9009controlUnit && oscMain->_adrv9009controlUnit->btnThCw3) oscMain->_adrv9009controlUnit->btnThCw3->click(); });
    QTimer::singleShot(500, this, [this]{ if(oscMain && oscMain->_adrv9009controlUnit && oscMain->_adrv9009controlUnit->btnWrite) oscMain->_adrv9009controlUnit->btnWrite->click(); });
    QTimer::singleShot(500, this, [this]{ if(oscMain && oscMain->_adrv9009controlUnit && oscMain->_adrv9009controlUnit->btnTh1) oscMain->_adrv9009controlUnit->btnTh1->click(); });
    QTimer::singleShot(500, this, [this]{ if(oscMain && oscMain->_adrv9009controlUnit && oscMain->_adrv9009controlUnit->btnTh2) oscMain->_adrv9009controlUnit->btnTh2->click(); });

    // Preserve the original automatic spectrum startup, but bind the delayed
    // callback to ReceiverMain so it cannot execute after this object is gone.
    QTimer::singleShot(100, this, [this]{
        if (receiverIsConnected && frqDomainPlot && timeDomainPlot)
            changingPlotMode(1);
    });

    //turn on capture btn for frq and time domain
    //    QTimer::singleShot(500,[&]{frqDomainPlot->on_btn_capture_clicked();});
    //    QTimer::singleShot(500,[&]{timeDomainPlot->on_btn_capture_clicked();});

    //    frqDomainPlot->btn_plot_frq->clicked();

    //    isFrqBtnOn = true;
}

void ReceiverMain::changingPlotMode(int selectedMode)
{
    if (selectedMode > 3 || selectedMode < 0)
        return;
    if (!receiverIsConnected || !frqDomainPlot || !timeDomainPlot)
    {
        qWarning() << "Receiver: plot-mode change ignored because plots are not ready";
        return;
    }

    //first we should turn off time domain
    if(timeDomainPlot->isCaptureOn)
    {
        timeDomainPlot->btn_capt->clicked();
    }

    //second we should turn off frq domain
    if(frqDomainPlot->isCaptureOn)
    {
        frqDomainPlot->btnfftcapt->clicked();
    }

    //changing current index of frq domain combobox and calling onItemClicked function in order to set chanel

    if (selectedMode == 1)
    {
        if (frqChanged) frqDomainPlot->frqValueIsChanged(true);
        QTimer::singleShot(100, this, [this]{ if(frqDomainPlot && frqDomainPlot->cmb_plot_type) frqDomainPlot->cmb_plot_type->setCurrentIndex(1); });
        QTimer::singleShot(200, this, [this]{ if(frqDomainPlot && frqDomainPlot->cmb_plot_type) frqDomainPlot->cmb_plot_type->currentIndexChanged(1); });
    }

    if (selectedMode == 2)
    {
        frqChanged = true;
        frqHold = ui->spnFrq->value();

        if (chk_trigger_mode)
        {
            chk_trigger_mode->setChecked(true);
        }

        QTimer::singleShot(100, this, [this]{ if(frqDomainPlot && frqDomainPlot->cmb_plot_type) frqDomainPlot->cmb_plot_type->setCurrentIndex(2); });
        QTimer::singleShot(200, this, [this]{ if(frqDomainPlot && frqDomainPlot->cmb_plot_type) frqDomainPlot->cmb_plot_type->currentIndexChanged(2); });
    }

    //turn on btn capture again for time and frq domain for plotting
    if (selectedMode == 1)
    {
        QTimer::singleShot(500, this, [this]{ if(frqDomainPlot) frqDomainPlot->onItemClicked(); }); //changing channels and set them
        QTimer::singleShot(1000, this, [this]{ if(frqDomainPlot && frqDomainPlot->btnfftcapt) frqDomainPlot->btnfftcapt->click(); }); // start frequency capture
        QTimer::singleShot(2000, this, [this]{ if(timeDomainPlot && timeDomainPlot->btn_capt) timeDomainPlot->btn_capt->click(); });


    }

    else if (selectedMode == 2)
    {
        QTimer::singleShot(500, this, [this]{ if(frqDomainPlot) frqDomainPlot->onItemClicked(); });
        QTimer::singleShot(800, this, [this]{ if(frqDomainPlot && frqDomainPlot->btnfftcapt) frqDomainPlot->btnfftcapt->click(); });
        QTimer::singleShot(1000, this, [this]{ if(timeDomainPlot && timeDomainPlot->btn_capt) timeDomainPlot->btn_capt->click(); });
    }
}


void ReceiverMain::frqAndSeekStopCapturing(bool val)
{
    isExciterOn = val;

    if(val)
    {
        holdFrq = frqDomainPlot->frqValueStr.toDouble();


        if(timeDomainPlot->isCaptureOn) timeDomainPlot->btn_capt->clicked();

        if(frqDomainPlot->isCaptureOn) frqDomainPlot->btnfftcapt->clicked();

        frqDomainPlot->isExciterOn= false;


        frqDomainPlot->btn_plot_frq->setEnabled(false);
        frqDomainPlot->btn_plot_seek->setEnabled(false);
        frqDomainPlot->btn_plot_seek->clicked();
        //        frqDomainPlot->exciterModeOn();
        emit seekingStatusSignal("sweep is started successfully. All received is off.");
    }


    else
    {

        //        if(!timeDomainPlot->isCaptureOn) timeDomainPlot->btn_capt->clicked();

        //        if(!frqDomainPlot->isCaptureOn) frqDomainPlot->btnfftcapt->clicked();

        frqDomainPlot->isExciterOn= true;
        frqDomainPlot->isCaptureOn=false;

        frqDomainPlot->txt_start_freq->setValue(1700);
        frqDomainPlot->txt_stop_freq->setValue(5700);
        frqDomainPlot->txt_freq_step->setValue(500);
        frqDomainPlot->txt_freq_delay->setValue(500);

        chk_enable->setChecked(true);
        chk_exit_mode->setChecked(true);
        //        chk_trigger_mode->setChecked(true);
        chk_msc_sync->setChecked(true);
        frqDomainPlot->isFrqClicked = false;
        frqDomainPlot->isSeekClicked = true;
        frqDomainPlot->emit HoppingSignal(false);



        QTimer::singleShot(1000,[&]
        {
            frqDomainPlot->btn_plot_frq->clicked();
            emit seekingStatusSignal("sweep is turned off.");
            frqDomainPlot->btn_plot_frq->setEnabled(true);
            frqDomainPlot->btn_plot_seek->setEnabled(true);
        });

        QTimer::singleShot(4000,[&]{

            ui->spnFrq->setValue(holdFrq);

            //            on_btnSet_clicked();
        });
    }
}

//================================================================================

void ReceiverMain::on_btnSet_clicked()
{
    //    if(ui->spnFrq->value() < 8000 or ui->spnFrq->value() > 12000) return;

    double enteredFrq = abs(DC_6_UPTO_8_12 - ui->spnFrq->value());



    if (enteredFrq >= minFrqLimit and  enteredFrq <= maxFrqLimit ) return;

    frqSpn->setValue(enteredFrq);

    //    frqDomainPlot->btnfftcapt->clicked();
    //    timeDomainPlot->btn_capt->clicked();

    QTimer::singleShot(500, [&]{timeDomainPlot->btn_capt->clicked();  });
    QTimer::singleShot(1000, [&]{frqDomainPlot->btnfftcapt->clicked(); });


    QTimer::singleShot(500, [&]{timeDomainPlot->btn_capt->clicked();  });
    QTimer::singleShot(1000, [&]{frqDomainPlot->btnfftcapt->clicked(); });

    //    oscMain->_adrv9009->tx_sample_rate_changed(oscMain->_adrv9009->frqSpn->value());
    settingMinMaxBand();
}

double ReceiverMain::maximumFinder(double * newVal,double * max)
{

    if(*newVal > *max)
    {
        max = newVal;
    }
    return *max;
}

double ReceiverMain::minimumFinder(double * newVal,double * min)
{
    if(*newVal < *min)
    {
        min = newVal;
    }

    return *min;
}


void ReceiverMain::on_btnSetBandSelect_clicked()
{
    //    ui->chbSmartNoise->setChecked(false);

    double fs1 = ui->spnMin->value(); //?
    double fs2 = ui->spnMax->value();//?

    bool   instop{};
    double fc{centerFrq}; //frq
    double fs1ch{};
    double fs2ch{};
    double inchn{};
    int p{};
    double tmp{};
    //        fc TODO= receive frq value
    if (ui->radBandSelect->isChecked())
    {
        instop = false;
    }

    else
    {
        instop = true;
    }

    double bw = 122.88*4;
    double chbw = bw / 32;
    double fs2n = (fs2-fc);
    double fs1n = (fs1-fc);

    if(fs1>fc)
    {
        fs1ch= floor(fs1n/chbw);
    }

    else
    {
        fs1ch= ceil(fs1n/chbw);
    }


    if(fs1ch<0)
    {
        fs1ch = fs1ch +32;
    }

    if(fs2>fc)
    {
        fs2ch= floor(fs2n/chbw);
    }
    else
    {
        fs2ch= ceil(fs2n/chbw);
    }


    if(fs2ch<0){
        fs2ch = fs2ch +32;
    }


    if(((fs1ch <16) && (fs2ch >15)) || ((fs2ch <16) && (fs1ch >15)))
    {
        instop = !instop;
    }

    if(instop)
    {
        inchn=4294967295;
        p=-1;
    }

    else
    {
        inchn=0;
        p=1;
    }

    if(fs1ch>fs2ch && fs1ch!=0)
    {
        tmp=fs2ch;
        fs2ch=fs1ch;
        fs1ch=tmp;
    }


    if(fs2ch>15 && fs1ch==0)
    {
        tmp=fs2ch;
        fs2ch=31;
        fs1ch=tmp;
        inchn=inchn+p* pow(2,0) ;
    }

    for (int i=int(fs1ch) ; i<= fs2ch ; i++)
    {
        inchn=inchn+p* pow(2,i) ;
    }

    ui->lblTest->setNum(inchn);
    oscMain->_adrv9009controlUnit->setInchannelSignal(inchn);

}

void ReceiverMain::frqValueChenged(QString val)
{
    //    Q_UNUSED(val);
    frqSpn->setValue(val.toDouble());
    int frqVal= frqSpn->value();
    //    if(frqVal > 1600 && frqVal <6000 )
    //    {
    //        frqSpn->clearFocus();
    //        frqDomainPlot->btnfftcapt->click();
    //        timeDomainPlot->btn_capt->click();
    ////        QTimer::singleShot(1000, [&]{timeDomainPlot->btn_capt->clicked();  });
    ////        QTimer::singleShot(2000, [&]{frqDomainPlot->btnfftcapt->clicked(); });
    //    }
}

void ReceiverMain::sendSpotFileToCart(QString path)
{


}

void ReceiverMain::getPlotDataFromThread()
{
    //    double key = timeStart.msecsTo(QTime::currentTime())/1000.0;
    //    customPlot->graph(0)->addData(key, xAx);
    //    customPlot->graph(0)->rescaleValueAxis();
    //    customPlot->xAxis->setRange(key, 20, Qt::AlignRight);
    //    customPlot->replot();
}

void ReceiverMain::histoMainPlotSlot(double key, double val)
{
    //    customPlot->graph(0)->addData(key, val);
    //    customPlot->graph(0)->rescaleValueAxis();
    //    customPlot->xAxis->setRange(key, 20, Qt::AlignRight);
    //    //    customPlot->yAxis->setRange(yAx-1,yAx+1);
    //    customPlot->replot();
}

//if exciter is ran, smart noise must be off
void ReceiverMain::stopSmartNoiseSlot()
{
    ui->chbSmartNoise->stateChanged(0);

    ui->lblActiveNoise->setText("Exciter is on\nThere is no active smart noise. ");
    power_TX1_DownChk->setChecked(false);
}


//if exciter is ran, smart noise must be off
void ReceiverMain::stopExciterSlot()
{
    ui->chbSmartNoise->stateChanged(0);

    ui->lblActiveNoise->setText("There is no active smart noise.");
    power_TX1_DownChk->setChecked(false);
}

void ReceiverMain::calculatingCwTableSlot(QVector<double> frqVal,
                                               QVector<double> powerVal,
                                               QVector<QString> mode,
                                               QVector<long long int> frqTime)
{
    if (!timeDomainPlot || !ui)
        return;

    const int inputCount = qMin(qMin(frqVal.size(), powerVal.size()),
                                qMin(mode.size(), frqTime.size()));
    if (inputCount <= 0)
        return;

    if (frqVal.size() != inputCount || powerVal.size() != inputCount ||
        mode.size() != inputCount || frqTime.size() != inputCount)
    {
        qWarning() << "CW input vectors have different sizes; using"
                   << inputCount << "records from"
                   << frqVal.size() << powerVal.size()
                   << mode.size() << frqTime.size();
    }

    // Keep persistent table vectors synchronized before any indexed access.
    int savedCount = qMin(qMin(frqTimeVec.size(), frqValVec.size()),
                          qMin(powerValVec.size(), modValVec.size()));
    if (frqTimeVec.size() != savedCount || frqValVec.size() != savedCount ||
        powerValVec.size() != savedCount || modValVec.size() != savedCount)
    {
        qWarning() << "CW saved vectors have different sizes; normalizing to"
                   << savedCount;
        frqTimeVec.resize(savedCount);
        frqValVec.resize(savedCount);
        powerValVec.resize(savedCount);
        modValVec.resize(savedCount);
    }

    if (savedCount == 0)
        firstTime = true;

    const double deltaTime = timeDomainPlot->deltaValue
            ? timeDomainPlot->deltaValue->value() : 0.0;

    int valueIndex = -1;
    bool isFrqEqual = false;
    QVector<int> holdIndex(inputCount, -1);

    if (!firstTime && savedCount > 0)
    {
        for (int i = 0; i < inputCount; ++i)
        {
            if (frqVal.at(i) == 0)
                continue;

            isFrqEqual = false;
            valueIndex = -1;

            for (int j = 0; j < frqValVec.size(); ++j)
            {
                const double frqDiff = qAbs(int(frqValVec.at(j)) - int(frqVal.at(i)));

                if (frqDiff <= deltaTime)
                {
                    const long long diff = frqTime.at(i) - frqTimeVec.at(j);
                    if (diff < 70 && mode.at(i).isEmpty())
                        ++counter;
                    else if (mode.at(i).isEmpty())
                        counter = 0;

                    valueIndex = j;
                    frqValVec[j] = frqVal.at(i);
                    powerValVec[j] = powerVal.at(i);
                    frqTimeVec[j] = frqTime.at(i);
                    isFrqEqual = true;
                    break;
                }

                if (j == frqValVec.size() - 1)
                    holdIndex[i] = i;
            }

            if (isFrqEqual && valueIndex >= 0 && valueIndex < modValVec.size())
            {
                modValVec[valueIndex] = mode.at(i).isEmpty()
                        ? (counter < 50 ? QStringLiteral("CW") : QStringLiteral("Pulse"))
                        : mode.at(i);
            }
            counter = 0;
        }
    }
    else
    {
        for (int i = 0; i < inputCount; ++i)
        {
            if (frqVal.at(i) == 0)
                continue;
            frqTimeVec.append(QDateTime::currentMSecsSinceEpoch());
            frqValVec.append(frqVal.at(i));
            powerValVec.append(powerVal.at(i));
            modValVec.append(mode.at(i));
        }
        firstTime = false;
    }

    for (int i = 0; i < holdIndex.size(); ++i)
    {
        if (holdIndex.at(i) == -1)
            continue;
        frqTimeVec.append(QDateTime::currentMSecsSinceEpoch());
        frqValVec.append(frqVal.at(i));
        powerValVec.append(powerVal.at(i));
        modValVec.append(mode.at(i));
    }

    const int tableCount = qMin(qMin(frqTimeVec.size(), frqValVec.size()),
                                qMin(powerValVec.size(), modValVec.size()));
    frqTimeVec.resize(tableCount);
    frqValVec.resize(tableCount);
    powerValVec.resize(tableCount);
    modValVec.resize(tableCount);

    ui->tblCW->clearContents();
    ui->tblCW->setRowCount(tableCount);

    timeDomainPlot->cwFrqValue = &frqValVec;
    timeDomainPlot->cwPowerValue = &powerValVec;
    timeDomainPlot->cwModeValue = &modValVec;

    const int calibCount = qMin(sgFrqInCalibFile.size(),
                                qMin(sgPowerInCalibFile.size(), cardPowerInCalibFile.size()));

    int k = 0;
    int rowCnt = 0;
    int count = 0;

    for (int i = 0; i < tableCount; ++i)
    {
        QTableWidgetItem *t1 = new QTableWidgetItem;
        QTableWidgetItem *t2 = new QTableWidgetItem;
        QTableWidgetItem *t3 = new QTableWidgetItem;

        t1->setText(modValVec.at(i));

        // Calibration is optional.  The old code indexed element 0 even when
        // the calibration file was absent, which causes Qt to abort.
        if (calibCount > 0)
        {
            int nearest = 0;
            double minDiff = std::numeric_limits<double>::max();
            for (int j = 0; j < calibCount; ++j)
            {
                const double diff = qAbs(sgFrqInCalibFile.at(j) - frqValVec.at(i));
                if (diff < minDiff)
                {
                    minDiff = diff;
                    nearest = j;
                }
            }

            const double powDiff = qAbs(cardPowerInCalibFile.at(nearest) +
                                        sgPowerInCalibFile.at(nearest));
            powerValVec[i] -= powDiff;
        }

        t2->setText(QString::number(frqValVec.at(i) + .02));
        t3->setText(QString::number(powerValVec.at(i)));

        ui->tblCW->setItem(rowCnt, k, t1);
        ui->tblCW->setItem(rowCnt, k + 1, t2);
        ui->tblCW->setItem(rowCnt, k + 2, t3);

        if (count % 2 != 0)
        {
            ++rowCnt;
            k = 0;
        }
        else
        {
            k += 4;
        }
        ++count;
    }

    num = 0;
}



void ReceiverMain::frqModeIsSelected()
{
    changingPlotMode(1);
    //    if(timeDomainPlot->isCaptureOn)
    //    {
    //        QTimer::singleShot(100, [&]{timeDomainPlot->btn_capt->click();});
    //    }

    //    if(frqDomainPlot->isCaptureOn)
    //    {
    //        QTimer::singleShot(100, [&]{frqDomainPlot->btnfftcapt->click();});
    //    }

    //    //changing current index of frq domain combobox and calling onItemClicked function in order to set chanel
    //    QTimer::singleShot(100, [&]{frqDomainPlot->cmb_plot_type->setCurrentIndex(1);});
    //    QTimer::singleShot(100, [&]{frqDomainPlot->cmb_plot_type->currentIndexChanged(1);});
    //    QTimer::singleShot(100, [&]{frqDomainPlot->onItemClicked();});
    //    QTimer::singleShot(100, [&]{timeDomainPlot->onItemClicked();});

    //    //invoking btn capture again for time and frq domain for plotting
    //    QTimer::singleShot(400, [&]{frqDomainPlot->btnfftcapt->click();});
    //    QTimer::singleShot(800, [&]{timeDomainPlot->btn_capt->click();});
}

void ReceiverMain::on_btnFrqSeek_clicked()
{
    changingPlotMode(2);

    //    if(timeDomainPlot->isCaptureOn)
    //    {
    //        QTimer::singleShot(100, [&]{timeDomainPlot->btn_capt->click();});
    //    }

    //    if(frqDomainPlot->isCaptureOn)
    //    {
    //        QTimer::singleShot(100, [&]{frqDomainPlot->btnfftcapt->click();});
    //    }

    //    //changing current index of frq domain combobox and calling onItemClicked function in order to set chanel
    //    QTimer::singleShot(200, [&]{frqDomainPlot->cmb_plot_type->setCurrentIndex(2);});
    //    QTimer::singleShot(200, [&]{frqDomainPlot->cmb_plot_type->currentIndexChanged(2);});
    //    QTimer::singleShot(200, [&]{frqDomainPlot->onItemClicked();});

    //    //invoking btn capture again for time and frq domain for plotting
    //    QTimer::singleShot(400, [&]{frqDomainPlot->btnfftcapt->click();});
    //    QTimer::singleShot(800, [&]{timeDomainPlot->btn_capt->click();});
}

void ReceiverMain::settingMinMaxBand()
{
    centerFrq =  abs(DC_6_UPTO_8_12 - frqSpn->value());
    ui->spnMax->setValue(centerFrq+250);
    ui->spnMin->setValue(centerFrq -250);
}

void ReceiverMain::reboutWithSSN()
{
    QProcess prc;
    QString perm{"sudo"};
    QStringList arg;
    arg << "ssh" << "192.168.1.10";

    prc.start();
}

void ReceiverMain::initPlot()
{

    //    QCustomPlot * plotWidgets[] = {ui->wigPRI, ui->wigPW1, ui->wigPW2, ui->wigTOA, ui->wigPASUM, ui->wigPAPEAK, ui->wigCHANNEL,ui->wigPWhisto,ui->wigFRQhisto,ui->wigPRIhisto,ui->wigPASumhisto};

    //    QStringList titleName = {"FRQ","PRI","PW1","PW2","TOA","PASum","PAPeak","Channel",};

    //    for(size_t i{}; i < (sizeof (plotWidgets) / sizeof (*plotWidgets)); i ++)
    //    {
    //        plotWidgets[i]->xAxis->setBasePen(QPen(QColor(255,255,255)));
    //        plotWidgets[i]->yAxis->setBasePen(QPen(QColor(255,255,255)));
    //        plotWidgets[i]->setBackground(QBrush(Qt::black));
    //        plotWidgets[i]->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    //        plotWidgets[i]->xAxis->setTickLabelColor(QColor(255,255,255));
    //        plotWidgets[i]->yAxis->setTickLabelColor(QColor(255,255,255));
    //        plotWidgets[i]->yAxis->setSelectedLabelColor(QColor(255,255,255));
    //        plotWidgets[i]->yAxis->setSelectedTickLabelColor(QColor(255,255,255));
    //        plotWidgets[i]->setSelectionRectMode(QCP::srmSelect);
    //        plotWidgets[i]->setSelectionTolerance(100);
    //        plotWidgets[i]->addGraph();
    //        plotWidgets[i]->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // first graph will be filled with translucent blue
    //        plotWidgets[i]->addGraph();

    //        QPen pen;
    //        pen.setWidth(3);
    //        pen.setColor(Qt::yellow);
    //        plotWidgets[i]->graph(0)->setPen(pen);

    //        QCPGrid *xGrid = plotWidgets[i]->xAxis->grid();
    //        QPen xGridPen = xGrid->pen();
    //        xGridPen.setStyle(Qt::SolidLine);
    //        xGridPen.setColor(QColor( 0, 126, 78 ));
    //        xGrid->setPen(xGridPen);

    //        QCPGrid *yGrid = plotWidgets[i]->yAxis->grid();
    //        QPen yGridPen = yGrid->pen();
    //        yGridPen.setStyle(Qt::SolidLine);
    //        yGridPen.setColor(QColor( 0, 126, 78 ));
    //        yGrid->setPen(yGridPen);


    //        //        plotWidgets[i]->graph(1)->setPen(QPen(Qt::red)); // line color red for second graph
    //        //        connect(plotWidgets[i]->xAxis, SIGNAL(rangeChanged(QCPRange)), plotWidgets[i]->xAxis2, SLOT(setRange(QCPRange)));
    //        //        connect(plotWidgets[i]->yAxis, SIGNAL(rangeChanged(QCPRange)), plotWidgets[i]->yAxis2, SLOT(setRange(QCPRange)));
    //        //        plotWidgets[i]->xAxis->grid()->setVisible(true);
    //        //        plotWidgets[i]->yAxis->grid()->setVisible(true);
    //    }





    //    //    for(size_t i{}; i < 8; i ++)
    //    //    {
    //    //        plotWidgets[i]->addGraph();
    //    //        plotWidgets[i]->graph(0)->setPen(QPen(QColor(40, 110, 0)));
    //    //        plotWidgets[i]->graph(0)->setLineStyle((QCPGraph::LineStyle)0);
    //    //        plotWidgets[i]->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 3));
    //    //        //    customPlot->addGraph(); // red line
    //    //        //    customPlot->graph(1)->setPen(QPen(QColor(255, 110, 40)));

    //    //        QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    //    //        timeTicker->setTimeFormat("%h:%m:%s");
    //    //        plotWidgets[i]->xAxis->setTicker(timeTicker);
    //    //        plotWidgets[i]->axisRect()->setupFullAxesBox();
    //    //        plotWidgets[i]->yAxis->setRange(-1,1);

    //    //        connect(plotWidgets[i]->xAxis, SIGNAL(rangeChanged(QCPRange)), plotWidgets[i]->xAxis2, SLOT(setRange(QCPRange)));
    //    //        connect(plotWidgets[i]->yAxis, SIGNAL(rangeChanged(QCPRange)), plotWidgets[i]->yAxis2, SLOT(setRange(QCPRange)));

    //    //    }
}




void ReceiverMain::on_btnSetTH_clicked()
{
    oscMain->_adrv9009controlUnit->btnTh1->clicked();
    sleep(1);
    oscMain->_adrv9009controlUnit->btnTh2->clicked();

}

void ReceiverMain::on_btnSetTHCW_clicked()
{
    oscMain->_adrv9009controlUnit->btnThCw1->clicked();
    sleep(1);
    oscMain->_adrv9009controlUnit->btnThCw2->clicked();
    sleep(1);
    oscMain->_adrv9009controlUnit->btnThCw3->clicked();
}

void ReceiverMain::on_tabWidget_currentChanged(int index)
{
    currentTabState = index;
}







void ReceiverMain::on_label_43_linkActivated(const QString &link)
{

}


void ReceiverMain::on_cmbTransmit_activated(int index)
{
    int i = index;

}

























































































































































//#include "plotfunction.h"

//plotfunction::plotfunction(QObject *parent, QCustomPlot*plt, QCustomPlot* histoplt,QString name) : QThread(parent)
//{
//    plotName = name;
//    customPlot = plt;
//    histoCustomPlot = histoplt;
//    //    start(HighestPriority);
//    //    customPlot->moveToThread(this);
//    //    moveToThread(this);
//    //    ValueSlot();

//    customPlot->addGraph(); // blue line
//    customPlot->graph(0)->setPen(QPen(QColor(40, 110, 0)));
//    //    customPlot->addGraph(); // red line
//    //    customPlot->graph(1)->setPen(QPen(QColor(255, 110, 40)));

//    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
//    timeTicker->setTimeFormat("%h:%m:%s");
//    customPlot->xAxis->setTicker(timeTicker);
//    customPlot->axisRect()->setupFullAxesBox();
//    customPlot->yAxis->setRange(-1,1);

//    // make left and bottom axes transfer their ranges to right and top axes:
//    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
//    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

//    QTimer timerCleaHistoVector;

//    connect(&timerCleaHistoVector,&QTimer::timeout,this,[&]{

//        tmpMap.clear();
//        totalNumber = 0;
//        xAxisVal.clear();
//        yAxisVal.clear();
//    });


//    timerCleaHistoVector.start(500);
//}

//plotfunction::~plotfunction()
//{
//    //    if(isRunning())
//    //    {
//    //        QThread::exit(0);
//    //        QThread::wait();
//    //    }
//}




//void plotfunction::thisIsTest(double xAx , double yAx)
//{
//    //    double key = timeStart.msecsTo(QTime::currentTime())/1000.0;
//    //    customPlot->graph(0)->addData(key, xAx);
//    //    customPlot->graph(0)->rescaleValueAxis();
//    //    customPlot->graph(0)->setLineStyle((QCPGraph::LineStyle)0);
//    //    customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 3));
//    //    customPlot->xAxis->setRange(key, 20, Qt::AlignRight);
//    //    customPlot->replot();

//    if(! (  plotName == "frq"
//            || plotName == "pw-one"
//            || plotName == "pri"
//            || plotName == "psum"))
//    {
//        return;
//    }


//    if(plotName == "frq") calculateHistogram(&xAx);
//}


//void plotfunction::calculateHistogram(double * val)
//{
//    //    if( 6000< *val || *val< 1000) return;

//    //    qInfo()<<plotName;
//    tmpMap[static_cast<int>(floor(*val))]++;
//    totalNumber++;
//    qInfo() << tmpMap;
//    for(auto it=tmpMap.constBegin();it !=tmpMap.constEnd(); ++it)
//    {
//        if(it.value()< 2)continue;
//        qInfo() <<"key: "<< it.key();
//        qInfo() <<"value: "<< it.value();
//        double o2 = (double)( ( (double)it.value()  /  (double)totalNumber ) * 100 );
//        xAxisVal.append(o2);
//        yAxisVal.append(it.key());
//    }

//    qInfo()<<"%: " << xAxisVal;
//    qInfo()<<"num: " << yAxisVal;


//    QVector<double> x1(21), y1(21);
//    for (int i=0; i<21; ++i)
//    {
//      x1[i] = qCos(x1[i] );
//      y1[i] = i*2;
//    }

//    histoCustomPlot->plotLayout()->clear();
//    QCPAxisRect *wideAxisRect = new QCPAxisRect(histoCustomPlot);
//    histoCustomPlot->plotLayout()->addElement(0, 0, wideAxisRect);

//    QCPGraph *mainGraphCos = histoCustomPlot->addGraph(wideAxisRect->axis(QCPAxis::atLeft),wideAxisRect->axis(QCPAxis::atBottom));
//    mainGraphCos->setData(yAxisVal,xAxisVal);
//    mainGraphCos->valueAxis()->setRange(-1, 1);
//    mainGraphCos->rescaleKeyAxis();
//    mainGraphCos->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black), QBrush(Qt::white), 6));
//    mainGraphCos->setPen(QPen(QColor(50, 255, 150), 2));


//    QCPBars *bars1 = new QCPBars(histoCustomPlot->yAxis,histoCustomPlot->xAxis);
//    bars1->setWidth(9/(double)x1.size());
//    bars1->setData(yAxisVal, xAxisVal);
//    bars1->setPen(Qt::NoPen);
//    bars1->setBrush(QColor(10, 140, 70, 160));


//    //==============================================================
////    QCPAxisRect *wideAxisRect = new QCPAxisRect(histoCustomPlot);
////    QCPGraph *mainGraphCos = histoCustomPlot->addGraph(wideAxisRect->axis(QCPAxis::atLeft),wideAxisRect->axis(QCPAxis::atBottom));

////    mainGraphCos->setData(xAxisVal,yAxisVal);
////    mainGraphCos->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(Qt::white), 9));
////    mainGraphCos->setPen(QPen(QColor(100, 255, 120), 2));
////    histoCustomPlot->addGraph();
////    histoCustomPlot->graph(0)->setData(xAxisVal,yAxisVal );

//    histoCustomPlot->rescaleAxes();
////    histoCustomPlot->yAxis->setRange(2990, 3005);
////    histoCustomPlot->xAxis->setRange(0, 100);
//    histoCustomPlot->replot();
////    histoCustomPlot->clearGraphs();
////    histoCustomPlot->clearItems();

//    xAxisVal.clear();
//    yAxisVal.clear();
//}


void ReceiverMain::on_chbEn_PATTERN_clicked(bool checked)
{
    if(checked)
    {

        oscMain->_adrv9009controlUnit->enPatternSignal(1);

        //        ui->cmbTransmit->setCurrentIndex(2);
        //        ui->cmbTransmit->currentIndexChanged(2);

        ui->spnP_to_S->valueChanged(ui->spnP_to_S->value());
        ui->spnteTa_az->valueChanged(ui->spnteTa_az->value());
        ui->spnNPS->valueChanged(ui->spnNPS->value());
        ui->spnNum_Back->valueChanged(ui->spnNum_Back->value());

        //        QTimer::singleShot(100,[&]{oscMain->_adrv9009controlUnit->setPdivSignal         (ui->spnPdiv->value());});
        //        QTimer::singleShot(200,[&]{oscMain->_adrv9009controlUnit->setteTa_azSignal      (ui->spnteTa_az->value());});
        //        QTimer::singleShot(300,[&]{oscMain->_adrv9009controlUnit->setNum_SideSignal     (ui->spnNum_Side->value());});
        //        QTimer::singleShot(400,[&]{oscMain->_adrv9009controlUnit->setNum_BackSignal     (ui->spnNum_Back->value());});
        //        QTimer::singleShot(500,[&]{oscMain->_adrv9009controlUnit->setP_to_BSignal       (ui->spnP_to_B->value());});
        //        QTimer::singleShot(600,[&]{oscMain->_adrv9009controlUnit->setNPSSignal          (ui->spnNPS->value());});
        //        QTimer::singleShot(700,[&]{oscMain->_adrv9009controlUnit->setP_to_SSignal       (ui->spnP_to_S->value());});




        //        oscMain->_adrv9009controlUnit->setPdivSignal     (ui->spnPdiv->value());
        //        oscMain->_adrv9009controlUnit->setteTa_azSignal  (ui->spnteTa_az->value());
        //        oscMain->_adrv9009controlUnit->setNum_SideSignal (ui->spnNum_Side->value());
        //        oscMain->_adrv9009controlUnit->setNum_BackSignal (ui->spnNum_Back->value());
        //        oscMain->_adrv9009controlUnit->setP_to_BSignal   (ui->spnP_to_B->value());
        //        oscMain->_adrv9009controlUnit->setNPSSignal      (ui->spnNPS->value());
        //        oscMain->_adrv9009controlUnit->setP_to_SSignal   (ui->spnP_to_S->value());
        //        oscMain->_adrv9009controlUnit->sendTransmitSignal(2);
        //        ui->cmbTransmit->setCurrentIndex(2);
    }
}

//void ReceiverMain::on_tabWidgetSetting_tabBarClicked(int index)
//{
//    ui->grpSetting->setVisible(false);
//}

void ReceiverMain::on_btnReboot_clicked(bool checked)
{
    //ssh config in order to reboot card
    Q_UNUSED(checked)

    QStringList sshRebootCommand;
    sshRebootCommand << "-S" ;
    sshRebootCommand << "ssh" ;
    sshRebootCommand << "-T" ;
    sshRebootCommand<< "root@"+QString::fromStdString(frqDomainPlot->ipCard);
    sshRebootCommand << "reboot";
    QString password = "analog";


    QProcess process;
    process.start("sudo", sshRebootCommand);

    QString sudoPass = "123456";
    QTextStream sPS(&process);
    sPS<< sudoPass<<"\n";
    sPS.flush();


    process.waitForFinished();
    QByteArray error1 = process.readAllStandardError();
    qInfo()<< error1;

    qInfo()<< "---------------------------------------------------";
    process.write("analog\n");
    process.waitForFinished();

    QByteArray error = process.readAllStandardError();
    qInfo()<< error;


    //----------------------

    //----------------------







    //    //------------
    ////    qputenv("SSH_ASKPASS", "/home/seraj3/ssh.sh");
    //    qputenv("DISPLAY", "");

    //    //------------

    //    QProcess process;
    //    process.start("ssh", sshRebootCommand);

    //    if(process.waitForStarted() and process.waitForFinished())
    //    {

    //        process.write("anaog\n");
    //        process.closeWriteChannel();

    //        QByteArray output = process.readAllStandardOutput();
    //        QByteArray error = process.readAllStandardError();
    //        qInfo()<< error;


    //    }
    //    else {
    //        qInfo("hi");
    //    }





    //    "Pseudo-terminal will not be allocated because stdin is not a terminal.
    //    ssh_askpass: exec(/usr/bin/s"

    //    "ssh_askpass: exec(/usr/bin/ssh-askpass): No such file or directory
    //    Permission denied, please try ag"
}

void ReceiverMain::on_tableWidget_2_cellClicked(int row, int column)
{

    //    for(int i{}; i < ui->tableWidget_2->rowCount(); i++)
    //    {
    //        for(int j{}; j < ui->tableWidget_2->columnCount(); j++)
    //        {
    //            ui->tableWidget_2->item(i,j)->setBackground(QBrush());
    //        }
    //    }

    //    if(row == selectedRow)
    //    {
    //         selectedRow = -2;
    //        return;
    //    }

    //    for(int i{}; i < ui->tableWidget_2->columnCount(); i++)
    //    {
    //         ui->tableWidget_2->item(row,i)->setBackgroundColor(Qt::red);
    //         selectedRow = row;
    //    }
}



void ReceiverMain::on_pushButton_clicked()
{


}

void ReceiverMain::on_tblCW_cellClicked(int row, int column)
{

}


void ReceiverMain::on_btnNewUser_clicked()
{

    on_btnShowUser_clicked(false);
    QString usrName = ui->linUser->text().trimmed();
    if (!inputIsNotEmpty(usrName) or !lengthChecker(usrName)) return;

    QString password = ui->linPass->text();
    if (!inputIsNotEmpty(password) or !lengthChecker(password)) return;

    //upf stands for user password file
    writeToFile(fileAuth, usrName,password, ui->cmbRole->currentText());
    on_btnShowUser_clicked(true);
    //    writeToFile(fileAuth, usrName,hashPassword(password));
}

bool ReceiverMain::writeToFile(QString fileName,QString userName, QString password, QString role)
{

    QFile file(fileName);

    if ( !file.exists())
    {
        if(file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&file);
            out << userName << ", " << password <<", " << role << "\n";
            file.close();
            messageForAuth("User is created successfully.");
            return true;
        }
        messageForAuth("Somthing went wrong. Try again.");
        return false;
    }

    else
    {
        if(file.open(QIODevice::ReadWrite | QIODevice::Text))
        {
            QTextStream out(&file);
            while(!out.atEnd())
            {
                QString line = out.readLine();
                if( userName == line.split(",")[0])
                {
                    messageForAuth("User is already exist!");
                    return false;
                }
            }
            out << userName << ", " << password <<", " << role << "\n";
            file.close();
            messageForAuth("User is created successfully.");
            return true;
        }
        messageForAuth("Somthing went wrong. Try again.");
        return false;
    }
}



bool ReceiverMain::inputIsNotEmpty(QString inputVal)
{

    if(inputVal.isEmpty())
    {
        messageForAuth("Please fill both user name and password");
        return false;
    }

    return true;
}

bool ReceiverMain::lengthChecker(QString inputVal)
{
    if(     inputVal.length() > 20 and
            inputVal.length() < 4 )
    {
        messageForAuth("Maximum chracters must be less than 20.");
        return false;
    }
    return true;
}
void ReceiverMain::messageForAuth(QString msg)
{
    QMessageBox msgBox;
    msgBox.setText(msg);
    msgBox.exec();

}



void ReceiverMain::on_tabWidget_tabBarClicked(int index)
{
    tabPress ++;

    if(tabPress > 10)
    {
        //if you are in admin mode it means you have password

        tabPress = 0;

        for(int i{}; i < ui->tabWidget->count(); i++)
        {
            if (ui->tabWidget->tabText(i)== "Recovery Mode")
            {
                wigRecover = ui->tabRecovery;
                int indexTab = ui->tabWidget->indexOf(wigRecover);
                ui->tabWidget->removeTab(indexTab);
                return;
            }
        }

        ui->tabWidget->addTab(ui->tabRecovery,"Recovery Mode");
    }
}



QString ReceiverMain::hashPassword(const QString &pass)
{
    QByteArray hashedPass = QCryptographicHash::hash(pass.toUtf8(),QCryptographicHash::Sha256);
    return hashedPass;
}


bool ReceiverMain::verifyUser(const QString filePath,const QString &password, const QString &userName)
{

    QFile file(filePath);

    if ( !file.exists())
    {
        messageForAuth("There is no user file. Create new user in order to create file.");
        return false;
    }

    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream in(&file);


        while (!in.atEnd())
        {
            QString line = in.readLine();
            QStringList lineParts = line.split(", ");
            if(lineParts.size() == 3 and lineParts[0] == userName)
            {
                //                return hashPassword(password) == lineParts[1];
                if (password == lineParts[1])
                {
                    userRole = lineParts[2];
                    return true;
                }
            }
        }
        file.close();
    }

    return false;
}

void ReceiverMain::on_btnLogin_clicked()
{

    if(isUserLoggedin)
    {

        messageForAuth("You are loggedin.");
        return;
    }



    QString usrName = ui->linUser->text();
    timeDomainPlot->userName = ui->linUser->text();
    if (!inputIsNotEmpty(usrName) or !lengthChecker(usrName)) return;

    QString password = ui->linPass->text();
    if (!inputIsNotEmpty(password) or !lengthChecker(password)) return;


    bool res = verifyUser(fileAuth,password,usrName);

    if(res)
    {
        messageForAuth("loggin successfully.");
        ui->btnLogin->setVisible(false);
        ui->btnSet->setEnabled(true);

        emit userLoggedInSignal(true);
        ui->lblLoginStatus->setText("Welcome "+usrName+".");
        isUserLoggedin = true;
        //        int selectRole = ui->cmbRole->currentIndex();
        if(userRole == "admin" or userRole == "super")
        {
            hiddenUiElements(true);
        }
        else
        {
            ui->btnLogout->setVisible(true);
        }

    }

    else
    {

        messageForAuth("There is'nt a user with this username and password.");
    }
}

void ReceiverMain::on_btnLogout_clicked(bool checked)
{
    if(isUserLoggedin)
    {
        //emit signal;
        ui->lblLoginStatus->setText("User is not logged in.");
        isUserLoggedin = false;
        userRole = "";
        ui->grbUser->setVisible(false);
        hiddenUiElements(false);
        ui->btnLogin->setVisible(true);
        ui->btnSet->setEnabled(false);
        emit userLoggedInSignal(false);
    }
}

void ReceiverMain::on_btnRecovery_clicked(bool checked)
{
    QString recUser = ui->linRecUser->text();
    if (!inputIsNotEmpty(recUser) or !lengthChecker(recUser)) return;

    QString recPass = ui->linRecPass->text();
    if (!inputIsNotEmpty(recPass) or !lengthChecker(recPass)) return;

    if((superUser == recUser ) and  (passSuper == recPass))
    {
        ui->grbAdmin->setVisible(true);
        userRole = "super";

    }
}






void ReceiverMain::on_btnNewAdmin_clicked(bool checked)
{
    if(userRole != "super"  ) return;

    QString userName = ui->linAdminUser->text();
    if (!inputIsNotEmpty(userName) or !lengthChecker(userName)) return;

    QString password = ui->linAdminPass->text();
    if (!inputIsNotEmpty(password) or !lengthChecker(password)) return;

    QFile file(fileAuth);


    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        //        out << userName << ", " << hashPassword(password) << ", " << "admin" << "\n";
        out << userName << ", " << password << ", " << "admin" << "\n";
        file.close();
        messageForAuth("Admin user is created successfully. Now you can login with new admin.");

        userRole = "";
        hiddenUiElements(false);
        ui->tabWidget->removeTab(ui->tabWidget->count()-1);
        return;
    }
    messageForAuth("Somthing went wrong. Try again.");
    ui->tabWidget->removeTab(ui->tabWidget->count()-1);
    return;
}

void ReceiverMain::hiddenUiElements(bool state)
{
    //-------

    ui->cmbRole->setVisible(state);
    ui->btnLogout->setVisible(state);
    ui->btnNewUser->setVisible(state);
    ui->btnForget->setVisible(state);
    ui->btnChangePass->setVisible(state);
    ui->grbAdmin->setVisible(state);
    ui->lblRole->setVisible(state);
    ui->grbChangeRange->setVisible(state);
    ui->btnShowUser->setVisible(state);

    //-------
}

void ReceiverMain::on_btnSaveLimit_clicked(bool checked)
{
    QFile file(fileAuth);
    if ( !file.exists())
    {
        //        if(file.open(QIODevice::WriteOnly | QIODevice::Text))
        //        {
        //            QTextStream out(&file);
        //            out <<ui->dspnMinRangeLimit<<"," << ui->dspnMaxRangeLimit << "\n";
        //            file.close();
        //            messageForAuth("Limitation ranges are saved successfully.");
        //            return;
        //        }

        messageForAuth("Somthing went wrong. Try again.");
        return;
    }

    //    else
    //    {
    if(file.open(QIODevice::ReadWrite | QIODevice::Text))
    {

        QTextStream out(&file);
        QString allCont = out.readAll();
        file.seek(0);

        QTextStream str(&file);
        str <<ui->dspnMinRangeLimit->text()<<"," << ui->dspnMaxRangeLimit->text()<< "\n"<<allCont;

        file.close();
        messageForAuth("Limitation ranges are saved successfully.");
        return;
    }

    messageForAuth("Somthing went wrong. Try again.");
    //    }
}

void ReceiverMain::on_btnShowUser_clicked(bool checked)
{
    isShow = !isShow;
    ui->grbUser->setVisible(isShow);
    //    ui->tblUsers->setVisible(isShow);

    QFile file(fileAuth);

    int i{};
    if(isShow and file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        while (!out.atEnd())
        {

            QString userInfo = out.readLine();
            QStringList info = userInfo.split(",");
            if(info.size()  < 3) continue;

            QPushButton * btnDel = new QPushButton("Delete");
            btnDel->setStyleSheet("background-color:red");

            QPushButton * btnEdit = new QPushButton("Edit");
            btnEdit->setStyleSheet("background-color:green");





            //------------------ delete -------------------------------------
            connect(btnDel, &QPushButton::clicked,[=](bool state)
            {
                QFile f(fileAuth);
                if(f.open(QIODevice::ReadWrite | QIODevice::Text))
                {
                    QStringList users;
                    QTextStream val(&f);


                    while (!val.atEnd())
                    {
                        QString userName = val.readLine();
                        if(userName.isEmpty()) continue;
                        QString l = info.join(",");


                        if(userName == info.join(","))
                        {
                            continue;
                        }
                        users.append(userName);
                    }
                    f.close();



                    f.open(QIODevice::ReadWrite | QIODevice::Truncate);
                    f.close();



                    f.open(QIODevice::ReadWrite | QIODevice::Text);
                    for (int i{}; i < users.size(); i++)
                    {
                        val <<users[i]<<"\n";
                    }
                    on_btnShowUser_clicked(false);

                    f.close();
                    on_btnShowUser_clicked(true);

                    if(users.size()==0)
                    {
                        ui->tblUsers->setRowCount(0);
                        return ;
                    }
                }
            });
            //---------------------------------------------------------------

            //---------------------------- edit -----------------------------
            connect(btnEdit, &QPushButton::clicked,[=](bool state)
            {




                ui->tblUsers->item(i,0)->setFlags(ui->tblUsers->item(i,0)->flags() | Qt::ItemIsEditable);
                ui->tblUsers->item(i,1)->setFlags(ui->tblUsers->item(i,1)->flags() | Qt::ItemIsEditable);
                //                ui->tblUsers->item(i,0)->setBackgroundColor(Qt::red);
                btnEdit->setText("oK");
                btnEdit->setStyleSheet("background-color:gray");


                if(holdIndexTblUser != i)
                {
                    userNameInFile = ui->tblUsers->item(i,0)->text();
                    holdIndexTblUser = i;
                    return ;
                }
                holdIndexTblUser = -1;

                QString tblUserNme = ui->tblUsers->item(i,0)->text();


                QFile f(fileAuth);
                if(f.open(QIODevice::ReadWrite | QIODevice::Text))
                {
                    QStringList users;
                    QTextStream val(&f);


                    while (!val.atEnd())
                    {
                        QString userName = val.readLine();
                        if(userName.isEmpty()) continue;

                        if(userName.split(",")[0] == userNameInFile)
                        {
                            userName.replace(userName.split(",")[0],tblUserNme);
                            //                            continue;
                        }

                        users.append(userName);
                    }
                    f.close();



                    f.open(QIODevice::ReadWrite | QIODevice::Truncate);
                    f.close();



                    f.open(QIODevice::ReadWrite | QIODevice::Text);
                    for (int i{}; i < users.size(); i++)
                    {
                        val <<users[i]<<"\n";
                    }
                    on_btnShowUser_clicked(false);

                    f.close();
                    on_btnShowUser_clicked(true);

                    if(users.size()==0)
                    {
                        ui->tblUsers->setRowCount(0);
                        return ;
                    }
                }
                btnEdit->setText("Edit");
                btnEdit->setStyleSheet("background-color:green");
            });
            //---------------------------------------------------------------

            QTableWidgetItem * userName        = new QTableWidgetItem(info[0]);
            QTableWidgetItem * role            = new QTableWidgetItem(info[2]);
            //            QTableWidgetItem * createdTime     = new QTableWidgetItem(info[3]);

            ui->tblUsers->setRowCount(i+1);
            ui->tblUsers->setItem(i,0,userName);
            ui->tblUsers->setItem(i,1,role);
            //            ui->tblUsers->setItem(i,0,createdTime);
            ui->tblUsers->setCellWidget(i,3,btnDel);
            ui->tblUsers->setCellWidget(i,4,btnEdit);

            ui->tblUsers->item(i,0)->setFlags(ui->tblUsers->item(i,0)->flags() & ~Qt::ItemIsEditable);
            ui->tblUsers->item(i,1)->setFlags(ui->tblUsers->item(i,1)->flags() & ~Qt::ItemIsEditable);
            i++;
        }

        file.close();
    }
}









//=====================================================================================
//============================== [ Calibration ] ======================================
//=====================================================================================
void ReceiverMain::on_btnCalib_clicked(bool checked)
{
    calibVal = ui->dsbCalib->value();

    emit sendCommandToDeviceSignal("OUTP ON");

    //    emit sendCommandToDeviceSignal("POW?");

    ui->spnFrq->setValue(calibVal+200);
    ui->btnSet->clicked();

    QTimer::singleShot(2000,[&]{ startCalibSending(); });
}


void ReceiverMain::startCalibSending()
{
    QString frqStr = "FREQ:CW"+QString(" ")+QString::number(calibVal)+ "MHZ";
    QByteArray frqValue = frqStr.toUtf8();
    emit sendCommandToDeviceSignal(frqValue);
    QTimer::singleShot(1000,[&]{sendPowerToCalibDevice();});
}


void ReceiverMain::sendPowerToCalibDevice()
{
    powNw = -30 - (cntCalib * 10 );
    QString powStr = "POW"+QString(" ")+QString::number(powNw)+ "DBM";
    QByteArray powValue = powStr.toUtf8();
    emit sendCommandToDeviceSignal(powValue);



    //-------------------------------------

    QTimer::singleShot(300,[&]{
        emit sendCommandToDeviceSignal("FREQ:CW?");
        modeCalib="frq";
    });

    QTimer::singleShot(1000,[&]{
        emit sendCommandToDeviceSignal("POW?");
        modeCalib="power";
    });
    //--------------------------------------

}




void ReceiverMain::on_DeviceResponseSlot(QByteArray val)
{
    if( modeCalib=="frq")
    {
        val.remove(6,val.size()-1);
        val.replace(".","");
        val.replace("+","");
        double value = val.toDouble();
        if(abs( calibVal - value) < 1)
        {
            isFrqSetOnCalib = true;
        }
        else {
            emit sendCommandToDeviceSignal("FREQ:CW?");
            modeCalib="frq";
        }

    }

    else if (modeCalib=="power")
    {

        int coeff = 10;
        val.replace("\n","");
        if (val.contains("+002"))
        {
            coeff = 100;
        }

        val.remove(7,val.size()-1);
        double value = val.toDouble()*coeff;

        ui->label_91->setText(QString::number(powNw));
        ui->label_92->setText(QString::number(value));
        if(abs(powNw - value) < 0.9)
        {
            isPowerSetOnCalib = true;
        }
        else {
            emit sendCommandToDeviceSignal("POW?");
            modeCalib="power";
        }
    }
}

void ReceiverMain::calibManagment()
{


}


void ReceiverMain::checkingCalibResponseInSpectromSlot(double frq, double powerValue)
{


    if(cntVecCalib > 600)
    {
        cntVecCalib = 0;
        newPowerCalib = 0;
        saveCalibrToFile();
        return;
    }


    if(frq == -1) return;


    ui->label_89->setText(QString::number(frq));
    ui->label_90->setText(QString::number(powerValue));

    //save data
    frqCardVec[cntVecCalib]   = frq;
    powerCardVec[cntVecCalib] = powerValue;
    frqDomainPlot->isCalibAllowed = false;
    cntVecCalib++;


    cntCalib++;

    if(cntCalib == 7)
    {

        int o = 0;
    }
    if(cntCalib == 8)
    {
        calibVal += 200;
        ui->spnFrq->setValue(calibVal);
        ui->btnSet->clicked();

        cntCalib = 0;
        QTimer::singleShot(2000, [&]{startCalibSending();});
    }

    else
    {
        QTimer::singleShot(3000,[&]{

            sendPowerToCalibDevice();

        });
    }
}

void ReceiverMain::saveCalibrToFile()
{
    QFile file(calibFile);


    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);

        for(int i{}; i < frqCalibVec.size(); i++)
        {
            out << frqCalibVec [i] << "," << powerCalibVec[i] <<" ," << frqCardVec[i]<<"," << powerCardVec[i]<< "\n";
        }
        file.close();
    }
}


void ReceiverMain::on_btnStopCalib_clicked()
{
    cntVecCalib = 0;
    cntCalib = 0;
    newPowerCalib = 0;
    isCalibAllowed = false;
    //    calibFrqTimer->stop();
    //    calibPowerTimer->stop();
    saveCalibrToFile();
}



void ReceiverMain::on_btnCalibRff_clicked(bool checked)
{
    //    OUTP:MOD?
    //    emit sendCommandToDeviceSignal("OUTP:MOD?");

    //    isRffCalibOn =! isRffCalibOn;
    //    ui->btnCalibRff->setText(isRffCalibOn? "RFF:on" : "RFF:off");
    //    emit sendCommandToDeviceSignal(isRffCalibOn? "OUTP ON" : "OUTP OFF");


    //    QFile f(calibFile);

    //    if(f.open(QIODevice::ReadWrite | QIODevice::Text))
    //    {


    //        QTextStream d (&f);
    //        QStringList ss;

    //        while (!d.atEnd())
    //        {

    //            QString mtn = d.readLine();

    //            mtn.replace(" ----- ",",");
    //            mtn.replace(" ","");
    //            ss.append(mtn);
    //        }
    //        f.close();



    //        f.open(QIODevice::ReadWrite | QIODevice::Truncate);
    //        f.close();



    //        f.open(QIODevice::ReadWrite | QIODevice::Text);
    //        for (int i{}; i < ss.size(); i++)
    //        {
    //            d <<ss[i]<<"\n";
    //        }
    //        f.close();

    //    }

}

//=====================================================================================

















































//=====================================================================
//--------------------------[ calibration ]----------------------------
//=====================================================================
//void ReceiverMain::on_btnCalib_clicked(bool checked)
//{
//    calibVal = ui->dsbCalib->value();

//    emit sendCommandToDeviceSignal("OUTP ON");

//    frqCalibHold = calibVal+200;

//    ui->spnFrq->setValue(calibVal+200);
//    ui->btnSet->clicked();

//    QTimer::singleShot(2000,[&]{
//        calibFrqTimer->start();
//        calibPowerTimer->start();
//    });
//}


//void ReceiverMain::startCalibSending()
//{
//    QString frqStr = "FREQ:CW"+QString(" ")+QString::number(calibVal)+ "MHZ";
//    QByteArray frqValue = frqStr.toUtf8();

//    emit sendCommandToDeviceSignal(frqValue);
//    frqCalibVec[cntCalib] = calibVal;

//    if(calibFrqTimer->isActive())
//    {
//        calibFrqTimer->stop();
//    }
//}

//void ReceiverMain::sendPowerToCalibDevice()
//{
//    if(cntCalib > 600)
//    {
//        cntCalib = 0;
//        newPowerCalib = 0;
//        calibFrqTimer->stop();
//        calibPowerTimer->stop();
//        saveCalibrToFile();

//        return;
//    }

//    QString powStr = "POW"+QString(" ")+QString::number(-30-newPowerCalib)+ "DBM";
//    QByteArray powValue = powStr.toUtf8();

//    //    qInfo()<<"-------------------------- powStr: "<<powStr;

//    emit sendCommandToDeviceSignal(powValue);



//    powerCalibVec[cntCalib]   = -30-newPowerCalib;


//    if (newPowerCalib == 80)
//    {
//        newPowerCalib = 0;
//        calibVal += 100;

//        //        ui->spnFrq->setValue(calibVal);
//        //        ui->btnSet->clicked();
//    }

//    else newPowerCalib +=10;


//    cntCalib++;
////    if(!calibPowerTimer->isActive()) calibPowerTimer->start();

//    isCalibAllowed = true;




//    if(calibPowerTimer->isActive())
//    {
//        calibPowerTimer->stop();
//    }

//    QTimer::singleShot(500,[&]{
//        frqDomainPlot->powerCalib = -30-newPowerCalib;
//        frqDomainPlot->frqCalib   = calibVal;
//    });



//    if(cntCalib < 10 ) return;

//    if(abs(frqCalibHold -  calibVal) == 201)
//    {
//        isCalibAllowed = true;
//        frqCalibHold = calibVal;
//        ui->spnFrq->setValue(calibVal);
//        ui->btnSet->clicked();
//    }
//}





//void ReceiverMain::checkingCalibResponseInSpectromSlot(double frq, double powerValue)
//{

//    if(frq ==-1 and isCalibAllowed)
//    {
//        QTimer::singleShot(1000,[&]{
//            startCalibSending();
//        });

//        QTimer::singleShot(2000,[&]{
//            sendPowerToCalibDevice();});

//         return;
//    }

//    ui->label_89->setText(QString::number(frq));
//    ui->label_90->setText(QString::number(powerValue));


//    //    if (powerCalibVec[cntCalib]==0) return;
//    if(!isCalibAllowed) return;

//    if(frqCardVec[cntCalib] != 0) return;

//    frqCardVec[cntCalib] = frq;
//    powerCardVec[cntCalib] = powerValue;

////    calibFrqTimer->stop();
////    calibPowerTimer->stop();

////    QTimer::singleShot(1000,[&]{
////        startCalibSending();
////    });

//    QTimer::singleShot(1000,[&]{
//        sendPowerToCalibDevice();});
//}

//void ReceiverMain::saveCalibrToFile()
//{
//    QFile file(calibFile);


//    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
//    {
//        QTextStream out(&file);

//        for(int i{}; i < frqCalibVec.size(); i++)
//        {
//            out << frqCalibVec [i] << ", " << powerCalibVec[i] <<" ----- " << frqCardVec[i]<<", " << powerCardVec[i]<< "\n";
//        }
//        file.close();
//    }
//}


//void ReceiverMain::on_btnStopCalib_clicked()
//{
//    cntCalib = 0;
//    newPowerCalib = 0;
//    isCalibAllowed = false;
//    calibFrqTimer->stop();
//    calibPowerTimer->stop();
//    saveCalibrToFile();
//}

//=========================================================================================


























//void ReceiverMain::sendFrqToCalibDevice()
//{
//    if(cntCalib > 6)
//    {
//        calibFrqTimer->stop();
//        calibPowerTimer->stop();
//        saveCalibrToFile();
//        return;
//    }

//    QString frqStr = "FREQ:CW"+QString(" ")+QString::number(calibVal)+ "MHZ";
//    QByteArray frqValue = frqStr.toUtf8();

//    emit sendCommandToDeviceSignal(frqValue);
//    frqCalibVec[cntFrqCalib] =calibVal;


//    if (newPowerCalib==80)
//    {
//        newPowerCalib = 0;
//        calibVal += 100;
//        cntCalib++;
//        ui->spnFrq->setValue(calibVal);
//        ui->btnSet->clicked();
//    }

//    else newPowerCalib +=10;

//    cntFrqCalib++;
//}






////void ReceiverMain::sendPowerToCalibDevice()
////{

////    QString powStr = "POW"+QString(" ")+QString::number(-20-newPowerCalib)+ "DBM";
////    QByteArray powValue = powStr.toUtf8();

////    emit sendCommandToDeviceSignal(powValue);
////    powerCalibVec[cntPowerCalib] = -20-newPowerCalib;

////    cntPowerCalib++;

////}


//void ReceiverMain::checkFrqInColibr()
//{
//    //---------------[ getting value for calibration ]-------------------
//    //    for(int i{}; i < frqCalibVec.size(); i++)
//    //    {

//    //        if(abs(frqDomainPlot->frqValue - frqCalibVec[i]) < 5)
//    //        {
//    //            frqCardVec.append(frqVal[i]);
//    //            powerCardVec.append(powerVal[i]);
//    //            isCalibAllowedToSend = true;
//    //        }

//    //    }
//    //-------------------------------------------------------------------


//}

//void ReceiverMain::on_btnLimitation_clicked()
//{

//}


//void ReceiverMain::on_chbCalib_stateChanged(int val)
//{

//}
