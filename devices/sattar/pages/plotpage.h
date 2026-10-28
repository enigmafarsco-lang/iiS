// ============================================================================
// File: plotpage.h
// Subsystem: eLynxSDR pages
// Description: Subsystem implementation and interface guidance for plotpage.h.
// ============================================================================

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
    // Constructor: Initializes PlotPage subsystem components, default parameters, and state.
    explicit PlotPage(QWidget *parent = nullptr);
    // Destructor: Cleans up PlotPage resources, timers, and allocated memory.
    ~PlotPage();

private:
    Ui::PlotPage *ui;
    QCustomPlot * plotCustom;
    // Executes test plot routine.
    void testPlot();
};

#endif // PLOTPAGE_H
