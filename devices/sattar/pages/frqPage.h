#ifndef FRQPAGE_H
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

    ~FrqPage();
SattarUtils * utils = new SattarUtils;
private:
    Ui::FrqPage *ui;


    bool generateData(uint startBand, uint bw, frqSpace::frqDataStruct &frqData);
    int regeneratePulseFrq(uint count, frqSpace::frqDataStruct & frqData);
    bool agileCalc(float&);
    bool frqTableValue();
    bool firstRun{true};
    double frqCenter{};
    double alpha = 2.555*2;
    int row{};

public slots:
    void frqTypeSlot(frqSpace::typeEnum);
    void changeLable();
    void setFrqCenterSlot(double);

private slots:
    void on_tblFrq_cellChanged(int row, int column);
    void on_btnClear_clicked();
    void on_btnAdd_clicked();
    void on_btnOk_clicked();
    void btnGreen();
    void btnSlot();
    void btnRed();

signals:
    void enableModulPage();
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
