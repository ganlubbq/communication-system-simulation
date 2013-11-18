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
 */
 
#ifndef MODEM_H
#define MODEM_H

#include <complex>
using namespace std;

class PskModulator
{
   public:
      // Eb is the energy per coded bit.
      PskModulator(float Eb, int bitsPerSymbol);
      ~PskModulator();
      
      // For efficiency, BPSK has its own mod/demod function calls.
      complex<float> *bpskMod(int *bitSequenceToBeModulated);
      float *bpskDemod(complex<float> *modulatedSym, float N0);
      
      // Gray mapping.
      complex<float> *pskMod(int *bitSequenceToBeModulated);
      
      // Return the LLR of each data bit.
      float *pskDemod(complex<float> *modulatedSym, float N0);

      float getEs(void);

      // Return the constellation size.
      float getModOrder(void);
            
   private:
      complex<float> *_modTable;
      int _M;
      int _m;
      float _Es;
};

/* 
 * Support only the square-constellation QAM; that is, bitsPerSymbol has to be a
 * multiple of 2.
 */
class QamModulator
{
   public:
      // Eb is the energy per coded bit.
      // bitsPerSymbol has to be a multiple of 2.
      QamModulator(float Eb, int bitsPerSymbol);
      ~QamModulator();

      // Gray mapping.
      complex<float> *qamMod(int *bitSequenceToBeModulated);
      
      // Return the LLR of each data bit.
      float *qamDemod(complex<float> *modulatedSym, float N0);
      
      float getEs(void);
      
      // Return the constellation size.
      float getModOrder(void);
      
   private:
      complex<float> *_modTable;
      int _M;
      int _m;
      float _Es;
};
#endif
