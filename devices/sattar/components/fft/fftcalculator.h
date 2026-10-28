// ============================================================================
// File: fftcalculator.h
// Subsystem: eLynxSDR fft
// Description: Subsystem implementation and interface guidance for fftcalculator.h.
// ============================================================================

#ifndef FFTCALCULATOR_H
#define FFTCALCULATOR_H

#include <QObject>
#include <iostream>
#include <complex>
#define MAX 200

using namespace std;

#define M__PI 3.1415926535897932384

class fftCalculator : public QObject
{
    Q_OBJECT
public:
    // Constructor: Initializes fftCalculator subsystem components, default parameters, and state.
    explicit fftCalculator(QObject *parent = nullptr);
    // Executes f f t routine.
    void FFT(complex<double>* f, int N, double d);

private:
    // Executes log2 routine.
    int log2(int N);
    // Executes check routine.
    int check(int n);
    // Executes reverse routine.
    int reverse(int N, int n);
    // Executes ordina routine.
    void ordina(complex<double>* f1, int N);
    // Executes transform routine.
    void transform(complex<double>* f, int N);


signals:

};

#endif // FFTCALCULATOR_H
