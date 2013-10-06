#include "receiver.h"

#include <cstdlib>

#include "cc.h"
#include "fileIO.h"
#include "modulator.h"

//#define QUANTIZATION_BIT 1             // 1 for hard decision decoding

// Quantizing the soft input x[]. Place the length of x[] at x[0]
void quan(double *x, int bit)
{
   int symbolLen = (int)x[0];

   // Hard decision
   if (bit == 1)
   {
      for (int i=0;i<symbolLen;i++)
      {
         if (x[i+1] > 0) { x[i+1] = 1.0; }
         else if (x[i+1] < 0) { x[i+1] = -1.0; }
         else { x[i+1] = 2.0*(rand()&1) - 1.0; }
      }
   }

   else
   {
      int mul = (1<<(bit-1));
    
      for (int i=0;i<symbolLen;i++)
      {
         x[i+1] *= mul;
         x[i+1] = (int)x[i+1];
         
         if (x[i+1] > (1<<(bit-1))-1)
         {
            x[i+1] = (1<<(bit-1))-1;
         }
         else if (x[i+1] < -(1<<(bit-1)))
         {
            x[i+1] = -(1<<(bit-1));
         }
         
         x[i+1] /= mul;
      }
   }
}

int **rx(double *x, double N0, int nList)
{
#ifdef QUANTIZATION_BIT
   quan(x, QUANTIZATION_BIT);
#endif

   // Demodulation
   extern MODULATOR *bpsk;
   double *LLR = bpsk->bpskDemod(x,N0);
   
   // Decode
   extern CC *cc;
   int **y = cc->listViterbiDecode(LLR,nList);
   delete [] LLR;
   
   return y;
}

