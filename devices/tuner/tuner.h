// ============================================================================
// File: tuner.h
// Subsystem: eLynxSDR tuner
// Description: Subsystem implementation and interface guidance for tuner.h.
// ============================================================================

#ifndef TUNER_H
#define TUNER_H

#include <QWidget>
#include <QDebug>
#include <QTimer>
#include "components/GLChart/QGLchart.h"
#include "components/GLChart/SetUpTheme.h"
#include "devices/joshan/datastruct.h"
#include "constants/project_constans.h"
#include <QIntValidator>

namespace Ui {
class Tuner;
}

class Tuner : public QWidget
{
    Q_OBJECT

public:
    // Constructor: Initializes Tuner subsystem components, default parameters, and state.
    explicit Tuner(QWidget *parent = nullptr);
    // Destructor: Cleans up Tuner resources, timers, and allocated memory.
    ~Tuner();

private:
    Ui::Tuner *ui;
    // Executes tuner series routine.
    QCustomSeries tunerSeries();

    // Executes plot init routine.
    void plotInit();
    QVector<QGLchart *> vectorOfCharts;
    // Sets q glcharts parameters value and updates internal subsystem configuration.
    void setQGlchartsParameters(QGLchart *glChart, ChartType glChartType);
    int counter{};
    QTimer timerAutoSend;
    QByteArray temp;
    int sizeCount{};
    int timerCount{};
    // Initializes init for Tuner.
    void init();
    int yAxiosCounter{};
    bool firstRun{true};
    // Executes remove all points routine.
    void removeAllPoints();
    // Executes emit to plot routine.
    void emitToPlot();
    // Executes filling plot vector routine.
    void fillingPlotVector(QStringList &frame);
    int removeNumber{100};
    // Sets vector to qcustom value and updates internal subsystem configuration.
    void setVectorToQcustom(QVector2D&, QCustomSeries&, int);
    int LOmaxou{0};
    bool plotIsAllowed{false};
    bool isConnect{false};


    //==========================
    QGroupBox   * group1      = new QGroupBox();
    QVBoxLayout * vBox1       = new QVBoxLayout();
    QVBoxLayout * vert1       = new QVBoxLayout();
    QGLchart    * tunerChart1 = new QGLchart();

    QGroupBox   * group2      = new QGroupBox();
    QVBoxLayout * vBox2       = new QVBoxLayout();
    QVBoxLayout * vert2       = new QVBoxLayout();
    QGLchart    * tunerChart2 = new QGLchart();

    QGroupBox   * group3      = new QGroupBox();
    QVBoxLayout * vBox3       = new QVBoxLayout();
    QVBoxLayout * vert3       = new QVBoxLayout();
    QGLchart    * tunerChart3 = new QGLchart();

    QGroupBox   * group4      = new QGroupBox();
    QVBoxLayout * vBox4       = new QVBoxLayout();
    QVBoxLayout * vert4       = new QVBoxLayout();
    QGLchart    * tunerChart4 = new QGLchart();

    QGroupBox   * group5      = new QGroupBox();
    QVBoxLayout * vBox5       = new QVBoxLayout();
    QVBoxLayout * vert5       = new QVBoxLayout();
    QGLchart    * tunerChart5 = new QGLchart();

    QGroupBox   * group6      = new QGroupBox();
    QVBoxLayout * vBox6       = new QVBoxLayout();
    QVBoxLayout * vert6       = new QVBoxLayout();
    QGLchart    * tunerChart6 = new QGLchart();

    QGroupBox   * group7      = new QGroupBox();
    QVBoxLayout * vBox7       = new QVBoxLayout();
    QVBoxLayout * vert7       = new QVBoxLayout();
    QGLchart    * tunerChart7 = new QGLchart();

    QGroupBox   * group8      = new QGroupBox();
    QVBoxLayout * vBox8       = new QVBoxLayout();
    QVBoxLayout * vert8       = new QVBoxLayout();
    QGLchart    * tunerChart8 = new QGLchart();

    QGroupBox   * group9      = new QGroupBox();
    QVBoxLayout * vBox9       = new QVBoxLayout();
    QVBoxLayout * vert9       = new QVBoxLayout();
    QGLchart    * tunerChart9 = new QGLchart();

    QGroupBox   * group10      = new QGroupBox();
    QVBoxLayout * vBox10       = new QVBoxLayout();
    QVBoxLayout * vert10       = new QVBoxLayout();
    QGLchart    * tunerChart10 = new QGLchart();

    //======================================
    QVector2D   vector1 ;
    QVector2D   vector2 ;
    QVector2D   vector3 ;
    QVector2D   vector4 ;
    QVector2D   vector5 ;
    QVector2D   vector6 ;
    QVector2D   vector7 ;
    QVector2D   vector8 ;
    QVector2D   vector9 ;
    QVector2D   vector10;
    QVector2D   vector11;
    //======================================
    QCustomSeries series1;
    QCustomSeries series2;
    QCustomSeries series3;
    QCustomSeries series4;
    QCustomSeries series5;
    QCustomSeries series6;
    QCustomSeries series7;
    QCustomSeries series8;
    QCustomSeries series9;
    QCustomSeries series10;
    QCustomSeries series11;




public slots:
    // Event/Signal handler for on  device response slot.
    void on_DeviceResponseSlot(QByteArray);
    // Retrieves current evice connected slot state.
    void isDeviceConnectedSlot(bool);
    // Executes joshan func data slot routine.
    void joshanFuncDataSlot(QString);


signals:
    // Executes create plot signal1 routine.
    void createPlotSignal1(QCustomSeries);
    // Executes create plot signal2 routine.
    void createPlotSignal2(QCustomSeries);
    // Executes create plot signal3 routine.
    void createPlotSignal3(QCustomSeries);
    // Executes create plot signal4 routine.
    void createPlotSignal4(QCustomSeries);
    // Executes create plot signal5 routine.
    void createPlotSignal5(QCustomSeries);
    // Executes create plot signal6 routine.
    void createPlotSignal6(QCustomSeries);
    // Executes create plot signal7 routine.
    void createPlotSignal7(QCustomSeries);
    // Executes create plot signal8 routine.
    void createPlotSignal8(QCustomSeries);
    // Executes create plot signal9 routine.
    void createPlotSignal9(QCustomSeries);
    // Executes create plot signal10 routine.
    void createPlotSignal10(QCustomSeries);
    // Executes create plot signal11 routine.
    void createPlotSignal11(QCustomSeries);

private slots:
    // Event/Signal handler for on push button 5 clicked.
    void on_pushButton_5_clicked();
    // Updates and synchronizes update auto send data stream.
    void updateAutoSend();


    // Event/Signal handler for on btn start clicked.
    void on_btnStart_clicked();

    // Event/Signal handler for on btn set frq clicked.
    void on_btnSetFrq_clicked();

signals:
    // Transmits send command to device signal command/packet over communication interface.
    void sendCommandToDeviceSignal(QByteArray);
};

#endif // TUNER_H
