// ============================================================================
// File: modulationPage.h
// Subsystem: eLynxSDR pages
// Description: Subsystem implementation and interface guidance for modulationPage.h.
// ============================================================================

#ifndef MODULATIONPAGE_H
#define MODULATIONPAGE_H

#include <QLCDNumber>
#include <QWidget>
#include <iostream>

#include "../components/Store.h"
#include "../components/sattarutils.h"

namespace Ui {
class ModulationPage;
}

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


    // Destructor: Cleans up ModulationPage resources, timers, and allocated memory.
    ~ModulationPage();

private:
    Ui::ModulationPage *ui;

    // Executes bpsk action routine.
    bool bpskAction ();
    // Executes qpsk action routine.
    void qpskAction ();
    // Executes xfm action routine.
    bool xfmAction ();
    // Executes fsk action routine.
    bool fskAction ();

    SattarUtils * utils = new SattarUtils;
    // Executes custom code modifier routine.
    QString customCodeModifier();
    // Executes bit infi modifier routine.
    QString bitInfiModifier();
    // Executes barker modifier routine.
    QString barkerModifier();
    // Executes costas modifier routine.
    QString costasModifier();
    // Executes delta check simple routine.
    void deltaCheckSimple();
    // Executes delta check step routine.
    void deltaCheckStep();
    bool firstRun{false};

public slots:
    // Executes modul info slot routine.
    void modulInfoSlot(modulSpace::modeTypeEnum);

private slots:

    // Event/Signal handler for on bpsk cmb barker code activated.
    void on_bpsk_cmbBarkerCode_activated(int index);
    // Event/Signal handler for on bpsk rad barker code toggled.
    void on_bpsk_radBarkerCode_toggled(bool checked);
    // Event/Signal handler for on fsk rad fsk simple toggled.
    void on_fsk_radFskSimple_toggled(bool checked);
    // Event/Signal handler for on qpsk rad frank toggled.
    void on_qpsk_radFrank_toggled(bool checked);
    // Event/Signal handler for on xfm rad lfm toggled.
    void on_xfm_radLfm_toggled(bool checked);
    // Event/Signal handler for on bpsk btn random generate clicked.
    void on_bpsk_btnRandomGenerate_clicked();
    // Event/Signal handler for on qpsk btn random generate clicked.
    void on_qpsk_btnRandomGenerate_clicked();
    // Executes btn green routine.
    void btnGreen();
    // Executes btn red routine.
    void btnRed();

    // Event/Signal handler for on btn ok clicked.
    void on_btnOk_clicked();


signals:
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

#endif // MODULATION_H
