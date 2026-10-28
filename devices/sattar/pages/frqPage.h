/**
 * @file frqPage.h
 * @brief Subsystem module for pages in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

﻿#ifndef FRQPAGE_H
#define FRQPAGE_H

#include <QWidget>
#include <QComboBox>

#include "../components/Store.h"
#include "../components/constant.h"
#include "../components/sattarutils.h"

namespace Ui {
/**
 * @class FrqPage
 * @brief High-level controller class for FrqPage.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class FrqPage;
}

/**
 * @class FrqPage
 * @brief High-level controller class for FrqPage.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class FrqPage : public QWidget
{
    Q_OBJECT

public:
    explicit FrqPage(QWidget *parent = nullptr,
                     frqSpace::frqDataStruct             * frqData   = nullptr,
                     modulSpace::modulDataStruct         * modulData = nullptr,
                     radarSpace::radarDataStruct         * radarData = nullptr,
                     priSpace::priDataStruct             * priData   = nullptr,
                     pwSpace::pwDataStruct               * pwData    = nullptr,
                     spureSpace::spureStruct             * spureData = nullptr,
                     projectDataSpace::projectDataStruct * prjData   = nullptr
            );

    radarSpace::radarDataStruct         * radarData;
    spureSpace::spureStruct             * spureData;
    modulSpace::modulDataStruct         * modulData;
    projectDataSpace::projectDataStruct * prjData;
    frqSpace::frqDataStruct             * frqData;
    priSpace::priDataStruct             * priData;
    pwSpace::pwDataStruct               * pwData;

    /**
     * @brief Destructor for FrqPage, releasing allocated hardware resources, timers, and buffers.
     */
    ~FrqPage();
SattarUtils * utils = new SattarUtils;
private:
    Ui::FrqPage *ui;


    /**
     * @brief Executes the generate data procedure for FrqPage.
     * @param startBand Input parameter specifying startBand (uint).
     * @param bw Input parameter specifying bw (uint).
     * @param frqData Input parameter specifying frqData (frqSpace::frqDataStruct).
     * @return Output value of type bool.
     */
    bool generateData(uint startBand, uint bw, frqSpace::frqDataStruct &frqData);
    /**
     * @brief Executes the regenerate pulse frq procedure for FrqPage.
     * @param count Input parameter specifying count (uint).
     * @param frqData Input parameter specifying frqData (frqSpace::frqDataStruct &).
     * @return Output value of type int.
     */
    int regeneratePulseFrq(uint count, frqSpace::frqDataStruct & frqData);
    /**
     * @brief Executes the agile calc procedure for FrqPage.
     * @param float& Input parameter specifying float& (float&).
     * @return Output value of type bool.
     */
    bool agileCalc(float&);
    /**
     * @brief Executes the frq table value procedure for FrqPage.
     * @return Output value of type bool.
     */
    bool frqTableValue();
    bool firstRun{true};
    double frqCenter{};
    double alpha = 2.555*2;
    int row{};

public slots:
    /**
     * @brief Executes the frq type slot procedure for FrqPage.
     * @param frqSpace::typeEnum Input parameter specifying frqSpace::typeEnum (frqSpace::typeEnum).
     */
    void frqTypeSlot(frqSpace::typeEnum);
    /**
     * @brief Executes the change lable procedure for FrqPage.
     */
    void changeLable();
    /**
     * @brief Sets the frq center slot parameter and updates internal state.
     * @param double Input parameter specifying double (double).
     */
    void setFrqCenterSlot(double);

private slots:
    /**
     * @brief Slot handler responding to on tbl frq cell changed event.
     * @param row Input parameter specifying row (int).
     * @param column Input parameter specifying column (int).
     */
    void on_tblFrq_cellChanged(int row, int column);
    /**
     * @brief Slot handler responding to on btn clear clicked event.
     */
    void on_btnClear_clicked();
    /**
     * @brief Slot handler responding to on btn add clicked event.
     */
    void on_btnAdd_clicked();
    /**
     * @brief Slot handler responding to on btn ok clicked event.
     */
    void on_btnOk_clicked();
    /**
     * @brief Executes the btn green procedure for FrqPage.
     */
    void btnGreen();
    /**
     * @brief Executes the btn slot procedure for FrqPage.
     */
    void btnSlot();
    /**
     * @brief Executes the btn red procedure for FrqPage.
     */
    void btnRed();

signals:
    /**
     * @brief Executes the enable modul page procedure for FrqPage.
     */
    void enableModulPage();
    /**
     * @brief Executes the show menu option signal procedure for FrqPage.
     * @param bool Input parameter specifying bool (bool).
     */
    void showMenuOptionSignal(bool);
    void showSelectedRadarInfo(modulSpace::modulDataStruct         &,
                               radarSpace::radarDataStruct         &,
                               frqSpace::frqDataStruct             &,
                               priSpace::priDataStruct             &,
                               pwSpace::pwDataStruct               &,
                               spureSpace::spureStruct             &,
                               projectDataSpace::projectDataStruct &);

};

#endif // FRQPAGE_H
