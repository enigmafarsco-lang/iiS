#ifndef USERTYPE_H
#define USERTYPE_H

#include <math.h>
#include <assert.h>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>

struct AppSettings
{
public:
    //===========Rff
    QString RffCommunicationType;
    double  compassPitchOffset;
    QString RffLoggingFormat;
    double  compassPanOffset;
    QString RffIpAddress;
    quint16 RffPort;
    //===========Tuner
    QString tunerCommunicationType;
    QString tunerLoggingFormat;
    QString tunerIpAddress;
    quint16 tunerPort;
    //===========Servo
    double  servoPanTiltMechanicalOffset;
    QString servoCommunicationType;
    double  Servo_TiltThreshold;
    QString panTiltStopEarlier;
    QString servoLoggingFormat;
    double  Servo_PanThreshold;
    QString servoIpAddress;
    double  PanThreshold;
    quint16 servoPort;
    //===========Gis
    double  gis_elevation_range;
    double  gis_azimuth_range;
    double  gis_emit_state;
    double  gis_elevation;
    double  gisRadarLong;
    double  gisRadarLat;
    double  gis_azimuth;
    double  gisLong;
    double  gisLAt;
    //===========Solid
    QString solidCommunicationType;
    QString solidLoggingFormat;
    QString solidIpAddress;
    quint16 solidPort;
    //===========Cooling
    double coolingManualTempretureOffset;
    double coolingMinNormalMotorCurrent;
    double coolingMaxNormalMotorCurrent;
    double coolingMinNormalCompCurrent;
    double coolingMaxNormalCompCurrent;
    double coolingMinNormalFanCurrent;
    double coolingMaxNormalFanCurrent;
    double coolingCriticalTempreture;
    QString coolingCommunicationType;
    QString coolingLoggingFormat;
    double coolingMotorCosnt;
    QString coolingIpAddress;
    double coolingCompConst;
    double  coolingFanConst;
    quint16 coolingPort;
    //===========Outer_node
    QString includeRadar;
    //===========Card
    QString pBoardIpAddress;
    //===========Joshan
    QString joshanIpAddress;
    quint16 joshanPort;
    //===========Colibr
    QString calibrationAddress;
    quint16 calibrationPort;

};

enum enumRffComman
{
    PowTx      = 0,
    PowerRet   = 1,
    TmpA       = 2,
    TmpB       = 3,
    TmpC       = 4,
    CmpAngle   = 5,
    CmpPitch   = 6,
    CmpRoll    = 7,
    TmpInGun   = 8,
    HumInGun   = 9,
    GpsTime    = 10,
    GpsLat     = 11,
    GpsLong    = 12,
    GpsHeight  = 13,
    GpsNS      = 14,
    GpsEW      = 15,
    LanOn      = 16,
    LanStatus  = 17,
    ATT        = 18,
    BIT        = 19,
    ERR        = 20,
    RST        = 21
};

//=================================
class UserType
{
public:


    //===========Rff
    static double  compassPitchOffset;
    static double  compassPanOffset;
    //===========Servo
    static double  servoPanTiltMechanicalOffset;
    static double  Servo_TiltThreshold;
    static QString panTiltStopEarlier;
    static double  Servo_PanThreshold;
    //===========Gis
    static double  gis_elevation_range;
    static double  gis_azimuth_range;
    static double  gis_emit_state;
    static double  gis_elevation;
    static double  gisRadarLong;
    static double  gisRadarLat;
    static double  gis_azimuth;
    static double  gisLong;
    static double  gisLAt;
    //===========Cooling
    static double coolingMinNormalMotorCurrent;
    static double coolingMaxNormalMotorCurrent;
    static double coolingMinNormalCompCurrent;
    static double coolingMaxNormalCompCurrent;
    static double coolingCriticalTemperature;
    static double coolingMinNormalFanCurrent;
    static double coolingMaxNormalFanCurrent;
    static double coolingManualOffset;
    static bool   coolingSystemOn;
//    static QString coolingMode1;
//    static QString coolingMode2;
//    static QString coolingMode3;


public:
    UserType();
};

#endif // USERTYPE_H
