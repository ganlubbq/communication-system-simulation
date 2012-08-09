#ifndef RECEIVER_H
#define RECEIVER_H

#include <complex>
using namespace std;

#include "LAB524Rand.h"

// Return the decoded data bits

int *rx(int *u, complex<float> **receivedSignalPoints, float N0, int modulationFormat, int bitsPerSymbol, LAB524Rand* rng);

#endif
