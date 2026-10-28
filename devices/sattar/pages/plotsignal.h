/**
 * @file plotsignal.h
 * @brief Subsystem module for pages in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

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
/**
 * @class PlotSignal
 * @brief High-level controller class for PlotSignal.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class PlotSignal;
}

/**
 * @class PlotSignal
 * @brief High-level controller class for PlotSignal.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class PlotSignal : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new PlotSignal instance and initializes subsystem components and UI state.
     * @param parent Input parameter specifying parent (QWidget).
     */
    explicit PlotSignal(QWidget *parent = nullptr);

    /**
     * @brief Destructor for PlotSignal, releasing allocated hardware resources, timers, and buffers.
     */
    ~PlotSignal();

private slots:
    /**
     * @brief Slot handler responding to on push button clicked event.
     */
    void on_pushButton_clicked();


private:
    Ui::PlotSignal *ui;
    fftCalculator * fftInstance = new fftCalculator;

    /**
     * @brief Initializes and configures the init for the PlotSignal subsystem.
     */
    void init();
    /**
     * @brief Executes the zoom plot procedure for PlotSignal.
     * @param  Input parameter specifying  (QVector<QGLchart *>).
     */
    void zoomPlot(QVector<QGLchart *> &);
    /**
     * @brief Sets the q glcharts parameters parameter and updates internal state.
     * @param glChart Input parameter specifying glChart (QGLchart).
     * @param glChartType Input parameter specifying glChartType (ChartType).
     */
    void setQGlchartsParameters(QGLchart *glChart, ChartType glChartType);
    /**
     * @brief Calculates and returns the mathematical calculate f f t w formulation.
     * @return Output value of type QCustomSeries.
     */
    QCustomSeries calculateFFTW();
    /**
     * @brief Executes the plot file procedure for PlotSignal.
     */
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
    /**
     * @brief Executes the create plot signal2 procedure for PlotSignal.
     * @param QCustomSeries Input parameter specifying QCustomSeries (QCustomSeries).
     */
    void createPlotSignal2(QCustomSeries);

};

#endif // PLOTSIGNAL_H
