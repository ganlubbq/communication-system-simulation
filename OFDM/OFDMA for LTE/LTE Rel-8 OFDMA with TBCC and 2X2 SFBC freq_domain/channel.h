#ifndef CHANNEL_H
#define CHANNEL_H

#include <complex>
using namespace std;

double Sinc(const double &val);

void readFile(const char* filename, complex<double>** buffer);

complex<double> **FastFading_2(complex<double> *signalPoints, int numRxANT);

complex<double> **Dummy(complex<double> *signalPoints, int numRxANT);

#endif
