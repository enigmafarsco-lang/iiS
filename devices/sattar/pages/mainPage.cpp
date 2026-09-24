#include "mainPage.h"
#include "ui_mainPage.h"


MainPage::MainPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainPage)
{
    ui->setupUi(this);
    init();
    connections();

}

void MainPage::init()
{
    // init pages
    modulation = new ModulationPage(nullptr,frqData,modulData,radarData,priData,pwData,spureData,prjData);
    frqPage = new FrqPage(nullptr,frqData,modulData,radarData,priData,pwData,spureData,prjData);
    pwPage = new PWPage(nullptr,pwData,radarData,priData);
    priPage = new PRIPage(nullptr,priData,radarData);
    pulsePage = new PulsePage(nullptr,radarData);
    spurePage = new SpurePage();
    utils = new SattarUtils();

    //adding menu in mainpage
    menuPage = new MenuPage;


    ui->mainMenu->addWidget(menuPage);
    menuPage->setVisible(false);


    ui->tabMenu->addTab(pulsePage,Pulse_C);
    ui->tabMenu->setStyleSheet("QTabBar::tab:selected {background-color:  #186a3b;}");
}

void MainPage::connections()
{
    //PW connects
    connect(pulsePage, &PulsePage::pwTypeSignal, pwPage, &PWPage::pwInfoSlot);
    connect(pwPage, &PWPage::enableFrqPage, this, &MainPage::ShowFrqTab);
    connect(pwPage, &PWPage::enableFrqPage, frqPage, &FrqPage::changeLable);

    //PRI connects
    connect(pulsePage, &PulsePage::priTypeSignal, priPage, &PRIPage::enableDisablePRIPageSlot);
    connect(pulsePage, &PulsePage::priTypeSignal, this, &MainPage::ShowPRITab);
    connect(priPage, &PRIPage::enablePWPage, this, &MainPage::ShowPWTab);
    connect(priPage, &PRIPage::enablePWPage, pwPage, &PWPage::changeLable);
    connect(priPage, &PRIPage::enablePWPage, frqPage, &FrqPage::changeLable);
    connect(priPage, &PRIPage::enablePWPage, pwPage, &PWPage::btnRed);

    //FRQ connects
    connect(pulsePage, &PulsePage::frqTypeSignal, frqPage, &FrqPage::frqTypeSlot);
    connect(frqPage, &FrqPage::enableModulPage, this,  &MainPage::showModulTab);

    //PRI-PRF connects
    connect(pulsePage, &PulsePage::periodTypeSignal, priPage, &PRIPage::periodTypeSlot);

    //Modul connects
    connect(pulsePage, &PulsePage::modulTypeSignal, modulation, &ModulationPage::modulInfoSlot);

    //frq to menu
    connect(frqPage, &FrqPage::showMenuOptionSignal, menuPage, &MenuPage::showMenuOptionSlot);
    connect(modulation, &ModulationPage::showMenuOptionSignal, menuPage, &MenuPage::showMenuOptionSlot);

    //menu to main
    connect(menuPage, &MenuPage::menuGenerateSignal, this, &MainPage::menuGenerateSlot );

    connect(this, &MainPage::generateSignal, utils, &SattarUtils::generateRadarData);

//    connect(frqPage, &FrqPage::showSelectedRadarInfo, sattUtil, &SattarUtils::showSelectedRadar);
    connect(modulation, &ModulationPage::showSelectedRadarInfo, utils, &SattarUtils::showSelectedRadar);

    connect(frqPage->utils, &SattarUtils::loadSattarFileToCart,this, &MainPage::loadSattarFileSignal );

    connect(this, &MainPage::sendFrqValueToFrqPageSig ,frqPage, &FrqPage::setFrqCenterSlot );

}



//adding new tab to tabBar -- PRI
void MainPage::ShowPRITab()
{
    ui->tabMenu->removeTab(1);
    ui->tabMenu->insertTab(1,priPage,PRI_C);
    ui->tabMenu->setCurrentIndex(1);
}

//adding new tab to tabBar -- PW
void MainPage::ShowPWTab()
{
    ui->tabMenu->insertTab(2,pwPage,PW_C);
    ui->tabMenu->setCurrentIndex(2);
}

//adding new tab to tabBar -- FRQ
void MainPage::ShowFrqTab()
{
    ui->tabMenu->removeTab(3);
    ui->tabMenu->insertTab(3,frqPage,Frq_C);
    ui->tabMenu->setCurrentIndex(3);
}

//adding new tab to tabBar -- MODUL
void MainPage::showModulTab()
{
    ui->tabMenu->insertTab(4,modulation,Modul_C);
    ui->tabMenu->setCurrentIndex(4);
}

void MainPage::menuGenerateSlot()
{

//    SattarUtils::generateRadarData(* radarData, * modulData, * spureData, * priData, * frqData, * pwData, * prjData);

    emit generateSignal(* radarData,
                        * modulData,
                        * spureData,
                        * priData,
                        * frqData,
                        * pwData,
                        * prjData,
                        2);
}


MainPage::~MainPage()
{
//        delete radarData;
//        delete modulData;
//        delete priData  ;
//        delete frqData  ;
//        delete pwData   ;
//        delete frqPage;
//        delete priPage;
//        delete pulsePage ;
//        delete modulation;
//        delete pwPage;
    delete ui;


}
