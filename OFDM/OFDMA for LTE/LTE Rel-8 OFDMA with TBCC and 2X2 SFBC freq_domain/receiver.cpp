#include "define.h"
#include "receiver.h"
#include "modem.h"
#include "fft.h"
#include "misc.h"
#include "LAB524Rand.h"
#include "fileIO.h"
#include "stbc.h"
#include "tbcc.h" 

#include <fstream>

extern struct TxAttribute txAttribute;

// "channelGain1" and "channelGain2" are for the 1st and 2nd Rx ANTs
// respectively. Sizes of them are both 2*Nfft for the case of 2 Tx antennas.
extern complex<double> *channelGain1;
extern complex<double> *channelGain2;

double mP = 1.0;

int *rx(complex<double> **z, double N0, int OFDMSymIdx, LAB524Rand *rng)
{
   /* Channel estimation */
   int signalLen = (int)real(z[0][0]);
   int numTxAnt = signalLen/(Nfft);

   complex<double> chEstimate1[Nfft*numTxAnt +1];
   complex<double> chEstimate2[Nfft*numTxAnt +1];
   chEstimate1[0] = Nfft*numTxAnt; chEstimate2[0] = Nfft*numTxAnt;
   
   // Demodulation and channel estimation parameters.
   // Assuming the modulation format is unchanged for one whole OFDM symbol
   // except for the RSs.
   int modFormat = txAttribute.modFormat;
   int bitPerSym = txAttribute.bitPerSym;

#ifdef THEORETICAL_BER
   // Perfect channel estimation 
   for (int iTxAnt=1;iTxAnt<=numTxAnt;iTxAnt++)
   {
      int offset = Nfft*(iTxAnt-1);
      
      for (int i=1;i<=NUM_USEDCAR;i++)
      {
         chEstimate1[i+offset] = channelGain1[i+offset];
         chEstimate2[i+offset] = channelGain2[i+offset];
      }
   }


/*   
   ofstream data;
   data.open("channel1.txt",ios::out);

   for (int i=1;i<=Nfft;i++)
   {
//      cout << i << " " << abs(channelGain1[i])*abs(channelGain1[i]) << endl;
      data << abs(channelGain1[i])*abs(channelGain1[i]) << endl;
   }
   ofstream data2;
   data2.open("channel2.txt",ios::out);

   for (int i=1;i<=Nfft;i++)
   {
//      cout << i << " " << abs(channelGain2[i])*abs(channelGain2[i]) << endl;
     data2 << abs(channelGain2[i])*abs(channelGain2[i]) << endl;
   }
   
   cout << endl;
   cout << "Channel gain printed. Press enter."<< endl;
   getchar();
*/

#endif

#ifndef THEORETICAL_BER
   // Perform 2D channel estimation across both the frequency and time
   // (1 subframe)
#endif

   /* Simulate receiver noise. 2X2 MIMO. AWGN. */
   // The 1st and 2nd rows are for the 1st and 2nd rx ANTs respectively. The 1st
   // and 2nd half of each row are for the 1st and 2nd tx ANTs respectively.
   static double sd;
   static double _N0 = -1.0;
   if (_N0 != N0) { sd = sqrt(N0/2.0); _N0 = N0; }
//   if (_N0 != N0) { sd = sqrt(N0/4.0); _N0 = N0; }

//sd = 0;

   for (int i=1;i<=NUM_USEDCAR;i++)
   {
      // 1st Rx antenna; add AWGN.
      complex<double> n = (complex<double>)rng->getCmplxGaussian() * sd;
      z[0][i] += n;

      // 2nd Rx antenna; add AWGN.
      n = (complex<double>)rng->getCmplxGaussian() * sd;
      z[1][i] += n;
      
      // For numTxAnt > 1, combine signals. 
      if (numTxAnt > 1)
      {
         for (int iTxAnt=2;iTxAnt<=numTxAnt;iTxAnt++)
         {
            int offset = Nfft*(iTxAnt-1);
         
            z[0][i] += z[0][i+offset];
            z[1][i] += z[1][i+offset];
         }
      }
   }
   z[0][0] = Nfft; z[1][0] = Nfft;

   /* De SFBC */ 
#ifdef SFBC
   STBC Alamouti;
   complex<double> *x1 = Alamouti.AlamoutiDecode(z[0],chEstimate1);
   complex<double> *x2 = Alamouti.AlamoutiDecode(z[1],chEstimate2);

   // Modification to the N0 due to SFBC operation.
   N0 *= 12*mP*mP;
#else
   // For single tx ANT.
   complex<double> *x1 = new complex<double> [Nfft+1]; x1[0] = Nfft;
   complex<double> *x2 = new complex<double> [Nfft+1]; x2[0] = Nfft;
   for (int i=1;i<=Nfft;i++) { x1[i] = z[0][i]; x2[i] = z[1][i]; }
#endif

   // MRC; note that we reuse the array x1[].
   for (int i=1;i<=NUM_USEDCAR;i++)
   { x1[i] = conj(chEstimate1[i])*x1[i] + conj(chEstimate2[i])*x2[i]; }
   
   // Remake the channel gain due to MRC. Reuse chEstimate1[].
   for (int i=1;i<=NUM_USEDCAR;i++)
   {
      chEstimate1[i] =  chEstimate1[i]*conj(chEstimate1[i])
                      + chEstimate2[i]*conj(chEstimate2[i]);
   }
   
   // Modification to the N0 due to MRC.
   N0 *= 2*mP;

   /* Beginning of a new subframe. Perform some initializations. */
   static int nBitRx_c = 0;             // Number of bits (OFDM) modulated.
   static double *LLR = NULL;           // Stores the LLR of coded bits.

   if (OFDMSymIdx == 0)
   {
      // Allocate memory for LLR.
      // Deal with rate matching. Case 1, code extension.
      if (txAttribute.codedBitLen_c_trun > txAttribute.codedBitLen_c)
      {
         LLR = new double [txAttribute.codedBitLen_c_trun +1];
         LLR[0] = txAttribute.codedBitLen_c_trun;
      }
      // Case 2, truncation or codeword length = matching size. Still allocate
      // size codedBitLen_c for LLR for better compatibility with the decoder.
      else
      {
         LLR = new double [txAttribute.codedBitLen_c +1];
         LLR[0] = txAttribute.codedBitLen_c;
      }
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
      
   delete [] x1; delete [] x2;
   
   int *y = NULL;

   // If this is the last subframe, decode the data.
   if (OFDMSymIdx == (N_SYM-1))
   {
      // Deal with rate matching.
      // Case 1, code extension
      double *_LLR;
      if (txAttribute.codedBitLen_c_trun > txAttribute.codedBitLen_c)
      {
         _LLR = new double [txAttribute.codedBitLen_c +1]; _LLR[0] = txAttribute.codedBitLen_c;
         for (int i=1;i<=txAttribute.codedBitLen_c;i++) { _LLR[i] = 0; }
         
         int j = 1;
         for (int i=1;i<=txAttribute.codedBitLen_c_trun;i++)
         {
            _LLR[j] += LLR[i];
            
            j++;
            if (j > txAttribute.codedBitLen_c) { j = 1; }
         }

         delete LLR;
      }
      // Case 2, truncation
      else if (txAttribute.codedBitLen_c_trun < txAttribute.codedBitLen_c)
      {
         for (int i=txAttribute.codedBitLen_c_trun+1;i<=txAttribute.codedBitLen_c;i++) { LLR[i] = 0; }
         _LLR = LLR;
      }
      // Case 3, codeword length = matching size
      else { _LLR = LLR; }

      // Deinterleaving
      int FL = (int)(txAttribute.codedBitLen_c*txAttribute.tbcc.getCodeRate() + 0.5);
      
      deinterleave(_LLR+1        ,txAttribute.permutation,FL);
      deinterleave(_LLR+FL+1     ,txAttribute.permutation,FL);
      deinterleave(_LLR+(FL<<1)+1,txAttribute.permutation,FL);

      // Decoding
      y = txAttribute.tbcc.viterbiDecode(_LLR);

      delete [] _LLR;
   }

   return y;
}
