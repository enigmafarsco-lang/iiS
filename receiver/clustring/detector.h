/**
 * @file detector.h
 * @brief Subsystem module for clustring in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef DETECTOR_H
#define DETECTOR_H

#include "receiver/clustring/dataextractor.h"
#include "pulse.h"
#include <QObject>
#include <QThread>
#include <QTimer>
#include <QVector>
#include <math.h>
#include "qdebug.h"
#include "qmutex.h"
#include "iostream"
#include <thread>
#include "QDebug"


/**
 * @class Detector
 * @brief High-level controller class for Detector.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class Detector:public QThread
{
    Q_OBJECT
public:
    /**
     * @brief Constructs a new Detector instance and initializes subsystem components and UI state.
     */
    Detector();
    /**
     * @brief Executes the add to queue procedure for Detector.
     * @param QVector<double> Input parameter specifying QVector<double> (QVector<double>).
     */
    void AddToQueue(QVector<double>);
    /**
     * @brief Executes the read block data x procedure for Detector.
     * @param yItems Input parameter specifying yItems (QVector<double>).
     */
    void ReadBlockDataX(QVector<double> yItems);
    /**
     * @brief Sets the tuner frequency parameter and updates internal state.
     * @param newTunerFrequency Input parameter specifying newTunerFrequency (double).
     */
    void setTunerFrequency(double newTunerFrequency);

    int cnt{};
    /**
     * @brief Sets the stop parameter and updates internal state.
     * @param newStop Input parameter specifying newStop (bool).
     */
    void setStop(bool newStop);

private:
    DataExtractor  *dataExtractor;
    QList<QVector<double>> *yItemsQueue;
    QList<Pulse*> *pulseList;
    QList<Pulse*> *plsLst;
    int cnt1{};
    //saeid raziani ========
    bool duplicate = false;
    std::vector<std::vector<Pulse>> pulseVector;
     std::vector<Pulse> h;
    //sr05
    void calculatingPRI(Pulse);
    //===============
    QList<Pulse*> *temppulseList;

    long double test{};

    QMap<double,double> *FreqToaMap;

    bool Processing=false;

    int clearCounter=0;

    double ft=0;
    QVector<long int> *pow2X;
    double fsX=1, FIRFilterGainX=1, CICGainMismatchX=1;
    int NumWordsX=1, acquisitiondecrateX=1;
    double startFrequency;

    double ev;
    double ev2;
    double PASum = 0, PAPeak = 0,PW1 = 0, PW2 = 0, SelPW = 0, pre_TOA0 = 0, PRI_cal = 0, PRI_cal0 = 0;

    double fs=122.88*2;
    const float alpha=31.5;
    double fr=0;
    bool stop=false;

    QMutex mutex;

    double pulseIndicator=-24576;
    double cwIndicator=-24319;

    /**
     * @brief Executes the check pulse procedure for Detector.
     * @param Pulse* Input parameter specifying Pulse* (Pulse*).
     * @param bool Input parameter specifying bool (bool).
     * @return Output value of type bool.
     */
    bool CheckPulse(Pulse*,bool);

    /**
     * @brief Executes the clear unused freqs procedure for Detector.
     */
    void ClearUnusedFreqs();

signals:
    /**
     * @brief Executes the new list procedure for Detector.
     * @param >* Input parameter specifying >* (QList<Pulse).
     */
    void newList(QList<Pulse *>*);
    /**
     * @brief Executes the show list procedure for Detector.
     * @param >* Input parameter specifying >* (QList<Pulse).
     */
    void showList(QList<Pulse *>*);

    // QThread interface
protected:
    void run();

};

#endif // DETECTOR_H
