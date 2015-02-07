#ifndef CHANNEL_H
#define CHANNEL_H

#include "LAB524Rand.h"

complex<double> **Dummy(complex<double> *signalPoints, double N0, LAB524Rand* rng);

complex<double> **blockRayleigh(complex<double> *signalPoints, double N0,
                                int blockLenInNumOfSymbols, LAB524Rand* rng,
                                double meanPower = 1.0);

#endif
