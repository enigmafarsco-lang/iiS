// ============================================================================
// File: detector.h
// Subsystem: eLynxSDR clustring
// Description: Subsystem implementation and interface guidance for detector.h.
// ============================================================================

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


class Detector:public QThread
{
    Q_OBJECT
public:
    // Constructor: Initializes Detector subsystem components, default parameters, and state.
    Detector();
    // Executes add to queue routine.
    void AddToQueue(QVector<double>);
    // Executes read block data x routine.
    void ReadBlockDataX(QVector<double> yItems);
    // Sets tuner frequency value and updates internal subsystem configuration.
    void setTunerFrequency(double newTunerFrequency);

    int cnt{};
    // Sets stop value and updates internal subsystem configuration.
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

    // Executes check pulse routine.
    bool CheckPulse(Pulse*,bool);

    // Executes clear unused freqs routine.
    void ClearUnusedFreqs();

signals:
    // Executes new list routine.
    void newList(QList<Pulse *>*);
    // Executes show list routine.
    void showList(QList<Pulse *>*);

    // QThread interface
protected:
    // Executes run routine.
    void run();

};

#endif // DETECTOR_H
