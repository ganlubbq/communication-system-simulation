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

int *rx(complex<double> **z, double E_tot, double N0, int OFDMSymIdx, LAB524Rand *rng)
{  
   /* Simulate receiver noise. 2X2 MIMO. AWGN. */
   // The 1st and 2nd rows are for the 1st and 2nd rx ANTs respectively. The 1st
   // and 2nd half of each row are for the 1st and 2nd tx ANTs respectively.
   complex<double> **r = new complex<double>* [2];
   r[0] = new complex<double> [Nfft +1];
   r[1] = new complex<double> [Nfft +1];
   r[0][0] = Nfft;
   r[1][0] = Nfft;

   static double sd;
   static double _N0 = -1.0;
   if (_N0 != N0) { sd = sqrt(N0/2.0); _N0 = N0; }

//sd = 0;

   for (int i=1;i<=(NUM_USEDCAR>>1);i++)
   {
      // 1st Rx antenna; combine signals and add AWGN.
      complex<double> n1 = (complex<double>)rng->getCmplxGaussian() * sd;
      complex<double> n2 = (complex<double>)rng->getCmplxGaussian() * sd;
      r[0][2*i-1] = z[0][2*i-1] + z[0][2*i-1+Nfft] + n1;
      r[0][2*i  ] = z[0][2*i  ] + z[0][2*i  +Nfft] + n2;

      // 2nd Rx antenna; combine signals and add AWGN.
      n1 = (complex<double>)rng->getCmplxGaussian() * sd;
      n2 = (complex<double>)rng->getCmplxGaussian() * sd;
      r[1][2*i-1] = z[1][2*i-1] + z[1][2*i-1+Nfft] + n1;
      r[1][2*i  ] = z[1][2*i  ] + z[1][2*i  +Nfft] + n2;
   }
 
   /* Channel estimation */
   // Demodulation and channel estimation parameters.
   // Assuming the modulation format is unchanged for one whole OFDM symbol
   // except for the RSs.
   int modFormat = txAttribute.modFormat;
   int bitPerSym = txAttribute.bitPerSym;

   // Calculate RS energy
   double E_car_data = N_SYM*E_tot/(NUM_RB*NUM_RS_PER_RB*(txAttribute.pwrBoost
                       -1.0)+NUM_USEDCAR*N_SYM);
   double E_car_RS = E_car_data*txAttribute.pwrBoost;

   // "channelGain1" and "channelGain2" are for the 1st and 2nd Rx ANTs
   // respectively. Sizes of them are both 2*Nfft since there are 2 Tx antennas.
   extern complex<double> *channelGain1;
   extern complex<double> *channelGain2;
   complex<double> *chEstimate1 = new complex<double> [(Nfft<<1) +1];
   complex<double> *chEstimate2 = new complex<double> [(Nfft<<1) +1];
   chEstimate1[0] = (Nfft<<1);   chEstimate2[0] = (Nfft<<1);
   
#ifdef THEORETICAL_BER
   // Perfect channel estimation
   for (int i=1;i<=(Nfft<<1);i++)
   { chEstimate1[i] = channelGain1[i]; chEstimate2[i] = channelGain2[i]; }
#endif

#ifndef THEORETICAL_BER
   // Perform 2D channel estimation across both the frequency and time
   // (1 subframe)
#endif

   /* De SFBC */ 
   STBC Alamouti;
   complex<double> *x1 = Alamouti.AlamoutiDecode(r[0],chEstimate1);
   complex<double> *x2 = Alamouti.AlamoutiDecode(r[1],chEstimate2);
   delete [] r[0];   delete [] r[1];   delete [] r;

   // MRC; note that we reuse the array x1[].
   for (int i=1;i<=NUM_USEDCAR;i++)
   {
      x1[i] = chEstimate1[i]*x1[i] + chEstimate2[i]*x2[i];
   }
   delete [] x2;

   // Remake the channel gain due to MRC. Reuse chEstimate1[].
   for (int i=1;i<=Nfft;i++)
   {
      chEstimate1[i] =  real(chEstimate1[i])*real(chEstimate1[i])
                      + real(chEstimate2[i])*real(chEstimate2[i]);
   }
   delete [] chEstimate2;
   
   // Modification to the N0; some calculation is needed.
   double mP = 1.0;
   N0 *= 24*mP*mP*mP;
  
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
               { _llr = txAttribute.psk.one_pskDemod(x1[iCar+1],bitPerSym,E_car_data,chEstimate1[iCar+1],N0); }
               else if (modFormat == QAM)
               { _llr = txAttribute.qam.one_qamDemod(x1[iCar+1],bitPerSym,E_car_data,chEstimate1[iCar+1],N0); }

               // Write the results into LLR[]
               for (int jBit=0;jBit<bitPerSym;jBit++) { LLR[nBitRx_c+jBit +1] = _llr[jBit]; }
            
               delete [] _llr;
            
               nBitRx_c += bitPerSym;
            }
         }
      }
   }

   delete [] chEstimate1;
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
