// ============================================================================
// File: plotsignal.h
// Subsystem: eLynxSDR pages
// Description: Subsystem implementation and interface guidance for plotsignal.h.
// ============================================================================

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
    // Constructor: Initializes PlotSignal subsystem components, default parameters, and state.
    explicit PlotSignal(QWidget *parent = nullptr);

    // Destructor: Cleans up PlotSignal resources, timers, and allocated memory.
    ~PlotSignal();

private slots:
    // Event/Signal handler for on push button clicked.
    void on_pushButton_clicked();


private:
    Ui::PlotSignal *ui;
    fftCalculator * fftInstance = new fftCalculator;

    // Initializes init for PlotSignal.
    void init();
    // Executes zoom plot routine.
    void zoomPlot(QVector<QGLchart *> &);
    // Sets q glcharts parameters value and updates internal subsystem configuration.
    void setQGlchartsParameters(QGLchart *glChart, ChartType glChartType);
    // Computes mathematical calculate f f t w calculation.
    QCustomSeries calculateFFTW();
    // Executes plot file routine.
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
    // Executes create plot signal routine.
    void createPlotSignal(QCustomSeries);
    // Executes create plot signal2 routine.
    void createPlotSignal2(QCustomSeries);

};

#endif // PLOTSIGNAL_H
