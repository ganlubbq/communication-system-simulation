/*
 * Rate 1/2 convolution encoder and decoder
 * 
 */

#ifndef CC_H
#define CC_H

void ccEncode(int *uncodedBits, int uncodedBitsLength, int *codedBits);

void viterbiDecode(float *softValuesToBeDecoded, int softValuesLength,
                   int *decodedBits);

#endif
