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
    explicit Tuner(QWidget *parent = nullptr);
    ~Tuner();

private:
    Ui::Tuner *ui;
    QCustomSeries tunerSeries();

    void plotInit();
    QVector<QGLchart *> vectorOfCharts;
    void setQGlchartsParameters(QGLchart *glChart, ChartType glChartType);
    int counter{};
    QTimer timerAutoSend;
    QByteArray temp;
    int sizeCount{};
    int timerCount{};
    void init();
    int yAxiosCounter{};
    bool firstRun{true};
    void removeAllPoints();
    void emitToPlot();
    void fillingPlotVector(QStringList &frame);
    int removeNumber{100};
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
    void on_DeviceResponseSlot(QByteArray);
    void isDeviceConnectedSlot(bool);
    void joshanFuncDataSlot(QString);


signals:
    void createPlotSignal1(QCustomSeries);
    void createPlotSignal2(QCustomSeries);
    void createPlotSignal3(QCustomSeries);
    void createPlotSignal4(QCustomSeries);
    void createPlotSignal5(QCustomSeries);
    void createPlotSignal6(QCustomSeries);
    void createPlotSignal7(QCustomSeries);
    void createPlotSignal8(QCustomSeries);
    void createPlotSignal9(QCustomSeries);
    void createPlotSignal10(QCustomSeries);
    void createPlotSignal11(QCustomSeries);

private slots:
    void on_pushButton_5_clicked();
    void updateAutoSend();


    void on_btnStart_clicked();

    void on_btnSetFrq_clicked();

signals:
    void sendCommandToDeviceSignal(QByteArray);
};

#endif // TUNER_H
