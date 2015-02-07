#include "define.h"
#include "receiver.h"
#include "modem.h"
#include "fft.h"
#include "misc.h"
#include "LAB524Rand.h"
#include "fileIO.h"
#include "stbc.h"
#include "tbcc.h" 

extern struct TxAttribute txAttribute;

// "channelGain1" and "channelGain2" are for the 1st and 2nd Rx ANTs
complex<double> *channelGain1 = NULL;
complex<double> *channelGain2 = NULL;

int *rx(complex<double> *w_t,complex<double> **z, double N0, int OFDMSymIdx, LAB524Rand *rng)
{  
   /* Channel estimation */
   complex<double> chEstimate1[Nfft +1];
   complex<double> chEstimate2[Nfft +1];
   chEstimate1[0] = Nfft; chEstimate2[0] = Nfft;
   
   // Allocate memory for channelGain[]
   if (channelGain1 == NULL)
   { channelGain1 = new complex<double> [Nfft +1]; channelGain1[0] = Nfft; }
   if (channelGain2 == NULL)
   { channelGain2 = new complex<double> [Nfft +1]; channelGain2[0] = Nfft; }

   // Demodulation and channel estimation parameters.
   // Assuming the modulation format is unchanged for one whole OFDM symbol
   // except for the RSs.
   int modFormat = txAttribute.modFormat;
   int bitPerSym = txAttribute.bitPerSym;
   
#ifdef THEORETICAL_BER
   // Perform FFT to acquire the transmitted signal and received signal (without
   // noise) in freq. domain. Assume 1 tx ANT.
   complex<double> w_f[Nfft +1]; w_f[0] = Nfft;
   fft(w_t,w_f,Nfft,CPLen);
   
   complex<double> r[2][Nfft +1]; r[0][0] = Nfft; r[1][0] = Nfft;
   fft(z[0],r[0],Nfft,CPLen);
   fft(z[1],r[1],Nfft,CPLen);
   
   // Perfect channel estimation
   for (int i=1;i<=NUM_USEDCAR;i++)
   {
      if (w_f[i] != 0.0)
      { channelGain1[i] = r[0][i]/w_f[i]; channelGain2[i] = r[1][i]/w_f[i]; }
      
      chEstimate1[i] = channelGain1[i]; chEstimate2[i] = channelGain2[i];
   }
#endif

#ifndef THEORETICAL_BER
   // Perform 2D channel estimation across both the frequency and time
   // (1 subframe)
#endif
   
   /* Simulate receiver noise. 1X2 MIMO. AWGN. */
   // The 1st and 2nd rows are for the 1st and 2nd rx ANTs respectively.
   static double sd;
   static double _N0 = -1.0;
   if (_N0 != N0) { sd = sqrt(N0/2.0); _N0 = N0; }

//sd = 0;

   for (int i=1;i<=(Nfft+CPLen)>>1;i++)
   {
      // 1st Rx antenna; combine signals and add AWGN.
      complex<double> n1 = (complex<double>)rng->getCmplxGaussian() * sd;
      complex<double> n2 = (complex<double>)rng->getCmplxGaussian() * sd;
      z[0][2*i-1] += n1;
      z[0][2*i  ] += n2;

      // 2nd Rx antenna; combine signals and add AWGN.
      n1 = (complex<double>)rng->getCmplxGaussian() * sd;
      n2 = (complex<double>)rng->getCmplxGaussian() * sd;
      z[1][2*i-1] += n1;
      z[1][2*i  ] += n2;
   }

   // FFT; convert back to the freq domain.
   fft(z[0],r[0],Nfft,CPLen);
   fft(z[1],r[1],Nfft,CPLen);
   
   /* De SFBC */
/*
   STBC Alamouti;
   complex<double> *x1 = Alamouti.AlamoutiDecode(r[0],chEstimate1);
   complex<double> *x2 = Alamouti.AlamoutiDecode(r[1],chEstimate2);
   delete [] r[0];   delete [] r[1];   delete [] r;
*/

   complex<double> *x1 = new complex<double> [Nfft+1]; x1[0] = Nfft;
   complex<double> *x2 = new complex<double> [Nfft+1]; x2[0] = Nfft;
   for (int i=1;i<=Nfft;i++) { x1[i] = r[0][i]; x2[i] = r[1][i]; }
   
   // MRC; note that we reuse the array x1[].
   for (int i=1;i<=NUM_USEDCAR;i++)
   { x1[i] = conj(chEstimate1[i])*x1[i] + conj(chEstimate2[i])*x2[i]; }
   
   delete [] x2;

   // Remake the channel gain due to MRC. Reuse chEstimate1[].
   for (int i=1;i<=Nfft;i++)
   {
      chEstimate1[i] =  chEstimate1[i]*conj(chEstimate1[i])
                      + chEstimate2[i]*conj(chEstimate2[i]);
   }
   
   // Modification to the N0 due to MRC. Assume the mean power (mP) of h1 and h2
   // are the same = mP.
   double mP = 1.0;
   N0 *= 2*mP;
  
   /* Beginning of a new subframe. Perform some initializations. */
   static int nBitRx_c = 0;             // Number of bits (OFDM) modulated.
   static double *LLR = NULL;           // Stores the LLR of coded bits.

   if (OFDMSymIdx == 0)
   {
      // Allocate memory for LLR.
      LLR = new double [txAttribute.codedBitLen_c +1];
      LLR[0] = txAttribute.codedBitLen_c;
      for (int i=1;i<=txAttribute.codedBitLen_c;i++) { LLR[i] = 0; }

      // Reset some counters.
      nBitRx_c = 0;
   }

   /* DeOFDM and demodulation */
   for (int iRB=0;iRB<NUM_RB;iRB++)
   {
      for (int iCar_RB=0;iCar_RB<N_SC;iCar_RB++)
      {
         int iCar = iRB*N_SC + iCar_RB;
         
         int RE_type = RE_mapping(OFDMSymIdx,iRB,iCar_RB);
  
         if (RE_type == RPDCCH)
         {
            // Check if the data has all been received in the case of
            // codedBitLen_c < capacity
            if (nBitRx_c < txAttribute.codedBitLen_c_trun)
            {
               double *_llr;
            
               // Demodulate this symbol
               if (modFormat == PSK)
               { _llr = txAttribute.psk.one_pskDemod(x1[iCar+1],bitPerSym,txAttribute.E_car_data,chEstimate1[iCar+1],N0); }
               else if (modFormat == QAM)
               { _llr = txAttribute.qam.one_qamDemod(x1[iCar+1],bitPerSym,txAttribute.E_car_data,chEstimate1[iCar+1],N0); }

               // Write the results into LLR[]
               for (int jBit=0;jBit<bitPerSym;jBit++) { LLR[nBitRx_c+jBit +1] = _llr[jBit]; }
            
               delete [] _llr;
            
               nBitRx_c += bitPerSym;
            }
         }
      }
   }

   delete [] x1;
   
   int *y = NULL;

   // If this is the last subframe, decode the data.
   if (OFDMSymIdx == (N_SYM-1))
   {
      // Deinterleaving
      int FL = txAttribute.codedBitLen_c/3;
      
      deinterleave(LLR+1,txAttribute.permutation,FL);
      deinterleave(LLR+FL+1,txAttribute.permutation,FL);
      deinterleave(LLR+(FL<<1)+1,txAttribute.permutation,FL);

      // Decoding
      LLR[0] = txAttribute.codedBitLen_c;
      y = txAttribute.tbcc.viterbiDecode(LLR);

      delete [] LLR;
   }

   return y;
}
