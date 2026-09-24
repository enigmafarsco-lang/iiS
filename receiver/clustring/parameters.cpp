#include "parameters.h"

#pragma region S Band Parameters{

Enumerations::PulseLogPoint Parameters::LogPointS = Enumerations::PulseLogPoint::ADC_DDS;
Enumerations::PulseDataPath Parameters::DataPathS = Enumerations::PulseDataPath::Signal_In_1;
Enumerations::SignalType    Parameters::PulseModeS = Enumerations::SignalType::Pulse;
double  Parameters::PulseWidthS = 3E-6; //3 * (10 ^ -6);
double  Parameters::PulsePeriodS = 12E-6; //12 * (10 ^ -6);
double  Parameters::F0S = 20E-6;//20 * (10 ^ -6);
double  Parameters::DftSpaceS = 2E-6;
int  Parameters::Threshold1S = 20;
int  Parameters::Threshold2S = 30;

#pragma endregion S Band Parameters}

#pragma region L Band Parameters{

Enumerations::PulseLogPoint Parameters::LogPointL =  Enumerations::PulseLogPoint::ADC_DDS;
Enumerations::PulseDataPath Parameters::DataPathL =  Enumerations::PulseDataPath::Signal_In_1;
Enumerations::SignalType    Parameters::PulseModeL = Enumerations::SignalType::Pulse;
double  Parameters::PulseWidthL = 3E-6; //3 * (10 ^ -6);
double  Parameters::PulsePeriodL = 12E-6; //12 * (10 ^ -6);
double  Parameters::F0L = 20E-6;//20 * (10 ^ -6);
double  Parameters::DftSpaceL = 2E-6;
int  Parameters::Threshold1L = 20;
int  Parameters::Threshold2L = 30;

#pragma endregion L Band Parameters}

#pragma region X Band Parameters{

Enumerations::PulseLogPoint Parameters::LogPointX =  Enumerations::PulseLogPoint::ADC_DDS;
Enumerations::PulseDataPath Parameters::DataPathX =  Enumerations::PulseDataPath::Signal_In_1;
Enumerations::SignalType    Parameters::PulseModeX = Enumerations::SignalType::Pulse;
double  Parameters::PulseWidthX = 3E-6; //3 * (10 ^ -6);
double  Parameters::PulsePeriodX = 12E-6; //12 * (10 ^ -6);
double  Parameters::F0X = 20E-6;//20 * (10 ^ -6);
double  Parameters::DftSpaceX = 2E-6;
int  Parameters::Threshold1X = 20;
int  Parameters::Threshold2X = 30;

#pragma endregion X Band Parameters}

Parameters::Parameters()
{

}
