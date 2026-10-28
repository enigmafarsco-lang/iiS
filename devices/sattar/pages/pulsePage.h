// ============================================================================
// File: pulsePage.h
// Subsystem: eLynxSDR pages
// Description: Subsystem implementation and interface guidance for pulsePage.h.
// ============================================================================

﻿#ifndef PULSEPAGE_H
#define PULSEPAGE_H

#include "../components/sattarutils.h"
#include "../components/constant.h"
#include "../components/Store.h"



namespace Ui {
class PulsePage;
}



class PulsePage : public QWidget
{
    Q_OBJECT

public:

    // Constructor: Initializes PulsePage subsystem components, default parameters, and state.
    explicit PulsePage(QWidget *parent = nullptr, radarSpace::radarDataStruct * radarData= nullptr);
    radarSpace::radarDataStruct * radarData;
    // Destructor: Cleans up PulsePage resources, timers, and allocated memory.
    ~PulsePage();


private:
    Ui::PulsePage *ui;

    SattarUtils * utils;


private slots:
    // Event/Signal handler for on btn ok clicked.
    void on_btnOk_clicked();
    // Executes btn green routine.
    void btnGreen();
    // Executes btn red routine.
    void btnRed();

public slots:


signals:
    // Executes period type signal routine.
    void periodTypeSignal(priSpace::periodicTypeEnum);
    // Executes modul type signal routine.
    void modulTypeSignal(modulSpace::modeTypeEnum);
    // Executes pri type signal routine.
    void priTypeSignal(priSpace::priTypeEnum);
    // Executes frq type signal routine.
    void frqTypeSignal(frqSpace::typeEnum);
    // Executes pw type signal routine.
    void pwTypeSignal(pwSpace::pwTypeEnum);
};

#endif // PULSEPAGE_H
