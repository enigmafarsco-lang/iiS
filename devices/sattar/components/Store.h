#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QObject>
#include <QVector>
#include <QMetaType>
#include<QSettings>

#include "constant.h"

namespace projectDataSpace {
// enum rdBand{
//    BandA,BandB,BandC,BandD,BandE
//} ;


//typedef enum{
//    e_BandA1,e_BandA2,
//    e_BandB1,e_BandB2,e_BandB3,
//    e_BandC1,e_BandC2,e_BandC3,e_BandC4,e_BandC5,
//    e_BandD1,e_BandD2,e_BandD3,e_BandD4,e_BandD5,e_BandD6,e_BandD7,e_BandD8,
//    e_BandE1,e_BandE2,e_BandE3,e_BandE4,e_BandE5,e_BandE6,e_BandE7,e_BandE8,e_BandE9,e_BandE10,e_BandE11,e_BandE12,e_BandE13,e_BandE14,e_BandE15,e_BandE16
//} rdFrq;


struct projectDataStruct
{
    //    rdBand radarBand;
    //    rdFrq radarFreq;

    QString prjAddr;
    QString prjDir;
    QString dataDir;
    QString prjName;

    int radarBand{0};
    int radarFreq{0};
    int radarMode{0};

    bool isLoaded{false};
    bool isSaved{false};
    bool isSaveNeeded{true};
    bool isGenerated{false};

    QVector <uint> TimingData;
    uint srate{1250};

};

}
//#######################################
namespace priSpace {

struct priDwellStruct
{
    uint multiplier{1};
    float pri{};
    QString unit;
};


typedef  enum{
    constant,
    jitter,
    stagger,
    dwell,
    sliding,
    periodic,
    triangular
} priTypeEnum;


typedef  enum{
    PRF,
    PRI,

} periodicTypeEnum;


struct priDataStruct
{

    priTypeEnum type = priTypeEnum::constant;

    priDwellStruct priStr;
    QVector<priDwellStruct> dwell;
    QVector<float> pri;

//    QString periodType {};
    float maxPri {100000};
    float minPri {100000};
    uint  CountPri {1};
    QString priBase  {};

    uint jitterPercent {5};
    uint jitterCount   {10};

    float   slidingStep  {1};
    uint slidingCount {10};

    uint triangularCount {10};
    float triangularStep  {1};

    uint periodicCount {10};
    uint periodicMean  {1};
    float periodicFreq  {5};

    QString radarName;
    periodicTypeEnum prifType = periodicTypeEnum::PRF;

    bool isError{false};
    bool isGenerated{false};

    const uint maxPriValue {50000000};
    const uint minPriValue {500};
};

}
//#######################################
namespace radarSpace {

struct radarDataStruct
{
    double  phaseI {};
    double  phaseQ  {};
    bool    generateSimData {false};
    bool    generateAsync {false};
    ulong   durationTime {};
    ulong   shiftTime {};
    uint    maxPulseCount {};
    uint    radarPriority {};
    int     outPutPower {};
    uint    spurPercent {};
    uint    spurCount {};
    uint    missPulse {};
    uint    radarNumber {1};
    QString radarName;
    //    int mode{};
    //    int radarBand{};

};

}
//#######################################
namespace pwSpace {

typedef enum{
    constant,
    multilevel,
    random
} pwTypeEnum;

struct pwStruct
{
    uint multiplier{};
    uint pw{};
    QString unint;
};

struct pwDataStruct{


    pwStruct pwStr;

    QVector<pwStruct> multiLevel;

    QVector<uint> pw;

    pwTypeEnum Type = pwTypeEnum::constant;
    double pwConstant{25000};
    uint maxPw{};
    uint minPw{};
    uint countPw{1};

    uint randomPwPercent{10};
    uint randomPwCount{10};

    bool isError{false},isGenerated{false};

    const uint MaxPwValue = 2000000;
    const uint MinPwValue = 50;
};
}
//#######################################
namespace frqSpace {

struct diversityStruct{
    uint multiplier{1};
    float frq{};
};

typedef enum{
    Constant,
    Agile,
    Diversity,
    Hopping,

} typeEnum;

struct frqDataStruct
{

    typeEnum frqType = typeEnum::Constant;

    diversityStruct frqStr;
    QVector<diversityStruct> diversity;
    QVector<float> frq;

    float baseFrq{100};
    float maxFrq{100};
    float minFrq{100};
    float agilePercent{10};
    uint  countFrq{1};
    uint  agileCount{10};

    bool isError{false};
    bool isGenerated{false};
};
}
//#######################################
namespace modulSpace {

typedef enum{
    Simple,
    xFM,
    FSK,
    BPSK,
    QPSK
} modeTypeEnum;

typedef enum{
    LFM,
    NLFM
} xfmTypeEnum;

typedef enum{

    UniPolar,
    Bipolar
} lfmSweepDirectionEnum;

typedef enum{
    Simple1,
    Step

} fskTypeEnum;

typedef enum{
    Barker,
    Custom
} bpskTypeEnum;

typedef enum{
    Frank,
    Custom1
} QpskTypeEnum;

struct modulDataStruct{

    modeTypeEnum type = modeTypeEnum::Simple;

    //xfm
    float xfmTargetFrq{200};
    xfmTypeEnum xFMType = xfmTypeEnum::LFM;
    lfmSweepDirectionEnum LFMSweetDirection = lfmSweepDirectionEnum::UniPolar;
    float xfmTargetTime{10000};

    //fsk
    fskTypeEnum FSKType = fskTypeEnum::Simple1;
    float fskDeltaF{1};
    float fskSimpleTB{1};
    QString fskSimpleBitInfo = "1,1,0,0,1,0,1";
    QString fskStepCostas = "1,2,5,8";
    uint fskSlope {0};

    //bpsk
    bpskTypeEnum bpsKType = bpskTypeEnum::Barker;
    float bpskTB{1};
    uint bpskBarkerCode{7};
    QString bpskCustomCode = "1,0,1,0,1,1,0,0";

    //qpsk
    QpskTypeEnum qpskType = QpskTypeEnum::Frank;
    float qpskTB {1};
    uint qpskFrankCode {3};
    int isReversed {0};
    QString qpskCustomCode = "00,11,10,01";

};
}
//#######################################
namespace spureSpace {

struct spureStruct{

    priSpace::priDataStruct priSpure;
    pwSpace::pwDataStruct pwSpure;
    frqSpace::frqDataStruct frqSpure;
    modulSpace::modulDataStruct modulSpure;

    uint spurePercent{};
    uint spureCount{};
};
}
//#######################################

#endif // CONSTANTS_H
