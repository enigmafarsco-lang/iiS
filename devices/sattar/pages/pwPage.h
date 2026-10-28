/**
 * @file pwPage.h
 * @brief Subsystem module for pages in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef PWPAGE_H
#define PWPAGE_H

#include <QWidget>
#include <QComboBox>
#include <QLineEdit>

#include "../components/Store.h"
#include "../components/constant.h"
#include "../components/sattarutils.h"

namespace Ui {
/**
 * @class PWPage
 * @brief High-level controller class for PWPage.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class PWPage;
};


enum pwTypeData{};


/**
 * @class PWPage
 * @brief High-level controller class for PWPage.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
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

    /**
     * @brief Destructor for PWPage, releasing allocated hardware resources, timers, and buffers.
     */
    ~PWPage();

private:
    /**
     * @brief Executes the generate p w data procedure for PWPage.
     * @param data Input parameter specifying data (pwSpace::pwDataStruct).
     * @return Output value of type bool.
     */
    bool generatePWData(pwSpace::pwDataStruct &data );
    /**
     * @brief Executes the pw table value procedure for PWPage.
     * @param QString Input parameter specifying QString (QString).
     * @return Output value of type bool.
     */
    bool pwTableValue(QString);
    /**
     * @brief Executes the pw compare procedure for PWPage.
     * @param float& Input parameter specifying float& (float&).
     * @return Output value of type bool.
     */
    bool pwCompare(float&);
    /**
     * @brief Executes the pw range procedure for PWPage.
     * @param float& Input parameter specifying float& (float&).
     * @return Output value of type bool.
     */
    bool pwRange(float&);
    SattarUtils * utils;
    bool firstRun{true};
    QString pwType;
    Ui::PWPage *ui;
    double alpha{2.555*2};
    int row=0;

public slots:
    /**
     * @brief Executes the pw info slot procedure for PWPage.
     * @param pwSpace::pwTypeEnum Input parameter specifying pwSpace::pwTypeEnum (pwSpace::pwTypeEnum).
     */
    void pwInfoSlot(pwSpace::pwTypeEnum);
    /**
     * @brief Executes the change lable procedure for PWPage.
     */
    void changeLable();
    /**
     * @brief Executes the btn red procedure for PWPage.
     */
    void btnRed();
//    void changeColor();

private slots:
    void on_tblPW_cellChanged(int row, int column);
    void on_btnClear_clicked();
    void on_btnAdd_clicked();
    /**
     * @brief Slot handler responding to on btn ok clicked event.
     */
    void on_btnOk_clicked();
    /**
     * @brief Executes the btn green procedure for PWPage.
     */
    void btnGreen();
    /**
     * @brief Executes the btn slot procedure for PWPage.
     */
    void btnSlot();


signals:
    /**
     * @brief Executes the enable frq page procedure for PWPage.
     */
    void enableFrqPage();
};

#endif // PWPAGE_H
