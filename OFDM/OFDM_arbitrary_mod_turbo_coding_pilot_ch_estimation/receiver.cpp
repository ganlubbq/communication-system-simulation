#include "define.h"
#include "receiver.h"

#include <cstdlib>
#include <cmath>
#include <fstream>

#include "modem.h"
#include "fft.h"
#include "turbo.h"

#include <iostream>
using namespace std;

//#define QUANTIZATION_BIT 2             // 1 for hard decision decoding

// Quantize the soft input x[]
inline void quantize(double *x, int bit)
{
   int symbolLen = (int)x[0];

   // Special case for hard decision
   if (bit == 1)
   {
      for (int i=1;i<=symbolLen;i++)
      {
         if (x[i] > 0) { x[i] = 1.0; }
         else if (x[i] < 0) { x[i] = -1.0; }
         else { x[i] = 2.0*(rand()&1) - 1.0; }
      }
   }
   else
   {
      int mul = (1<<(bit-1));
    
      for (int i=1;i<=symbolLen;i++)
      {
         x[i] *= mul;
         x[i] = (int)x[i];
         
         if (x[i] > (1<<(bit-1))-1) { x[i] = (1<<(bit-1))-1; }
         else if (x[i] < -(1<<(bit-1))) { x[i] = -(1<<(bit-1)); }
         
         x[i] /= mul;
      }
   }
}

inline void deinterleave(double *x1,int *interleavingOrder)
{
   int k = (int)x1[0];
   double temp1[k +1];
      
   for (int i=1;i<=k;i++) { temp1[interleavingOrder[i-1]+1] = x1[i]; }
     
   for (int i=1;i<=k;i++) { x1[i] = temp1[i]; }
}

int *rx(complex<double> *x, double N0, int modFormat, int bitsPerSym, int turboIteration)
{ 
   extern int Nfft;
   extern complex<double> *channelGain;
   extern int pilotSpacing;
   int numPilot = (pilotSpacing==0) ? 0 : (int)((double)Nfft/pilotSpacing+0.9999999999);
   int numDataBitsPerOFDMSym = (Nfft-numPilot)*bitsPerSym;
   int numSym = (int)real(x[0]);
   int numOFDMSym = numSym/Nfft;

/*
   // Change the input *x to *r and delete x[] afterwards
   // FFT; convert back to the freq domain
   complex<double> *x = new complex<double> [Nfft+1];
   x[0] = Nfft;

   fft(r,x,Nfft,CPLen);
*/

   complex<double> *chEstimate = new complex<double> [numSym +1];
   chEstimate[0] = numSym;

#ifdef THEORETICAL_BER
   for (int i=1;i<=numSym;i++) { chEstimate[i] = channelGain[i]; }
#endif

#ifndef THEORETICAL_BER
/*
ofstream data1;
data1.open("chGain.txt",ios::out);
for (int i=0;i<numSym;i++) { data1 << imag(channelGain[i]) << endl; }
data1 << endl << endl;
*/
   if (numPilot <= 1) { cout << "Too few pilots!" << endl; getchar(); }
   
   extern PskModulator *psk_pilot;
   extern double E_subcar_pilot;
   static int pilotBit[2] = {1,1};
   complex<double> pilotSym = psk_pilot->one_pskMod(E_subcar_pilot,pilotBit);

   for (int iOFDMSym=0;iOFDMSym<numOFDMSym;iOFDMSym++)
   {
      int subcarOffset = iOFDMSym * Nfft;
       
      // Estimate the channel using pilots 
      int iSubcar = 1;
      while (iSubcar <= Nfft)
      {
         chEstimate[iSubcar+subcarOffset] = x[iSubcar+subcarOffset]/pilotSym;
         iSubcar += pilotSpacing;
      }
   
      // Interpolation
      int iPilot = 0;
      for (int iSubcar=2;iSubcar<=Nfft;iSubcar++)
      {
         if (iSubcar>=((iPilot+1)*pilotSpacing+1) && iPilot<(numPilot-2))
         {
            iPilot++;
            continue;
         }

         complex<double> temp = 1.0/pilotSpacing*(iSubcar-iPilot*pilotSpacing-1); 
         chEstimate[iSubcar+subcarOffset] = (chEstimate[(iPilot+1)*pilotSpacing+1+subcarOffset] - chEstimate[iPilot*pilotSpacing+1+subcarOffset])*temp + chEstimate[iPilot*pilotSpacing+1+subcarOffset];
      }
   }
/*
for (int i=0;i<numSym;i++) { data1 << imag(chEstimate[i]) << endl; }
data1 << endl;
system("pause");
*/
#endif

   // Demodulate
   int FL = numDataBitsPerOFDMSym*numOFDMSym;
   double *LLR = new double [FL +1];
   // Trick here!
   extern int codedBitsLen;
   LLR[0] = codedBitsLen;

   extern PskModulator *psk;
   extern QamModulator *qam;
      
   int bitIdx = 1;
   
   for (int iOFDMSym=0;iOFDMSym<numOFDMSym;iOFDMSym++)
   {   
      for (int iSubcar=1;iSubcar<=Nfft;iSubcar++)
      {
         if ((iSubcar-1)%pilotSpacing != 0) // Only demodulate data carriers
         {
            double *_llr;
            
            // Demodulate this symbol
            if (modFormat == PSK)
            { _llr = psk->one_pskDemod(x[iSubcar+iOFDMSym*Nfft],chEstimate[iSubcar+iOFDMSym*Nfft],N0); }
            else if (modFormat == QAM)
            { _llr = qam->one_qamDemod(x[iSubcar+iOFDMSym*Nfft],chEstimate[iSubcar+iOFDMSym*Nfft],N0); }
                        
            // Write the results into LLR[]
            for (int jBit=0;jBit<bitsPerSym;jBit++) { LLR[bitIdx+jBit] = _llr[jBit]; }
            
            delete [] _llr;
            
            bitIdx += bitsPerSym;
         }
      }
   }
   
   delete [] chEstimate;

#ifdef QUANTIZATION_BIT
   quantize(LLR, QUANTIZATION_BIT);
#endif

   // Deinterleaving
   extern int *interleavingOrder;
   deinterleave(LLR,interleavingOrder);

   int *y = bcjrDecode(LLR,N0,turboIteration);

   delete [] LLR;
   
   return y;
}
