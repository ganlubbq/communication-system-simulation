#include <cmath>
#include "modulator.h"

Modulator::Modulator(float Eb, int bitsPerSymbol)
{
   _bitsPerSymbol = bitsPerSymbol;
   _Es = Eb*bitsPerSymbol;
}


float *Modulator::bpskMod(int *v)
{
   const int symbolLen = v[0]/_bitsPerSymbol;
   
   // Take squre root
   float sqrtEs = sqrt(_Es); 

   float *modulatedSym = new float [symbolLen+1];
   
   // Mapping
   modulatedSym[0] = symbolLen;
   for (int i=1;i<=symbolLen;i++)
   {
      modulatedSym[i] = 2.0*sqrtEs*v[i] - sqrtEs;
   }
   
   return modulatedSym;
}

float *Modulator::bpskDemod(float *r, float N0)
{
   int bitLen = (int)r[0];
   float *LLR = new float [bitLen +1];
   LLR[0] = (float)bitLen;
   
   float sqrtEb = sqrt(_Es);
 
   extern float *channelGain;
   
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

float Modulator::getEs(void)
{
   return _Es;
}
