#ifndef CHANNEL_H
#define CHANNEL_H

void AWGN(float *signalPoints, int length, float SNR, float *noisyOutput);

void Rayleigh(float *signalPoints, int length, float SNR, float *noisyOutput,
              float *fadingAmplitudes, float meanPower = 1.0);

void Rician();

void Nakagami();

void dispersive();

#endif
