/**
 * @file priPage.h
 * @brief Subsystem module for pages in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef PRIPAGE_H
#define PRIPAGE_H

#include <QWidget>
#include <QMessageBox>
#include "../components/constant.h"
#include "../components/Store.h"
#include "../components/sattarutils.h"
#include <iostream>
#include <QList>


namespace Ui {

/**
 * @class PRIPage
 * @brief High-level controller class for PRIPage.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class PRIPage;
}


/**
 * @class PRIPage
 * @brief High-level controller class for PRIPage.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class PRIPage : public QWidget
{
    Q_OBJECT

public:
    explicit PRIPage(QWidget                     * parent    = nullptr,
                     priSpace::priDataStruct     * priData   = nullptr,
                     radarSpace::radarDataStruct * radarData = nullptr);

    priSpace::priDataStruct     * priData ;
    radarSpace::radarDataStruct * radarData ;

    /**
     * @brief Destructor for PRIPage, releasing allocated hardware resources, timers, and buffers.
     */
    ~PRIPage();

private:
    Ui::PRIPage *ui;
    /**
     * @brief Executes the min max p r i procedure for PRIPage.
     * @param data Input parameter specifying data (priSpace::priDataStruct).
     * @param priVal Input parameter specifying priVal (float&).
     * @param float&amplitude Input parameter specifying float&amplitude (float&amplitude).
     * @param double&phaseAngle Input parameter specifying double&phaseAngle (double&phaseAngle).
     * @param double&frq Input parameter specifying double&frq (double&frq).
     * @param double&sampleRate Input parameter specifying double&sampleRate (double&sampleRate).
     */
    void minMaxPRI(priSpace::priDataStruct &data, float& priVal, float&amplitude , double&phaseAngle, double&frq, double&sampleRate);
    /**
     * @brief Executes the gen procedure for PRIPage.
     * @param amp Input parameter specifying amp (float &).
     * @param phaseAng Input parameter specifying phaseAng (double).
     * @param frq Input parameter specifying frq (double).
     * @param sampleRate Input parameter specifying sampleRate (double).
     * @return Output value of type float.
     */
    float Gen(float & amp, double &phaseAng, double &frq, double &sampleRate);
    /**
     * @brief Executes the min max p r i dwell procedure for PRIPage.
     * @param data Input parameter specifying data (priSpace::priDataStruct).
     * @param pris Input parameter specifying pris (float&).
     */
    void minMaxPRIDwell(priSpace::priDataStruct &data, float& pris);
    /**
     * @brief Executes the min max p r i procedure for PRIPage.
     * @param data Input parameter specifying data (priSpace::priDataStruct).
     * @param pris Input parameter specifying pris (float&).
     */
    void minMaxPRI(priSpace::priDataStruct &data, float& pris);
    /**
     * @brief Executes the generate p r i data procedure for PRIPage.
     * @param  Input parameter specifying  (priSpace::priDataStruct).
     * @return Output value of type bool.
     */
    bool  generatePRIData(priSpace::priDataStruct &);
    /**
     * @brief Executes the else function procedure for PRIPage.
     * @param  Input parameter specifying  (priSpace::priDataStruct).
     * @return Output value of type bool.
     */
    bool elseFunction(priSpace::priDataStruct &);
    /**
     * @brief Initializes and configures the init p r i val for the PRIPage subsystem.
     * @param  Input parameter specifying  (priSpace::priDataStruct).
     */
    void initPRIVal(priSpace::priDataStruct &);
    bool isPriTypeChanged{false};
    bool isPeriodChanged{false};
    /**
     * @brief Executes the reset max pulse count procedure for PRIPage.
     */
    void resetMaxPulseCount();
    /**
     * @brief Executes the table level count procedure for PRIPage.
     */
    void tableLevelCount();
    /**
     * @brief Executes the pri table value procedure for PRIPage.
     * @return Output value of type bool.
     */
    bool priTableValue();
    bool firstRun{true};
    SattarUtils * utils;
    /**
     * @brief Executes the disable p r i procedure for PRIPage.
     */
    void disablePRI();
    uint sumCount{};
    /**
     * @brief Executes the btn green procedure for PRIPage.
     */
    void btnGreen();
    double alpha{2.555*2};
    int row{0};


private slots:
    /**
     * @brief Slot handler responding to on tbl p r i one cell changed event.
     * @param row Input parameter specifying row (int).
     * @param column Input parameter specifying column (int).
     */
    void on_tblPRIOne_cellChanged(int row, int column);
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
     * @brief Executes the btn slot procedure for PRIPage.
     */
    void btnSlot();
    /**
     * @brief Executes the btn red procedure for PRIPage.
     */
    void btnRed();

public slots:
    /**
     * @brief Executes the enable disable p r i page slot procedure for PRIPage.
     * @param priSpace::priTypeEnum Input parameter specifying priSpace::priTypeEnum (priSpace::priTypeEnum).
     */
    void enableDisablePRIPageSlot(priSpace::priTypeEnum);
    /**
     * @brief Executes the period type slot procedure for PRIPage.
     * @param priSpace::periodicTypeEnum Input parameter specifying priSpace::periodicTypeEnum (priSpace::periodicTypeEnum).
     */
    void periodTypeSlot(priSpace::periodicTypeEnum);

signals:
    /**
     * @brief Executes the p r i data signal procedure for PRIPage.
     * @param data Input parameter specifying data (priSpace::priDataStruct &).
     */
    void PRIDataSignal(priSpace::priDataStruct & data);
    /**
     * @brief Executes the enable p w page procedure for PRIPage.
     */
    void enablePWPage();

};

#endif // PRITWOPAGE_H
