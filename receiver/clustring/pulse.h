/**
 * @file pulse.h
 * @brief Subsystem module for clustring in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef PULSE_H
#define PULSE_H

#include "enumerations.h"
#include "constants.h"
#include "qlist.h"
/**
 * @class Pulse
 * @brief High-level controller class for Pulse.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class Pulse
{

public:
    /**
     * @brief Constructs a new Pulse instance and initializes subsystem components and UI state.
     */
    Pulse();
    /**
     * @brief Constructs a new Pulse instance and initializes subsystem components and UI state.
     * @param pulseBand Input parameter specifying pulseBand (Enumerations::Band).
     */
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
