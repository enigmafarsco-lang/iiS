/**
 * @file plotpage.h
 * @brief Subsystem module for pages in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef PLOTPAGE_H
#define PLOTPAGE_H

#include <QWidget>
#include "components/chart/qcustomplot.h"

namespace Ui {
/**
 * @class PlotPage
 * @brief High-level controller class for PlotPage.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class PlotPage;
}

/**
 * @class PlotPage
 * @brief High-level controller class for PlotPage.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class PlotPage : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new PlotPage instance and initializes subsystem components and UI state.
     * @param parent Input parameter specifying parent (QWidget).
     */
    explicit PlotPage(QWidget *parent = nullptr);
    /**
     * @brief Destructor for PlotPage, releasing allocated hardware resources, timers, and buffers.
     */
    ~PlotPage();

private:
    Ui::PlotPage *ui;
    QCustomPlot * plotCustom;
    /**
     * @brief Executes the test plot procedure for PlotPage.
     */
    void testPlot();
};

#endif // PLOTPAGE_H
