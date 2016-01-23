#include "channel.h"
#include "LAB524Rand.h"
#include "fileIO.h"
#include <cmath>

float *channelGain = NULL;

float *AWGN(float *x ,float N0, LAB524Rand* rng)
{
   const int symLen = (int)x[0];
   float *r = new float [symLen +1];
   r[0] = symLen;
 
//   float sd = sqrt(N0/2.0);                     //standard deviation of gaussian r.v.
   const static float sd = getValue("Noise std", "config.txt");
    
   int j = 1;
   while (j<=symLen)
   {
      float n = rng->getRealGaussian()*sd;
       
      r[j] = x[j] + n;
       
      j++;
   }
   
   return r;
}



float *Rayleigh(float *x, float N0, LAB524Rand* rng, float mP)
{
   const int symLen = (int)x[0];
   float *r = new float [symLen +1];r[0] = symLen;
   
   // Allocate memeory for channelGain[]
   static bool hasInitialized = false;
   if (hasInitialized == false)
   {
      channelGain = new float [symLen +1];channelGain[0] = symLen;
      hasInitialized = true;
   }
   
   float sd = sqrt(N0/2.0);                     //standard deviation of gaussian r.v.
   
   int j = 1;
   while (j<=symLen)
   {
      // Fade
      channelGain[j] = rng->getRayleigh(mP);
      r[j] = x[j]*channelGain[j];
       
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
