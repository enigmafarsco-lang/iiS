#ifndef PLOTPAGE_H
#define PLOTPAGE_H

#include <QWidget>
#include "components/chart/qcustomplot.h"

namespace Ui {
class PlotPage;
}

class PlotPage : public QWidget
{
    Q_OBJECT

public:
    explicit PlotPage(QWidget *parent = nullptr);
    ~PlotPage();

private:
    Ui::PlotPage *ui;
    QCustomPlot * plotCustom;
    void testPlot();
};

#endif // PLOTPAGE_H
