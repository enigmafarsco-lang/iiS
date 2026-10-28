// ============================================================================
// File: plotfunction.cpp
// Subsystem: eLynxSDR receiver
// Description: Subsystem implementation and interface guidance for plotfunction.cpp.
// ============================================================================

#include "plotfunction.h"

// Constructor: Initializes plotfunction subsystem components, default parameters, and state.
plotfunction::plotfunction(QObject *parent) : QThread(parent)
{
    this->start();
    moveToThread(this);
}

// Destructor: Cleans up plotfunction allocated resources, timers, and buffers.
plotfunction::~plotfunction()
{
    //    if(isRunning())
    //    {
    //        QThread::exit(0);
    //        QThread::wait();
    //    }
}

// Sets plot parameter and updates internal configuration.
void plotfunction::setPlot(QCustomPlot *plt)
{
    customPlot = plt;
}


// Sets histo plot parameter and updates internal configuration.
void plotfunction::setHistoPlot(QCustomPlot *plt)
{
    histogramPlot = plt;
}

// Executes creating time plot routine.
void plotfunction::creatingTimePlot()
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

// Executes creating histogram plot routine.
void plotfunction::creatingHistogramPlot()
{

    if(!histogramPlot) return;
    histogramPlot->plotLayout()->clear();
    wideAxisRect = new QCPAxisRect(histogramPlot);
    histogramPlot->plotLayout()->addElement(0, 0, wideAxisRect);

    mainGraph = histogramPlot->addGraph(wideAxisRect->axis(QCPAxis::atLeft),wideAxisRect->axis(QCPAxis::atBottom));
    //        mainGraph->setData(yAxisVal,xAxisVal);
    mainGraph->valueAxis()->setRange(-1, 1);
    mainGraph->rescaleKeyAxis();
    mainGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black), QBrush(Qt::white), 6));
    mainGraph->setPen(QPen(QColor(50, 255, 150), 2));

    bars1 = new QCPBars(histogramPlot->yAxis,histogramPlot->xAxis);
    //    bars1->setWidth(9/(double)x1.size());
    //        bars1->setData(yAxisVal, xAxisVal);
    bars1->setPen(Qt::NoPen);
    bars1->setBrush(QColor(10, 140, 70, 160));

    //===================================================================================================================

}




// Executes main plot routine.
void plotfunction::mainPlot(double xAx , double yAx)
{
    double key = timeStart.msecsTo(QTime::currentTime())/1000.0;

    emit histoMainPlotSignal(key, xAx);
    //    customPlot->graph(0)->addData(key, xAx);
    //    customPlot->graph(0)->rescaleValueAxis();
    //    customPlot->xAxis->setRange(key, 20, Qt::AlignRight);
    //    //    customPlot->yAxis->setRange(yAx-1,yAx+1);
    //    customPlot->replot();

    calculateHistogram(&xAx);
}



// Executes plt get data slot routine.
void plotfunction::pltGetDataSlot(double val , double toa)
{

    //    for(int i = 0; i < 10; i++)
    //    {
    //    vec.setX(cnt);
    //    vec.setY(20 * log10f(abs( 1.0 / ((RAND_MAX) * qrand()) )) + 100);
    //    a.addPoints(vec);
    //    cnt++;

    //    }

    //    emit pltUpdateSignal(a);
    //    a.removeAllPoints();

    if(cnt < 100)
    {
        vec.setX(toa);
        vec.setY(val);
        a.addPoints(vec);
        cnt++;
        if (hasHistogram) calculateHistogram(&val);
    }
    else
    {
        emit pltUpdateSignal(a);
        a.removeAllPoints();
        cnt=0;
    }

    //    qInfo() << "t: " << QThread::currentThreadId();
}

// Executes run routine.
void plotfunction::run()
{

    exec();
}



// Computes mathematical calculate histogram calculation.
void plotfunction::calculateHistogram(double * val)
{
    //    if( 6000< *val || *val< 1000) return;

    tmpMap[static_cast<int>(floor(*val))]++;
    totalNumber++;
    for(auto it=tmpMap.constBegin();it !=tmpMap.constEnd(); ++it)
    {
        if(it.value()< 10)continue;

        double mapVal = (double)( ( (double)it.value()  /  (double)totalNumber ) * 100 );

        tmp2Map[it.key()] = mapVal;



        if (it.value()  > 1000)
        {
            tmpMap.clear();
            return;
        }
    }

    for(auto it=tmp2Map.constBegin();it !=tmp2Map.constEnd(); ++it)
    {
        xAxisVal.append(it.value());
        yAxisVal.push_back(it.key());
    }



    //        qInfo()<<"1) " << "yAxisVal: " << yAxisVal << "--------yAxisVal : "<<xAxisVal;
    emit histoBarPlotSignal(yAxisVal, xAxisVal);



    //    mainGraph->setData(yAxisVal,xAxisVal);
    //    bars1->setData(yAxisVal, xAxisVal);
    //    histogramPlot->rescaleAxes();
    //    histogramPlot->replot();


        xAxisVal.clear();
        yAxisVal.clear();
}

//void plotfunction::histPlot()
//{

//}



