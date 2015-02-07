#include "channel.h"
#include "LAB524Rand.h"
#include "fft.h"
#include "fileIO.h"
#include "define.h"

#include <iostream>
using namespace std;

// For Rx antenna 1 and 2
complex<double> *channelGain1 = NULL;
complex<double> *channelGain2 = NULL;

complex<double> **Dummy(complex<double> *z ,double N0, LAB524Rand* rng)
{
   const int signalLen = (int)real(z[0]);
   complex<double> **r = new complex<double>* [2];
   r[0] = new complex<double> [signalLen +1];
   r[1] = new complex<double> [signalLen +1];
   r[0][0] = signalLen;
   r[1][0] = signalLen;
   
   // Allocate memory for channelGain[]
   if (channelGain1 == NULL)
   {
      channelGain1 = new complex<double> [signalLen +1];
      for (int j=1;j<=signalLen;j++) { channelGain1[j] = 1.0; }
      channelGain1[0] = signalLen;
   }
   if (channelGain2 == NULL)
   {
      channelGain2 = new complex<double> [signalLen +1];
      for (int j=1;j<=signalLen;j++) { channelGain2[j] = 1.0; }
      channelGain2[0] = signalLen;
   }

   // No channel effect.
   for (int j=1;j<=signalLen;j++)
   {
      r[0][j] = z[j];
      r[1][j] = z[j];
   }
   
   return r;
}

complex<double> **blockRayleigh(complex<double> *z, double N0, int blockLen, LAB524Rand* rng, double mP)
{
   const int signalLen = (int)real(z[0]);
   complex<double> **r = new complex<double>* [2];
   r[0] = new complex<double> [signalLen +1];
   r[1] = new complex<double> [signalLen +1];
   r[0][0] = signalLen;
   r[1][0] = signalLen;
   
   // Allocate memory for channelGain[]
   if (channelGain1 == NULL) { channelGain1 = new complex<double> [signalLen +1]; }
   if (channelGain2 == NULL) { channelGain2 = new complex<double> [signalLen +1]; }
   channelGain1[0] = signalLen;
   channelGain2[0] = signalLen;
   
   int nSymbol = 0;
   complex<double> currentCmplxChGain1 = (complex<double>)rng->getCmplxGaussian();
   complex<double> currentCmplxChGain2 = (complex<double>)rng->getCmplxGaussian();
   
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
