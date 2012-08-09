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
      complex<float> *bpskMod(float Es,int *bitSequenceToBeModulated);
      float *bpskDemod(complex<float> *modulatedSym, float N0);
      
      // Gray mapping.
      complex<float> *pskMod(float Es,int *bitSequenceToBeModulated);
      
      // Return the LLR of each data bit.
      float *pskDemod(complex<float> *modulatedSym, float N0);

      //--------------------------------------------------------
      
      inline complex<float> bpskMod(float Es, int b)
      {
         _sqrtEs = sqrt(Es);
         return 2.0*_sqrtEs*b - _sqrtEs;
      }
      
      inline float bpskDemod(complex<float> r, complex<float> chGain, float N0)
      { return 4*_sqrtEs/N0*(real(r)*real(chGain) + imag(r)*imag(chGain)); }
      
      // Gray mapping.
      complex<float> one_pskMod(float Es, int *v)
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
      float *one_pskDemod(complex<float> modulatedSym, complex<float> chGain, float N0);
      
      //--------------------------------------------------------

      // Return the constellation size.
      inline float getModOrder(void)
      { return _M; }
            
   private:
      complex<float> *_modTable;
      int _M;
      int _m;
      float _sqrtEs;
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
      complex<float> *qamMod(float Es,int *bitSequenceToBeModulated);
      
      // Return the LLR of each data bit.
      float *qamDemod(complex<float> *modulatedSym, float N0);

      //--------------------------------------------------------
      
      // Gray mapping.
      complex<float> one_qamMod(float Es, int *v)
      {  
         _sqrtE0 = sqrt(3.0*Es/2.0/(_M-1));  
         
         int modIdx = 0;
         for (int j=_m-1;j>=0;j--)
         {
            modIdx |= v[_m-j-1]<<j;
         }
      
         return _sqrtE0*_modTable[modIdx];
      }

      // Return the LLR of each data bit.
      float *one_qamDemod(complex<float> modulatedSym, complex<float> chGain, float N0);
      
      //--------------------------------------------------------
          
      // Return the constellation size.
      inline float getModOrder(void)
      { return _M; }
      
   private:
      complex<float> *_modTable;
      int _M;
      int _m;
      float _sqrtE0;
};
#endif
