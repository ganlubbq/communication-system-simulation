/* 
 * Except bpsk, the ML demodulation function (psk8Demod, qam16Demod) actually
 * is applicable to all modulation formats.
 * There should be a mother Modulator class and each sub
 * Modulator class inherits from it. Need to be revised someday.
 * 
 * Need to write a general modulation mapping table for PSK and QAM. The table
 * for 8PSK and 16QAM is inputted by hand.
 * 
 * Date: 2008/1/8
 * ----------------------------------------------------------------------------
 * Add M-QAM and M-PSK modulator and demodulator.
 * M-PSK not verified.
 * 256 QAM BICM performance do not match that in Yeh's paper ??
 * 
 * Date: 2008/6/23
 * ----------------------------------------------------------------------------
 * Verify the M-PSK and M-QAM BER performance.
 * For PSK, M = 2, 4, 8, 16, 64 have been verified.
 * For QAM, M = 4, 16, 64, 256 have been verified.
 *
 * Date: 2009/5/21 
 * ----------------------------------------------------------------------------
 * Fix bugs for bpskDemod().
 * Add #ifdef MAX_LOG.
 * It turns out that the LLR method used here is for transmission with codeword
 * interleaving. For uncoded transmissions, simple constellation distance
 * calculation should be used!
 * Re-verification is needed.
 * Add sigal_mod/demod methods.
 * Add private variable _sqrtEs.
 * Make getEs() and getModOrder inline.
 * 
 * Date: 2009/9/12
 */
 
#ifndef MODEM_H
#define MODEM_H

#include <iostream>
#include <complex>
using namespace std;

class PskModulator
{
   public:
      // Eb is the energy per coded bit.
      PskModulator();
      ~PskModulator();
      
      // For efficiency, BPSK has its own mod/demod function calls.
      inline complex<double> bpskMod(double Es, int b)
      {
         _sqrtEs = sqrt(Es);
         return 2.0*_sqrtEs*b - _sqrtEs;
      }
      
      inline double bpskDemod(complex<double> r, complex<double> chGain, double N0)
      { return 4*_sqrtEs/N0*(real(r)*real(chGain) + imag(r)*imag(chGain)); }
      
      // Gray mapping.
      complex<double> one_pskMod(int bitsPerSymbol, double Es, int *v)
      {  
         if (bitsPerSymbol > 4)
         { cout << "Modulation format not supported" << endl; getchar(); }
         
         _Es = Es;
         _sqrtEs = sqrt(Es);
         _m = bitsPerSymbol;
         
         int modIdx = 0;
         for (int j=_m-1;j>=0;j--) { modIdx |= v[_m-j-1]<<j; }

              if (_m == 1) { return _sqrtEs*_mapTableBPSK[modIdx]; }
         else if (_m == 2) { return _sqrtEs*_mapTableQPSK[modIdx]; }
         else if (_m == 3) { return _sqrtEs*_mapTable8PSK[modIdx]; }
         else if (_m == 4) { return _sqrtEs*_mapTable16PSK[modIdx]; }
      }
       
      // Return the LLR of each data bit.
      double *one_pskDemod(complex<double> modulatedSym, int bitsPerSymbol, 
                           double Es, complex<double> chGain, double N0);

      // Return the constellation size.
      inline int getModOrder(void)
      { return 1<<_m; }

      inline int getBitsPerSym(void)
      { return _m; }   
      
      inline double getEngPerSym(void)
      { return _Es; }
      
   private:
      complex<double> *_mapTableBPSK;
      complex<double> *_mapTableQPSK;
      complex<double> *_mapTable8PSK;
      complex<double> *_mapTable16PSK;

      // The last modulation setting will be memorized
      int _m;
      double _Es;
      double _sqrtEs;
      int _bitsPerSymbol;
};


// Support only the square-constellation QAM, i.e., bitsPerSymbol has to be a
// multiple of 2.
class QamModulator
{
   public:
      // Eb is the energy per coded bit.
      // bitsPerSymbol has to be a multiple of 2.
      QamModulator();
      ~QamModulator();

      // Gray mapping.
      complex<double> one_qamMod(int bitsPerSymbol, double Es, int *v)
      {  
         if (bitsPerSymbol > 8)
         { cout << "Modulation format not supported" << endl; getchar(); }
         
         _Es = Es;
         _m = bitsPerSymbol;
         _sqrtE0 = sqrt(3.0*Es/2.0/((1<<_m)-1));
         
         int modIdx = 0;
         for (int j=_m-1;j>=0;j--) { modIdx |= v[_m-j-1]<<j; }
      
              if (_m == 2) { return _sqrtE0*_mapTable4QAM[modIdx]; }
         else if (_m == 4) { return _sqrtE0*_mapTable16QAM[modIdx]; }
         else if (_m == 6) { return _sqrtE0*_mapTable64QAM[modIdx]; }
         else if (_m == 8) { return _sqrtE0*_mapTable256QAM[modIdx]; }
      }

      // Return the LLR of each data bit.
      double *one_qamDemod(complex<double> modulatedSym, int bitsPerSymbol, 
                           double Es, complex<double> chGain, double N0);
      
      // Return the constellation size.
      inline int getModOrder(void)
      { return 1<<_m; }
      
      inline int getBitsPerSym(void)
      { return _m; }   
      
      inline double getEngPerSym(void)
      { return _Es; }
      
   private:
      complex<double> *_mapTable4QAM;
      complex<double> *_mapTable16QAM;
      complex<double> *_mapTable64QAM;
      complex<double> *_mapTable256QAM;
      
      int _m;
      double _Es;
      double _sqrtE0;
};
#endif
