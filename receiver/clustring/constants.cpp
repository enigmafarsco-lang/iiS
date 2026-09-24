#include "constants.h"

     QString Constants::sweepMode = " Sweep Mode";
     QString Constants::fixMode = " Fix Mode";
     QString Constants::fastScan = " Fast Scan Mode";
     QString Constants::hrsMode = " High Resolution Scan";
     double  Constants::Min_Frequency = 1000;
     double  Constants::Max_Frequency = 12000;

     const int Constants::MinFrequency_L = 1000;
     const int Constants::MinFrequency_S = 2000;
     const int Constants::MinFrequency_X = 8000;

     const int Constants::MaxFrequency_L = 2000;
     const int Constants::MaxFrequency_S = 6000;
     const int Constants::MaxFrequency_X = 12000;

     const int Constants::Min_PA = -100;
     const int Constants::Max_PA = 0;

     const uint Constants::Min_PW = 30;
     const uint Constants::Max_PW = 50000;

     const uint Constants::Min_PRI = 1;
     const uint Constants::Max_PRI = 20000;

     const uint Constants::Min_PRF = 20;
     const uint Constants::Max_PRF = 1000000;

     const int Constants::Min_ClusterNumber = 0;
     const int Constants::Max_ClusterNumber = 100;

     int Constants::Filter160[] = {5, 10, 20, 40 };
     int Constants::Filter125[] = {75, 100, 250, 500 };
     int Constants::filter160 = 10;
     int Constants::filter160_Fix = 40;

     int Constants::stepNormalS = 10;
     int Constants::stepOnRadarS = 1;
     int Constants::stepFsTunerS; // Filter / 2

     int Constants::stepNormalL = 5;
     int Constants::stepOnRadarL = 1;
     int Constants::stepFsTunerL; // Filter / 2

     int Constants::stepNormalX = 5;
     int Constants::stepOnRadarX = 1;
     int Constants::stepFsTunerX = 10;

     const int Constants::pulseCountThreshold = 10;
     const int Constants::batchCount_FS = 2;
     const int Constants::tunerErrors = 10;
     const int Constants::invalidPulsePA = -80;

    // Card Parameters
     int Constants::numWordsFsS = 512;
     int Constants::numWordsNormalS = 512;
     int Constants::numWordsRadarS = 1024;
     int Constants::numWordsFixS = 1024;

     int Constants::numWordsFsL = 512;
     int Constants::numWordsNormalL = 128;
     int Constants::numWordsRadarL = 1024;
     int Constants::numWordsFixL = 1024;

     int Constants::numWordsFsX = 512;
     int Constants::numWordsNormalX = 128;
     int Constants::numWordsRadarX = 256;
    ////////////////////////////////////////////////////////////////////

    // Sweep Algorithm Parameters
     int  Constants::sweepCountClusteringThresholdS = 1;
     int  Constants::sweepCountClusteringThresholdL = 1;
     long Constants::pulseBatchCeil = 50000;

    // Links
     int Constants::gsmLower = 1710;
     int Constants::gsmUpper = 1990;
     int Constants::wimaxLower = 2305;
     int Constants::wimaxUpper = 2360;

    //
     double Constants::paThreshold_Reduce = 0.8;
     double Constants::freqThreshold_Reduce = 0.9;
     double Constants::freqThreshold_cwPulse = 100.0;

     bool Constants::maxHoldX = false;
    ////////////////////////////////////////////////////////////////////

    // Logging
     bool Constants::activeLogL = false;
     bool Constants::activeLogS = false;
     bool Constants::activeLogX = false;
     QString Constants::logPathL = "D:\\";
     QString Constants::logPathS = "D:\\";
     QString Constants::logPathX = "D:\\";
     QString Constants::picPath = "";
     QString Constants::startLog = "Logging Started...";
     QString Constants::finishLog = "Logging Finished!";

     double Constants::freqTolerance = 5;
     double Constants::paTolerance = 5;

     const int Constants::gridMaxRows = 5;

    // Spectrum Mode
     const double Constants::TUNER_BAND_WIDTH = 61.5;


#pragma region Clustering{

    const int Constants::maxClusterCount=50;

    const double Constants::Low_PA = 0.2 ;
    const int Constants::High_PA =5;
    const int Constants::PW = 100;
    const int Constants::Near_Freq = 150;
    const double Constants::Exact_Freq = 2;

#pragma endregion}

Constants::Constants()
{

}
