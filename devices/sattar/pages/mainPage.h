// ============================================================================
// File: mainPage.h
// Subsystem: eLynxSDR pages
// Description: Subsystem implementation and interface guidance for mainPage.h.
// ============================================================================

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
class MainPage;
}

class MainPage : public QWidget
{
    Q_OBJECT

public:
    // Constructor: Initializes MainPage subsystem components, default parameters, and state.
    explicit MainPage(QWidget *parent = nullptr);
    // Destructor: Cleans up MainPage resources, timers, and allocated memory.
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

    // Initializes init for MainPage.
    void init();
    // Executes connections routine.
    void connections();

public slots:


private slots:
    // Executes menu generate slot routine.
    void menuGenerateSlot();
    // Executes show modul tab routine.
    void showModulTab();
    // Executes show p r i tab routine.
    void ShowPRITab();
    // Executes show frq tab routine.
    void ShowFrqTab();
    // Executes show p w tab routine.
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
    // Executes load sattar file signal routine.
    void loadSattarFileSignal(QString, double,QString);
    // Transmits send frq value to frq page sig command/packet over communication interface.
    void sendFrqValueToFrqPageSig(double);

};

#endif // MAINPAGE_H
