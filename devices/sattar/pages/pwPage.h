#ifndef PWPAGE_H
#define PWPAGE_H

#include <QWidget>
#include <QComboBox>
#include <QLineEdit>

#include "../components/Store.h"
#include "../components/constant.h"
#include "../components/sattarutils.h"

namespace Ui {
class PWPage;
};


enum pwTypeData{};


class PWPage : public QWidget
{
    Q_OBJECT

public:

    explicit PWPage(QWidget                     * parent     = nullptr,
                    pwSpace::pwDataStruct       * pwData    = nullptr,
                    radarSpace::radarDataStruct * radarData = nullptr,
                    priSpace::priDataStruct     * priData   = nullptr);

    pwSpace::pwDataStruct       * pwData;
    radarSpace::radarDataStruct * radarData;
    priSpace::priDataStruct     * priData;

    ~PWPage();

private:
    bool generatePWData(pwSpace::pwDataStruct &data );
    bool pwTableValue(QString);
    bool pwCompare(float&);
    bool pwRange(float&);
    SattarUtils * utils;
    bool firstRun{true};
    QString pwType;
    Ui::PWPage *ui;
    double alpha{2.555*2};
    int row=0;

public slots:
    void pwInfoSlot(pwSpace::pwTypeEnum);
    void changeLable();
    void btnRed();
//    void changeColor();

private slots:
    void on_tblPW_cellChanged(int row, int column);
    void on_btnClear_clicked();
    void on_btnAdd_clicked();
    void on_btnOk_clicked();
    void btnGreen();
    void btnSlot();


signals:
    void enableFrqPage();
};

#endif // PWPAGE_H
