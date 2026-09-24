#ifndef PULSE_H
#define PULSE_H

#include "enumerations.h"
#include "constants.h"
#include "qlist.h"
class Pulse
{

public:
    Pulse();
    Pulse(Enumerations::Band pulseBand);

    int          MinFrequency=0;
    int          MaxFrequency=0;
    double       Freq=0;
    double       Freq2=0;
    double       Db2=0;
    double       Channel=0;
    long double  TOA=0;
    double       PAPeak=0;
    double       PASum=0;
    double       PW1=0;
    double       PW2=0;
    double       PRI=0;
    double       PRF=0;
    int          CNP=0; // Cluster Number of Pulse
    int          Mode=0; // Old CW //Enumerations.SignalType
    int          Count=0;
    //saeid razini
    long double  diffToa=0;

    QList<Pulse*> *Clone(QList<Pulse*>*);

    void Update(Pulse*);



};

#endif // PULSE_H
