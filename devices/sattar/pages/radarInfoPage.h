/**
 * @file radarInfoPage.h
 * @brief Subsystem module for pages in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef RADARINFOPAGE_H
#define RADARINFOPAGE_H

#include <QWidget>
#include <iostream>



namespace Ui {
/**
 * @class RadarInfoPage
 * @brief High-level controller class for RadarInfoPage.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class RadarInfoPage;
}

/**
 * @class RadarInfoPage
 * @brief High-level controller class for RadarInfoPage.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class RadarInfoPage : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new RadarInfoPage instance and initializes subsystem components and UI state.
     * @param parent Input parameter specifying parent (QWidget).
     */
    explicit RadarInfoPage(QWidget *parent = nullptr);
    /**
     * @brief Destructor for RadarInfoPage, releasing allocated hardware resources, timers, and buffers.
     */
    ~RadarInfoPage();

private:
    Ui::RadarInfoPage *ui;
    /**
     * @brief Initializes and configures the init for the RadarInfoPage subsystem.
     */
    void init();
    int radarOperation = 0;


private slots:

    /**
     * @brief Slot handler responding to on rad  s band clicked event.
     */
    void on_rad_SBand_clicked();
    /**
     * @brief Slot handler responding to on rad  x band clicked event.
     */
    void on_rad_XBand_clicked();
    /**
     * @brief Slot handler responding to on rad  c band clicked event.
     */
    void on_rad_CBand_clicked();
    /**
     * @brief Slot handler responding to on rad  k band clicked event.
     */
    void on_rad_KBand_clicked();

    /**
     * @brief Slot handler responding to on btn ok clicked event.
     */
    void on_btnOk_clicked();

signals:

    /**
     * @brief Executes the btn clicked signal procedure for RadarInfoPage.
     * @param int Input parameter specifying int (int).
     */
    void btnClickedSignal(int);
};

#endif // RADARINFOPAGE_H
