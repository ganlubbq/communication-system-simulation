#ifndef CHANNEL_H
#define CHANNEL_H

float *AWGN(float *signalPoints, float N0, float *noiseBuff);

// Return the array of fading amplitudes
float *Rayleigh(float *signalPoints, float N0, float meanPower = 1.0);

void Rician();

void Nakagami();

void dispersive();

#endif
