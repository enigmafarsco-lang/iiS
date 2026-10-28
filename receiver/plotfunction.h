// ============================================================================
// File: plotfunction.h
// Subsystem: eLynxSDR receiver
// Description: Subsystem implementation and interface guidance for plotfunction.h.
// ============================================================================

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
    // Executes plotting values routine.
    int plottingValues(const double *val, QCustomPlot *plot, QVector<double> *x, QVector<double> *y, double min, double max, int count);
    // Executes fill historam array routine.
    void fillHistoramArray(int type, QCustomPlot *plot, double *val, int cnt);
    // Executes plot histogram routine.
    void plotHistogram(QCustomPlot *plot, std::unordered_map<double, double> *histogramData);

    QCPAxisRect * wideAxisRect = nullptr;
    QCPBars     * bars1        = nullptr;
    QCPGraph    * mainGraph;
    QCustomPlot * plot;

public:
    // Constructor: Initializes plotfunction subsystem components, default parameters, and state.
    explicit plotfunction(QObject *parent = nullptr);
    // Destructor: Cleans up plotfunction resources, timers, and allocated memory.
    ~plotfunction();
    double centerFrq{};
    // Executes maximum finder routine.
    double maximumFinder(double *, double *);
    // Executes minimum finder routine.
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

    // Sets plot value and updates internal subsystem configuration.
    void setPlot(QCustomPlot *);

    // Sets histo plot value and updates internal subsystem configuration.
    void setHistoPlot(QCustomPlot *);
    // Executes creating time plot routine.
    void creatingTimePlot();
    // Executes creating histogram plot routine.
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

    // Computes mathematical calculate histogram calculation.
    void calculateHistogram(double *);
    //    void histPlot();
public slots:
    //    void realtimeDataSlot();
    //    void ValueSlot(double,QCustomPlot *);
    //    void ValueSlot();

    // Executes main plot routine.
    void mainPlot(double,double);
    //    void histoPlot(double,double);
    void pltGetDataSlot(double x, double y);
signals:
    // Executes plt show routine.
    void pltShow(QCustomPlot*);
    // Executes histo main plot signal routine.
    void histoMainPlotSignal( double, double);
    // Executes histo bar plot signal routine.
    void histoBarPlotSignal (QVector<double>, QVector<double>);

    // Executes plt update signal routine.
    void pltUpdateSignal(QCustomSeries);

    // QThread interface
protected:
    // Executes run routine.
    void run();
};

#endif // PLOTFUNCTION_H
