#ifndef RECEIVER_H
#define RECEIVER_H

#include <complex>
using namespace std;

// Return the decoded data bits

int *rx(complex<float> *receivedSignalPoints, float N0, int modulationFormat, int modulationOrder);

#endif
