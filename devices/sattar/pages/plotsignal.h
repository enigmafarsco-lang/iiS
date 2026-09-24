#ifndef PLOTSIGNAL_H
#define PLOTSIGNAL_H

#include <QWidget>
#include <fstream>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <stdio.h>
#include "components/chart/qcustomplot.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include "components/GLChart/QGLchart.h"
#include "components/GLChart/SetUpTheme.h"
#include "../components/fft/fftcalculator.h"
#include <fftw3.h>
#include "../components/sattarutils.h"

using namespace std;
namespace Ui {
class PlotSignal;
}

class PlotSignal : public QWidget
{
    Q_OBJECT

public:
    explicit PlotSignal(QWidget *parent = nullptr);

    ~PlotSignal();

private slots:
    void on_pushButton_clicked();


private:
    Ui::PlotSignal *ui;
    fftCalculator * fftInstance = new fftCalculator;

    void init();
    void zoomPlot(QVector<QGLchart *> &);
    void setQGlchartsParameters(QGLchart *glChart, ChartType glChartType);
    QCustomSeries calculateFFTW();
    void plotFile();
    QGLchart *panoramaChart;
    QGLchart *panoramaChart2;
    QString filecontent;
    QVector<double> x,y;
    QVector<QGLchart *> vectorOfCharts;
    vector<double> fftAxios_y, fftAxios_x;
    QString fileContent{};
    fftw_complex *data_in  , *data_out ;
    QGroupBox *gbPanoramaCharts;
    QGroupBox *gbPanoramaCharts2;
    SattarUtils * utils;

    //--config fft function----
//    int arrayDimension{8};
//    double samplingStep{1};
//    int N{SattarUtils::normalSample/SattarUtils::sampleStep};



signals:
    void createPlotSignal(QCustomSeries);
    void createPlotSignal2(QCustomSeries);

};

#endif // PLOTSIGNAL_H
