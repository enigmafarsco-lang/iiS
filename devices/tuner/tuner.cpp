#include "tuner.h"
#include "ui_tuner.h"

Tuner::Tuner(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Tuner)
{
    ui->setupUi(this);

    init();
    plotInit();
    timerAutoSend.setInterval(150);
    connect(&timerAutoSend, &QTimer::timeout, this, &Tuner::updateAutoSend);
    ui->btnStart->setStyleSheet("background-color:"+QString(RED)+";color:white");

    //validating user input
    QIntValidator * validNum = new QIntValidator(0,99999, this);

    ui->lneCenterTuner->setValidator(validNum);

}


void Tuner::plotInit()
{
    //================== one plot ==========================
    vectorOfCharts.append(tunerChart1);
    setQGlchartsParameters(tunerChart1, ChartType::Spectrum);
    vBox1->addWidget(tunerChart1);

    group1->setLayout(vBox1);
    group1->setTitle("RX3.v");
    vert1->addWidget(group1);
    connect(this, SIGNAL(createPlotSignal1(QCustomSeries)), tunerChart1, SLOT(updateSeriesData(QCustomSeries)));
    ui->w1->setLayout(vert1);

    //================== two plot ==========================

    vectorOfCharts.append(tunerChart2);
    setQGlchartsParameters(tunerChart2, ChartType::Spectrum);
    vBox2->addWidget(tunerChart2);

    group2->setLayout(vBox2);
    group2->setTitle("RXrfin");
    vert2->addWidget(group2);
    connect(this, SIGNAL(createPlotSignal2(QCustomSeries)), tunerChart2, SLOT(updateSeriesData(QCustomSeries)));
    ui->w2->setLayout(vert2);

    //================== three plot ==========================

    vectorOfCharts.append(tunerChart3);
    setQGlchartsParameters(tunerChart3, ChartType::Spectrum);
    vBox3->addWidget(tunerChart3);

    group3->setLayout(vBox3);
    group3->setTitle("RX1.5v");
    vert3->addWidget(group3);
    connect(this, SIGNAL(createPlotSignal3(QCustomSeries)), tunerChart3, SLOT(updateSeriesData(QCustomSeries)));
    ui->w3->setLayout(vert3);

    //================== four plot ==========================

    vectorOfCharts.append(tunerChart4);
    setQGlchartsParameters(tunerChart4, ChartType::Spectrum);
    vBox4->addWidget(tunerChart4);

    group4->setLayout(vBox4);
    group4->setTitle("RX5v");
    vert4->addWidget(group4);
    connect(this, SIGNAL(createPlotSignal4(QCustomSeries)), tunerChart4, SLOT(updateSeriesData(QCustomSeries)));
    ui->w4->setLayout(vert4);

    //================== five plot ==========================

    vectorOfCharts.append(tunerChart5);
    setQGlchartsParameters(tunerChart5, ChartType::Spectrum);
    vBox5->addWidget(tunerChart5);

    group5->setLayout(vBox5);
    group5->setTitle("RX8v");
    vert5->addWidget(group5);
    connect(this, SIGNAL(createPlotSignal5(QCustomSeries)), tunerChart5, SLOT(updateSeriesData(QCustomSeries)));
    ui->w5->setLayout(vert5);

    //================== six plot ==========================

    vectorOfCharts.append(tunerChart6);
    setQGlchartsParameters(tunerChart6, ChartType::Spectrum);
    vBox6->addWidget(tunerChart6);

    group6->setLayout(vBox6);
    group6->setTitle("TX3.3v");
    vert6->addWidget(group6);
    connect(this, SIGNAL(createPlotSignal6(QCustomSeries)), tunerChart6, SLOT(updateSeriesData(QCustomSeries)));
    ui->w6->setLayout(vert6);

    //================== seven plot ==========================

    vectorOfCharts.append(tunerChart7);
    setQGlchartsParameters(tunerChart7, ChartType::Spectrum);
    vBox7->addWidget(tunerChart7);

    group7->setLayout(vBox7);
    group7->setTitle("TX2.5v");
    vert7->addWidget(group7);
    connect(this, SIGNAL(createPlotSignal7(QCustomSeries)), tunerChart7, SLOT(updateSeriesData(QCustomSeries)));
    ui->w7->setLayout(vert7);

    //================== eight plot ==========================

    vectorOfCharts.append(tunerChart8);
    setQGlchartsParameters(tunerChart8, ChartType::Spectrum);
    vBox8->addWidget(tunerChart8);

    group8->setLayout(vBox8);
    group8->setTitle("TXLO");
    vert8->addWidget(group8);
    connect(this, SIGNAL(createPlotSignal8(QCustomSeries)), tunerChart8, SLOT(updateSeriesData(QCustomSeries)));
    //    ui->w8->setLayout(vert8);

    //================== nine plot ==========================

    vectorOfCharts.append(tunerChart9);
    setQGlchartsParameters(tunerChart9, ChartType::Spectrum);
    vBox9->addWidget(tunerChart9);

    group9->setLayout(vBox9);
    group9->setTitle("TX1.5v");
    vert9->addWidget(group9);
    connect(this, SIGNAL(createPlotSignal9(QCustomSeries)), tunerChart9, SLOT(updateSeriesData(QCustomSeries)));
    ui->w9->setLayout(vert9);

    //================== ten plot ==========================

    vectorOfCharts.append(tunerChart10);
    setQGlchartsParameters(tunerChart10, ChartType::Spectrum);


    vBox10->addWidget(tunerChart10);

    group10->setLayout(vBox10);
    group10->setTitle("TX5v");


    vert10->addWidget(group10);
    connect(this, SIGNAL(createPlotSignal10(QCustomSeries)), tunerChart10, SLOT(updateSeriesData(QCustomSeries)));
    ui->w10->setLayout(vert10);
}




void Tuner::setQGlchartsParameters(QGLchart *glChart, ChartType glChartType)
{
//    glChart->setMinimumSize(QSize(50, 50));
    QCustomAxis *verticalAxis = new QCustomAxis(this);
    verticalAxis->setAlignment(Qt::AlignLeft);
    verticalAxis->setOrientation(Qt::Vertical);
    verticalAxis->setGridLineColor(Qt::white);
    //    verticalAxis->setTitleText("Amplitude");
    //    verticalAxis->setLabelsText("        (dBm)");
    verticalAxis->setVisible(true);
    glChart->addAxis(verticalAxis, Qt::AlignLeft);

    QCustomAxis *horizantalAxis = new QCustomAxis(this);
    horizantalAxis->setAlignment(Qt::AlignBottom);
    horizantalAxis->setOrientation(Qt::Horizontal);
    horizantalAxis->setGridLineColor(Qt::white);
    //    horizantalAxis->setTitleText("Frequency");
    //    horizantalAxis->setLabelsText("(MHz)");
    glChart->addAxis(horizantalAxis, Qt::AlignBottom);
    glChart->enableMasking(true);
    glChart->setMaskingModifiable(true);
    glChart->setMouseTracking(true);
    glChart->setVerticalUpperRangeMargin(10);
    glChart->setVerticalLowerRangeMargin(10);
    glChart->setHorizontalNumberCount(6000);
    glChart->enableToolTip(true);
    glChart->setChartType(glChartType);
    glChart->enableSyncedZoom(false);
    //    glChart->zoomEnable(true);
}

void Tuner::init()
{
    //    vector1.setX(0);
    //    vector1.setY(0);
    //    series1.addPoints(vector1);
    //    series2;
    //    series3;
    //    series4;
    //    series5;
    //    series6;
    //    series7;
    //    series8;
    //    series9;
    //    series10;
    //    series11;
}

void Tuner::removeAllPoints()
{

    series1.removeAllPoints();
    series2.removeAllPoints();
    series3.removeAllPoints();
    series4.removeAllPoints();
    series5.removeAllPoints();
    series6.removeAllPoints();
    series7.removeAllPoints();
    series8.removeAllPoints();
    series9.removeAllPoints();
    series10.removeAllPoints();
    series11.removeAllPoints();
    firstRun = true;
    yAxiosCounter = 0;

}
//i have     int plottingValues( const double *, QCustomPlot * , QVector<double> *, QVector<double> *, double min, double max,int);
// function now i want to make optional QCustomPlot *

void Tuner::emitToPlot()
{
    emit createPlotSignal1(series1);
    emit createPlotSignal2(series2);
    emit createPlotSignal3(series3);
    emit createPlotSignal4(series4);
    emit createPlotSignal5(series5);
    emit createPlotSignal6(series6);
    emit createPlotSignal7(series7);
    //    emit createPlotSignal8(series8);
    emit createPlotSignal9(series9);
    emit createPlotSignal10(series10);
}

void Tuner::fillingPlotVector(QStringList &frame)
{
    for (int i{}; i < frame.size(); i ++)
    {
        if(frame[i].isEmpty()||frame[i].isNull()) continue;

        QStringList splitData;
        splitData = frame[i].split("=");

        if(splitData[0] == "RX3.v"){
            setVectorToQcustom(vector1,series1,splitData[1].toInt());
        }

        else if (splitData[0] == "RXrfin") {
            setVectorToQcustom(vector2,series2,splitData[1].toInt());
            if ( splitData[1].toInt() > 68 or  splitData[1].toInt() < 48)
            {
                ui->rfinBtn->setStyleSheet("background-color:"+QString(RED)+";color:white");
                int c = 50-splitData[1].toInt();
                ui->rfinBtn->setText("RxrFin : " + QString::number(c)+" dbm");


            }
            else {
                ui->rfinBtn->setStyleSheet("background-color:"+QString(YELLOW_GREEN)+";color:"+QString(BLACK));
                int c = 50-splitData[1].toInt();
                ui->rfinBtn->setText("RxrFin : " + QString::number(c)+" dbm");
            }
        }

        else if (splitData[0] == "RX1.5v") {
            setVectorToQcustom(vector3,series3,splitData[1].toInt());
        }

        else if (splitData[0] == "RX5v") {
            setVectorToQcustom(vector4,series4,splitData[1].toInt());
        }

        else if (splitData[0] == "RX8v") {
            setVectorToQcustom(vector5,series5,splitData[1].toInt());
        }

        else if (splitData[0] == "TX3.3v") {
            setVectorToQcustom(vector6,series6,splitData[1].toInt());
        }

        else if (splitData[0] == "TX2.5v") {
            setVectorToQcustom(vector7,series7,splitData[1].toInt());
        }

        else if (splitData[0] == "TXLO") {
            //            setVectorToQcustom(vector8,series8,splitData[1].toInt());
            if ( splitData[1].toInt() > 68 or  splitData[1].toInt() < 48)
            {
                ui->txloBtn->setStyleSheet("background-color:"+QString(RED)+";color:white");

                int c = 25-splitData[1].toInt();
                ui->txloBtn->setText("TxLo: " + QString::number(c)+" dbm");
            }
            else {
                ui->txloBtn->setStyleSheet("background-color:"+QString(YELLOW_GREEN)+";color:"+QString(BLACK));
                int c = 25-splitData[1].toInt();
                ui->txloBtn->setText("TxLo : " + QString::number(c)+" dbm");
            }

        }

        else if (splitData[0] == "TX1.5v") {
            setVectorToQcustom(vector9,series9,splitData[1].toInt());
        }

        else if (splitData[0] == "TX5v") {
            setVectorToQcustom(vector10,series10,splitData[1].toInt());
        }

        else if (splitData[0] == "TX8v") {
            setVectorToQcustom(vector11,series11,splitData[1].toInt());
        }
        else if (splitData[0] == "LOmaxou") {
            if (splitData[1].toInt() != 255)
            {
                ui->pllBtn->setStyleSheet("background-color:"+QString(RED)+";color:white");

            }
            else {
                ui->pllBtn->setStyleSheet("background-color:"+QString(YELLOW_GREEN)+";color:"+QString(BLACK));
            }

        }
    }
}

void Tuner::setVectorToQcustom(QVector2D & vec, QCustomSeries & QCustom, int data)
{
    vec.setX(yAxiosCounter);
    vec.setY(data);
    QCustom.addPoints(vec);
}

void Tuner::on_DeviceResponseSlot(QByteArray response)
{
//    qDebug ()<<"tuner ---> "  <<response;

    if(!plotIsAllowed) return;

    if(yAxiosCounter == removeNumber)
    {
        removeAllPoints();
    }

    QString packet;
    QStringList frame;

    packet = response;
    packet = packet.replace("\n\r", " ");
    packet = packet.replace("and", "");
    frame = packet.split(" ");
    if (frame[0] == "maxout=" ) return;
    if (frame[0] == "cnt=11" ) return;
    if (!frame[0].contains("RX3")) return;

    //    qDebug ()<<"---> "  <<frame;
    fillingPlotVector(frame);

    if(firstRun)
    {
        firstRun=false;
        yAxiosCounter++;
        return;
    }

    emitToPlot();
    yAxiosCounter++;
}

void Tuner::isDeviceConnectedSlot(bool val)
{
    isConnect = val;
    ui->btnConnect->setStyleSheet(isConnect?"background-color:"+QString(YELLOW_GREEN)+";color:"+QString(BLACK):"background-color:#"+QString(RED)+";color:white");
}

void Tuner::joshanFuncDataSlot(QString)
{

}

void Tuner::on_pushButton_5_clicked()
{
    if(isConnect){
        QString f = ui->lneCenterTuner->text();

        for (int i{};i< f.size();i++)
        {
            temp.append(QChar(f[i]));
        }

        temp.append(13);
        timerAutoSend.start();
    }
}

void Tuner::updateAutoSend()
{
    if(timerCount == temp.size())
    {
        timerAutoSend.stop();
        timerCount = 0;
        temp.clear();
        return;
    }

    QByteArray sendFrq ;
    sendFrq.append(temp[timerCount]);
    emit sendCommandToDeviceSignal(sendFrq);
    timerCount++;
}





Tuner::~Tuner()
{
    //    delete vBox1       ;
    //    delete vert1       ;
    //    delete tunerChart1 ;
    //    delete group1      ;

    //    delete vBox2       ;
    //    delete vert2       ;
    //    delete tunerChart2 ;
    //    delete group2      ;

    //    delete vBox3       ;
    //    delete vert3       ;
    //    delete tunerChart3 ;
    //    delete group3      ;

    //    delete vBox4       ;
    //    delete vert4       ;
    //    delete tunerChart4 ;
    //    delete group4      ;

    //    delete vBox5       ;
    //    delete vert5       ;
    //    delete tunerChart5 ;
    //    delete group5      ;

    //    delete vBox6       ;
    //    delete vert6       ;
    //    delete tunerChart6 ;
    //    delete group6      ;

    //    delete vBox7       ;
    //    delete vert7       ;
    //    delete tunerChart7 ;
    //    delete group7      ;

    //    delete vBox8       ;
    //    delete vert8       ;
    //    delete tunerChart8 ;
    //    delete group8      ;

    //    delete vBox9       ;
    //    delete vert9       ;
    //    delete tunerChart9 ;
    //    delete group9      ;
    //    delete vBox10      ;
    //    delete vert10      ;
    //    delete tunerChart10;
    //    delete group10     ;


    delete ui;

}

void Tuner::on_btnStart_clicked()
{
    if(isConnect){
        removeAllPoints();
        plotIsAllowed = !plotIsAllowed;
        ui->btnStart->setStyleSheet(plotIsAllowed?"background-color:"+QString(YELLOW_GREEN)+";color:"+QString(BLACK):"background-color:"+QString(RED)+";color:white");
        ui->btnStart->setText(plotIsAllowed?"Stop" :"Start");
    }
}

void Tuner::on_btnSetFrq_clicked()
{

}
