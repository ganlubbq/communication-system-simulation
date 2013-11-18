#include "channel.h"
#include "LAB524Rand.h"
#include <cmath>

complex<float> *channelGain = NULL;

complex<float> *AWGN(complex<float> *x ,float N0, LAB524Rand* rng)
{
   const int symLen = (int)real(x[0]);
   complex<float> *r = new complex<float> [symLen +1];
   r[0] = symLen;
 
   // Allocate memeory for channelGain[]
   if (channelGain == NULL)
   {
      channelGain = new complex<float> [symLen +1];
      channelGain[0] = symLen;
      for (int j=1;j<=symLen;j++) { channelGain[j] = 1.0; }
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

complex<float> *Rayleigh(complex<float> *x, float N0, LAB524Rand* rng, float mP)
{
   const int symLen = (int)real(x[0]);
   complex<float> *r = new complex<float> [symLen +1];
   r[0] = symLen;
   
   // Allocate memeory for channelGain[]
   if (channelGain == NULL)
   {
      channelGain = new complex<float> [symLen +1];
      channelGain[0] = symLen;
   }
   
   float sd = sqrt(N0/2.0);                     //standard deviation of gaussian r.v.
   
   int j = 1;
   while (j<=symLen)
   {
      // Fade
      channelGain[j] = rng->getCmplxGaussian();
      channelGain[j] *= sqrt(mP/2.0);
      r[j] = x[j]*channelGain[j];
       
      // Add noise
      complex<float> n = (complex<float>)rng->getCmplxGaussian();
      n *= sd;
      r[j] += n;
     
      j++;
   }
   
   return r;
}

void Rician() {}

void Nakagami() {}

void dispersive() {}
