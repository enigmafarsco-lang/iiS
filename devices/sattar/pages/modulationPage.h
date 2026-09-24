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


    ~ModulationPage();

private:
    Ui::ModulationPage *ui;

    bool bpskAction ();
    void qpskAction ();
    bool xfmAction ();
    bool fskAction ();

    SattarUtils * utils = new SattarUtils;
    QString customCodeModifier();
    QString bitInfiModifier();
    QString barkerModifier();
    QString costasModifier();
    void deltaCheckSimple();
    void deltaCheckStep();
    bool firstRun{false};

public slots:
    void modulInfoSlot(modulSpace::modeTypeEnum);

private slots:

    void on_bpsk_cmbBarkerCode_activated(int index);
    void on_bpsk_radBarkerCode_toggled(bool checked);
    void on_fsk_radFskSimple_toggled(bool checked);
    void on_qpsk_radFrank_toggled(bool checked);
    void on_xfm_radLfm_toggled(bool checked);
    void on_bpsk_btnRandomGenerate_clicked();
    void on_qpsk_btnRandomGenerate_clicked();
    void btnGreen();
    void btnRed();

    void on_btnOk_clicked();


signals:
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
