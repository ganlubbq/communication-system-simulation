/* Rate 1/3 turbo encoder and decoder
 * Use Random interleaver
   Support frame length up to 70000
 *
 * Date: 2007/10/23
 */


#ifndef TURBO_H
#define TURBO_H

int *turboEncode(int *uncodedBits);

int *bcjrDecode(float *softValuesToBeDecoded, float N0, int iteration);

#endif
