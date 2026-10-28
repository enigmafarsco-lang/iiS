/**
 * @file fftcalculator.h
 * @brief Subsystem module for fft in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef FFTCALCULATOR_H
#define FFTCALCULATOR_H

#include <QObject>
#include <iostream>
#include <complex>
#define MAX 200

using namespace std;

#define M__PI 3.1415926535897932384

/**
 * @class fftCalculator
 * @brief High-level controller class for fftCalculator.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class fftCalculator : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Constructs a new fftCalculator instance and initializes subsystem components and UI state.
     * @param parent Input parameter specifying parent (QObject).
     */
    explicit fftCalculator(QObject *parent = nullptr);
    /**
     * @brief Executes the f f t procedure for fftCalculator.
     * @param f Input parameter specifying f (complex<double>*).
     * @param N Input parameter specifying N (int).
     * @param d Input parameter specifying d (double).
     */
    void FFT(complex<double>* f, int N, double d);

private:
    /**
     * @brief Executes the log2 procedure for fftCalculator.
     * @param N Input parameter specifying N (int).
     * @return Output value of type int.
     */
    int log2(int N);
    /**
     * @brief Executes the check procedure for fftCalculator.
     * @param n Input parameter specifying n (int).
     * @return Output value of type int.
     */
    int check(int n);
    /**
     * @brief Executes the reverse procedure for fftCalculator.
     * @param N Input parameter specifying N (int).
     * @param n Input parameter specifying n (int).
     * @return Output value of type int.
     */
    int reverse(int N, int n);
    /**
     * @brief Executes the ordina procedure for fftCalculator.
     * @param f1 Input parameter specifying f1 (complex<double>*).
     * @param N Input parameter specifying N (int).
     */
    void ordina(complex<double>* f1, int N);
    /**
     * @brief Executes the transform procedure for fftCalculator.
     * @param f Input parameter specifying f (complex<double>*).
     * @param N Input parameter specifying N (int).
     */
    void transform(complex<double>* f, int N);


signals:

};

#endif // FFTCALCULATOR_H
