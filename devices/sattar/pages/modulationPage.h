/**
 * @file modulationPage.h
 * @brief Subsystem module for pages in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef MODULATIONPAGE_H
#define MODULATIONPAGE_H

#include <QLCDNumber>
#include <QWidget>
#include <iostream>

#include "../components/Store.h"
#include "../components/sattarutils.h"

namespace Ui {
/**
 * @class ModulationPage
 * @brief High-level controller class for ModulationPage.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class ModulationPage;
}

/**
 * @class ModulationPage
 * @brief High-level controller class for ModulationPage.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class ModulationPage : public QWidget
{
    Q_OBJECT

public:
    explicit ModulationPage(QWidget *parent = nullptr,
                            frqSpace::frqDataStruct     * frqData=nullptr,
                            modulSpace::modulDataStruct * modulData = nullptr,
                            radarSpace::radarDataStruct * radarData = nullptr,
                            priSpace::priDataStruct     * priData = nullptr,
                            pwSpace::pwDataStruct       * pwData = nullptr,
                            spureSpace::spureStruct     * spureData = nullptr,
                            projectDataSpace::projectDataStruct * prjData = nullptr
                            );

    frqSpace::frqDataStruct     * frqData;
    modulSpace::modulDataStruct * modulData;
    radarSpace::radarDataStruct * radarData;
    priSpace::priDataStruct     * priData;
    pwSpace::pwDataStruct       * pwData;
    spureSpace::spureStruct     * spureData;
    projectDataSpace::projectDataStruct * prjData ;


    /**
     * @brief Destructor for ModulationPage, releasing allocated hardware resources, timers, and buffers.
     */
    ~ModulationPage();

private:
    Ui::ModulationPage *ui;

    /**
     * @brief Executes the bpsk action procedure for ModulationPage.
     * @return Output value of type bool.
     */
    bool bpskAction ();
    /**
     * @brief Executes the qpsk action procedure for ModulationPage.
     */
    void qpskAction ();
    /**
     * @brief Executes the xfm action procedure for ModulationPage.
     * @return Output value of type bool.
     */
    bool xfmAction ();
    /**
     * @brief Executes the fsk action procedure for ModulationPage.
     * @return Output value of type bool.
     */
    bool fskAction ();

    SattarUtils * utils = new SattarUtils;
    /**
     * @brief Executes the custom code modifier procedure for ModulationPage.
     * @return Output value of type QString.
     */
    QString customCodeModifier();
    /**
     * @brief Executes the bit infi modifier procedure for ModulationPage.
     * @return Output value of type QString.
     */
    QString bitInfiModifier();
    /**
     * @brief Executes the barker modifier procedure for ModulationPage.
     * @return Output value of type QString.
     */
    QString barkerModifier();
    /**
     * @brief Executes the costas modifier procedure for ModulationPage.
     * @return Output value of type QString.
     */
    QString costasModifier();
    /**
     * @brief Executes the delta check simple procedure for ModulationPage.
     */
    void deltaCheckSimple();
    /**
     * @brief Executes the delta check step procedure for ModulationPage.
     */
    void deltaCheckStep();
    bool firstRun{false};

public slots:
    /**
     * @brief Executes the modul info slot procedure for ModulationPage.
     * @param modulSpace::modeTypeEnum Input parameter specifying modulSpace::modeTypeEnum (modulSpace::modeTypeEnum).
     */
    void modulInfoSlot(modulSpace::modeTypeEnum);

private slots:

    /**
     * @brief Slot handler responding to on bpsk cmb barker code activated event.
     * @param index Input parameter specifying index (int).
     */
    void on_bpsk_cmbBarkerCode_activated(int index);
    /**
     * @brief Slot handler responding to on bpsk rad barker code toggled event.
     * @param checked Input parameter specifying checked (bool).
     */
    void on_bpsk_radBarkerCode_toggled(bool checked);
    /**
     * @brief Slot handler responding to on fsk rad fsk simple toggled event.
     * @param checked Input parameter specifying checked (bool).
     */
    void on_fsk_radFskSimple_toggled(bool checked);
    /**
     * @brief Slot handler responding to on qpsk rad frank toggled event.
     * @param checked Input parameter specifying checked (bool).
     */
    void on_qpsk_radFrank_toggled(bool checked);
    /**
     * @brief Slot handler responding to on xfm rad lfm toggled event.
     * @param checked Input parameter specifying checked (bool).
     */
    void on_xfm_radLfm_toggled(bool checked);
    /**
     * @brief Slot handler responding to on bpsk btn random generate clicked event.
     */
    void on_bpsk_btnRandomGenerate_clicked();
    /**
     * @brief Slot handler responding to on qpsk btn random generate clicked event.
     */
    void on_qpsk_btnRandomGenerate_clicked();
    /**
     * @brief Executes the btn green procedure for ModulationPage.
     */
    void btnGreen();
    /**
     * @brief Executes the btn red procedure for ModulationPage.
     */
    void btnRed();

    /**
     * @brief Slot handler responding to on btn ok clicked event.
     */
    void on_btnOk_clicked();


signals:
    /**
     * @brief Executes the show menu option signal procedure for ModulationPage.
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

#endif // MODULATION_H
