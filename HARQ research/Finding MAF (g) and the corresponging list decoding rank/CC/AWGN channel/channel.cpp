#include "channel.h"
#include "LAB524Rand.h"
#include <cmath>

double *channelGain = NULL;

double *AWGN(double *x ,double N0, LAB524Rand* rng)
{
   const int symLen = (int)x[0];
   double *r = new double [symLen +1];
   r[0] = symLen;
 
   double sd = sqrt(N0/2.0);                     //standard deviation of gaussian r.v.
   
   int j = 1;
   while (j<=symLen)
   {
      double n = rng->getRealGaussian()*sd;
       
      r[j] = x[j] + n;
       
      j++;
   }
   
   return r;
}



double *Rayleigh(double *x, double N0, LAB524Rand* rng, double mP)
{
   const int symLen = (int)x[0];
   double *r = new double [symLen +1];r[0] = symLen;
   
   // Allocate memeory for channelGain[]
   static bool hasInitialized = false;
   if (hasInitialized == false)
   {
      channelGain = new double [symLen +1];channelGain[0] = symLen;
      hasInitialized = true;
   }
   
   double sd = sqrt(N0/2.0);                     //standard deviation of gaussian r.v.
   
   int j = 1;
   while (j<=symLen)
   {
      // Fade
      channelGain[j] = rng->getRayleigh(mP);
      r[j] = x[j]*channelGain[j];
       
      // Add noise
      double n = rng->getRealGaussian()*sd;
      r[j] += n;
     
      j++;
   }
   
   return r;
}

void Rician() {}

void Nakagami() {}

void dispersive() {}
