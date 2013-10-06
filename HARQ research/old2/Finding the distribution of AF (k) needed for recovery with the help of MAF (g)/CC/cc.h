/*
 * Rate 1/2 convolution encoder and decoder
 * The 1st elements of arrays always store the array size!!
 */

#ifndef CC_H
#define CC_H

// Return the coded bits array.
int *ccEncode(int *uncodedBits);

// Return the decoded bits array.
int *viterbiDecode(float *softValuesToBeDecoded);

#endif
