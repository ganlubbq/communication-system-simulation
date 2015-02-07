#ifndef CHANNEL_H
#define CHANNEL_H

#include "LAB524Rand.h"

complex<double> *AWGN(complex<double> *signalPoints, double N0, LAB524Rand* rng);

complex<double> *Rayleigh(complex<double> *signalPoints, double N0, LAB524Rand* rng, double meanPower = 1.0);

// iPac is passed from main() to determine whether the signalPoints is "new" (not a repeated one for pilotSpacing search)
// nPacError is passed from main() to determine whether the previous pk fails or not
complex<double> *multipathCh(complex<double> *signalPoints, double N0, LAB524Rand* rng, int iPac, unsigned int nPacError);

void Rician();

void Nakagami();

void dispersive();

#endif
