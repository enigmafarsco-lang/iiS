/**
 * @file sattarutils.h
 * @brief Subsystem module for components in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef SATTARUTILS_H
#define SATTARUTILS_H

#include <QObject>
#include <receiver/datafiles.h>
#include <QMessageBox>
#include <QVector>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <qdebug.h>
#include <QFileDialog>
#include <QRandomGenerator>
#include <iostream>
#include <QProcess>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <fstream>
#include <cstdio>
#include <stdio.h>
#include "devices/sattar/components/constant.h"
#include "devices/sattar/components/Store.h"
#include <sstream>
#include <iomanip>
#include <unistd.h>
#include <QtWidgets/qtwidgetsglobal.h>
#include <QtCore/qcoreapplication.h>
#include <QtGui/qwindowdefs.h>
#include <QtCore/qpoint.h>
#include <QtCore/qsize.h>
#include <QtGui/qcursor.h>
#include <QProgressBar>
#include <vector>


#include <QDebug>
#include <math.h>
#include <QThread>
#include <iostream>
#include <cstdint>
#include <cstring>


/**
 * @class SattarUtils
 * @brief High-level controller class for SattarUtils.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class SattarUtils : public QObject
{
    Q_OBJECT

private:

    QString fileAdress;
    projectDataSpace::projectDataStruct * prjData ;
    modulSpace::modulDataStruct * modulData;
    radarSpace::radarDataStruct * radarData ;
    frqSpace::frqDataStruct     * frqData;
    priSpace::priDataStruct     * priData ;
    pwSpace::pwDataStruct       * pwData;
    spureSpace::spureStruct     * spureData;
    QProgressBar                * newProg     = new QProgressBar();
    QProcess                    * processIq   = new QProcess();
    QWidget                     * d           = new QWidget();
    QVBoxLayout                 * s           = new QVBoxLayout;

public:
    /**
     * @brief Constructs a new SattarUtils instance and initializes subsystem components and UI state.
     * @param parent Input parameter specifying parent (QObject).
     */
    explicit SattarUtils(QObject *parent = nullptr);

    //Configs ==================================================================================================
    static int normalSample ;
    static const int  sampleStep     {1};
    static int N ;

    const int maxNormal{48800};
    const QString matlabRunTimePath = qEnvironmentVariable(
        "ELYNXSDR_MCR_ROOT", "/usr/local/MATLAB/MATLAB_Runtime/v99");
    const QString program = resourceFilePath(QStringLiteral("signal/run_Executable_Generator_DoubleChannel.sh"));
    //==========================================================================================================


    bool  spureGenerateDate( spureSpace::spureStruct &spurData,modulSpace::modulDataStruct&, radarSpace::radarDataStruct&, frqSpace::frqDataStruct &, priSpace::priDataStruct &, pwSpace::pwDataStruct &);
    bool  generateSpureData(uint&,float&, modulSpace::modulDataStruct &,spureSpace::spureStruct &);
    void  minMaxPRI(priSpace::priDataStruct &, float&, float&, double&, double&, double&);
    /**
     * @brief Executes the generate spure data procedure for SattarUtils.
     * @param uint& Input parameter specifying uint& (uint&).
     * @param  Input parameter specifying  (priSpace::priDataStruct).
     * @param  Input parameter specifying  (spureSpace::spureStruct).
     * @return Output value of type bool.
     */
    bool  generateSpureData(uint&, priSpace::priDataStruct &,spureSpace::spureStruct &);
    /**
     * @brief Executes the generate spure data procedure for SattarUtils.
     * @param  Input parameter specifying  (frqSpace::frqDataStruct).
     * @param  Input parameter specifying  (spureSpace::spureStruct).
     * @return Output value of type bool.
     */
    bool  generateSpureData(frqSpace::frqDataStruct &,spureSpace::spureStruct &);
    /**
     * @brief Executes the generate spure data procedure for SattarUtils.
     * @param  Input parameter specifying  (pwSpace::pwDataStruct).
     * @param  Input parameter specifying  (spureSpace::spureStruct).
     * @return Output value of type bool.
     */
    bool  generateSpureData(pwSpace::pwDataStruct &,spureSpace::spureStruct &);
    typedef enum{pulsePage, priPage,pwPage,frqPage,modulPage} colorStatus;
    /**
     * @brief Executes the check error procedure for SattarUtils.
     * @param  Input parameter specifying  (priSpace::priDataStruct).
     * @param  Input parameter specifying  (pwSpace::pwDataStruct).
     * @return Output value of type bool.
     */
    bool  checkError(priSpace::priDataStruct &, pwSpace::pwDataStruct & );
    /**
     * @brief Executes the min max p r i dwell procedure for SattarUtils.
     * @param data Input parameter specifying data (priSpace::priDataStruct).
     * @param pris Input parameter specifying pris (float).
     */
    void  minMaxPRIDwell(priSpace::priDataStruct &data, float &pris);
    /**
     * @brief Sets the max pulse count parameter and updates internal state.
     * @param uint Input parameter specifying uint (uint).
     * @param  Input parameter specifying  (radarSpace::radarDataStruct).
     * @return Output value of type bool.
     */
    bool  setMaxPulseCount(uint, radarSpace::radarDataStruct & );
    /**
     * @brief Executes the regenerate pulse p r i procedure for SattarUtils.
     * @param uint Input parameter specifying uint (uint).
     * @param  Input parameter specifying  (priSpace::priDataStruct).
     * @return Output value of type int.
     */
    int  regeneratePulsePRI(uint, priSpace::priDataStruct & );
    /**
     * @brief Executes the generate data procedure for SattarUtils.
     * @param uint Input parameter specifying uint (uint).
     * @param uint Input parameter specifying uint (uint).
     * @param  Input parameter specifying  (frqSpace::frqDataStruct).
     * @return Output value of type bool.
     */
    bool  generateData(uint,uint, frqSpace::frqDataStruct & );
    /**
     * @brief Executes the regenerate pulse frq procedure for SattarUtils.
     * @param uint Input parameter specifying uint (uint).
     * @param  Input parameter specifying  (frqSpace::frqDataStruct).
     * @return Output value of type uint.
     */
    uint  regeneratePulseFrq(uint, frqSpace::frqDataStruct &);
    /**
     * @brief Retrieves the current total pulse count value.
     * @param  Input parameter specifying  (radarSpace::radarDataStruct).
     * @return Output value of type int.
     */
    int  getTotalPulseCount(radarSpace::radarDataStruct & );
    /**
     * @brief Executes the regenerate pulse p w procedure for SattarUtils.
     * @param uint Input parameter specifying uint (uint).
     * @param  Input parameter specifying  (pwSpace::pwDataStruct).
     * @return Output value of type int.
     */
    int  regeneratePulsePW(uint, pwSpace::pwDataStruct & );
    /**
     * @brief Executes the min max p r i procedure for SattarUtils.
     * @param  Input parameter specifying  (priSpace::priDataStruct).
     * @param float& Input parameter specifying float& (float&).
     */
    void  minMaxPRI(priSpace::priDataStruct &, float&);
    /**
     * @brief Executes the time to u long procedure for SattarUtils.
     * @param  Input parameter specifying  (const uint).
     * @param  Input parameter specifying  (const QString).
     * @return Output value of type ulong.
     */
    ulong timeToULong(const uint &, const QString &);
    /**
     * @brief Executes the generate p r i data procedure for SattarUtils.
     * @param  Input parameter specifying  (priSpace::priDataStruct).
     * @return Output value of type bool.
     */
    bool  generatePRIData(priSpace::priDataStruct &);
    /**
     * @brief Executes the time to int procedure for SattarUtils.
     * @param  Input parameter specifying  (const uint).
     * @param  Input parameter specifying  (const QString).
     * @return Output value of type uint.
     */
    uint  timeToInt(const uint &, const QString &);
    /**
     * @brief Executes the generate p w data procedure for SattarUtils.
     * @param  Input parameter specifying  (pwSpace::pwDataStruct).
     * @return Output value of type bool.
     */
    bool  generatePWData(pwSpace::pwDataStruct &);
    /**
     * @brief Executes the else function procedure for SattarUtils.
     * @param  Input parameter specifying  (priSpace::priDataStruct).
     * @return Output value of type bool.
     */
    bool  elseFunction(priSpace::priDataStruct &);
    /**
     * @brief Executes the gen procedure for SattarUtils.
     * @param float& Input parameter specifying float& (float&).
     * @param double& Input parameter specifying double& (double&).
     * @param double& Input parameter specifying double& (double&).
     * @param double& Input parameter specifying double& (double&).
     * @return Output value of type float.
     */
    float Gen(float&, double&, double&, double&);
    /**
     * @brief Initializes and configures the init p r i val for the SattarUtils subsystem.
     * @param  Input parameter specifying  (priSpace::priDataStruct).
     */
    void  initPRIVal(priSpace::priDataStruct &);
    /**
     * @brief Executes the simplified i q procedure for SattarUtils.
     * @param  Input parameter specifying  (std::vector <int16_t>).
     */
    void simplifiedIQ(std::vector <int16_t> &);
    /**
     * @brief Executes the normal iq procedure for SattarUtils.
     * @param  Input parameter specifying  (std::vector <int16_t>).
     */
    void normalIq(std::vector <int16_t> &);
    /**
     * @brief Executes the color state procedure for SattarUtils.
     * @param colorStatus Input parameter specifying colorStatus (colorStatus).
     * @param bool Input parameter specifying bool (bool).
     */
    void colorState(colorStatus,bool);
    /**
     * @brief Executes the remove iq file procedure for SattarUtils.
     * @param  Input parameter specifying  (QJsonObject).
     */
    void removeIqFile(QJsonObject &);
    /**
     * @brief Executes the save project procedure for SattarUtils.
     * @param QJsonObject& Input parameter specifying QJsonObject& (QJsonObject&).
     * @return Output value of type bool.
     */
    bool  saveProject(QJsonObject&);
    static bool isBtnColorGreen[4];
    /**
     * @brief Executes the float to time procedure for SattarUtils.
     * @param float& Input parameter specifying float& (float&).
     * @return Output value of type QString.
     */
    QString floatToTime(float&);
    /**
     * @brief Executes the msg function procedure for SattarUtils.
     * @param QString Input parameter specifying QString (QString).
     */
    void  msgFunction(QString);
    /**
     * @brief Executes the time to float procedure for SattarUtils.
     * @param QString Input parameter specifying QString (QString).
     * @return Output value of type float.
     */
    float timeToFloat(QString);
    /**
     * @brief Executes the show loading bar procedure for SattarUtils.
     * @param bool Input parameter specifying bool (bool).
     */
    void showLoadingBar(bool);
    /**
     * @brief Executes the frq to u long procedure for SattarUtils.
     * @param QString Input parameter specifying QString (QString).
     * @return Output value of type ulong.
     */
    ulong frqToULong(QString);
    /**
     * @brief Executes the frq to time procedure for SattarUtils.
     * @param QString Input parameter specifying QString (QString).
     * @return Output value of type float.
     */
    float frqToTime(QString);
    /**
     * @brief Executes the minimized iq file procedure for SattarUtils.
     */
    void minimizedIqFile();
    bool isSendToCard{};
    QString radarName;
    static QString filePath;
    int status{};
    int count;


private slots:
    /**
     * @brief Executes the create iq is done procedure for SattarUtils.
     * @param int Input parameter specifying int (int).
     * @param QProcess::ExitStatus Input parameter specifying QProcess::ExitStatus (QProcess::ExitStatus).
     */
    void createIqIsDone(int,QProcess::ExitStatus);


public slots:

    void generateRadarData(radarSpace::radarDataStruct &,
                           modulSpace::modulDataStruct &,
                           spureSpace::spureStruct &,
                           priSpace::priDataStruct &,
                           frqSpace::frqDataStruct &,
                           pwSpace::pwDataStruct&,
                           projectDataSpace::projectDataStruct&,
                           int status);

    void showSelectedRadar(modulSpace::modulDataStruct&,
                           radarSpace::radarDataStruct&,
                           frqSpace::frqDataStruct &,
                           priSpace::priDataStruct &,
                           pwSpace::pwDataStruct &,
                           spureSpace::spureStruct&,
                           projectDataSpace::projectDataStruct &);




signals:
    /**
     * @brief Executes the load sattar file to cart procedure for SattarUtils.
     * @param QString Input parameter specifying QString (QString).
     * @param double Input parameter specifying double (double).
     * @param QString Input parameter specifying QString (QString).
     */
    void loadSattarFileToCart(QString, double,QString);

};

#endif // SATTARUTILS_H
