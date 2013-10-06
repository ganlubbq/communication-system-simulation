#include "channel.h"

#include <cmath>

#include "random.h"

void AWGN(float *x , int len, float snr, float *r, float *noiseBuff)
{
    float codeRate = 1.0/2.0;
    float N0 = 1.0/snr/codeRate;                          //SNR = Eb*2/N0 , Eb=1
    float sd = sqrt(N0/2.0);                     //standard deviation of gaussian r.v.
   
    int j = 0;
    while (j<len)
    {
       r[j] = x[j];
       
       // 2 noise!
       float n = normalRV()*sd + normalRV()*sd;
       
       r[j] += n;
       
       noiseBuff[j] = n;
        
       j++;
    }
}

void Rayleigh(float *x , int len, float snr, float *r, float *ray, float mP)
{
    float codeRate = 1.0/2.0;
    float N0 = 1.0/snr/codeRate;                          //SNR = Eb*2/N0 , Eb=1
    float sd = sqrt(N0/2.0);                     //standard deviation of gaussian r.v.
   
    int j = 0;
    while (j<len)
    {
       r[j] = x[j]; 

       ray[j] = rayleighRV(mP);
       r[j] *= ray[j];
       r[j] += normalRV()*sd;
      
       j++;
    }
}

void Rician() {}

void Nakagami() {}

void dispersive() {}
