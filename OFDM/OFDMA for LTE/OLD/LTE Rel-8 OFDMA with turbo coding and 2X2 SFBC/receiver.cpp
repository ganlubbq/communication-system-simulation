#include "define.h"
#include "receiver.h"
#include "modem.h"
#include "fft.h"
#include "turbo.h"
#include "misc.h"
#include "LAB524Rand.h"
#include "fileIO.h"
#include "stbc.h"

// Read from the tx side
extern double pwrBoost; 
extern int codedBitsLen;

extern int modFormat;
extern PskModulator *psk;
extern QamModulator *qam;
      
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
   int bitsPerSym;
   if (modFormat == PSK) { bitsPerSym = psk->getBitsPerSym(); }
   else if (modFormat == QAM) { bitsPerSym = qam->getBitsPerSym(); }
   
   // Calculate RS energy
   double E_car_data = N_SYM*E_tot/(NUM_RB*NUM_RS_PER_RB*(pwrBoost-1.0)+NUM_USEDCAR*N_SYM);
   double E_car_RS = E_car_data*pwrBoost;

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
  
   /* DeOFDM and demodulation */
   static double *LLR = NULL;
   static int codedBitRxIdx = 0;
   
   // If this is the 1st subframe, perform some initializations.
   if (OFDMSymIdx == 0)
   {
      // Allocate memory for LLR
      LLR = new double [codedBitsLen +1];
      LLR[0] = codedBitsLen;
      
      codedBitRxIdx = 0;
   }

   for (int iRB=0;iRB<NUM_RB;iRB++)
   {
      for (int iCar_RB=1;iCar_RB<=N_SC;iCar_RB++)
      {
         int iCar = iRB*N_SC + iCar_RB;
         
         // These are the RSs.
         if ((OFDMSymIdx==0 || OFDMSymIdx==4 || OFDMSymIdx==7 || OFDMSymIdx==11)
          && (iCar_RB==1 || iCar_RB==4 || iCar_RB==7 || iCar_RB==10))
         {}
         // These are PDCCH symbols sent by eNB.
         else if (OFDMSymIdx < NUM_PDCCH_SYM)
         {}
         else
         {
            double *_llr;
            
            // Demodulate this symbol
            if (modFormat == PSK)
            { _llr = psk->one_pskDemod(x1[iCar],bitsPerSym,E_car_data,chEstimate1[iCar],N0); }
            else if (modFormat == QAM)
            { _llr = qam->one_qamDemod(x1[iCar],bitsPerSym,E_car_data,chEstimate1[iCar],N0); }

            // Write the results into LLR[]
            for (int jBit=0;jBit<bitsPerSym;jBit++) { LLR[codedBitRxIdx+jBit +1] = _llr[jBit]; }
            
            delete [] _llr;
            
            codedBitRxIdx += bitsPerSym;
         }
      }
   }

   delete [] chEstimate1;
   delete [] x1;
   
   int *y = NULL;

   // If this is the last subframe, decode the data.
   if (OFDMSymIdx == (N_SYM-1))
   {
      // Deal with rate matching; puncture the latter part
      int codedBitsLen_matched;
      if (NUM_PDCCH_SYM == 0)
      { codedBitsLen_matched = (NUM_USEDCAR*N_SYM - NUM_RB*NUM_RS_PER_RB)
                                *bitsPerSym; }
      else 
      { codedBitsLen_matched = (NUM_USEDCAR*(N_SYM-NUM_PDCCH_SYM) - NUM_RB
                                *(NUM_RS_PER_RB-4))*bitsPerSym; }


      if (codedBitsLen_matched == codedBitsLen) {}
      else 
      { for (int i=codedBitsLen_matched+1;i<=codedBitsLen;i++) { LLR[i] = 0; } }

      // Deinterleaving
      LLR[0] = codedBitsLen_matched;
      extern int *interleavingOrder;
      deinterleave(LLR,interleavingOrder);

      // Decoding
      LLR[0] = codedBitsLen;
      y = bcjrDecode(LLR,N0);

      delete [] LLR;
   }

   return y;
}
