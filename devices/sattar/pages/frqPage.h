// ============================================================================
// File: frqPage.h
// Subsystem: eLynxSDR pages
// Description: Subsystem implementation and interface guidance for frqPage.h.
// ============================================================================

﻿#ifndef FRQPAGE_H
#define FRQPAGE_H

#include <QWidget>
#include <QComboBox>

#include "../components/Store.h"
#include "../components/constant.h"
#include "../components/sattarutils.h"

namespace Ui {
class FrqPage;
}

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

    // Destructor: Cleans up FrqPage resources, timers, and allocated memory.
    ~FrqPage();
SattarUtils * utils = new SattarUtils;
private:
    Ui::FrqPage *ui;


    // Executes generate data routine.
    bool generateData(uint startBand, uint bw, frqSpace::frqDataStruct &frqData);
    // Executes regenerate pulse frq routine.
    int regeneratePulseFrq(uint count, frqSpace::frqDataStruct & frqData);
    // Executes agile calc routine.
    bool agileCalc(float&);
    // Executes frq table value routine.
    bool frqTableValue();
    bool firstRun{true};
    double frqCenter{};
    double alpha = 2.555*2;
    int row{};

public slots:
    // Executes frq type slot routine.
    void frqTypeSlot(frqSpace::typeEnum);
    // Executes change lable routine.
    void changeLable();
    // Sets frq center slot value and updates internal subsystem configuration.
    void setFrqCenterSlot(double);

private slots:
    // Event/Signal handler for on tbl frq cell changed.
    void on_tblFrq_cellChanged(int row, int column);
    // Event/Signal handler for on btn clear clicked.
    void on_btnClear_clicked();
    // Event/Signal handler for on btn add clicked.
    void on_btnAdd_clicked();
    // Event/Signal handler for on btn ok clicked.
    void on_btnOk_clicked();
    // Executes btn green routine.
    void btnGreen();
    // Executes btn slot routine.
    void btnSlot();
    // Executes btn red routine.
    void btnRed();

signals:
    // Executes enable modul page routine.
    void enableModulPage();
    // Executes show menu option signal routine.
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
