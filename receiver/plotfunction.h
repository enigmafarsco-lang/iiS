/**
 * @file plotfunction.h
 * @brief Threaded DSP Plot & Histogram Processor for Radar Pulse Parameter Visualizations.
 *
 * plotfunction provides real-time time-domain, frequency-domain, and statistical
 * histogram calculations for radar pulse characteristics (TOA, PW, PRI, Peak Power, Frequency).
 * It runs in a dedicated worker thread to prevent GUI latency during high PRF signal streams.
 *
 * @author eLynxSDR Development Team
 * @version 0.0.2
 */

#ifndef PLOTFUNCTION_H
#define PLOTFUNCTION_H

#include <QObject>
#include <QThread>
#include <QMap>
#include <QVector>
#include <unordered_map>
#include <iostream>

#include "components/chart/qcustomplot.h"
#include "components/GLChart/QGLchart.h"

/**
 * @class plotfunction
 * @brief Multi-threaded radar pulse parameter calculator and histogram renderer.
 */
class plotfunction : public QThread
{
    Q_OBJECT

    int plottingValues(const double *val, QCustomPlot *plot, QVector<double> *x, QVector<double> *y, double min, double max, int count);
    void fillHistoramArray(int type, QCustomPlot *plot, double *val, int cnt);
    void plotHistogram(QCustomPlot *plot, std::unordered_map<double, double> *histogramData);

    QCPAxisRect * wideAxisRect = nullptr;
    QCPBars     * bars1        = nullptr;
    QCPGraph    * mainGraph;
    QCustomPlot * plot;

public:
    /**
     * @brief Constructs plotfunction worker.
     * @param parent Optional parent QObject pointer.
     */
    explicit plotfunction(QObject *parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~plotfunction();

    double centerFrq{};
    double maximumFinder(double *, double *);
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
     * @brief Binds target QCustomPlot canvas.
     * @param p Pointer to QCustomPlot widget.
     */
    void setPlot(QCustomPlot *p);

    /**
     * @brief Binds target histogram QCustomPlot canvas.
     * @param p Pointer to QCustomPlot widget.
     */
    void setHistoPlot(QCustomPlot *p);

    /**
     * @brief Creates time-domain plot setup.
     */
    void creatingTimePlot();

    /**
     * @brief Creates histogram plot setup.
     */
    void creatingHistogramPlot();

    QMap<double, int> tmpMap;
    QMap<double, double> tmp2Map;

public slots:
    /**
     * @brief Main slot ingesting incoming pulse telemetry data.
     * @param val Raw value string.
     * @param mode Measurement type (1=FRQ, 2=PAPeak, 3=PASum, 4=PRI, 5=PW, 6=PW2, 7=TOA, 8=Channel).
     */
    void ValueSlot(QString val, int mode);

    /**
     * @brief Clears accumulated plot buffers.
     */
    void clearPlot();

signals:
    /**
     * @brief Emitted to trigger UI repaint.
     */
    void replotSignal();
};

#endif // PLOTFUNCTION_H
