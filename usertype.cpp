#include "usertype.h"

double  UserType::servoPanTiltMechanicalOffset;
double  UserType::compassPitchOffset = 0.0;
double  UserType::compassPanOffset = 0.0;
double  UserType::Servo_TiltThreshold;
double  UserType::gisRadarLong = 51.0;
double  UserType::gis_elevation_range;
double  UserType::Servo_PanThreshold;
double  UserType::gisRadarLat = 30.0;
double  UserType::gis_azimuth_range;
double  UserType::gisLong = 54.45;
double  UserType::gis_emit_state;
double  UserType::gis_elevation;
double  UserType::gisLAt = 32.7;
double  UserType::gis_azimuth;
double  UserType::coolingManualOffset;
double  UserType::coolingCriticalTemperature;
double  UserType::coolingMinNormalFanCurrent;
double  UserType::coolingMaxNormalFanCurrent;
double  UserType::coolingMinNormalCompCurrent;
double  UserType::coolingMaxNormalCompCurrent;
double  UserType::coolingMinNormalMotorCurrent;
double  UserType::coolingMaxNormalMotorCurrent;
bool    UserType::coolingSystemOn  = false;
//QString UserType::coolingMode1;
//QString UserType::coolingMode2;
//QString UserType::coolingMode3;
QString UserType::panTiltStopEarlier;
UserType::UserType()
{
}
