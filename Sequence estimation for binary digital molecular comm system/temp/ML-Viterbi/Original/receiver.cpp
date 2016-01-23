#include "receiver.h"

#include <cstdlib>

#include "cc.h"
#include "modulator.h"

//#define QUANTIZATION_BIT 1             // 1 for hard decision decoding

// Quantizing the soft input x[]. Place the length of x[] at x[0]
void quan(float *x, int bit)
{
   int symbolLen = (int)x[0];

   // Hard decision
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
         
         if (x[i] > (1<<(bit-1))-1)
         {
            x[i] = (1<<(bit-1))-1;
         }
         else if (x[i] < -(1<<(bit-1)))
         {
            x[i] = -(1<<(bit-1));
         }
         
         x[i] /= mul;
      }
   }
}

int *rx(float *x, float N0)
{
#ifdef QUANTIZATION_BIT
   quan(x, QUANTIZATION_BIT);
#endif

/*
   // Demodulation
   extern Modulator *bpsk;
   float *LLR = bpsk->bpskDemod(x,N0);
   
   // Decode
   extern CC *cc;
   int *y = cc->viterbiDecode(LLR);
   delete [] LLR;
*/
   
   int FL = (int)x[0];
   int *y = new int [FL+1]; y[0] = FL;
   float eta = 1.3224e+006;
   
   for (int i=1;i<=FL;i++)
   {
      if (x[i]>eta) { y[i] = 1; }
      else { y[i] = 0; }
   }

   return y;
}

