/* Rate 1/3 turbo encoder and decoder.
 *
 * Random interleaver.
 * 
 * The coded bits are arranged as
 * systematic part | parity 1 | parity 2
 * 
 * Date: 2007/12/25
 *
 * Author: Ling-San Meng
 * Email : f95942117@ntu.edu.tw
 */

#ifndef TURBO_H
#define TURBO_H

// Should be clear.
const int turboIteration = 8;

// The number of registers (memory order) of each of the RSC.
const int m = 4;

// The generator sequence for the two RSCs.
const int G11 = 035;
const int G12 = 023;

// It dynamically allocate an integer array and put the encoded bits into it. 
// The pointer to the coded bits array is returned.
int *turboEncode(int *uncodedDataBits);

// It dynamically allocate an integer array and put the decoded bits into it. 
// The pointer to the decoded bits array is returned.
int *turboDecode(double *logLikelyHoodOfTheCodedBits, double N0);

#endif
