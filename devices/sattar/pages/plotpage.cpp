#include "plotpage.h"
#include "ui_plotpage.h"

PlotPage::PlotPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotPage)
{
    ui->setupUi(this);
    testPlot();
}

PlotPage::~PlotPage()
{
    delete ui;
}

void PlotPage::testPlot()
{
    plotCustom = new QCustomPlot;

    QVector<double> x(101) , y(110);

    for(int i=0; i<101; i++)
    {
        x[i] = i / 50.0 ;
        y[i] = x[i] * x[i];

    }

    plotCustom->addGraph();

    plotCustom->graph(0)->setData(x,y);
    plotCustom->xAxis->setLabel("x");
    plotCustom->yAxis->setLabel("y");
    plotCustom->xAxis->setRange(-1,1);
    plotCustom->yAxis->setRange(0,1);
    plotCustom->replot();

    QVBoxLayout * test = new QVBoxLayout();
    test->addWidget(plotCustom);
    this->setLayout(test);


}
