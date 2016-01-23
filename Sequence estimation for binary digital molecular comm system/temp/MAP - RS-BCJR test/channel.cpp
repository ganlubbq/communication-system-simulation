#include "channel.h"
#include "fileIO.h"
#include <cmath>

#include "LAB524Rand.h"

double *AWGN(double *x ,double N0, LAB524Rand* rng)
{
   const int symLen = (int)x[0];
   double *r = new double [symLen +1];
   r[0] = symLen;
 
//   double sd = sqrt(N0/2.0);                     //standard deviation of gaussian r.v.
   const static double sd = getValue("Noise std", "config.txt");
   
   int j = 1;
   while (j<=symLen)
   {
      double n = rng->getRealGaussian()*sd;
       
      r[j] = x[j] + n;
      
      j++;
   }
   
   return r;
}


double *Rayleigh(double *x, double N0, double mP)
{
/*    double sd = sqrt(N0/2.0);                     //standard deviation of gaussian r.v.
   
    double *ray = new double [(int)x[0] +1];
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
