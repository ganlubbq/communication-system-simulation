#include "channel.h"
#include "LAB524Rand.h"
#include "fileIO.h"

const int fadingLength = (int)getValue("Fading duration in number of symbols", "config.txt");

float *AWGN(float *x ,float N0, LAB524Rand* rng)
{
   const int symLen = (int)x[0];
   float *r = new float [symLen +1];
   r[0] = symLen;
 
   float sd = sqrt(N0/2.0);                     //standard deviation of gaussian r.v.
   
   int j = 1;
   while (j<=symLen)
   {
      float n = rng->getRealGaussian()*sd;
       
      r[j] = x[j] + n;
       
      j++;
   }
   
   return r;
}

float *fadingAmp;

float *Rayleigh(float *x, float N0, LAB524Rand* rng, float mP)
{
   const int symLen = (int)x[0];
   float *r = new float [symLen +1];r[0] = symLen;
   
   // Allocate memeory for fadingAmp[]
   static bool hasInitialized = false;
   if (hasInitialized == false)
   {
      fadingAmp = new float [symLen +1];fadingAmp[0] = symLen;
      hasInitialized = true;
   }
   
   float sd = sqrt(N0/2.0);                     //standard deviation of gaussian r.v.
   
   int j = 1;
   
//   fadingAmp[1] = rng->getRayleigh(mP);
   int nSymbol = 0;
   
   float thisFadingAmp = rng->getRayleigh(mP);
   
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
      float n = rng->getRealGaussian()*sd;
      r[j] += n;
     
      j++;
   }
   
   return r;
}

void Rician() {}

void Nakagami() {}

void dispersive() {}
