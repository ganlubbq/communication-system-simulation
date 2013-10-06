/*
 *
 * Date : 2008/1/8 
 */
 
#ifndef MADULATOR_H
#define MADULATOR_H

class MODULATOR
{
   public:
      MODULATOR(double Eb, int bitsPerSymbol);
      
      double *bpskMod(int *bitSequenceToBeModulated);
      
      // Output the LLR of original data bits
      double *bpskDemod(double *modulatedSym, double N0);
      
      double getEs(void);
      
   private:
      int _bitsPerSymbol;
      double _Es;
};

#endif
