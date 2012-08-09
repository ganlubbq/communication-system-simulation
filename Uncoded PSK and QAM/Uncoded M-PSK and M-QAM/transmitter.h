#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include "LAB524Rand.h"

// Return random data bits
int *generateDataBits(int FrameLength, LAB524Rand* rng);

// Return modulated symbols
complex<float> *tx(int *originalBitSequence, float energyPerBit, int modulationFormat, int modulationOrder);

#endif
