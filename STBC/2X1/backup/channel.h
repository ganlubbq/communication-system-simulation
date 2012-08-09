#ifndef CHANNEL_H
#define CHANNEL_H

#include "LAB524Rand.h"

complex<float> *AWGN(complex<float> *signalPoints, float N0, LAB524Rand* rng);

complex<float> *blockRayleigh(complex<float> *signalPoints, float N0, int blockLenInNumOfSymbols, LAB524Rand* rng, float meanPower = 1.0);

void Rician();

void Nakagami();

void dispersive();

#endif
