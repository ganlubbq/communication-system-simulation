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


int *rx(int *u, complex<float> **z, float N0, int modFormat, int bitsPerSym, LAB524Rand* rng)
{
   // 2X2 MIMO. Add noise.
   int symLen = ((int)real(z[0][0]))>>1;
   complex<float> **r = new complex<float>* [2];
   r[0] = new complex<float> [symLen +1];
   r[1] = new complex<float> [symLen +1];
   r[0][0] = symLen;
   r[1][0] = symLen;
   
   float sd = sqrt(N0/2.0);

   for (int i=1;i<=(symLen>>1);i++)
   {
      // 1st Rx antenna; combine signals and add AWGN
      complex<float> n1 = (complex<float>)rng->getCmplxGaussian() * sd;
      complex<float> n2 = (complex<float>)rng->getCmplxGaussian() * sd;
      r[0][2*i-1] = z[0][2*i-1] + z[0][2*i-1+symLen] + n1;
      r[0][2*i  ] = z[0][2*i  ] + z[0][2*i  +symLen] + n2;

      // 2nd Rx antenna; combine signals and add AWGN
      n1 = (complex<float>)rng->getCmplxGaussian() * sd;
      n2 = (complex<float>)rng->getCmplxGaussian() * sd;
      r[1][2*i-1] = z[1][2*i-1] + z[1][2*i-1+symLen] + n1;
      r[1][2*i  ] = z[1][2*i  ] + z[1][2*i  +symLen] + n2;
   }
   
   // De STBC.
   STBC Alamouti;
   extern complex<float> *channelGain1;
   extern complex<float> *channelGain2;
   complex<float> *x1 = Alamouti.AlamoutiDecode(r[0],channelGain1);
   complex<float> *x2 = Alamouti.AlamoutiDecode(r[1],channelGain2);
   delete [] r[0];   delete [] r[1];   delete [] r;
   
   // Remake channel gain. Don't care the other half
   // (originally used to store the chGain for the 2nd Tx antenna)
   channelGain1[0] = symLen;
   channelGain2[0] = symLen;
   for (int i=1;i<=(symLen>>1);i++)
   {
      // chGain for both x0 and x1 is (|h0|^2 + |h1|^2)/sqrt(2)
      // Note that the factor sqrt(2) is added to compensate for the halved signal energy at the transmitter
      // 1st Rx antenna;   
      channelGain1[2*i-1] = (channelGain1[2*i]*conj(channelGain1[2*i]) + channelGain1[2*i+symLen]*conj(channelGain1[2*i+symLen]))*sqrt_1_2;
      channelGain1[2*i  ] = channelGain1[2*i-1];
      
      // 2nd Rx antenna
      channelGain2[2*i-1] = (channelGain2[2*i]*conj(channelGain2[2*i]) + channelGain2[2*i+symLen]*conj(channelGain2[2*i+symLen]))*sqrt_1_2;
      channelGain2[2*i  ] = channelGain2[2*i-1];
   }
   
   // MRC; note that we reuse the array x1 again
   for (int i=1;i<=symLen;i++)
   {
      x1[i] = channelGain1[i]*x1[i] + channelGain2[i]*x2[i];
   }
   delete [] x2;
   
   // Remake the channel gain the second time due to MRC
   // We also reuse channelGain1 again
   for (int i=1;i<=symLen;i++)
   {
      channelGain1[i] = real(channelGain1[i])*real(channelGain1[i]) + real(channelGain2[i])*real(channelGain2[i]);
   }
   
   //  Modification to the N0; some calculation is needed
   float mP = 1.0;
   N0 *= 24*mP*mP*mP;
   
   float meanSquare = 0;
   for (int i=1;i<=symLen;i++)
   {
      float noise = real(x1[i] - channelGain1[i]*(complex<float>)(2*u[i]-1));
      meanSquare += noise*noise;
   }
   meanSquare /= symLen;
   cout << "meanSquare of noise = " << meanSquare << endl;
   cout << "predicted value = " << N0/2 << endl;
   getchar();
   
   // Demodulate
   float *LLR;
   if (modFormat == PSK)
   {
      // There is special speed-up for BPSK modulation
      if (bitsPerSym == 1)
      {
         extern PskModulator *bpsk;
         LLR = bpsk->bpskDemod(x1,N0, channelGain1);
      }
      else
      {
         extern PskModulator *psk;
         LLR = psk->pskDemod(x1,N0, channelGain1);
      }
      
   }
   else if (modFormat == QAM)
   {
      extern QamModulator *qam;
      LLR = qam->qamDemod(x1,N0, channelGain1);
   }

   delete [] x1;
   
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

