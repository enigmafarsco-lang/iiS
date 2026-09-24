#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "receiver/clustring/enumerations.h"

class Parameters
{

public:
    Parameters();

#pragma region S Band Parameters{

    static Enumerations::PulseLogPoint LogPointS ;
    static Enumerations::PulseDataPath DataPathS ;
    static Enumerations::SignalType PulseModeS;
    static double PulseWidthS;
    static double PulsePeriodS;
    static double F0S;
    static double DftSpaceS;
    static int Threshold1S;
    static int Threshold2S;

#pragma endregion S Band Parameters}

#pragma region L Band Parameters{

    static Enumerations::PulseLogPoint LogPointL;
    static Enumerations::PulseDataPath DataPathL;
    static Enumerations::SignalType PulseModeL;
    static double PulseWidthL;
    static double PulsePeriodL;
    static double F0L;
    static double DftSpaceL;
    static int Threshold1L;
    static int Threshold2L;

#pragma endregion L Band Parameters}

#pragma region X Band Parameters{

    static Enumerations::PulseLogPoint LogPointX;
    static Enumerations::PulseDataPath DataPathX;
    static Enumerations::SignalType PulseModeX;
    static double PulseWidthX;
    static double PulsePeriodX;
    static double F0X;
    static double DftSpaceX;
    static int Threshold1X;
    static int Threshold2X;

#pragma endregion X Band Parameters}
};

#endif // PARAMETERS_H
