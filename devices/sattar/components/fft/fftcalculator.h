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
    explicit fftCalculator(QObject *parent = nullptr);
    void FFT(complex<double>* f, int N, double d);

private:
    int log2(int N);
    int check(int n);
    int reverse(int N, int n);
    void ordina(complex<double>* f1, int N);
    void transform(complex<double>* f, int N);


signals:

};

#endif // FFTCALCULATOR_H
