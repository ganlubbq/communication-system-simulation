#include "channel.h"

#include "LAB524Rand.h"

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

double *fadingAmp;

double *Rayleigh(double *x, double N0, LAB524Rand* rng, double mP)
{
   const int symLen = (int)x[0];
   double *r = new double [symLen +1];r[0] = symLen;
   
   // Allocate memeory for fadingAmp[]
   static bool hasInitialized = false;
   if (hasInitialized == false)
   {
      fadingAmp = new double [symLen +1];fadingAmp[0] = symLen;
      hasInitialized = true;
   }
   
   double sd = sqrt(N0/2.0);                     //standard deviation of gaussian r.v.
   
   int j = 1;
   while (j<=symLen)
   {
      // Fade
      fadingAmp[j] = rng->getRayleigh(mP);
      r[j] = x[j]*fadingAmp[j];
       
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
