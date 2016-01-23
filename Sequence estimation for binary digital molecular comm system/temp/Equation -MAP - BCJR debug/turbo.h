/* Rate 1/3 turbo encoder and decoder
 * Use Random interleaver
 * Support frame length up to 70000
 *
 * Date: 2007/10/23
 * ----------------------------------
 * Rewrite turbo.cpp.
 * Massive test has not been performed yet, so there is a backup file
 * 
 * Date: 2007/11/28
 * ----------------------------------
 * Rearrange the coded bits as
 * systematic part | parity 1 | parity 2
 *
 * Date: 2007/12/25
 *
 * Author: Ling-San Meng
 * Email : f95942117@ntu.edu.tw
 */

#ifndef TURBO_H
#define TURBO_H

int *turboEncode(int *uncodedBits);

int *bcjrDecode(double *softValuesToBeDecoded, double N0);

#endif
