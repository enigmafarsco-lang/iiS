#ifndef SATTARUTILS_H
#define SATTARUTILS_H

#include <QObject>
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
    explicit SattarUtils(QObject *parent = nullptr);

    //Configs ==================================================================================================
    static int normalSample ;
    static const int  sampleStep     {1};
    static int N ;

    const int maxNormal{48800};
    const QString matlabRunTimePath = "/usr/local/MATLAB/MATLAB_Runtime/v99";
    const QString program = QDir::currentPath() + "/signal/run_Executable_Generator_DoubleChannel.sh";
    //==========================================================================================================


    bool  spureGenerateDate( spureSpace::spureStruct &spurData,modulSpace::modulDataStruct&, radarSpace::radarDataStruct&, frqSpace::frqDataStruct &, priSpace::priDataStruct &, pwSpace::pwDataStruct &);
    bool  generateSpureData(uint&,float&, modulSpace::modulDataStruct &,spureSpace::spureStruct &);
    void  minMaxPRI(priSpace::priDataStruct &, float&, float&, double&, double&, double&);
    bool  generateSpureData(uint&, priSpace::priDataStruct &,spureSpace::spureStruct &);
    bool  generateSpureData(frqSpace::frqDataStruct &,spureSpace::spureStruct &);
    bool  generateSpureData(pwSpace::pwDataStruct &,spureSpace::spureStruct &);
    typedef enum{pulsePage, priPage,pwPage,frqPage,modulPage} colorStatus;
    bool  checkError(priSpace::priDataStruct &, pwSpace::pwDataStruct & );
    void  minMaxPRIDwell(priSpace::priDataStruct &data, float &pris);
    bool  setMaxPulseCount(uint, radarSpace::radarDataStruct & );
    int  regeneratePulsePRI(uint, priSpace::priDataStruct & );
    bool  generateData(uint,uint, frqSpace::frqDataStruct & );
    uint  regeneratePulseFrq(uint, frqSpace::frqDataStruct &);
    int  getTotalPulseCount(radarSpace::radarDataStruct & );
    int  regeneratePulsePW(uint, pwSpace::pwDataStruct & );
    void  minMaxPRI(priSpace::priDataStruct &, float&);
    ulong timeToULong(const uint &, const QString &);
    bool  generatePRIData(priSpace::priDataStruct &);
    uint  timeToInt(const uint &, const QString &);
    bool  generatePWData(pwSpace::pwDataStruct &);
    bool  elseFunction(priSpace::priDataStruct &);
    float Gen(float&, double&, double&, double&);
    void  initPRIVal(priSpace::priDataStruct &);
    void simplifiedIQ(std::vector <int16_t> &);
    void normalIq(std::vector <int16_t> &);
    void colorState(colorStatus,bool);
    void removeIqFile(QJsonObject &);
    bool  saveProject(QJsonObject&);
    static bool isBtnColorGreen[4];
    QString floatToTime(float&);
    void  msgFunction(QString);
    float timeToFloat(QString);
    void showLoadingBar(bool);
    ulong frqToULong(QString);
    float frqToTime(QString);
    void minimizedIqFile();
    bool isSendToCard{};
    QString radarName;
    static QString filePath;
    int status{};
    int count;


private slots:
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
    void loadSattarFileToCart(QString, double,QString);

};

#endif // SATTARUTILS_H
