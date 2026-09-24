#include "targetdetector.h"
#include "ui_targetdetector.h"

TargetDetector::TargetDetector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TargetDetector)
{
    ui->setupUi(this);
}

TargetDetector::~TargetDetector()
{
    delete ui;
}

void TargetDetector::on_chbAutoSet_clicked(bool checked)
{

}

void TargetDetector::receiveCommandFromObjectSlot(QString response)
{

    QStringList frame;
    frame = response.split(',');
    if (frame[0]==("SETBTNCLICKED") && frame.length()==2)
    {

    }
}

void TargetDetector::udpDeviceResponseSlot(QByteArray response)
{
    QString packet;
    bool result1 = true;
    bool result2 = true;
    double latitude = 0;
    double longtitude = 0;

    QStringList frame;
    packet = response.simplified();
    packet =packet.replace(" ", "");

    if (packet.contains(("@RadarData,")) && packet.contains("$")){

        packet = packet.replace("@RadarData", "");
        packet = packet.replace("$", "");
        frame = packet.split(',');

        if(frame.isEmpty() && frame.length() == 2)
        {
            latitude = frame[0].toDouble(&result1);
            longtitude = frame[1].toDouble(&result2);

            UserType::gisRadarLat = latitude;
            UserType::gisRadarLong = longtitude;

            if(    result1
                   && result2
                   && latitude   >= 0
                   && longtitude <=360
                   && longtitude >= -10
                   && longtitude <= 30 )
            {

                BearingDegree = latitude;
                elevationDegree = longtitude;
                result1 = result2 = true;
            }

            else {
                BearingDegree = -1;
                elevationDegree = -1;
                result1 = result2 = false;
            }

            if(result1 && result2)
            {
                ui->EdtLat->setText(QString::number(latitude,'f',2));
                ui->EdtLong->setText(QString::number(longtitude,'f',2));
            }
        }
    }
}

void TargetDetector::tcpDeviceResponseSlot(QByteArray response)
{
    if (response.isNull() || response.isEmpty())
    {
        return;
    }

    int len = response.length();
    int bearing = 0;
    int elevation = 0;
    char checksum = 0;
    double bearingDegree = 0;
    double elevationDegree = 0;

    for (int i = 0; i < len - 7 ; i++){
        char result = Utils::AND_TowBytes(response[i], 0x44);
        if(Utils::compare_TowBytes(result , 0x40)){
            checksum = 0;
            for(int j =0; j < 7; j++){
                checksum = Utils::XOR_TowBytes(checksum, response[i+j]);
            }

            checksum = Utils::AND_TowBytes(checksum, 0x3f);

            if (Utils::compare_TowBytes( Utils::AND_TowBytes(response[i+7], 0x3f),checksum)){

                if(Utils::compare_TowBytes(0x80 , Utils::AND_TowBytes(response[i+7], 0x80))){

                    if(Utils::compare_TowBytes(0x00 , Utils::AND_TowBytes(response[i+6], 0x1))){

                        bearing = 0;
                        bearing |= (Utils::AND_TowBytes(response[i+0], 0x3f) << 12);
                        bearing |= (Utils::AND_TowBytes(response[i+1], 0x3f) << 6) ;
                        bearing |= (Utils::AND_TowBytes(response[i+2], 0x3f)) ;
                        bearingDegree = bearingDegree * 360.0 / 262144.0;
                        if(bearingDegree != BearingDegree ){
                            BearingDegree = bearingDegree;
                        }

                        //==============================

                        elevation = 0;
                        elevation |= (Utils::AND_TowBytes(response[i+3], 0x3f) << 12);
                        elevation |= (Utils::AND_TowBytes(response[i+4], 0x3f) << 6) ;
                        elevation |= (Utils::AND_TowBytes(response[i+4], 0x3f)) ;
                        elevationDegree = elevationDegree * 360.0 / 262144.0 +45.0;
                        if(bearingDegree != BearingDegree ){
                            BearingDegree = bearingDegree;
                        }
                        while (elevationDegree > 360.0) {
                            elevationDegree -= 360.0;
                        }
                        if(elevationDegree > 180){
                            elevationDegree -= 360;
                        }
                        if(elevationDegree != ElevationDegree){
                            ElevationDegree = elevationDegree;
                        }



                    }
                }
            }
        }
    }
}

