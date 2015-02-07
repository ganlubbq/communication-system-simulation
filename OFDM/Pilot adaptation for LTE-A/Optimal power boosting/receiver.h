#ifndef RECEIVER_H
#define RECEIVER_H

#include <complex>
using namespace std;

// Return the decoded data bits

int *rx(complex<double> *receivedSignalPoints, double N0, int modulationFormat,
        int bitsPerSymbol, int turboIteration);

#endif
