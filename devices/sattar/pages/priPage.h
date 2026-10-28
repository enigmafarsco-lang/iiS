// ============================================================================
// File: priPage.h
// Subsystem: eLynxSDR pages
// Description: Subsystem implementation and interface guidance for priPage.h.
// ============================================================================

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

class PRIPage;
}


class PRIPage : public QWidget
{
    Q_OBJECT

public:
    explicit PRIPage(QWidget                     * parent    = nullptr,
                     priSpace::priDataStruct     * priData   = nullptr,
                     radarSpace::radarDataStruct * radarData = nullptr);

    priSpace::priDataStruct     * priData ;
    radarSpace::radarDataStruct * radarData ;

    // Destructor: Cleans up PRIPage resources, timers, and allocated memory.
    ~PRIPage();

private:
    Ui::PRIPage *ui;
    // Executes min max p r i routine.
    void minMaxPRI(priSpace::priDataStruct &data, float& priVal, float&amplitude , double&phaseAngle, double&frq, double&sampleRate);
    // Executes gen routine.
    float Gen(float & amp, double &phaseAng, double &frq, double &sampleRate);
    // Executes min max p r i dwell routine.
    void minMaxPRIDwell(priSpace::priDataStruct &data, float& pris);
    // Executes min max p r i routine.
    void minMaxPRI(priSpace::priDataStruct &data, float& pris);
    // Executes generate p r i data routine.
    bool  generatePRIData(priSpace::priDataStruct &);
    // Executes else function routine.
    bool elseFunction(priSpace::priDataStruct &);
    // Initializes init p r i val for PRIPage.
    void initPRIVal(priSpace::priDataStruct &);
    bool isPriTypeChanged{false};
    bool isPeriodChanged{false};
    // Executes reset max pulse count routine.
    void resetMaxPulseCount();
    // Executes table level count routine.
    void tableLevelCount();
    // Executes pri table value routine.
    bool priTableValue();
    bool firstRun{true};
    SattarUtils * utils;
    // Executes disable p r i routine.
    void disablePRI();
    uint sumCount{};
    // Executes btn green routine.
    void btnGreen();
    double alpha{2.555*2};
    int row{0};


private slots:
    // Event/Signal handler for on tbl p r i one cell changed.
    void on_tblPRIOne_cellChanged(int row, int column);
    // Event/Signal handler for on btn clear clicked.
    void on_btnClear_clicked();
    // Event/Signal handler for on btn add clicked.
    void on_btnAdd_clicked();
    // Event/Signal handler for on btn ok clicked.
    void on_btnOk_clicked();
    // Executes btn slot routine.
    void btnSlot();
    // Executes btn red routine.
    void btnRed();

public slots:
    // Executes enable disable p r i page slot routine.
    void enableDisablePRIPageSlot(priSpace::priTypeEnum);
    // Executes period type slot routine.
    void periodTypeSlot(priSpace::periodicTypeEnum);

signals:
    // Executes p r i data signal routine.
    void PRIDataSignal(priSpace::priDataStruct & data);
    // Executes enable p w page routine.
    void enablePWPage();

};

#endif // PRITWOPAGE_H
