#ifndef RECEIVER_H
#define RECEIVER_H

#include "LAB524Rand.h"

// Return the decoded data bits
int *rx(complex<double> **receivedSignalPoints, double N0, int OFDMSymIdx, 
        LAB524Rand *rng);

#endif
