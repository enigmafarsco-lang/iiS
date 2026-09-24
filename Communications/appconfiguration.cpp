#include "appconfiguration.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QTextStream>
#include <QJsonObject>


AppConfiguration::AppConfiguration()
{
    m_settingFilename = "Appsetting.json";
    m_setting = new AppSettings;
    defaultSetting();
    readSettingFromFile();
}



void AppConfiguration::updatAttributesFromUserType()
{
    // === Servo
    m_setting->servoPanTiltMechanicalOffset = UserType::servoPanTiltMechanicalOffset;
    m_setting->Servo_TiltThreshold = UserType::Servo_TiltThreshold;
    m_setting->Servo_PanThreshold = UserType::Servo_PanThreshold;

    //=== Gis
    m_setting->gis_elevation_range = UserType::gis_elevation_range;
    m_setting->gis_azimuth_range = UserType::gis_azimuth_range;
    m_setting->gis_emit_state = UserType::gis_emit_state;
    m_setting->gis_elevation = UserType::gis_elevation;
    m_setting->gisRadarLong = UserType::gisRadarLong;
    m_setting->gisRadarLat = UserType::gisRadarLat;
    m_setting->gis_azimuth = UserType::gis_azimuth;
    m_setting->gisLong = UserType::gisLong;
    m_setting->gisLAt = UserType::gisLAt;

    //====
    m_setting->compassPitchOffset = UserType::compassPitchOffset;
    m_setting->panTiltStopEarlier = UserType::panTiltStopEarlier;

    //===
    m_setting->compassPanOffset = UserType::compassPanOffset;
}


AppConfiguration::~AppConfiguration()
{
    updatAttributesFromUserType();
    WriteSettingToFile();
}


bool AppConfiguration::readSettingFromFile()
{
    QFile loadFile(m_settingFilename);

    if ( (!loadFile.open(QIODevice::ReadOnly))) {
        qWarning("Couldn't open save file.");
        return false;
    }

    QByteArray saveData = loadFile.readAll();

    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    QJsonObject json = loadDoc.object();

    QJsonObject networkConfigJson;
    QJsonObject servoConfigJson;
    QJsonObject gisConfigJson;
    QJsonObject rffConfigJson;
    QJsonObject coolingCofigJson;
    //    QJsonObject tunerCofigJson;
    QJsonObject outerNodeConfigJson;

    //========== Network ===================
    if(json.contains("network_configuration"))
    {
        networkConfigJson = json["network_configuration"].toObject();

        if (networkConfigJson.contains("RffIpAddress"))
            m_setting->RffIpAddress = (networkConfigJson["RffIpAddress"].toString());
        if (networkConfigJson.contains("RffPort"))
            m_setting->RffPort = (networkConfigJson["RffPort"].toInt());
        if (networkConfigJson.contains("RffCommunicationType"))
            m_setting->RffCommunicationType = networkConfigJson["RffCommunicationType"].toString();
        if (networkConfigJson.contains("RffLoggingFormat"))
            m_setting->RffLoggingFormat = networkConfigJson["RffLoggingFormat"].toString();

        if (networkConfigJson.contains("ServoIpAddress"))
            m_setting->servoIpAddress = (networkConfigJson["ServoIpAddress"].toString());
        if (networkConfigJson.contains("ServoPort"))
            m_setting->servoPort = (networkConfigJson["ServoPort"].toInt());
        if (networkConfigJson.contains("ServoCommunicationType"))
            m_setting->servoCommunicationType = networkConfigJson["ServoCommunicationType"].toString();
        if (networkConfigJson.contains("ServoLoggingFormat"))
            m_setting->servoLoggingFormat = networkConfigJson["ServoLoggingFormat"].toString();

        if (networkConfigJson.contains("SolidIpAddress"))
            m_setting->solidIpAddress = (networkConfigJson["SolidIpAddress"].toString());
        if (networkConfigJson.contains("SolidPort"))
            m_setting->solidPort = (networkConfigJson["SolidPort"].toInt());
        if (networkConfigJson.contains("SolidCommunicationType"))
            m_setting->solidCommunicationType = networkConfigJson["SolidCommunicationType"].toString();
        if (networkConfigJson.contains("SolidLoggingFormat"))
            m_setting->solidLoggingFormat = networkConfigJson["SolidLoggingFormat"].toString();

        if(networkConfigJson.contains("coolingIpAddress"))
            m_setting->coolingIpAddress = networkConfigJson["coolingIpAddress"].toString();
        if(networkConfigJson.contains("coolingPort"))
            m_setting->coolingPort = networkConfigJson["coolingPort"].toInt();
        if(networkConfigJson.contains("coolingCommunicationType"))
            m_setting->coolingCommunicationType = networkConfigJson["coolingCommunicationType"].toString();
        if(networkConfigJson.contains("coolingLoggingFormat"))
            m_setting->coolingLoggingFormat = networkConfigJson["coolingLoggingFormat"].toString();

        if (networkConfigJson.contains("tunerIpAddress"))
            m_setting->tunerIpAddress = (networkConfigJson["tunerIpAddress"].toString());
        if (networkConfigJson.contains("tunerPort"))
            m_setting->tunerPort = (networkConfigJson["tunerPort"].toInt());
        if (networkConfigJson.contains("tunerCommunicationType"))
            m_setting->tunerCommunicationType = networkConfigJson["tunerCommunicationType"].toString();
        if (networkConfigJson.contains("tunerLoggingFormat"))
            m_setting->tunerLoggingFormat = networkConfigJson["tunerLoggingFormat"].toString();

        if (networkConfigJson.contains("pBoardIpAddress"))
            m_setting->pBoardIpAddress = (networkConfigJson["pBoardIpAddress"].toString());

        if (networkConfigJson.contains("joshanIpAddress"))
            m_setting->joshanIpAddress = (networkConfigJson["joshanIpAddress"].toString());

        if (networkConfigJson.contains("joshanPort"))
            m_setting->joshanPort = (networkConfigJson["joshanPort"].toInt());

        if (networkConfigJson.contains("calibrationIpAddress"))
            m_setting->calibrationAddress = (networkConfigJson["calibrationIpAddress"].toString());

        if (networkConfigJson.contains("calibrationPort"))
            m_setting->calibrationPort = (networkConfigJson["calibrationPort"].toInt());
    }

    //========== Servo ===================
    if(json.contains("servo_configuration"))
    {
        servoConfigJson = json["servo_configuration"].toObject();

        if (servoConfigJson.contains("servoPanTiltMechanicalOffset"))
            m_setting->servoPanTiltMechanicalOffset = servoConfigJson["servoPanTiltMechanicalOffset"].toDouble(0);

        if (servoConfigJson.contains("panTiltStopEarlier"))
            m_setting->panTiltStopEarlier = servoConfigJson["panTiltStopEarlier"].toString();

        if (servoConfigJson.contains("Servo_PanThreshold"))
            m_setting->Servo_PanThreshold = servoConfigJson["Servo_PanThreshold"].toDouble(0);

        if (servoConfigJson.contains("Servo_TiltThreshold"))
            m_setting->Servo_TiltThreshold = servoConfigJson["Servo_TiltThreshold"].toDouble(0);
    }

    //========== Gis ===================
    if(json.contains("gis_configuration"))
    {
        gisConfigJson = json["gis_configuration"].toObject();

        if (gisConfigJson.contains("gis_azimuth"))
            m_setting->gis_azimuth = gisConfigJson["gis_azimuth"].toDouble(0);

        if (gisConfigJson.contains("gis_elevation"))
            m_setting->gis_elevation = gisConfigJson["gis_elevation"].toDouble(0);

        if (gisConfigJson.contains("gisLAt"))
            m_setting->gisLAt = gisConfigJson["gisLAt"].toDouble(0);

        if (gisConfigJson.contains("gisLong"))
            m_setting->gisLong = gisConfigJson["gisLong"].toDouble(0);

        if (gisConfigJson.contains("gisRadarLat"))
            m_setting->gisRadarLat = gisConfigJson["gisRadarLat"].toDouble(0);

        if (gisConfigJson.contains("gisRadarLong"))
            m_setting->gisRadarLong = gisConfigJson["gisRadarLong"].toDouble(0);
    }

    //========== RFF ===================
    if(json.contains("rff_configuration"))
    {
        rffConfigJson = json["rff_configuration"].toObject();

        if (rffConfigJson.contains("compassPanOffset"))
            m_setting->compassPanOffset = rffConfigJson["compassPanOffset"].toDouble(0);

        if (rffConfigJson.contains("compassPitchOffset"))
            m_setting->compassPitchOffset = rffConfigJson["compassPitchOffset"].toDouble(0);
    }


    //========== Cooling ===================
    if(json.contains("cooling_configuration"))
    {
        coolingCofigJson = json["cooling_configuration"].toObject();

        if(coolingCofigJson.contains("coolingManualTempretureOffset"))
            m_setting->coolingManualTempretureOffset = coolingCofigJson["coolingManualTempretureOffset"].toDouble();

        if(coolingCofigJson.contains("coolingMinNormalMotorCurrent"))
            m_setting->coolingMinNormalMotorCurrent = coolingCofigJson["coolingMinNormalMotorCurrent"].toDouble();

        if(coolingCofigJson.contains("coolingMaxNormalMotorCurrent"))
            m_setting->coolingMaxNormalMotorCurrent = coolingCofigJson["coolingMaxNormalMotorCurrent"].toDouble();

        if(coolingCofigJson.contains("coolingMinNormalCompCurrent"))
            m_setting->coolingMinNormalCompCurrent = coolingCofigJson["coolingMinNormalCompCurrent"].toDouble();

        if(coolingCofigJson.contains("coolingMaxNormalCompCurrent"))
            m_setting->coolingMaxNormalCompCurrent = coolingCofigJson["coolingMaxNormalCompCurrent"].toDouble();

        if(coolingCofigJson.contains("coolingMinNormalFanCurrent"))
            m_setting->coolingMinNormalFanCurrent = coolingCofigJson["coolingMinNormalFanCurrent"].toDouble();

        if(coolingCofigJson.contains("coolingMaxNormalFanCurrent"))
            m_setting->coolingMaxNormalFanCurrent = coolingCofigJson["coolingMaxNormalFanCurrent"].toDouble();

        if(coolingCofigJson.contains("coolingCriticalTempreture"))
            m_setting->coolingCriticalTempreture = coolingCofigJson["coolingCriticalTempreture"].toDouble();

        if(coolingCofigJson.contains("coolingMotorCosnt"))
            m_setting->coolingMotorCosnt = coolingCofigJson["coolingMotorCosnt"].toDouble();

        if(coolingCofigJson.contains("coolingFanConst"))
            m_setting->coolingFanConst = coolingCofigJson["coolingFanConst"].toDouble();

        if(coolingCofigJson.contains("coolingCompConst"))
            m_setting->coolingCompConst = coolingCofigJson["coolingCompConst"].toDouble();
    }

    //========== Outer node ===================
    if(json.contains("outernode_configuration"))
    {
        outerNodeConfigJson = json["outernode_configuration"].toObject();

        if (outerNodeConfigJson.contains("includeRadar"))

            m_setting->includeRadar = outerNodeConfigJson["includeRadar"].toString();
    }


    // ========== init static variable =========
    UserType::compassPanOffset = m_setting->compassPanOffset;
    UserType::compassPitchOffset = m_setting->compassPitchOffset;
    UserType::panTiltStopEarlier = m_setting->panTiltStopEarlier;

    UserType::Servo_PanThreshold = m_setting->Servo_PanThreshold;
    UserType::Servo_TiltThreshold =m_setting->Servo_TiltThreshold;
    UserType::servoPanTiltMechanicalOffset =m_setting->servoPanTiltMechanicalOffset;

    UserType::gisLAt = m_setting->gisLAt;
    UserType::gisLong = m_setting->gisLong;
    UserType::gis_azimuth = m_setting->gis_azimuth;
    UserType::gisRadarLat = m_setting->gisRadarLat;
    UserType::gisRadarLong = m_setting->gisRadarLong;
    UserType::gis_elevation = m_setting->gis_elevation;
    UserType::gis_emit_state = m_setting->gis_emit_state;
    UserType::gis_azimuth_range = m_setting->gis_azimuth_range;
    UserType::gis_elevation_range = m_setting->gis_elevation_range;

    //    UserType::coolingMode1 = m_setting->coolingMood1;
    //    UserType::coolingMode2 = m_setting->coolingMood2;
    //    UserType::coolingMode3 = m_setting->coolingMood3;

    UserType::coolingManualOffset = m_setting->coolingManualTempretureOffset;
    UserType::coolingCriticalTemperature = m_setting->coolingCriticalTempreture;
    UserType::coolingMinNormalFanCurrent = m_setting->coolingMinNormalFanCurrent;
    UserType::coolingMaxNormalFanCurrent = m_setting->coolingMaxNormalFanCurrent;
    UserType::coolingMinNormalCompCurrent = m_setting->coolingMinNormalCompCurrent;
    UserType::coolingMaxNormalCompCurrent = m_setting->coolingMaxNormalCompCurrent;
    UserType::coolingMinNormalMotorCurrent = m_setting->coolingMinNormalMotorCurrent;
    UserType::coolingMaxNormalMotorCurrent = m_setting->coolingMaxNormalMotorCurrent;

    return true;
}

bool AppConfiguration::WriteSettingToFile()
{
    QFile saveFile(m_settingFilename);

    if ( (!saveFile.open(QIODevice::WriteOnly)) ) {
        qWarning("Couldn't open save file.");
        return false;
    }

    QByteArray doc = makeJson();
    saveFile.write(doc);
    return true;
}

bool AppConfiguration::defaultSetting()
{

    //=======Network====
    {
        m_setting->RffIpAddress = "192.168.1.7";
        m_setting->RffPort = 33000;
        m_setting->RffCommunicationType ="tcp";
        m_setting->RffLoggingFormat = "hex";

        m_setting->servoIpAddress = "192.168.1.8";
        m_setting->servoPort = 4002;
        m_setting->servoCommunicationType ="tcp";
        m_setting->servoLoggingFormat = "string";

        m_setting->solidIpAddress = "192.168.1.122";
        m_setting->solidPort = 8700;
        m_setting->solidCommunicationType ="udp";
        m_setting->solidLoggingFormat = "string";

        m_setting->coolingIpAddress = "192.168.1.111";
        m_setting->coolingPort=5000;
        m_setting->coolingCommunicationType = "tcp";
        m_setting->coolingLoggingFormat = "string";

        //tuner
        m_setting->tunerIpAddress = "192.168.1.9";
        m_setting->tunerPort = 23;
        m_setting->tunerCommunicationType ="tcp";
        m_setting->tunerLoggingFormat = "hex";

        //process board
        m_setting->pBoardIpAddress = "192.168.1.10";

        //joshan
        m_setting->joshanIpAddress = "5.190.48.246";
        m_setting->joshanPort = 23;


       //calibration
        m_setting->calibrationAddress = "192.168.124.55";
        m_setting->calibrationPort = 5025;

    }

    //=======Servo====
    {
        m_setting->panTiltStopEarlier = "yes";
        m_setting->Servo_PanThreshold = 2.0;
        m_setting->Servo_TiltThreshold = 1.0;
        m_setting->servoPanTiltMechanicalOffset = 0.0;
    }

    //=======Gis====
    {
        m_setting->gis_elevation_range= 20.0;
        m_setting->gis_azimuth_range= 20.0;
        m_setting->gis_emit_state= 0.0;
        m_setting->gis_elevation = 3.0;
        m_setting->gis_azimuth = 30.0;
        m_setting->gisRadarLong= 51.0;
        m_setting->gisRadarLat= 30.0 ;
        m_setting->gisLong= 54.45;
        m_setting->gisLAt = 32.7;
    }


    //=======RFF====
    {
        m_setting->compassPanOffset = 0.0;
        m_setting->compassPitchOffset = 0.0;
    }

    //=======Cooling====
    {
        m_setting->coolingManualTempretureOffset=0;
        m_setting->coolingMinNormalMotorCurrent=2.0;
        m_setting->coolingMaxNormalMotorCurrent=5.5;
        m_setting->coolingMinNormalCompCurrent=3.5;
        m_setting->coolingMaxNormalCompCurrent=8.0;
        m_setting->coolingMinNormalFanCurrent=0.4;
        m_setting->coolingMaxNormalFanCurrent=1.2;
        m_setting->coolingCriticalTempreture=70;
        m_setting->coolingMotorCosnt=9500;
        m_setting->coolingCompConst=9600;
        m_setting->coolingFanConst=5000;

    }
    //=======outernode====
    {
        m_setting->includeRadar = "yes";
    }

    return true;
}

QByteArray AppConfiguration::makeJson()
{
    QJsonObject networksettingObject;
    {
        networksettingObject["RffCommunicationType"] = m_setting->RffCommunicationType;
        networksettingObject["RffLoggingFormat"] = m_setting->RffLoggingFormat;
        networksettingObject["RffIpAddress"] = m_setting->RffIpAddress;
        networksettingObject["RffPort"] = m_setting->RffPort;

        networksettingObject["ServoCommunicationType"] = m_setting->servoCommunicationType;
        networksettingObject["ServoLoggingFormat"] = m_setting->servoLoggingFormat;
        networksettingObject["ServoIpAddress"] = m_setting->servoIpAddress;
        networksettingObject["ServoPort"] = m_setting->servoPort;

        networksettingObject["SolidCommunicationType"] = m_setting->solidCommunicationType;
        networksettingObject["SolidLoggingFormat"] = m_setting->solidLoggingFormat;
        networksettingObject["SolidIpAddress"] = m_setting->solidIpAddress;
        networksettingObject["SolidPort"] = m_setting->solidPort;

        networksettingObject["coolingCommunicationType"] = m_setting-> coolingCommunicationType;
        networksettingObject["coolingLoggingFormat"] = m_setting-> coolingLoggingFormat;
        networksettingObject["coolingIpAddress"] = m_setting-> coolingIpAddress;
        networksettingObject["coolingPort"] = m_setting->coolingPort;

        networksettingObject["tunerCommunicationType"] = m_setting->tunerCommunicationType;
        networksettingObject["tunerLoggingFormat"] = m_setting->tunerLoggingFormat;
        networksettingObject["tunerIpAddress"] = m_setting->tunerIpAddress;
        networksettingObject["tunerPort"] = m_setting->tunerPort;

        networksettingObject["pBoardIpAddress"] = m_setting->pBoardIpAddress;

        networksettingObject["joshanIpAddress"] = m_setting->joshanIpAddress;
        networksettingObject["joshanPort"] = m_setting->joshanPort;

        networksettingObject["calibrationIpAddress"] = m_setting->calibrationAddress;
        networksettingObject["calibrationPort"] = m_setting->calibrationPort;
    }

    QJsonObject servoSettingObject;
    {
        servoSettingObject["servoPanTiltMechanicalOffset"] = m_setting->servoPanTiltMechanicalOffset;
        servoSettingObject["Servo_TiltThreshold"]= m_setting->Servo_TiltThreshold;
        servoSettingObject["panTiltStopEarlier"] = m_setting->panTiltStopEarlier;
        servoSettingObject["Servo_PanThreshold"]= m_setting->Servo_PanThreshold;
    }

    QJsonObject gisSettingObject;
    {
        gisSettingObject["gis_elevation_range"] = m_setting->gis_elevation_range;
        gisSettingObject["gis_azimuth_range"] = m_setting->gis_azimuth_range;
        gisSettingObject["gis_emit_state"] = m_setting->gis_emit_state;
        gisSettingObject["gis_elevation"] = m_setting->gis_elevation;
        gisSettingObject["gisRadarLong"] = m_setting->gisRadarLong;
        gisSettingObject["gisRadarLat"] = m_setting->gisRadarLat;
        gisSettingObject["gis_azimuth"] = m_setting->gis_azimuth;
        gisSettingObject["gisLong"] = m_setting->gisLong;
        gisSettingObject["gisLAt"] = m_setting->gisLAt;
    }


    QJsonObject rffSettingObject;
    {
        rffSettingObject["compassPitchOffset"] = m_setting->compassPitchOffset;
        rffSettingObject["compassPanOffset"] = m_setting->compassPanOffset;
    }

    QJsonObject coolingSettingObject;
    {
        coolingSettingObject["coolingManualTempretureOffset"] = m_setting->coolingManualTempretureOffset;
        coolingSettingObject["coolingMinNormalMotorCurrent"] = m_setting->coolingMinNormalMotorCurrent;
        coolingSettingObject["coolingMaxNormalMotorCurrent"] = m_setting->coolingMaxNormalMotorCurrent;
        coolingSettingObject["coolingMinNormalCompCurrent"] = m_setting->coolingMinNormalCompCurrent;
        coolingSettingObject["coolingMaxNormalCompCurrent"] = m_setting->coolingMaxNormalCompCurrent;
        coolingSettingObject["coolingMinNormalFanCurrent"] = m_setting->coolingMinNormalFanCurrent;
        coolingSettingObject["coolingMaxNormalFanCurrent"] = m_setting->coolingMaxNormalFanCurrent;
        coolingSettingObject["coolingCriticalTempreture"] = m_setting->coolingCriticalTempreture;
        coolingSettingObject["coolingMotorCosnt"] = m_setting->coolingMotorCosnt;
        coolingSettingObject["coolingFanConst"] = m_setting-> coolingFanConst;
        coolingSettingObject["coolingCompConst"] = m_setting->coolingCompConst;
    }

    QJsonObject outerSettingObject;
    {
        outerSettingObject["includeRadar"] = m_setting->includeRadar;
    }



    QJsonObject totalSettingObject;
    totalSettingObject["network_configuration"] = networksettingObject;
    totalSettingObject["outernode_configuration"] = outerSettingObject;
    totalSettingObject["cooling_configuration"] = coolingSettingObject;
    totalSettingObject["servo_configuration"] = servoSettingObject;
    totalSettingObject["gis_configuration"] = gisSettingObject;
    totalSettingObject["rff_configuration"] = rffSettingObject;


    QJsonDocument saveDoc(totalSettingObject);

    return(saveDoc.toJson());
}
