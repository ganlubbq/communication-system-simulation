#include <cmath>
#include "modulator.h"

MODULATOR::MODULATOR(double Eb, int bitsPerSymbol)
{
   _bitsPerSymbol = bitsPerSymbol;
   _Es = Eb*bitsPerSymbol;
}


double *MODULATOR::bpskMod(int *v)
{
   const int symbolLen = v[0]/_bitsPerSymbol;
   
   // Take squre root
   double sqrtEs = sqrt(_Es); 

   double *modulatedSym = new double [symbolLen+1];
   
   // Mapping
   modulatedSym[0] = symbolLen;
   for (int i=1;i<=symbolLen;i++)
   {
      modulatedSym[i] = 2.0*sqrtEs*v[i] - sqrtEs;
   }
   
   return modulatedSym;
}

double *MODULATOR::bpskDemod(double *r, double N0)
{
   int bitLen = (int)r[0];
   double *LLR = new double [bitLen +1];
   LLR[0] = (double)bitLen;
   
   double sqrtEb = sqrt(_Es);
 
   extern double *channelGain;
   
   // Special case for AWGN
   if (channelGain == 0)
   {
      for (int i=1;i<=bitLen;i++)
      {
         LLR[i] = 4*sqrtEb*r[i]/N0;
      }
   }
   else
   {
      for (int i=1;i<=bitLen;i++)
      {
         LLR[i] = 4*channelGain[i]*sqrtEb*r[i]/N0;
      }
   }
   return LLR;
}

double MODULATOR::getEs(void)
{
   return _Es;
}
