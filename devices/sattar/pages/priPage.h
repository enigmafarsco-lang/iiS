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

    ~PRIPage();

private:
    Ui::PRIPage *ui;
    void minMaxPRI(priSpace::priDataStruct &data, float& priVal, float&amplitude , double&phaseAngle, double&frq, double&sampleRate);
    float Gen(float & amp, double &phaseAng, double &frq, double &sampleRate);
    void minMaxPRIDwell(priSpace::priDataStruct &data, float& pris);
    void minMaxPRI(priSpace::priDataStruct &data, float& pris);
    bool  generatePRIData(priSpace::priDataStruct &);
    bool elseFunction(priSpace::priDataStruct &);
    void initPRIVal(priSpace::priDataStruct &);
    bool isPriTypeChanged{false};
    bool isPeriodChanged{false};
    void resetMaxPulseCount();
    void tableLevelCount();
    bool priTableValue();
    bool firstRun{true};
    SattarUtils * utils;
    void disablePRI();
    uint sumCount{};
    void btnGreen();
    double alpha{2.555*2};
    int row{0};


private slots:
    void on_tblPRIOne_cellChanged(int row, int column);
    void on_btnClear_clicked();
    void on_btnAdd_clicked();
    void on_btnOk_clicked();
    void btnSlot();
    void btnRed();

public slots:
    void enableDisablePRIPageSlot(priSpace::priTypeEnum);
    void periodTypeSlot(priSpace::periodicTypeEnum);

signals:
    void PRIDataSignal(priSpace::priDataStruct & data);
    void enablePWPage();

};

#endif // PRITWOPAGE_H
