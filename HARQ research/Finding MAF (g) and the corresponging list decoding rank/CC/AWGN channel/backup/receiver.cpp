#include "receiver.h"

#include <cstdlib>

#include "cc.h"
#include "fileIO.h"

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

int *rx(double *x, double N0)
{
#ifdef QUANTIZATION_BIT
   quan(x, QUANTIZATION_BIT);
#endif

/*
   // Puncturing.
   // Remember to change to code rate from 1/3 to 1/2.
   for (int i=0;i<(int)x[0]/3;i++)
   {
      if((i&1)==0) { x[3*i+1 +1] = 0; }
      else { x[3*i+2 +1] = 0; }
   }
*/

   int *y = viterbiDecode(x);

   return y;
}

