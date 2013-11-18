#include "define.h"
#include "receiver.h"

#include <cstdlib>

#include "cc.h"
#include "modem.h"

#define QUANTIZATION_BIT 1             // 1 for hard decision decoding

// Quantize the soft input x[]
inline void quantize(float *x, int bit)
{
   int symbolLen = (int)x[0];

   // Special case for hard decision
   if (bit == 1)
   {
      for (int i=1;i<=symbolLen;i++)
      {
         if (x[i] > 0) { x[i] = 1.0; }
         else if (x[i] < 0) { x[i] = -1.0; }
         else { x[i] = 2.0*(rand()&1) - 1.0; }
      }
   }
   else
   {
      int mul = (1<<(bit-1));
    
      for (int i=1;i<=symbolLen;i++)
      {
         x[i] *= mul;
         x[i] = (int)x[i];
         
         if (x[i] > (1<<(bit-1))-1) { x[i] = (1<<(bit-1))-1; }
         else if (x[i] < -(1<<(bit-1))) { x[i] = -(1<<(bit-1)); }
         
         x[i] /= mul;
      }
   }
}


inline void deinterleave(float *x1,int *interleavingOrder)
{
   int k = (int)x1[0];
   float temp1[k +1];
      
   for (int i=1;i<=k;i++) { temp1[interleavingOrder[i-1]+1] = x1[i]; }
     
   for (int i=1;i<=k;i++) { x1[i] = temp1[i]; }
}


int *rx(complex<float> *x, float N0, int modFormat, int bitsPerSym)
{
   // Demodulate
   float *LLR;
   if (modFormat == PSK)
   {
      // There is special speed-up for BPSK modulation
      if (bitsPerSym == 1)
      {
         extern PskModulator *bpsk;
         LLR = bpsk->bpskDemod(x,N0);
      }
      else
      {
         extern PskModulator *psk;
         LLR = psk->pskDemod(x,N0);
      }
      
   }
   else if (modFormat == QAM)
   {
      extern QamModulator *qam;
//      LLR = qam->qamDemod(x,N0);
      LLR = qam->qamDemod_hard(x,N0);
   }

#ifdef QUANTIZATION_BIT
   quantize(LLR, QUANTIZATION_BIT);
#endif

   // Deinterleaving
   extern int *interleavingOrder;
   deinterleave(LLR,interleavingOrder);

   // Decode
   extern CC *cc;
   int *y = cc->viterbiDecode(LLR);
   delete [] LLR;

   return y;
}

