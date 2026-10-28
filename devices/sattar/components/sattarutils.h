// ============================================================================
// File: sattarutils.h
// Subsystem: eLynxSDR components
// Description: Subsystem implementation and interface guidance for sattarutils.h.
// ============================================================================

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
    // Constructor: Initializes SattarUtils subsystem components, default parameters, and state.
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


    // Executes spure generate date routine.
    bool  spureGenerateDate( spureSpace::spureStruct &spurData,modulSpace::modulDataStruct&, radarSpace::radarDataStruct&, frqSpace::frqDataStruct &, priSpace::priDataStruct &, pwSpace::pwDataStruct &);
    // Executes generate spure data routine.
    bool  generateSpureData(uint&,float&, modulSpace::modulDataStruct &,spureSpace::spureStruct &);
    // Executes min max p r i routine.
    void  minMaxPRI(priSpace::priDataStruct &, float&, float&, double&, double&, double&);
    // Executes generate spure data routine.
    bool  generateSpureData(uint&, priSpace::priDataStruct &,spureSpace::spureStruct &);
    // Executes generate spure data routine.
    bool  generateSpureData(frqSpace::frqDataStruct &,spureSpace::spureStruct &);
    // Executes generate spure data routine.
    bool  generateSpureData(pwSpace::pwDataStruct &,spureSpace::spureStruct &);
    typedef enum{pulsePage, priPage,pwPage,frqPage,modulPage} colorStatus;
    // Executes check error routine.
    bool  checkError(priSpace::priDataStruct &, pwSpace::pwDataStruct & );
    // Executes min max p r i dwell routine.
    void  minMaxPRIDwell(priSpace::priDataStruct &data, float &pris);
    // Sets max pulse count value and updates internal subsystem configuration.
    bool  setMaxPulseCount(uint, radarSpace::radarDataStruct & );
    // Executes regenerate pulse p r i routine.
    int  regeneratePulsePRI(uint, priSpace::priDataStruct & );
    // Executes generate data routine.
    bool  generateData(uint,uint, frqSpace::frqDataStruct & );
    // Executes regenerate pulse frq routine.
    uint  regeneratePulseFrq(uint, frqSpace::frqDataStruct &);
    // Retrieves current total pulse count state.
    int  getTotalPulseCount(radarSpace::radarDataStruct & );
    // Executes regenerate pulse p w routine.
    int  regeneratePulsePW(uint, pwSpace::pwDataStruct & );
    // Executes min max p r i routine.
    void  minMaxPRI(priSpace::priDataStruct &, float&);
    // Executes time to u long routine.
    ulong timeToULong(const uint &, const QString &);
    // Executes generate p r i data routine.
    bool  generatePRIData(priSpace::priDataStruct &);
    // Executes time to int routine.
    uint  timeToInt(const uint &, const QString &);
    // Executes generate p w data routine.
    bool  generatePWData(pwSpace::pwDataStruct &);
    // Executes else function routine.
    bool  elseFunction(priSpace::priDataStruct &);
    // Executes gen routine.
    float Gen(float&, double&, double&, double&);
    // Initializes init p r i val for SattarUtils.
    void  initPRIVal(priSpace::priDataStruct &);
    // Executes simplified i q routine.
    void simplifiedIQ(std::vector <int16_t> &);
    // Executes normal iq routine.
    void normalIq(std::vector <int16_t> &);
    // Executes color state routine.
    void colorState(colorStatus,bool);
    // Executes remove iq file routine.
    void removeIqFile(QJsonObject &);
    // Executes save project routine.
    bool  saveProject(QJsonObject&);
    static bool isBtnColorGreen[4];
    // Executes float to time routine.
    QString floatToTime(float&);
    // Executes msg function routine.
    void  msgFunction(QString);
    // Executes time to float routine.
    float timeToFloat(QString);
    // Executes show loading bar routine.
    void showLoadingBar(bool);
    // Executes frq to u long routine.
    ulong frqToULong(QString);
    // Executes frq to time routine.
    float frqToTime(QString);
    // Executes minimized iq file routine.
    void minimizedIqFile();
    bool isSendToCard{};
    QString radarName;
    static QString filePath;
    int status{};
    int count;


private slots:
    // Executes create iq is done routine.
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
    // Executes load sattar file to cart routine.
    void loadSattarFileToCart(QString, double,QString);

};

#endif // SATTARUTILS_H
