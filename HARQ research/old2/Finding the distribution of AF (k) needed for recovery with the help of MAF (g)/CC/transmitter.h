#ifndef TRANSMITTER_H
#define TRANSMITTER_H

// Return random data bits
int *generateDataBits(int FrameLength);

// Return modulated symbols
float *tx(int *originalBitSequence, float energyPerBitOfTheModulatedSymbols);

#endif
