#ifndef ENUMERATIONS_H
#define ENUMERATIONS_H

class Enumerations
{

public:
    Enumerations();
    enum class Band{ L, S, X, None} ;
    enum class FunctionMode { Sweep, Spectrum, Scope, SweepFS, SweepHRS, SpectrumFix, SpectrumScan, Fix, NotFix, None}  ;
    enum class CardMode { Scope = 0, Receiver = 1, Spectrum = 2 }  ;
    enum class ClusteringCriterion{ Low_PA, High_PA, PW, Exact_Freq, Near_Freq } ;
    enum class PortName{ COM1, COM2, COM3, COM4, COM5, COM6, COM7, LAN1, LAN2, LAN } ;
    enum class TunerError{ WrongBand, ConnectionLost, InternalError, None } ;
    enum class ButtonStatus{ Start = 0, Stop = 1, Connect = 2, Disconnect = 3, Fix, Scan } ;

#pragma region Pulse Enumerations {

    enum class PulseLogPoint { ADC_DDS = 0, Mixer = 1, CIC = 2, FIR = 3, IFM = 6, VideoSmoothed_Video = 11, DFT_Flags = 14, Noise_Level = 12 } ;
    enum class PulseDataPath{ Signal_In_1 = 0, Signal_In_2 = 1, Signal_In_3 = 2, Synthetic = 3 } ;
    enum class SignalType { CW = 0, Pulse = 1, GSM } ;

#pragma endregion }

};

#endif // ENUMERATIONS_H
