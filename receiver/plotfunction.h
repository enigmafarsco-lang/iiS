/**
 * @file plotfunction.h
 * @brief Threaded Real-Time Waveform & Histogram Plotting Engine for eLynxSDR.
 *
 * plotfunction handles high-rate radar pulse telemetry extraction and visualization:
 * - Pulse Width (PW1/PW2), Pulse Repetition Interval (PRI), Peak Power (PAPeak),
 *   Integrated Energy (PASum), and Time of Arrival (TOA).
 * - Real-time QCustomPlot graphics rendering on a dedicated worker QThread.
 * - Dynamic histogram binning and distribution curve generation.
 *
 * @author eLynxSDR Development Team
 * @version 0.0.2
 */

#ifndef PLOTFUNCTION_H
#define PLOTFUNCTION_H

#include <QObject>
#include <QThread>
#include "components/chart/qcustomplot.h"
#include <unordered_map>
#include <iostream>
#include "components/GLChart/QGLchart.h"

/**
 * @class plotfunction
 * @brief Threaded controller generating pulse parameter plots and distribution histograms.
 */
class plotfunction : public QThread
{
    Q_OBJECT
    int plottingValues(const double *val, QCustomPlot *plot, QVector<double> *x, QVector<double> *y, double min, double max, int count);
    void fillHistoramArray(int type, QCustomPlot *plot, double *val, int cnt);
    /**
     * @brief Executes the plot histogram procedure for plotfunction.
     * @param plot Input parameter specifying plot (QCustomPlot).
     * @param std::unordered_map<double Input parameter specifying std::unordered_map<double (std::unordered_map<double).
     * @param histogramData Input parameter specifying histogramData (double>).
     */
    void plotHistogram(QCustomPlot *plot, std::unordered_map<double, double> *histogramData);

    QCPAxisRect * wideAxisRect = nullptr;
    QCPBars     * bars1        = nullptr;
    QCPGraph    * mainGraph;
    QCustomPlot * plot;

public:
    /**
     * @brief Constructs a new plotfunction instance and initializes subsystem components and UI state.
     * @param parent Input parameter specifying parent (QObject).
     */
    explicit plotfunction(QObject *parent = nullptr);
    /**
     * @brief Destructor for plotfunction, releasing allocated hardware resources, timers, and buffers.
     */
    ~plotfunction();
    double centerFrq{};
    /**
     * @brief Executes the maximum finder procedure for plotfunction.
     * @param  Input parameter specifying  (double).
     * @param  Input parameter specifying  (double).
     * @return Output value of type double.
     */
    double maximumFinder(double *, double *);
    /**
     * @brief Executes the minimum finder procedure for plotfunction.
     * @param  Input parameter specifying  (double).
     * @param  Input parameter specifying  (double).
     * @return Output value of type double.
     */
    double minimumFinder(double *, double *);

    double frqMax{0};
    double PAPeakMax{0};
    double PASumMax{0};
    double PRIMax{0};
    double PRIMin{9999999999};
    double PW1Max{0};
    double PW2Max{0};
    double TOAMax{0};
    double ChannelMax{0};

    double xMax{};
    double xMin{9999999999};
    double yMax{};
    double yMin{};
    bool hasHistogram{false};

    int totalNumber{};
    int PAPeakCnt{-1};
    int PASumCnt{};
    int PRICnt{};
    int PW1Cnt{};
    int PW2Cnt{-1};
    int TOACnt{-1};
    int ChannelCnt{-1};
    int clickCnt{};
    int frqValue{};

    /**
     * @brief Sets the plot parameter and updates internal state.
     * @param  Input parameter specifying  (QCustomPlot).
     */
    void setPlot(QCustomPlot *);

    /**
     * @brief Sets the histo plot parameter and updates internal state.
     * @param  Input parameter specifying  (QCustomPlot).
     */
    void setHistoPlot(QCustomPlot *);
    /**
     * @brief Executes the creating time plot procedure for plotfunction.
     */
    void creatingTimePlot();
    /**
     * @brief Executes the creating histogram plot procedure for plotfunction.
     */
    void creatingHistogramPlot();

    QMap<double, int> tmpMap;
    QMap<double, double> tmp2Map;
    //    QMap<double, int>::const_iterator it;
//    std::unordered_map<int, double> tmp2Map;
    //    std::unordered_map<double, double> priMap;
    //    std::unordered_map<double, double> piSumMap;

    QVector<double> xAxisVal,     yAxisVal;
    QVector<double*> PW1,     yPW1;
    QVector<double*> PW2,     yPW2;
    QVector<double*> PAPEAK,  yPAPEAK;
    QVector<double*> PASUM,   yPASUM;
    QVector<double*> TOA,     yTOA;
    QVector<double*> CHANNEL, yCHANNEL;
    QVector<double*> PRI,     yPRI;
    QTimer dataTimer;
    QCustomPlot * customPlot = nullptr;
    QCustomPlot * histogramPlot = nullptr;

    QTime timeStart = QTime::currentTime();
    double lastPointKey = 0;
    double lastFpsKey;
    int frameCount;
    //    QVector<double>
    QString plotName;

    QVector2D  vec;
    QCustomSeries a;
    int cnt{};

    /**
     * @brief Calculates and returns the mathematical calculate histogram formulation.
     * @param  Input parameter specifying  (double).
     */
    void calculateHistogram(double *);
    //    void histPlot();
public slots:
    //    void realtimeDataSlot();
    //    void ValueSlot(double,QCustomPlot *);
    //    void ValueSlot();

    void mainPlot(double,double);
    //    void histoPlot(double,double);
    void pltGetDataSlot(double x, double y);
signals:
    void pltShow(QCustomPlot*);
    void histoMainPlotSignal( double, double);
    void histoBarPlotSignal (QVector<double>, QVector<double>);

    /**
     * @brief Executes the plt update signal procedure for plotfunction.
     * @param QCustomSeries Input parameter specifying QCustomSeries (QCustomSeries).
     */
    void pltUpdateSignal(QCustomSeries);

    // QThread interface
protected:
    void run();
};

#endif // PLOTFUNCTION_H
