/**
 * @file tuner.h
 * @brief Subsystem module for tuner in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

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
/**
 * @class Tuner
 * @brief High-level controller class for Tuner.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class Tuner;
}

/**
 * @class Tuner
 * @brief High-level controller class for Tuner.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class Tuner : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new Tuner instance and initializes subsystem components and UI state.
     * @param parent Input parameter specifying parent (QWidget).
     */
    explicit Tuner(QWidget *parent = nullptr);
    /**
     * @brief Destructor for Tuner, releasing allocated hardware resources, timers, and buffers.
     */
    ~Tuner();

private:
    Ui::Tuner *ui;
    /**
     * @brief Executes the tuner series procedure for Tuner.
     * @return Output value of type QCustomSeries.
     */
    QCustomSeries tunerSeries();

    /**
     * @brief Executes the plot init procedure for Tuner.
     */
    void plotInit();
    QVector<QGLchart *> vectorOfCharts;
    /**
     * @brief Sets the q glcharts parameters parameter and updates internal state.
     * @param glChart Input parameter specifying glChart (QGLchart).
     * @param glChartType Input parameter specifying glChartType (ChartType).
     */
    void setQGlchartsParameters(QGLchart *glChart, ChartType glChartType);
    int counter{};
    QTimer timerAutoSend;
    QByteArray temp;
    int sizeCount{};
    int timerCount{};
    /**
     * @brief Initializes and configures the init for the Tuner subsystem.
     */
    void init();
    int yAxiosCounter{};
    bool firstRun{true};
    /**
     * @brief Executes the remove all points procedure for Tuner.
     */
    void removeAllPoints();
    /**
     * @brief Executes the emit to plot procedure for Tuner.
     */
    void emitToPlot();
    /**
     * @brief Executes the filling plot vector procedure for Tuner.
     * @param frame Input parameter specifying frame (QStringList).
     */
    void fillingPlotVector(QStringList &frame);
    int removeNumber{100};
    /**
     * @brief Sets the vector to qcustom parameter and updates internal state.
     * @param QVector2D& Input parameter specifying QVector2D& (QVector2D&).
     * @param QCustomSeries& Input parameter specifying QCustomSeries& (QCustomSeries&).
     * @param int Input parameter specifying int (int).
     */
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
    /**
     * @brief Slot handler responding to on  device response slot event.
     * @param QByteArray Input parameter specifying QByteArray (QByteArray).
     */
    void on_DeviceResponseSlot(QByteArray);
    /**
     * @brief Retrieves the current evice connected slot value.
     * @param bool Input parameter specifying bool (bool).
     */
    void isDeviceConnectedSlot(bool);
    /**
     * @brief Executes the joshan func data slot procedure for Tuner.
     * @param QString Input parameter specifying QString (QString).
     */
    void joshanFuncDataSlot(QString);


signals:
    /**
     * @brief Executes the create plot signal1 procedure for Tuner.
     * @param QCustomSeries Input parameter specifying QCustomSeries (QCustomSeries).
     */
    void createPlotSignal1(QCustomSeries);
    /**
     * @brief Executes the create plot signal2 procedure for Tuner.
     * @param QCustomSeries Input parameter specifying QCustomSeries (QCustomSeries).
     */
    void createPlotSignal2(QCustomSeries);
    /**
     * @brief Executes the create plot signal3 procedure for Tuner.
     * @param QCustomSeries Input parameter specifying QCustomSeries (QCustomSeries).
     */
    void createPlotSignal3(QCustomSeries);
    /**
     * @brief Executes the create plot signal4 procedure for Tuner.
     * @param QCustomSeries Input parameter specifying QCustomSeries (QCustomSeries).
     */
    void createPlotSignal4(QCustomSeries);
    /**
     * @brief Executes the create plot signal5 procedure for Tuner.
     * @param QCustomSeries Input parameter specifying QCustomSeries (QCustomSeries).
     */
    void createPlotSignal5(QCustomSeries);
    /**
     * @brief Executes the create plot signal6 procedure for Tuner.
     * @param QCustomSeries Input parameter specifying QCustomSeries (QCustomSeries).
     */
    void createPlotSignal6(QCustomSeries);
    /**
     * @brief Executes the create plot signal7 procedure for Tuner.
     * @param QCustomSeries Input parameter specifying QCustomSeries (QCustomSeries).
     */
    void createPlotSignal7(QCustomSeries);
    /**
     * @brief Executes the create plot signal8 procedure for Tuner.
     * @param QCustomSeries Input parameter specifying QCustomSeries (QCustomSeries).
     */
    void createPlotSignal8(QCustomSeries);
    /**
     * @brief Executes the create plot signal9 procedure for Tuner.
     * @param QCustomSeries Input parameter specifying QCustomSeries (QCustomSeries).
     */
    void createPlotSignal9(QCustomSeries);
    /**
     * @brief Executes the create plot signal10 procedure for Tuner.
     * @param QCustomSeries Input parameter specifying QCustomSeries (QCustomSeries).
     */
    void createPlotSignal10(QCustomSeries);
    /**
     * @brief Executes the create plot signal11 procedure for Tuner.
     * @param QCustomSeries Input parameter specifying QCustomSeries (QCustomSeries).
     */
    void createPlotSignal11(QCustomSeries);

private slots:
    /**
     * @brief Slot handler responding to on push button 5 clicked event.
     */
    void on_pushButton_5_clicked();
    /**
     * @brief Updates and synchronizes the update auto send data stream and visual indicators.
     */
    void updateAutoSend();


    /**
     * @brief Slot handler responding to on btn start clicked event.
     */
    void on_btnStart_clicked();

    /**
     * @brief Slot handler responding to on btn set frq clicked event.
     */
    void on_btnSetFrq_clicked();

signals:
    /**
     * @brief Transmits send command to device signal command/packet over the communication bus.
     * @param QByteArray Input parameter specifying QByteArray (QByteArray).
     */
    void sendCommandToDeviceSignal(QByteArray);
};

#endif // TUNER_H
