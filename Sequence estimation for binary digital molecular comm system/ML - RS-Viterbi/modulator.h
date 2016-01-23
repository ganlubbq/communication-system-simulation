/*
 *
 * Date : 2008/1/8 
 */
 
#ifndef MADULATOR_H
#define MADULATOR_H

class Modulator
{
   public:
      Modulator(float Eb, int bitsPerSymbol);
      
      float *bpskMod(int *bitSequenceToBeModulated);
      
      // Output the LLR of original data bits
      float *bpskDemod(float *modulatedSym, float N0);
      
      float getEs(void);
      
   private:
      int _bitsPerSymbol;
      float _Es;
};

#endif
