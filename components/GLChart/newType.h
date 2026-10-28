/**
 * @file newType.h
 * @brief Subsystem module for GLChart in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#pragma once

#include <QtWidgets/QWidget>
#include <QPen>
#include <QVector2D>
//#include <armadillo>
#include <QQueue>
#include <QDateTime>
#include <qopengl.h>
#include <QMutex>
#include <QDebug>
//#include "qcustomplot.h"
#include <QMetaType>
#include <QJsonObject>
// Holds all state information relevant to a character as loaded using FreeType
typedef struct {
	GLuint TextureID;   // ID handle of the glyph texture
	QVector2D Size;     // Size of glyph
	QVector2D Bearing;  // Offset from baseline to left/top of glyph
	GLuint Advance;     // Horizontal offset to advance to next glyph
}Character;

//using namespace arma;
//#include "qcustomplot.h"

#define DB_CONSTANT 0.0117589842056242654380366755752//0.75257498915995298803434723681123//1.5051499783199059760686944736225
#define OFFSET_CONSTANT_ONLINE -131.45 //-96.37
#define	CONST_2_POW_32 4294967296

#define NUMBER_OF_PANORAMA_BAND 4
#define PANORAMA_START_TUNER_FREQUENCY 2250
#define PANORAMA_STEP_TUNER_FREQUENCY 500
#define PANORAMA_STOP_TUNER_FREQUENCY 3750
#define PANORAMA_START_INDEX_FFT128 27 // 125 MHz
#define PANORAMA_STOP_INDEX_FFT128 81 // 675 MHz

#define PANORAMA_START_INDEX_FFT1024  214 //
#define PANORAMA_STOP_INDEX_FFT1024 641 // 


#define DB_COEFMUL_6_4 0.188143747289988
#define packetSizeInBytes 49
#define CONST_2_POW_20 1048576.0
#define CONST_2_POW_11 2048.0
#define CONST_2_POW_10 1024.0
#define INTER_PULSE_DB_CONSTANT 0.00293974605140606635950916889379
#define INTER_PULSE_OFFSET_CONSTANT -107.37 // -107.69
#define Bandwitdh_DAC 1100
#define POWER_MAX 25
#define CLOCK125MHZ 125 
//
//typedef struct 
//{
//	quint32			samplingFrequency;
//	quint16			filterSelect;
//	float			ddsPhase;
//	quint16			tunerFreq;
//	quint16			tunerStartFreq;
//	quint16			tunerStepFreq;
//	quint16			tunerStopFreq;
//
//}frequencyRangeParameters;
/**
 * @class Status
 * @brief High-level controller class for Status.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class Status
{
public:
	// fpga status
	bool   fpgaConnection;
	bool   fpgaWaitForConnection;
	bool   fpgaTimeoutAck;
	double fpgaTemperature;
	double fpgaTemperatureMin;
	double fpgaTemperatureMax;
	double fpgaVccInt;
	double fpgaVccIntMin;
	double fpgaVccIntMax;
	double fpgaVccAux;
	double fpgaVccAuxMin;
	double fpgaVccAuxMax;
	double fpgaVccBram;
	double fpgaVccBramMin;
	double fpgaVccBramMax;

	// tuner status
	bool   tunerConnection;
	double tunerVrIn1;
	double tunerCrTu1;
	double tunerVrIn2;
	double tunerCrTu2;
	double tunerVrLoFix;
	double tunerCrLoFix;
	double tunerVrLoVar;
	double tunerCrLoVar;

	// up converter status
	bool   upConverterConnection;
	double upConverterVrIn1;
	double upConverterCrUp1;
	double upConverterVrIn2;
	double upConverterCrUp2;
	double upConverterVrLoFix;
	double upConverterCrLoFix;
	double upConverterVrLoVar;
	double upConverterCrLoVar;

	//Power Status
	bool	microConnection;
	double	powerTunerVoltage;
	double	powerTunerCurrent;
	double	powerUpVoltage;
	double	powerUpCurrent;
	double	powerJupiterVoltage;
	double	powerJupiterCurrent;
	double	powerAmpVoltage;
	double	powerAmpCurrent;
	bool	powerTunerIsReset;
	bool	powerUpIsReset;
	bool	powerJupiterIsReset;
	bool	powerAmpIsReset;

	/**
	 * @brief Executes the to json procedure for Status.
	 * @return Output value of type QJsonObject.
	 */
	QJsonObject toJson();
	
};

/**
 * @brief Executes the q  d e c l a r e  m e t a t y p e procedure for Status.
 * @param Status Input parameter specifying Status (Status).
 */
Q_DECLARE_METATYPE(Status);
typedef struct
{
	int noiseLevel;
	int sfdrLevel;
	int alphaMA3;
	int alphaMA1;
	int LRthreshold;
	int deactiveCtrThr;
	int alphaRabbitMA1;
	int alphaRabbitMA3;
	int alpha024MA;
}hardwarePulseData;
/**
 * @brief Executes the q  d e c l a r e  m e t a t y p e procedure for Status.
 * @param hardwarePulseData Input parameter specifying hardwarePulseData (hardwarePulseData).
 */
Q_DECLARE_METATYPE(hardwarePulseData);
typedef struct {
	quint16 BW;
	quint8 inputSelect;
	quint8 hardwareAverageTime;
	qint32 centerFrequency;
	qint32 testCenterFrequency;
}hardwareZoomSetting;

typedef struct
{
//	QPair<arma::colvec, arma::colvec> signalPlotData;
//	QPair<arma::colvec, arma::colvec> envPlotData;
//	QPair<arma::colvec, arma::colvec> instFreqPlotData;
//	QPair<arma::colvec, arma::colvec> instPhasePlotData;
//	QPair<arma::colvec, arma::colvec> fftPlotData;
	//QPair<QPair<QCPRange, QCPRange>, arma::mat> stftPlotData;
}plotDataType;


/*************************************/
/*--- Inter Pulse Related Structs ---*/
/*************************************/

typedef struct
{
	//quint16 totalCount;
	quint8	coreNumber;
	quint8	counterCore;
	double	TOA;
	double	RelativeToaDetection;
	double	RelativeToaPulse;
	float	pulseWidthDetection;//logarithmic
	double	PW_Detection;
	float	pulseWidthPulse;//logarithmic
	double	PW_Pulse;
    //float ShowSlc;
	//float	efficientPW;
	float	amplitudeDetection;
	float	amplitudePulse;
	float	amplitudeSlc;
	double	ampRootMean;
	double	ampSkewness;
	double	BW;
	double	peakFreq;
	double	meanFreq;
	//double minFreq;
	//double maxFreq;
	quint8	CWFlag;
	quint16 tunerFreq;
	double	PRF;
	QVector2D keyValue;

	double	noiseLevel;

	double freqVariance;
	double ampMaximum;
	quint8 jammerOn;
	quint8 mod_Amp;
	quint8 mod_Freq;
	double SNR_Pulse_12;
	/*bool operator<(const interPulseSignalInfo &other)
	{
		/**
		 * @brief Executes the return procedure for Status.
		 * @param other.TOA Input parameter specifying other.TOA (TOA <).
		 */
		return (TOA < other.TOA);
	}*/

}interPulseSignalInfo;
Q_DECLARE_METATYPE(interPulseSignalInfo);
typedef enum
{
	Seraj14_Connected = 0,
	Seraj14_WaitForConnecting = 1,
	Seraj14_Disconnected = 2,
}CONNECTION_STATUS;
/**
 * @brief Executes the q  d e c l a r e  m e t a t y p e procedure for Status.
 * @param CONNECTION_STATUS Input parameter specifying CONNECTION_STATUS (CONNECTION_STATUS).
 */
Q_DECLARE_METATYPE(CONNECTION_STATUS);
/**
 * @brief Executes the q  e n u m s procedure for Status.
 * @param CONNECTION_STATUS Input parameter specifying CONNECTION_STATUS (CONNECTION_STATUS).
 */
Q_ENUMS(CONNECTION_STATUS);
typedef enum
{
	PRF_NONE = 0,
	PRF_SIMPLE = 1,
	PRF_STAGGERED = 2,
	PRF_DWELL_AND_SWITCH = 3,
	PRF_JITTERED = 4
	
}PRF_TYPE;
/**
 * @brief Executes the q  d e c l a r e  m e t a t y p e procedure for Status.
 * @param PRF_TYPE Input parameter specifying PRF_TYPE (PRF_TYPE).
 */
Q_DECLARE_METATYPE(PRF_TYPE);
typedef enum
{
	FREQUENCY_NONE = 0,
	FREQUENCY_SIMPLE = 1,
	FREQUENCY_HOPPING = 2,
	FREQUENCY_AGILITY = 3,
	FREQUENCY_DIVERSITY = 4

}FREQUENCY_TYPE;
/**
 * @brief Executes the q  d e c l a r e  m e t a t y p e procedure for Status.
 * @param FREQUENCY_TYPE Input parameter specifying FREQUENCY_TYPE (FREQUENCY_TYPE).
 */
Q_DECLARE_METATYPE(FREQUENCY_TYPE);
typedef enum
{
	FORCE_NONE = 0,
	FORCE_NAVY = 1,
	FORCE_AIRBORN = 2
}FORCE_TYPE;
/**
 * @brief Executes the q  d e c l a r e  m e t a t y p e procedure for Status.
 * @param FORCE_TYPE Input parameter specifying FORCE_TYPE (FORCE_TYPE).
 */
Q_DECLARE_METATYPE(FORCE_TYPE);
typedef enum
{
	MODULATION_NONE = 0,
	MODULATION_PSK = 1,
	MODULATION_FSK = 2,
	MODULATION_SIMPLE = 3,
	MODULATION_LFM = 4
}MODULATION_TYPE;
/**
 * @brief Executes the q  d e c l a r e  m e t a t y p e procedure for Status.
 * @param MODULATION_TYPE Input parameter specifying MODULATION_TYPE (MODULATION_TYPE).
 */
Q_DECLARE_METATYPE(MODULATION_TYPE);
typedef enum
{
	SCAN_NONE = 0,
	SCAN_SECTOR = 1,
	SCAN_CIRCULAR = 2,
	SCAN_HELICAL = 3,
	SCAN_SPIRAL = 4,
	SCAN_RASTER = 5
}SCAN_TYPE;
/**
 * @brief Executes the q  d e c l a r e  m e t a t y p e procedure for Status.
 * @param SCAN_TYPE Input parameter specifying SCAN_TYPE (SCAN_TYPE).
 */
Q_DECLARE_METATYPE(SCAN_TYPE);
/**
 * @class EdwType
 * @brief High-level controller class for EdwType.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class EdwType
{
public:
	quint64							ID;
	QString							Name;
	FORCE_TYPE						Force_Type;
	SCAN_TYPE						Scan_Type;
	double							Scan_Time;
	double							Amplitude;
	FREQUENCY_TYPE					Frequency_Type;
//	arma::vec						Frequency;
//	arma::vec						PW;
	PRF_TYPE						PRF_Type;
//	arma::vec						PRF;
//	arma::vec						PRF_Parameters;
	double							AOA;
	MODULATION_TYPE					Modulation_Type;
//	arma::vec						Modulation_Parameters;
	double							BW;
	double							GPSLat;
	double							GPSLong;
	QDateTime						FirstSeen;
	QDateTime						LastSeen;
	QVector<interPulseSignalInfo>	Pulses;
	bool							Updated;
	bool							Manual;
	quint8							mod_Amp;
	quint8							mod_Freq;
	double							Amp_root_mean;
	double							Freq_std;

	/**
	 * @brief Executes the to json procedure for EdwType.
	 * @return Output value of type QJsonObject.
	 */
	QJsonObject toJson();
	/**
	 * @brief Executes the from json procedure for EdwType.
	 * @param QJsonObject Input parameter specifying QJsonObject (QJsonObject).
	 * @return Output value of type bool.
	 */
	bool fromJson(QJsonObject);
};
/**
 * @brief Executes the q  d e c l a r e  m e t a t y p e procedure for EdwType.
 * @param EdwType Input parameter specifying EdwType (EdwType).
 */
Q_DECLARE_METATYPE(EdwType);

typedef struct
{
	//pulse filter parameters
	double	pw1;
	double	SNR_Thr_Pulse_filter1;
	double	SNR_Thr_Pulse_filter2;

	// modulation recognition
	double SNR_THR1;
	double	SNR_THR2;
	double PW_with_out_Mod;
	double	freq_var_thr_1;
	double	amp_root_normalized_thr_1;
	double	freq_var_thr_2;
	double amp_root_normalized_thr_2;
	double freq_var_thr_3;
	double amp_root_normalized_thr_3;

	double	PW_normalized_factor;
	double	BW_normalized_factor;
	double  AMP_normalized_factor;
	double  Pulse_Amp_root_mean_factor;
	double  Pulse_Freq_std_factor;
	double  Pulse_Freq_err;

	int		minpulse_to_cluster;
	double	minRatio;
	double	PW_Error;
	double	BW_Error;
	double	PW_Offset;
	double	BW_Offset;

	double  alpha_Amp_Mod;
	double	alpha_freq_Mod;

	double	PRI_max;
	double	PRI_min;
	int		n_hist_bin;
	double	gamma;
	

	double  alpha_is_on_cluster;
	double  beta_is_on_cluster;
	double  SNR_under_0_3;
	double  SNR_above_0_3;
	double  dist_thr_normal;
	double  dist_thr_above_0_3;
	double  dist_thr_under_0_3;
	double  alpha;

	quint8  distanceMode;
	

	qint32 timeoutUpdateCluster;
	qint32 timeoutRemoveCluster;
	
	bool	slcEnable;
	double	slcThreshold;

	double freq_dist_thr;
	double Amp_root_mean_error;
	double Freq_std_error;


	bool selected_pw;
	bool selected_bw;
	bool selected_amp;
	bool selected_freq;
	bool selected_amp_root_mean;
	bool selected_freq_std;

	

}ClusteringAlgorithmParameters;
/**
 * @brief Executes the q  d e c l a r e  m e t a t y p e procedure for EdwType.
 * @param ClusteringAlgorithmParameters Input parameter specifying ClusteringAlgorithmParameters (ClusteringAlgorithmParameters).
 */
Q_DECLARE_METATYPE(ClusteringAlgorithmParameters);

//Q_DECLARE_METATYPE(arma::vec);
typedef struct
{
	QString     type;
	quint16     sequenceNumber;
	QJsonObject	parameters;
	quint16      command;
	quint8      packetNumber;
}PacketMessageType;
/**
 * @brief Executes the q  d e c l a r e  m e t a t y p e procedure for EdwType.
 * @param PacketMessageType Input parameter specifying PacketMessageType (PacketMessageType).
 */
Q_DECLARE_METATYPE(PacketMessageType);


/**
 * @class TunerSetting
 * @brief High-level controller class for TunerSetting.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class TunerSetting
{
public:
	qint8					m_tunerMode;
	quint32					m_tunerFrequency;
	quint32					m_startTunerFrequency;
	quint32					m_stopTunerFrequency;
	quint32					m_stepTunerFrequency;
	quint16					m_tunerRFAtt;
	quint16					m_tunerIFAtt;
	quint32					m_lockTime;
	quint32					m_processTime;
	quint16				    m_tunerFilter;
	/**
	 * @brief Executes the to json procedure for TunerSetting.
	 * @return Output value of type QJsonObject.
	 */
	QJsonObject toJson();
};
/**
 * @brief Executes the q  d e c l a r e  m e t a t y p e procedure for TunerSetting.
 * @param TunerSetting Input parameter specifying TunerSetting (TunerSetting).
 */
Q_DECLARE_METATYPE(TunerSetting);
typedef struct {
	bool				containsPulseData;
	QVector<QVector2D>	totalPulseAmp;
	QVector<QVector2D>	amplitude;
	QVector<QVector2D>	centerFreq;
	QVector<QVector2D>	pulseWidth;
	QVector<QVector2D>	PRF;

	bool				containsCWdata;
	QVector<QVector2D>	cw_totalPulseAmp;
	QVector<QVector2D>	cw_amplitude;
	QVector<QVector2D>	cw_centerFreq;
	QVector<QVector2D>	cw_pulseWidth;
	QVector<QVector2D>	cw_PRF;

	QPair<QVector<qreal>, QVector<qreal> > ampHistData;
	QPair<QVector<qreal>, QVector<qreal> > freqHistData;
	QPair<QVector<qreal>, QVector<qreal> > pulseWidthHistData;
	QPair<QVector<qreal>, QVector<qreal> > PRFHistData;

	QPair<double, double> ampMinMaxForHist;
	QPair<double, double> freqMinMaxForHist;
	QPair<double, double> pwMinMaxForHist;
	QPair<double, double> prfMinMaxForHist;
}interPulsePlotData;


typedef struct 
{
	int operationMode;		// From the ComboBox
	int bufferSize;
	int bufferProgressStep;
}interPulseOperationModeData;

typedef struct
{
	double minAmpValue;
	double maxAmpValue;
	double minFreqValue;
	double maxFreqValue;
	double minPulseWidthValue;
	double maxPulseWidthValue;
	bool   inversionStatus;
}FilterConfigValues;


typedef enum
{
	OnlyPulseMode = 0x00,
	OnlyCwMode = 0x01,
	PulseAndCwMode = 0x02
}selectedOnlineData;

typedef struct
{
	int coreNumber;
	int counterCore;
	double offset;
}coreCWoffsetCounter;

typedef struct
{
	QVector<qreal> AmpHist;
	QVector<qreal> FreqHist;
	QVector<qreal> PulseWidthHist;
	QVector<qreal> PRFHist;
}histRawData;

typedef struct
{
	QVector<qreal> pulseDataIndex;
	QVector<qreal> cwDataIndex;
	qreal lowerRange;
	qreal upperRange;
}zoomedIndexAndRange;


/*************************************/
/*--- Intra Pulse Related Structs ---*/
/*************************************/

typedef struct
{
//	arma::colvec ampData;
//	arma::colvec InsFreq;
//	arma::colvec InsPhase;
//	arma::colvec spectrum;
//	arma::colvec consellation;
//	arma::mat stft;
}intraPulsePlotData;

/**
 * @class InputSpectrumData
 * @brief High-level controller class for InputSpectrumData.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class InputSpectrumData
{
public:
	enum TYPESPECTRUM
	{
		FFT128 = 0,
		FFT1024 = 1

	}typeSpectrum;
	QVector<QVector2D>	meanSpectrum;
	QVector<QVector2D>	maxSpectrum;
	QVector<QVector2D>	estimatedNoise;
	quint32			samplingFrequency;
	quint16			filterSelect;
	float			ddsPhase;
	quint32			tunerFreq;
	quint32			tunerStartFreq;
	quint32			tunerStepFreq;
	quint32			tunerStopFreq;
	bool			isPanorama;
	bool			isTunerConnected;
	/**
	 * @brief Executes the to json procedure for InputSpectrumData.
	 * @return Output value of type QJsonObject.
	 */
	QJsonObject toJson();
	/**
	 * @brief Executes the from json procedure for InputSpectrumData.
	 * @param QJsonObject Input parameter specifying QJsonObject (QJsonObject).
	 */
	void fromJson(QJsonObject);
};

typedef struct
{
	qreal centerFrequency;
	qreal bandWidth;
	QString signalType;
}DetectedSignal;

/*****************************************/
/*-----	  Jamming Related Structs   -----*/
/*****************************************/
//
//typedef struct
//{
//	double frequency;
//	double amplitude;
//	double pulsewidth;
//	double bandwidth;
//	double pri;
//}PDWParameters;

//typedef struct
//{
//	QString clusterName;
//	double centerFrequency;
//	QVector<interPulseSignalInfo> pdwList;
//	double frequencyError;
//	double amplitudeError;
//	double pulsewidthError;
//	double bandwidthError;
//	double priError;
//}ClusterData;

typedef struct
{
	bool   autoFrequency;
	double frequency;
}CwMode;

typedef struct
{
	bool   autoFrequency;
	double frequency;

	bool   autoPw;
	double pw;

	bool   autoPrf;
	double prf;
}ImpulseMode;

typedef struct
{
	bool   autoFrequency;
	double frequency;

	bool   autoBw;
	double bw;
}SpotMode;

typedef struct
{
	bool   autoFrequency;
	double frequency;

	bool   autoBw;
	double bw;

	double rate;
	quint8 direction;
}LfmMode;

typedef struct
{
	double startFrequency;
	double stepFrequency;
	double stopFrequency;
	double stepTime;
	quint8 sweepType;
}SweepCwMode;

typedef struct
{
	double startFrequency;
	double stepFrequency;
	double stopFrequency;
	double stepTime;
	double pw;
	double prf;
	quint8 sweepType;
}SweepImpulseMode;

typedef struct
{
	double startFrequency;
	double stepFrequency;
	double stopFrequency;
	double stepTime;
	double bw;
	quint8 sweepType;
	
}SweepSpotMode;

typedef struct
{
	double startFrequency;
	double stepFrequency;
	double stopFrequency;
	double stepTime;
	double bw;
	quint8   sweepType;
	double rate;
	quint8 direction;
}SweepLfmMode;

/**
 * @class NoiseGeneratorParameters
 * @brief High-level controller class for NoiseGeneratorParameters.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class NoiseGeneratorParameters
{
public:
	enum { CWNJMode, IMNJMode, SNJMode, LFMNJMode, SCWNJMode, SIMNJMode, SSNJMode, SLFMNJMode, CWNJAutoMode, IMNJAutoMode, SNJAutoMode, LFMNJAutoMode } jammingMode;
	EdwType edw;
	double upConverterFreq;
	union
	{
		CwMode              cwnj;
		ImpulseMode         imnj;
		SpotMode            snj;
		LfmMode             lfmnj;
		SweepCwMode         scwnj;
		SweepImpulseMode    simnj;
		SweepSpotMode       ssnj;
		SweepLfmMode        slfmnj;
	};
	/**
	 * @brief Executes the to json procedure for NoiseGeneratorParameters.
	 * @return Output value of type QJsonObject.
	 */
	QJsonObject toJson();
	/**
	 * @brief Executes the from json procedure for NoiseGeneratorParameters.
	 * @param QJsonObject Input parameter specifying QJsonObject (QJsonObject).
	 */
	void fromJson(QJsonObject);
};

typedef struct {
	double			upConvertorCenterFreq;
	double			centerFreq;
	double			processingTime;
	quint8			processingTimeMode;
	QVector<double> jammingTime;
	quint8			jammingTimeMode;
	quint8			ifAtt;
    int 			rfPower;
	quint8			rfAtt;
	bool			calibrationEnable;
	quint8			noiseSelector;
	quint16			gaussianNoiseSaturationValue;
}JammingConversionParameters;

/*****************************************/
/*----- Table Model Related Structs -----*/
/*****************************************/

typedef struct
{
	int pulseNumber;
	int coreNumber;
	double TOA;
	double pulseWidth;
	double amplitude;
	double frequency;
	QString modulation;
	QString fileName;
}intraPulseSignalInfo;

typedef struct
{
	double Amplitude;
	double PulseWidth;
	double TOA;
	double Freq;
}dataBaseDataType;

typedef struct
{
	QString signalType;
	double centerFreq;
	double bandWidth;
	double amplitude;
	double startTime;
	double endTime;
}detectedSignalInfoType;

/**
 * @class maskingInfoType
 * @brief High-level controller class for maskingInfoType.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class maskingInfoType
{
public:
	bool selected;
	bool outerRange;
	qreal centerFrequency;
	qreal BW;
	/**
	 * @brief Executes the to json procedure for maskingInfoType.
	 * @return Output value of type QJsonObject.
	 */
	QJsonObject toJson();
	/**
	 * @brief Executes the from json procedure for maskingInfoType.
	 * @param QJsonObject Input parameter specifying QJsonObject (QJsonObject).
	 * @return Output value of type bool.
	 */
	bool fromJson(QJsonObject);
};

/*************************************/
/*---- QGLchart Related typdefs -----*/
/*************************************/

typedef struct
{
	QBrush			Brush;
	QPen			Pen;
	bool			Visibility;
	bool			Roundness;
	bool			AutoFill;
	bool			DropShadowEnable;
}backgroundProperties;

typedef struct
{
	QRectF			Area;
	QBrush			Brush;
	QPen			Pen;
	bool			Visibility;

}plotAreaProperties;


#define SYNC1	0xAA
#define SYNC2	0xBB

enum State_Type {
	WAITING_SYNC1, WAITING_SYNC2, WAITING_SYNC3, WAITING_SYNC4, WAITING_COUNTER1,
	WAITING_COUNTER2, WAITING_BYTE_COUNT1, WAITING_BYTE_COUNT2, WAITING_NOT_BYTE_COUNT1, WAITING_NOT_BYTE_COUNT2,
	WAITING_SUB_TYPE1, WAITING_SUB_TYPE2, WAITING_SEQUENCE1, WAITING_SEQUENCE2, WAITING_FLAG1, WAITING_FLAG2, WAITING_DATA,
	WAITING_CRC1, WAITING_CRC2
};
