#ifndef CHANNEL_H
#define CHANNEL_H

#include "LAB524Rand.h"

double *AWGN(double *signalPoints, double N0, LAB524Rand* rng);

// Return the array of fading amplitudes
double *Rayleigh(double *signalPoints, double N0, LAB524Rand* rng, double meanPower = 1.0);

void Rician();

void Nakagami();

void dispersive();

#endif
