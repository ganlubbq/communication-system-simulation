#include "define.h"
#include "receiver.h"

#include <cstdlib>

#include "cc.h"
#include "modem.h"
#include <iostream>
using namespace std;
//#define QUANTIZATION_BIT 2             // 1 for hard decision decoding

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


int *rx(complex<float> *x, float N0, int modFormat, int modOrder)
{
   // Demodulate
   float *LLR = new float [(int)real(x[0])*4 +1];
   LLR[0] = (int)real(x[0])*4;
   int bitIdx = 1;
   
   if (modFormat == QAM)
   {
      extern QamModulator *qam;
      
      for (int i=1;i<=(int)real(x[0]);i++)
      {
         float *_llr;
         _llr = qam->one_qamDemod(x[i],1.0,N0);
         
         // Write the results into LLR[]
         for (int jBit=0;jBit<4;jBit++) { LLR[bitIdx+jBit] = _llr[jBit]; }            
         
         bitIdx += 4;
         
         delete _llr;
      }
      
   }

#ifdef QUANTIZATION_BIT
   quantize(LLR, QUANTIZATION_BIT);
#endif

   int *y = new int [(int)LLR[0] +1];
   y[0] = (int)LLR[0];

   for (int i=1; i<=(int)LLR[0]; i++)
   {
      if (LLR[i]>0) { y[i] = 1; }
      else if (LLR[i]<0) { y[i] = 0; }
      else { y[i] = rand()&1; }
   }

   delete [] LLR; 
     
   return y;
}

