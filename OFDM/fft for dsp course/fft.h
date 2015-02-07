/* FFT and IFFT functions
 * w[] is the trigonometric coefficient table, and should be global
 * order[] is the fft input order generator
 * function arguments are orderd as (input, output, Nfft size)
 *
 * PS : some decalaration ,defines, and fft_coef should be placed 
 *      to the decalaration header file for consistency
 *
 * Date: ???
 * ------------------------------------------
 * Correct a bug; the calculation of "stage" is incorrect.
 * 
 * Date: 2010/1/2 
 *
 * Author: Ling-San Meng
 * Email: f95942117@ntu.edu.tw
 */

#ifndef FFT_H
#define FFT_H

const double pi = 3.141592653589793;

#include <complex>

using namespace std;

void fft(complex<double> *x, complex<double> *y, int N,int M, int KIND);

#endif
