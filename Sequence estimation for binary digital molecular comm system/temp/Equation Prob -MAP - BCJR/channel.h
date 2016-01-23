#ifndef CHANNEL_H
#define CHANNEL_H

#include "LAB524Rand.h"

long double *AWGN(long double *signalPoints, long double N0, LAB524Rand* rng);

// Return the array of fading amplitudes
long double *Rayleigh(long double *signalPoints, long double N0, long double meanPower = 1.0);

void Rician();

void Nakagami();

void dispersive();

#endif
