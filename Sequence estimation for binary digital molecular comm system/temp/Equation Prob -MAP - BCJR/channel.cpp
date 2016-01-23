#include "channel.h"
#include "fileIO.h"
#include <cmath>

#include "LAB524Rand.h"

long double *AWGN(long double *x ,long double N0, LAB524Rand* rng)
{
   const int symLen = (int)x[0];
   long double *r = new long double [symLen +1];
   r[0] = symLen;
 
//   long double sd = sqrt(N0/2.0);                     //standard deviation of gaussian r.v.
   const static long double sd = getValue("Noise std", "config.txt");
   
   int j = 1;
   while (j<=symLen)
   {
      long double n = rng->getRealGaussian()*sd;
       
      r[j] = x[j] + n;
      
      j++;
   }
   
   return r;
}


long double *Rayleigh(long double *x, long double N0, long double mP)
{
/*    long double sd = sqrt(N0/2.0);                     //standard deviation of gaussian r.v.
   
    long double *ray = new long double [(int)x[0] +1];
    ray[0] = (int)x[0];
    
    int j = 0;
    while (j<x[0])
    {
       ray[j +1] = rayleighRV(mP);
       x[j +1] *= ray[j +1];
       x[j +1] += normalRV()*sd;
      
       j++;
    }
*/
}

void Rician() {}

void Nakagami() {}

void dispersive() {}
