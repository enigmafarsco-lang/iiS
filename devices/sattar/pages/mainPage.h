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
    explicit MainPage(QWidget *parent = nullptr);
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

    void init();
    void connections();

public slots:


private slots:
    void menuGenerateSlot();
    void showModulTab();
    void ShowPRITab();
    void ShowFrqTab();
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
    void loadSattarFileSignal(QString, double,QString);
    void sendFrqValueToFrqPageSig(double);

};

#endif // MAINPAGE_H
