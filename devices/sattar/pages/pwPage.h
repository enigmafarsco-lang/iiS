// ============================================================================
// File: pwPage.h
// Subsystem: eLynxSDR pages
// Description: Subsystem implementation and interface guidance for pwPage.h.
// ============================================================================

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

    // Destructor: Cleans up PWPage resources, timers, and allocated memory.
    ~PWPage();

private:
    // Executes generate p w data routine.
    bool generatePWData(pwSpace::pwDataStruct &data );
    // Executes pw table value routine.
    bool pwTableValue(QString);
    // Executes pw compare routine.
    bool pwCompare(float&);
    // Executes pw range routine.
    bool pwRange(float&);
    SattarUtils * utils;
    bool firstRun{true};
    QString pwType;
    Ui::PWPage *ui;
    double alpha{2.555*2};
    int row=0;

public slots:
    // Executes pw info slot routine.
    void pwInfoSlot(pwSpace::pwTypeEnum);
    // Executes change lable routine.
    void changeLable();
    // Executes btn red routine.
    void btnRed();
//    void changeColor();

private slots:
    // Event/Signal handler for on tbl p w cell changed.
    void on_tblPW_cellChanged(int row, int column);
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


signals:
    // Executes enable frq page routine.
    void enableFrqPage();
};

#endif // PWPAGE_H
