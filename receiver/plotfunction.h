#ifndef PLOTFUNCTION_H
#define PLOTFUNCTION_H

#include <QObject>
#include <QThread>
#include "components/chart/qcustomplot.h"
#include <unordered_map>
#include "iostream"
#include "components/GLChart/QGLchart.h"

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
    explicit plotfunction(QObject *parent =      nullptr);
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

    void setPlot(QCustomPlot *);

    void setHistoPlot(QCustomPlot *);
    void creatingTimePlot();
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

    void pltUpdateSignal(QCustomSeries);


    // QThread interface
protected:
    void run();
};

#endif // PLOTFUNCTION_H
