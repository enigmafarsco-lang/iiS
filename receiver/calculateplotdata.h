//#ifndef CALCULATEPLOTDATA_H
//#define CALCULATEPLOTDATA_H

//#include <QObject>
//#include <QThread>
//#include <QVector>
//#include <iostream>
//#include <unordered_map>

//#include "components/chart/qcustomplot.h"


//class calculatePlotData : public QThread
//{
//    Q_OBJECT




//    void ValueSlot(QString *val, int mode);
//    double centerFrq{};
//    double maximumFinder(double *, double *);
//    double minimumFinder(double *, double *);
//    void plotHistogram(QCustomPlot *plot, std::unordered_map<double, double> *histogramData);
//    int plottingValues(const double *val, QCustomPlot *plot, QVector<double> *x, QVector<double> *y, double min, double max, int count);
//    void fillHistoramArray(int type, QCustomPlot *plot, double *val, int cnt);



//    double frqMax{0};
//    double PAPeakMax{0};
//    double PASumMax{0};
//    double PRIMax{0};
//    double PRIMin{9999999999};
//    double PW1Max{0};
//    double PW2Max{0};
//    double TOAMax{0};
//    double ChannelMax{0};

//    double xMax{};
//    double xMin{9999999999};
//    double yMax{};
//    double yMin{};

//    int frqCnt{};
//    int PAPeakCnt{-1};
//    int PASumCnt{};
//    int PRICnt{};
//    int PW1Cnt{};
//    int PW2Cnt{-1};
//    int TOACnt{-1};
//    int ChannelCnt{-1};
//    int clickCnt{};
//    int frqValue{};


//    QVector<double> xFrq,     yFrq;
//    QVector<double> xPW1,     yPW1;
//    QVector<double> xPW2,     yPW2;
//    QVector<double> xPAPEAK,  yPAPEAK;
//    QVector<double> xPASUM,   yPASUM;
//    QVector<double> xTOA,     yTOA;
//    QVector<double> xCHANNEL, yCHANNEL;
//    QVector<double> xPRI,     yPRI;

//    std::unordered_map<double, double> frqMap;
//    std::unordered_map<double, double> pwMap;
//    std::unordered_map<double, double> priMap;
//    std::unordered_map<double, double> piSumMap;




//public:
//    explicit calculatePlotData(QObject *parent = nullptr);
//    void run();


//signals:

//private slots:
//};

//#endif // CALCULATEPLOTDATA_H
