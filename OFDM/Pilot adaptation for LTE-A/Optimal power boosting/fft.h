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
 *
 * Author: Ling-San Meng
 * Email: f95942117@ntu.edu.tw
 *
 * Date: 2009/9/11
 */

#ifndef FFT_H
#define FFT_H

#include <complex>

using namespace std;

void ifft(complex<double> *x,complex<double> *r, int npoints, int CPLen);

void fft(complex<double> *x,complex<double> *r, int npoints, int CPLen);

#endif
