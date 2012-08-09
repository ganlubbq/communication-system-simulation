#include "channel.h"
#include "LAB524Rand.h"
#include <cmath>

// For Rx antenna 1 and 2
complex<float> *channelGain1 = NULL;
complex<float> *channelGain2 = NULL;

complex<float> *AWGN(complex<float> *x ,float N0, LAB524Rand* rng)
{
   const int symLen = (int)real(x[0]);
   complex<float> *r = new complex<float> [symLen +1];
   r[0] = symLen;
 
   // Allocate memeory for channelGain[]
   if (channelGain1 == NULL)
   {
      channelGain1 = new complex<float> [symLen +1];
      channelGain1[0] = symLen;
      for (int j=1;j<=symLen;j++) { channelGain1[j] = 1.0; }
   }
   
   float sd = sqrt(N0/2.0);                     //standard deviation of gaussian r.v.
   
   int j = 1;
   while (j<=symLen)
   {
      complex<float> n = (complex<float>)rng->getCmplxGaussian();
      n *= sd;
      
      r[j] = x[j] + n;
       
      j++;
   }
   
   return r;
}

complex<float> **blockRayleigh(complex<float> *z, float N0, int blockLen, LAB524Rand* rng, float mP)
{
   const int signalLen = (int)real(z[0]);
   complex<float> **r = new complex<float>* [2];
   r[0] = new complex<float> [signalLen +1];
   r[1] = new complex<float> [signalLen +1];
   r[0][0] = signalLen;
   r[1][0] = signalLen;
   
   // Allocate memory for channelGain[]
   if (channelGain1 == NULL) { channelGain1 = new complex<float> [signalLen +1]; }
   if (channelGain2 == NULL) { channelGain2 = new complex<float> [signalLen +1]; }
   channelGain1[0] = signalLen;
   channelGain2[0] = signalLen;
   
   int nSymbol = 0;
   complex<float> currentCmplxChGain1 = (complex<float>)rng->getCmplxGaussian();
   complex<float> currentCmplxChGain2 = (complex<float>)rng->getCmplxGaussian();
   
   int j = 1;
   while (j<=signalLen)
   {
      // Check if a new cmplx channel gain has to be generated
      if (nSymbol >= blockLen)
      {
         currentCmplxChGain1 = rng->getCmplxGaussian();
         currentCmplxChGain2 = rng->getCmplxGaussian();
         nSymbol = 0;
      }
      nSymbol++;
      
      // Apply cmplx channel gain
      channelGain1[j] = currentCmplxChGain1;
      channelGain1[j] *= sqrt(mP/2.0);
      r[0][j] = z[j]*channelGain1[j];
      
      channelGain2[j] = currentCmplxChGain2;
      channelGain2[j] *= sqrt(mP/2.0);
      r[1][j] = z[j]*channelGain2[j];
      
      j++;
   }

   return r;
}

void Rician() {}

void Nakagami() {}

void dispersive() {}
