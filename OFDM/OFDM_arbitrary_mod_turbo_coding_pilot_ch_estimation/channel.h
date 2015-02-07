#ifndef CHANNEL_H
#define CHANNEL_H

#include "LAB524Rand.h"

complex<double> *AWGN(complex<double> *signalPoints, double N0, LAB524Rand* rng);

complex<double> *Rayleigh(complex<double> *signalPoints, double N0, LAB524Rand* rng, double meanPower = 1.0);

complex<double> *multipathCh(complex<double> *signalPoints, double N0, LAB524Rand* rng);

void Rician();

void Nakagami();

void dispersive();

#endif
