/**
 * @file joshan.cpp
 * @brief Subsystem module for joshan in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#include "joshan.h"
#include "ui_joshan.h"

Joshan::Joshan(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Joshan)
{
    ui->setupUi(this);

    init();

    connections();
}

/**
 * @brief Retrieves the current oshan stop setter value.
 * @param val Input parameter specifying val (bool).
 */
void Joshan::isJoshanStopSetter(bool val)
{
    isJoshanStop = val;

    QJsonObject s;
    //    s.insert();

    QByteArray f{"@{\"parameters\":{\"cooling_compressor\":0,\"cooling_fan_current\":0,\"cooling_lan_status\":false,\"cooling_motor_current\":0,\"cooling_water_current\":0,\"rff_humid\":24,\"rff_lan_status\":false,\"rff_temp\":24,\"servo_lan_status\":false,\"solid_active\":false,\"solid_amplifire_temperature\":0,\"solid_current\":0,\"solid_dc_supply\":0,\"solid_driver_temperature\":0,\"solid_fan\":0,\"solid_forward_power\":0,\"solid_gate_level\":0,\"solid_internal_lock\":0,\"solid_internal_temperature\":0,\"solid_lan_status\":false,\"solid_reverse_power\":0,\"solid_serial_link\":0,\"solid_voltage\":0,\"solid_vswr\":null},\"sequencenumber\":1,\"type\":\"control_servo\"}$"};

    on_DeviceResponseSlot(f);
}

/**
 * @brief Executes the joshan is connected procedure for Joshan.
 * @param connected Input parameter specifying connected (bool).
 */
void Joshan::joshanIsConnected(bool connected)
{
    if(connected)
    {
        joshanFunctionalTimer->start(1000);
        joshanStatusTimer->start(1000);
    }

    else
    {

    }
}


/**
 * @brief Destructor for Joshan, releasing allocated hardware resources, timers, and buffers.
 */
Joshan::~Joshan()
{
    delete ui;
}

/**
 * @brief Initializes and configures the init for the Joshan subsystem.
 */
void Joshan::init()
{
    //    /*joshanFuncData*/ = new joshanFuncStruct;
    //    joshanStatusData = new joshanStatusStruct;

    mJoshan = new BTcpLan(mAppConfig, JOSHAN_DEVICE_NAME);
    mJoshan->setTimerCheckingInterval(-1);

    //these timers are for sending data to the joshan
    joshanFunctionalTimer = new QTimer();
    joshanStatusTimer = new QTimer();

}

/**
 * @brief Executes the connections procedure for Joshan.
 */
void Joshan::connections()
{

    connect(joshanFunctionalTimer, &QTimer::timeout, [=]{emit sendFuncDataToJoshanSignal();});
    connect(joshanStatusTimer,     &QTimer::timeout, [=]{emit sendStatusDataToJoshanSignal();});

}

/**
 * @brief Slot handler responding to on  device response slot event.
 * @param response Input parameter specifying response (QByteArray).
 */
void Joshan::on_DeviceResponseSlot(QByteArray response)
{
    if (isJoshanStop)
    {
        if(response.isEmpty()         ||
                response.isNull()     ||
                response.at(0) != '@' ||
                response.at(response.length() -1) != '$')
        {
            return;
        }

        response = response.simplified();
        response =response.replace(" ", "");

        response.remove(0,1);
        response.remove(response.length()-1, 1);
        QJsonDocument jd = QJsonDocument::fromJson(response);
        QJsonObject joshanData =jd.object();

        QString typeName = joshanData["type"].toString().trimmed();

//        bool p = typeName == "control_solid";
//        qInfo() << typeName;
        if (joshanData["type"].toString() == "control_servo")
        {
            emit joshanDataToServoSignal (joshanData);
        }

        else if (typeName == "control_solid")
        {
            emit joshanDataToSolidSignal (joshanData);
        }

        else if (typeName == "control_exciter_set_mode")
        {
            emit joshanDataToExciterSetSignal(joshanData);
        }

        else if (typeName == "control_exciter_enable")
        {
            emit joshanDataToExciterEnableSignal(joshanData);
        }

        else if (typeName == "control_cooling")
        {
            emit joshanDataToCoolingSignal (joshanData);
        }

        else if (typeName == "control_rff")
        {
            emit joshanDataToRffSignal (joshanData);
        }

        else if (typeName == "con") {

        }
    }
}

/**
 * @brief Retrieves the current func data joshan slot value.
 * @param deviceData Input parameter specifying deviceData (QJsonObject).
 */
void Joshan::getFuncDataJoshanSlot(QJsonObject deviceData)
{
    deviceData.remove("device");

    for(auto it = deviceData.begin(); it != deviceData.end();it++)
    {
        deviceFuncData.insert(it.key(),it.value());
    }
    cntFunc++;


    if( cntFunc == 5)
    {
        QJsonObject functionalDataJson;
        functionalDataJson["type"] =           "functional_report";
        functionalDataJson["sequencenumber"] = 1;
        functionalDataJson["parameters"] =     deviceFuncData;

        QJsonDocument c (functionalDataJson);
        QByteArray m_commandBytes = "@"+c.toJson(QJsonDocument::Compact)+"$";

        emit sendCommandToDeviceSignal(m_commandBytes);

        cntFunc = 0;
        QJsonObject t;
        deviceFuncData.swap(t);
    }
}

/**
 * @brief Retrieves the current status data joshan slot value.
 * @param deviceData Input parameter specifying deviceData (QJsonObject).
 */
void Joshan::getStatusDataJoshanSlot(QJsonObject deviceData)
{
    deviceData.remove("device");

    for(auto it = deviceData.begin(); it != deviceData.end();it++)
    {
        deviceStatusData.insert(it.key(),it.value());
    }
    cntStatus++;


    if( cntStatus == 5)
    {
        QJsonObject functionalDataJson;
        functionalDataJson["type"] =           "status_report";
        functionalDataJson["sequencenumber"] = 1;
        functionalDataJson["parameters"] =     deviceStatusData;

        QJsonDocument c (functionalDataJson);
        QByteArray m_commandBytes = "@"+c.toJson(QJsonDocument::Compact)+"$";

        emit sendCommandToDeviceSignal(m_commandBytes);

        cntStatus = 0;
        QJsonObject t;
        deviceStatusData.swap(t);
    }
}



//void Joshan::gettingRffData()
//{
//    this->lnaStatus=lnaStatus;
//}

void Joshan::handle_info(QString response)
{

}
