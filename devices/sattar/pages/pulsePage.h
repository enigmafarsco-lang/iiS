#ifndef PULSEPAGE_H
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

    explicit PulsePage(QWidget *parent = nullptr, radarSpace::radarDataStruct * radarData= nullptr);
    radarSpace::radarDataStruct * radarData;
    ~PulsePage();


private:
    Ui::PulsePage *ui;

    SattarUtils * utils;


private slots:
    void on_btnOk_clicked();
    void btnGreen();
    void btnRed();

public slots:


signals:
    void periodTypeSignal(priSpace::periodicTypeEnum);
    void modulTypeSignal(modulSpace::modeTypeEnum);
    void priTypeSignal(priSpace::priTypeEnum);
    void frqTypeSignal(frqSpace::typeEnum);
    void pwTypeSignal(pwSpace::pwTypeEnum);
};

#endif // PULSEPAGE_H
