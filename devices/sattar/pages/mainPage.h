/**
 * @file mainPage.h
 * @brief Subsystem module for pages in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef MAINPAGE_H
#define MAINPAGE_H

#include <QWidget>
#include "../components/Store.h"
#include "../components/sattarutils.h"

//pages

#include "frqPage.h"
#include "priPage.h"
#include "pulsePage.h"
#include "menupage.h"
#include "pwPage.h"
#include "modulationPage.h"
#include "spurepage.h"


namespace Ui {
/**
 * @class MainPage
 * @brief High-level controller class for MainPage.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class MainPage;
}

/**
 * @class MainPage
 * @brief High-level controller class for MainPage.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class MainPage : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new MainPage instance and initializes subsystem components and UI state.
     * @param parent Input parameter specifying parent (QWidget).
     */
    explicit MainPage(QWidget *parent = nullptr);
    /**
     * @brief Destructor for MainPage, releasing allocated hardware resources, timers, and buffers.
     */
    ~MainPage();
    //functions
    SattarUtils * utils;
    FrqPage * frqPage;

private:
    Ui::MainPage *ui;

    //stores components
    projectDataSpace::projectDataStruct * prjData = new projectDataSpace::projectDataStruct;
    radarSpace::radarDataStruct * radarData = new radarSpace::radarDataStruct;
    modulSpace::modulDataStruct * modulData = new modulSpace::modulDataStruct;
    spureSpace::spureStruct * spureData = new spureSpace::spureStruct;
    priSpace::priDataStruct * priData = new priSpace::priDataStruct;
    frqSpace::frqDataStruct * frqData = new frqSpace::frqDataStruct;
    pwSpace::pwDataStruct * pwData = new pwSpace::pwDataStruct;



    //pages
    ModulationPage * modulation;
    SpurePage *  spurePage;
    PulsePage * pulsePage;
    MenuPage * menuPage;

    PRIPage * priPage;
    PWPage * pwPage;

    /**
     * @brief Initializes and configures the init for the MainPage subsystem.
     */
    void init();
    /**
     * @brief Executes the connections procedure for MainPage.
     */
    void connections();

public slots:


private slots:
    /**
     * @brief Executes the menu generate slot procedure for MainPage.
     */
    void menuGenerateSlot();
    /**
     * @brief Executes the show modul tab procedure for MainPage.
     */
    void showModulTab();
    /**
     * @brief Executes the show p r i tab procedure for MainPage.
     */
    void ShowPRITab();
    /**
     * @brief Executes the show frq tab procedure for MainPage.
     */
    void ShowFrqTab();
    /**
     * @brief Executes the show p w tab procedure for MainPage.
     */
    void ShowPWTab();

signals:
    void generateSignal(radarSpace::radarDataStruct &,
                        modulSpace::modulDataStruct &,
                        spureSpace::spureStruct &,
                        priSpace::priDataStruct &,
                        frqSpace::frqDataStruct &,
                        pwSpace::pwDataStruct&,
                        projectDataSpace::projectDataStruct&,
                        int status);
    /**
     * @brief Executes the load sattar file signal procedure for MainPage.
     * @param QString Input parameter specifying QString (QString).
     * @param double Input parameter specifying double (double).
     * @param QString Input parameter specifying QString (QString).
     */
    void loadSattarFileSignal(QString, double,QString);
    /**
     * @brief Transmits send frq value to frq page sig command/packet over the communication bus.
     * @param double Input parameter specifying double (double).
     */
    void sendFrqValueToFrqPageSig(double);

};

#endif // MAINPAGE_H
