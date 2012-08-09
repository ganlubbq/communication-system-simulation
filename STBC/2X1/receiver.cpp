#include "define.h"
#include "receiver.h"

#include <cstdlib>

#include "cc.h"
#include "modem.h"
#include "stbc.h"

const float sqrt_1_2 = 0.707106781186548;

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


int *rx(complex<float> *z, float N0, int modFormat, int bitsPerSym, LAB524Rand* rng)
{
   // 1 Rx antenna. Add noise.
   // Assume 2 Tx antenna
   int symLen = ((int)real(z[0]))>>1;
   complex<float> *r = new complex<float> [symLen +1];
   r[0] = symLen;
   
   float sd = sqrt(N0/2.0);

   for (int i=1;i<=(symLen>>1);i++)
   {
      complex<float> n1 = (complex<float>)rng->getCmplxGaussian() * sd;
      complex<float> n2 = (complex<float>)rng->getCmplxGaussian() * sd;
      r[2*i-1] = z[2*i-1] + z[2*i-1+symLen] + n1;
      r[2*i  ] = z[2*i  ] + z[2*i  +symLen] + n2;
   }
   
   // De STBC.
   STBC Alamouti;
   extern complex<float> *channelGain;
   complex<float> *x = Alamouti.AlamoutiDecode(r, channelGain);
   delete [] r;
   
   // Remake channel gain. Don't care the other half.
   channelGain[0] = symLen;
   for (int i=1;i<=(symLen>>1);i++)
   {
      // chGain for both x0 and x1 is (|h0|^2 + |h1|^2)/sqrt(2)
      // Note that the factor sqrt(2) is added to compensate for the halved signal energy at the transmitter
      channelGain[2*i-1] = (channelGain[2*i]*conj(channelGain[2*i]) + channelGain[2*i+symLen]*conj(channelGain[2*i+symLen]))*sqrt_1_2;
      channelGain[2*i  ] = channelGain[2*i-1];
   }
   
   //  Modification to the N0
   float mP = 1.0;
   N0 *= 2*mP;
   
   // Demodulate
   float *LLR;
   if (modFormat == PSK)
   {
      // There is special speed-up for BPSK modulation
      if (bitsPerSym == 1)
      {
         extern PskModulator *bpsk;
         LLR = bpsk->bpskDemod(x,N0, channelGain);
      }
      else
      {
         extern PskModulator *psk;
         LLR = psk->pskDemod(x,N0, channelGain);
      }
      
   }
   else if (modFormat == QAM)
   {
      extern QamModulator *qam;
      LLR = qam->qamDemod(x,N0, channelGain);
   }

#ifdef QUANTIZATION_BIT
   quantize(LLR, QUANTIZATION_BIT);
#endif

/*
   // Deinterleaving
   extern int *interleavingOrder;
   deinterleave(LLR,interleavingOrder);

   // Decode
   extern CC *cc;
   int *y = cc->viterbiDecode(LLR);
*/
   int FL = (int)LLR[0];
   int *y = new int [FL+1]; y[0] = FL;
   for (int i=1;i<=FL;i++)
   {
      if (LLR[i]>0) { y[i] = 1; }
      else if (LLR[i]<0) { y[i] = 0; }
      else { y[i] = rand()&1; }
   }

   delete [] LLR;

   return y;
}

