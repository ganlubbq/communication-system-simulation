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

#include <complex>
using namespace std;

class PskModulator
{
   public:
      // Eb is the energy per coded bit.
      PskModulator(int bitsPerSymbol);
      ~PskModulator();
      
      // For efficiency, BPSK has its own mod/demod function calls.
      complex<double> *bpskMod(double Es,int *bitSequenceToBeModulated);
      double *bpskDemod(complex<double> *modulatedSym, double N0);
      
      // Gray mapping.
      complex<double> *pskMod(double Es,int *bitSequenceToBeModulated);
      
      // Return the LLR of each data bit.
      double *pskDemod(complex<double> *modulatedSym, double N0);

      //--------------------------------------------------------
      
      inline complex<double> bpskMod(double Es, int b)
      {
         _sqrtEs = sqrt(Es);
         return 2.0*_sqrtEs*b - _sqrtEs;
      }
      
      inline double bpskDemod(complex<double> r, complex<double> chGain, double N0)
      { return 4*_sqrtEs/N0*(real(r)*real(chGain) + imag(r)*imag(chGain)); }
      
      // Gray mapping.
      complex<double> one_pskMod(double Es, int *v)
      {  
         _sqrtEs = sqrt(Es);
         
         int modIdx = 0;
         for (int j=_m-1;j>=0;j--)
         {
            modIdx |= v[_m-j-1]<<j;
         }
            
         return _sqrtEs*_modTable[modIdx];
      }
       
      // Return the LLR of each data bit.
      double *one_pskDemod(complex<double> modulatedSym, complex<double> chGain, double N0);
      
      //--------------------------------------------------------

      // Return the constellation size.
      inline double getModOrder(void)
      { return _M; }
            
   private:
      complex<double> *_modTable;
      int _M;
      int _m;
      double _sqrtEs;
};


// Support only the square-constellation QAM, i.e., bitsPerSymbol has to be a
// multiple of 2.
class QamModulator
{
   public:
      // Eb is the energy per coded bit.
      // bitsPerSymbol has to be a multiple of 2.
      QamModulator(int bitsPerSymbol);
      ~QamModulator();

      // Gray mapping.
      complex<double> *qamMod(double Es,int *bitSequenceToBeModulated);
      
      // Return the LLR of each data bit.
      double *qamDemod(complex<double> *modulatedSym, double N0);

      //--------------------------------------------------------
      
      // Gray mapping.
      complex<double> one_qamMod(double Es, int *v)
      {  
         _sqrtE0 = sqrt(3*Es/2/(_M-1));  
         
         int modIdx = 0;
         for (int j=_m-1;j>=0;j--)
         {
            modIdx |= v[_m-j-1]<<j;
         }
      
         return _sqrtE0*_modTable[modIdx];
      }

      // Return the LLR of each data bit.
      double *one_qamDemod(complex<double> modulatedSym, complex<double> chGain, double N0);
      
      //--------------------------------------------------------
          
      // Return the constellation size.
      inline double getModOrder(void)
      { return _M; }
      
   private:
      complex<double> *_modTable;
      int _M;
      int _m;
      double _sqrtE0;
};
#endif
