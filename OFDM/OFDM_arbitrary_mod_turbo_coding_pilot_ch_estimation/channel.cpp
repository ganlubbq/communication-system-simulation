#include <cmath>

#include "channel.h"
#include "LAB524Rand.h"
#include "fft.h"
#include "fileIO.h"

#include <iostream>
using namespace std;

const double pi = 3.141592653589793;

complex<double> *channelGain = NULL;

complex<double> *AWGN(complex<double> *x ,double N0, LAB524Rand* rng)
{
   const int symLen = (int)real(x[0]);
   complex<double> *r = new complex<double> [symLen +1];
   r[0] = symLen;
 
   // Allocate memeory for channelGain[]
   if (channelGain == NULL)
   {
      channelGain = new complex<double> [symLen +1];
      channelGain[0] = symLen;
      for (int j=1;j<=symLen;j++) { channelGain[j] = 1.0; }
   }
   
   double sd = sqrt(N0/2.0);                     //standard deviation of gaussian r.v.
   
   int j = 1;
   while (j<=symLen)
   {
      complex<double> n = (complex<double>)rng->getCmplxGaussian();
      n *= sd;
      
      r[j] = x[j];// + n;
       
      j++;
   }
   
   return r;
}

complex<double> *Rayleigh(complex<double> *x, double N0, LAB524Rand* rng, double mP)
{
   const int symLen = (int)real(x[0]);
   complex<double> *r = new complex<double> [symLen +1];
   r[0] = symLen;
   
   // Allocate memeory for channelGain[]
   if (channelGain == NULL)
   {
      channelGain = new complex<double> [symLen +1];
      channelGain[0] = symLen;
   }
   
   double sd = sqrt(N0/2.0);                     //standard deviation of gaussian r.v.
   
   int j = 1;
   while (j<=symLen)
   {
      // Fade
      channelGain[j] = rng->getCmplxGaussian();
      channelGain[j] *= sqrt(mP/2.0);
      r[j] = x[j]*channelGain[j];
       
      // Add noise
      complex<double> n = (complex<double>)rng->getCmplxGaussian();
      n *= sd;
      r[j] += n;
     
      j++;
   }
   
   return r;
}

// x is supposed to be the freq. domain symbol complex values
complex<double> *multipathCh(complex<double> *x ,double N0, LAB524Rand* rng)
{  
   // Subcarrier freq. spacing = 15 kHz
   // Bandwidth = 15*Nfft kHz
   // Symbol duration = 1/15 kHz = 66 us
   // By defining Nfft_ch = 1024,
   // we have the unit of path delay = 66/1024 us = 64.5 ns
/*
   // ITU outdoor to indoor and pedestrian test environment A
   const int numPath = 4;
   const int Nfft_ch = 1024;
   const int pathDelay[numPath] = {0,2,3,6};
   static double pathPwr[numPath];
   const double K_factor = 0;
   static double pathPwr1_duffsed;
   static double A;
   static int hasInit = 0;
   if (hasInit==0)
   {
      pathPwr[0] = pow(10, 0/10.0);
      pathPwr[1] = pow(10, -9.7/10.0);
      pathPwr[2] = pow(10, -19.2/10.0);
      pathPwr[3] = pow(10, -22.8/10.0);
      
      pathPwr1_duffsed = pathPwr[0]/(K_factor+1);
      A = sqrt(K_factor*pathPwr1_duffsed);
      hasInit = 1;
   }
*/

   // ITU vehicular test environment A
   const int numPath = 6;
   const int Nfft_ch = 1024;
   const int pathDelay[numPath] = {0,5,11,17,27,39};
   static double pathPwr[numPath];
   const double K_factor = 0;
   static double pathPwr1_duffsed;
   static double A;
   static int hasInit = 0;
   if (hasInit==0)
   {
      pathPwr[0] = pow(10, 0/10.0);
      pathPwr[1] = pow(10, -1.0/10.0);
      pathPwr[2] = pow(10, -9.0/10.0);
      pathPwr[3] = pow(10, -10.0/10.0);
      pathPwr[4] = pow(10, -15.0/10.0);
      pathPwr[5] = pow(10, -20.0/10.0);
      
      pathPwr1_duffsed = pathPwr[0]/(K_factor+1);
      A = sqrt(K_factor*pathPwr1_duffsed);
      hasInit = 1;
   }


   // Allocate memory for channelGain[]
   int numSym = (int)(real(x[0]));
   if (channelGain == NULL)
   {
      channelGain = new complex<double> [numSym +1];
      channelGain[0] = numSym;
   }

   // For return
   complex<double> *r = new complex<double> [numSym +1];
   r[0] = numSym;

   extern int Nfft;
   int numOFDMSym = numSym/Nfft;
   
   static unsigned int nOFDMSymSimulated = 0;
   static int numOFDMSymSameFading = (int)getValue("Number of OFDM symbols for one fading block", "config.txt");

   // Initialized the channel impulse response
   static complex<double> *chImpulse;
   static complex<double> *freqResponse;
   if (nOFDMSymSimulated == 0)
   {
      chImpulse = new complex<double> [Nfft_ch +1];
      freqResponse = new complex<double> [Nfft_ch +1];
      chImpulse[0] = Nfft_ch; freqResponse[0] = Nfft_ch;
      for (int i=1;i<=Nfft_ch;i++) { chImpulse[i] = 0.0; }
   }

   for (int iOFDMSym=0;iOFDMSym<numOFDMSym;iOFDMSym++)
   {   
      if (nOFDMSymSimulated%numOFDMSymSameFading == 0)
      {
         // Speedup
         static double sqrt_pathPwr_2[numPath];
         static int hasInit1 = 0;
         if (hasInit1==0)
         {
            for(int i=0;i<numPath;i++)
            { sqrt_pathPwr_2[i] = sqrt(pathPwr[i]/2.0); }
            hasInit1 = 1;
         }
         static double sqrt_pathPwr1_duffsed_2 = sqrt(pathPwr1_duffsed/2.0);

         // Set path channel complex value
         // The first path has to be dealt with seperately due to LOS
         chImpulse[pathDelay[0] +1] = A + (rng->getCmplxGaussian())*sqrt_pathPwr1_duffsed_2;
         if (numPath>1)
         {
            // Construct the channel impulse response
            for (int iPath=1;iPath<numPath;iPath++)
            {
               chImpulse[pathDelay[iPath] +1] = (rng->getCmplxGaussian())*sqrt_pathPwr_2[iPath];
            }
         }
         
         // Convert the ch. impulse to ch. frequence response
         fft(chImpulse,freqResponse,Nfft_ch,0);

         // Note that for those freq. repsonse > Nfft, we don't care
         static double sqrtNfft = sqrt(Nfft);
         for (int i=1;i<=Nfft;i++)
         { freqResponse[i]*= sqrtNfft; }
      }

      int k = 1;
      for (int i=iOFDMSym*Nfft+1;i<=(iOFDMSym+1)*Nfft;i++)
      {
         channelGain[i] = freqResponse[k];
         k++;
      }
      
      static double sd;      
      static double _N0 = -1.0;
      if (_N0 != N0) { sd = sqrt(N0/2.0); }
      _N0 = N0;
         
      int j = 1 + iOFDMSym*Nfft;
      while (j <= (iOFDMSym+1)*Nfft)
      {
         r[j] = x[j]*channelGain[j];

         // Add noise
         complex<double> n = (complex<double>)rng->getCmplxGaussian();
         n *= sd;
         r[j] += n;
     
         j++;
      }
      
      nOFDMSymSimulated++;
   }

   return r;
}

void Rician() {}

void Nakagami() {}

void dispersive() {}
