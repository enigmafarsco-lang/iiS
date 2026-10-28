/**
 * @file pulsePage.h
 * @brief Subsystem module for pages in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

﻿#ifndef PULSEPAGE_H
#define PULSEPAGE_H

#include "../components/sattarutils.h"
#include "../components/constant.h"
#include "../components/Store.h"



namespace Ui {
/**
 * @class PulsePage
 * @brief High-level controller class for PulsePage.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class PulsePage;
}



/**
 * @class PulsePage
 * @brief High-level controller class for PulsePage.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class PulsePage : public QWidget
{
    Q_OBJECT

public:

    /**
     * @brief Constructs a new PulsePage instance and initializes subsystem components and UI state.
     * @param parent Input parameter specifying parent (QWidget).
     * @param radarData Input parameter specifying radarData (radarSpace::radarDataStruct *).
     */
    explicit PulsePage(QWidget *parent = nullptr, radarSpace::radarDataStruct * radarData= nullptr);
    radarSpace::radarDataStruct * radarData;
    /**
     * @brief Destructor for PulsePage, releasing allocated hardware resources, timers, and buffers.
     */
    ~PulsePage();


private:
    Ui::PulsePage *ui;

    SattarUtils * utils;


private slots:
    /**
     * @brief Slot handler responding to on btn ok clicked event.
     */
    void on_btnOk_clicked();
    /**
     * @brief Executes the btn green procedure for PulsePage.
     */
    void btnGreen();
    /**
     * @brief Executes the btn red procedure for PulsePage.
     */
    void btnRed();

public slots:


signals:
    /**
     * @brief Executes the period type signal procedure for PulsePage.
     * @param priSpace::periodicTypeEnum Input parameter specifying priSpace::periodicTypeEnum (priSpace::periodicTypeEnum).
     */
    void periodTypeSignal(priSpace::periodicTypeEnum);
    /**
     * @brief Executes the modul type signal procedure for PulsePage.
     * @param modulSpace::modeTypeEnum Input parameter specifying modulSpace::modeTypeEnum (modulSpace::modeTypeEnum).
     */
    void modulTypeSignal(modulSpace::modeTypeEnum);
    /**
     * @brief Executes the pri type signal procedure for PulsePage.
     * @param priSpace::priTypeEnum Input parameter specifying priSpace::priTypeEnum (priSpace::priTypeEnum).
     */
    void priTypeSignal(priSpace::priTypeEnum);
    /**
     * @brief Executes the frq type signal procedure for PulsePage.
     * @param frqSpace::typeEnum Input parameter specifying frqSpace::typeEnum (frqSpace::typeEnum).
     */
    void frqTypeSignal(frqSpace::typeEnum);
    /**
     * @brief Executes the pw type signal procedure for PulsePage.
     * @param pwSpace::pwTypeEnum Input parameter specifying pwSpace::pwTypeEnum (pwSpace::pwTypeEnum).
     */
    void pwTypeSignal(pwSpace::pwTypeEnum);
};

#endif // PULSEPAGE_H
