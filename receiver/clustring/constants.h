#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

class Constants
{
public:
    Constants();

    static QString sweepMode ;
    static QString fixMode ;
    static QString fastScan ;
    static QString hrsMode ;

    static double Min_Frequency ;
    static double Max_Frequency ;

    static const int MinFrequency_L;
    static const int MinFrequency_S;
    static const int MinFrequency_X;

    static const int MaxFrequency_L;
    static const int MaxFrequency_S;
    static const int MaxFrequency_X;

    static const int Min_PA;
    static const int Max_PA;

    static const uint Min_PW;
    static const uint Max_PW;

    static const uint Min_PRI;
    static const uint Max_PRI;

    static const uint Min_PRF;
    static const uint Max_PRF;

    static const int Min_ClusterNumber;
    static const int Max_ClusterNumber;

    static int Filter160[4];
    static int Filter125[4];
    static int filter160;
    static int filter160_Fix;

    static int stepNormalS;
    static int stepOnRadarS;
    static int stepFsTunerS; // Filter / 2

    static int stepNormalL;
    static int stepOnRadarL;
    static int stepFsTunerL; // Filter / 2

    static int stepNormalX;
    static int stepOnRadarX;
    static int stepFsTunerX;

    static const int pulseCountThreshold;
    static const int batchCount_FS ;
    static const int tunerErrors ;
    static const int invalidPulsePA ;

    // Card Parameters
    static int numWordsFsS ;
    static int numWordsNormalS;
    static int numWordsRadarS ;
    static int numWordsFixS   ;

    static int numWordsFsL    ;
    static int numWordsNormalL;
    static int numWordsRadarL ;
    static int numWordsFixL   ;

    static int numWordsFsX    ;
    static int numWordsNormalX;
    static int numWordsRadarX ;
    ////////////////////////////////////////////////////////////////////

    // Sweep Algorithm Parameters
    static int sweepCountClusteringThresholdS;
    static int sweepCountClusteringThresholdL;
    static long pulseBatchCeil ;

    // Links
    static int gsmLower  ;
    static int gsmUpper  ;
    static int wimaxLower;
    static int wimaxUpper;

    //
    static double paThreshold_Reduce   ;
    static double freqThreshold_Reduce ;
    static double freqThreshold_cwPulse;

    static bool maxHoldX;
    ////////////////////////////////////////////////////////////////////

    // Logging
    static bool activeLogL ;
    static bool activeLogS ;
    static bool activeLogX ;
    static QString logPathL ;
    static QString logPathS ;
    static QString logPathX ;
    static QString picPath;
    static QString startLog ;
    static QString finishLog ;

    static double freqTolerance ;
    static double paTolerance ;

    static const int gridMaxRows ;

    // Spectrum Mode
    static const double TUNER_BAND_WIDTH ;

#pragma region Clustering{

    static const int maxClusterCount;

    static const double Low_PA ;
    static const int High_PA ;
    static const int PW ;
    static const int Near_Freq ;
    static const double Exact_Freq ;

#pragma endregion}

};

#endif // CONSTANTS_H
