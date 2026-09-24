#include "plotsignal.h"
#include "ui_plotsignal.h"

PlotSignal::PlotSignal(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotSignal)
{
    ui->setupUi(this);
    plotFile();
}



PlotSignal::~PlotSignal()
{
    delete ui;
    //    delete fftInstance;
    //    delete panoramaChart;
    //    delete panoramaChart2;

}


void PlotSignal::plotFile()
{
    //------ firsr plot ----------------
    gbPanoramaCharts = new QGroupBox();
    QVBoxLayout *vbLayoutPanorama = new QVBoxLayout();
    QVBoxLayout *vert = new QVBoxLayout();

    panoramaChart = new QGLchart();
    vectorOfCharts.append(panoramaChart);
    setQGlchartsParameters(panoramaChart, ChartType::Spectrum);
    vbLayoutPanorama->addWidget(panoramaChart);

    gbPanoramaCharts->setLayout(vbLayoutPanorama);
    gbPanoramaCharts->setTitle("Normal Data");
    vert->addWidget(gbPanoramaCharts);
    connect(this, SIGNAL(createPlotSignal(QCustomSeries)), panoramaChart, SLOT(updateSeriesData(QCustomSeries)));
    ui->widget->setLayout(vert);

    //------ second plot ----------------
    gbPanoramaCharts2 = new QGroupBox();
    QVBoxLayout *vbLayoutPanorama1 = new QVBoxLayout();
    QVBoxLayout *vert1 = new QVBoxLayout();

    panoramaChart2 = new QGLchart();
    vectorOfCharts.append(panoramaChart2);
    setQGlchartsParameters(panoramaChart2, ChartType::Spectrum);
    vbLayoutPanorama1->addWidget(panoramaChart2);

    gbPanoramaCharts2->setLayout(vbLayoutPanorama1);
    gbPanoramaCharts2->setTitle("Frequency");
    vert1->addWidget(gbPanoramaCharts2);
    connect(this, SIGNAL(createPlotSignal2(QCustomSeries)), panoramaChart2, SLOT(updateSeriesData(QCustomSeries)));
    ui->widget_2->setLayout(vert1);

    zoomPlot(vectorOfCharts);
}




void PlotSignal::setQGlchartsParameters(QGLchart *glChart, ChartType glChartType)
{
    glChart->setMinimumSize(QSize(200, 140));
    QCustomAxis *verticalAxis = new QCustomAxis(this);
    verticalAxis->setAlignment(Qt::AlignLeft);
    verticalAxis->setOrientation(Qt::Vertical);
    verticalAxis->setGridLineColor(Qt::green);
    //    verticalAxis->setTitleText("Amplitude");
    //    verticalAxis->setLabelsText("        (dBm)");
    verticalAxis->setVisible(true);
    glChart->addAxis(verticalAxis, Qt::AlignLeft);

    QCustomAxis *horizantalAxis = new QCustomAxis(this);
    horizantalAxis->setAlignment(Qt::AlignBottom);
    horizantalAxis->setOrientation(Qt::Horizontal);
    horizantalAxis->setGridLineColor(Qt::green);
    //    horizantalAxis->setTitleText("Frequency");
    //    horizantalAxis->setLabelsText("(MHz)");
    glChart->addAxis(horizantalAxis, Qt::AlignBottom);
    glChart->enableMasking(true);
    glChart->setMaskingModifiable(true);
    glChart->setMouseTracking(true);
    glChart->setVerticalUpperRangeMargin(1);
    glChart->setVerticalLowerRangeMargin(1.5);
    glChart->enableToolTip(true);
    glChart->setChartType(glChartType);
    glChart->enableSyncedZoom(false);
    //    glChart->zoomEnable(true);
}

void PlotSignal::on_pushButton_clicked()
{

//    QString filename = QFileDialog::getOpenFileName(this, "Choose File");

//    if(filename.isEmpty()) return ;
//    QFile file(filename);

    QFile file(SattarUtils::filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return ;
    QTextStream in(&file);

    int i{};
    x.clear();
    y.clear();

    data_in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*SattarUtils::N);
    QString line;
    QStringList strList;

    vector<complex<double>> vex ;

    QCustomSeries normalSeriesVal;
    QVector2D v2d;

    while (!in.atEnd())
    {
        line = in.readLine();
        strList = line.split(",");

        if (strList[0] == "TEXT") continue;

        //---first plot
        v2d.setX(i);
        v2d.setY(strList[1].toDouble());
        normalSeriesVal.addPoints(v2d);

        //---second plot
        data_in[i][0] = strList[0].toDouble();
        data_in[i][1] = strList[1].toDouble();

        i++;
    }

    QCustomSeries fftSeriesVal = calculateFFTW();
    file.close();
    emit createPlotSignal(normalSeriesVal);
    emit createPlotSignal2(fftSeriesVal);

}

QCustomSeries PlotSignal::calculateFFTW()
{
    QCustomSeries fftSeriesVal;
    QVector2D v2d;

    data_out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*SattarUtils::N);

    fftw_plan p;
    p = fftw_plan_dft_1d(SattarUtils::N,data_in,data_out,FFTW_FORWARD,FFTW_ESTIMATE);
    fftw_execute(p) ;
    fftw_destroy_plan(p) ;
    fftw_free(data_in) ;
    fftw_free(data_out) ;

    double Fs = 1250000000 ;

    for(int  i = 0  ; i < SattarUtils::N ; i++)
    {
        v2d.setY( pow(data_out[i][0],2) + pow(data_out[i][1],2) );
        v2d.setX( i );
        fftSeriesVal.addPoints(v2d);
    }

    return fftSeriesVal;
}


void PlotSignal::zoomPlot(QVector<QGLchart *>&vectorOfCharts)
{
    for(int i = 0; i < vectorOfCharts.length(); i++)
        vectorOfCharts[i]->zoomEnable(!vectorOfCharts[i]->isZoomed());
}
