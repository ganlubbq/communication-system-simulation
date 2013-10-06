#include "channel.h"

#include <cmath>

#include "random.h"

float *AWGN(float *x ,float N0, float *noiseBuff)
{
      const int symLen = (int)x[0];
      float *r = new float [symLen +1];
      r[0] = symLen;
      
      float sd = sqrt(N0/2.0);                     //standard deviation of gaussian r.v.
   
      int j = 0;
      while (j<symLen)
      {
         float n = normalRV()*sd;
       
         r[j +1] = x[j +1] + n;
       
         noiseBuff[j] = n;

         j++;
      }
      
      return r;
}


float *Rayleigh(float *x, float N0, float mP)
{
    float sd = sqrt(N0/2.0);                     //standard deviation of gaussian r.v.
   
    float *ray = new float [(int)x[0] +1];
    ray[0] = (int)x[0];
    
    int j = 0;
    while (j<x[0])
    {
       ray[j +1] = rayleighRV(mP);
       x[j +1] *= ray[j +1];
       x[j +1] += normalRV()*sd;
      
       j++;
    }
}

void Rician() {}

void Nakagami() {}

void dispersive() {}
