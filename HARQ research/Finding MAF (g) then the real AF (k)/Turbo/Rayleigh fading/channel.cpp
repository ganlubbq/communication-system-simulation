#include "channel.h"
#include "LAB524Rand.h"
#include "fileIO.h"

const int fadingLength = (int)getValue("Fading duration in number of symbols", "config.txt");

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
   
//   fadingAmp[1] = rng->getRayleigh(mP);
   int nSymbol = 0;
   
   double thisFadingAmp = rng->getRayleigh(mP);
   
   while (j<=symLen)
   {
      if (nSymbol >= fadingLength)
      {
         thisFadingAmp = rng->getRayleigh(mP);
         nSymbol = 0;
      }
      fadingAmp[j] = thisFadingAmp;
      nSymbol++;
      
      r[j] = x[j]*thisFadingAmp;
       
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
