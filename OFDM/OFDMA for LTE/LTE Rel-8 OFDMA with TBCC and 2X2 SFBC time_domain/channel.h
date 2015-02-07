#ifndef CHANNEL_H
#define CHANNEL_H

#include <complex>
using namespace std;

double Sinc(const double &val);

void readFile(const char* filename, complex<double>** buffer);

complex<double> **FastFading(complex<double> *signalPoints,int numTxANT, int numRxANT);

complex<double> **Dummy(complex<double> *signalPoints, int numTxANT, int numRxANT);

#endif
