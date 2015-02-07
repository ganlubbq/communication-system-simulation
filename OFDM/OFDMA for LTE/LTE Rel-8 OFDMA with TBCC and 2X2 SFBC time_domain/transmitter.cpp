#include "define.h"
#include "transmitter.h"
#include "LAB524Rand.h"
#include "modem.h"
#include "fft.h"
#include "misc.h"
#include "fileIO.h"
#include "stbc.h"
#include "tbcc.h"

struct TxAttribute txAttribute;

const int codedBitLenM = (int)getValue("Matched coded bits length", "config.txt");
   
int *generateDataBits(int FL, LAB524Rand* rng)
{
   int *u = new int [FL +1]; u[0] = FL;
   
   for (int i=1;i<=FL;i++) { u[i] = rng->getRandBinary(); }
   
   return u;
}

inline complex<double> giveRandSym(double E_sym, LAB524Rand* rng)
{
   static double sd;
   static double _E = -1.0;
   if (_E != E_sym) { sd = sqrt(E_sym/2.0); _E = E_sym; }

   return ((complex<double>)rng->getCmplxGaussian() * sd);
}

complex<double> *tx(int *rpdcch, double E_b, int OFDMSymIdx, LAB524Rand* rng)
{
   // Read and determine the modulation format.
   int modFormat = PSK;
   int bitPerSym = 2;
   txAttribute.modFormat = modFormat;
   txAttribute.bitPerSym = bitPerSym;
   
   static int nBitTx_c = 0;        // Number of bits (OFDM) modulated.
   static int *v_c = NULL;         // Storing the coded bits.

   /* Beginning of a new subframe. Prepare new blocks of data and encode. */
   if (OFDMSymIdx == 0)
   {
      // Calcuate the available bits for RPDCCH transmission taking
      // NUM_PDCCH_SYM, switch time, and RS into account. Assuming 2 Tx antennas
      // and "NUM_PDCCH_SYM" <= 4 (which is the case in Rel-8). See p. 201 in
      // "LTE for 4G mobile broadband".
      int capacity;
      if (NUM_PDCCH_SYM == 4)
      { capacity = ((N_SYM - 6)*N_SC - 8)*bitPerSym; }
      else
      { capacity = ((N_SYM - (NUM_PDCCH_SYM+2))*N_SC - 12)*bitPerSym; }
      
      // TBCC encoding for RPDCCH
      int *_v_c = txAttribute.tbcc.encode(rpdcch);
      txAttribute.codedBitLen_c = _v_c[0];

      // Block interleaving.
      int FL = rpdcch[0];
      static int _previousFL = 0;
      
      if (_previousFL == 0)
      {
         // Initialize interleaver here.
         txAttribute.permutation = new int [FL];
         tbccInterleave(txAttribute.permutation, FL);
         _previousFL = FL;
      }
      else if (_previousFL != FL)
      {
         // FL changed. Reset interleaver.
         delete [] txAttribute.permutation;
         txAttribute.permutation = new int [FL];
         tbccInterleave(txAttribute.permutation, FL);
         _previousFL = FL;
      }
      
      interleave(_v_c+1        ,txAttribute.permutation,FL);
      interleave(_v_c+FL+1     ,txAttribute.permutation,FL);
      interleave(_v_c+(FL<<1)+1,txAttribute.permutation,FL);

      // Rate matching (RM) based on the available capacity.
      txAttribute.codedBitLen_c_trun = codedBitLenM;

      if (v_c != NULL) { delete [] v_c; }

      // Case 1, code extension
      if (txAttribute.codedBitLen_c_trun > txAttribute.codedBitLen_c)
      {
         v_c = new int [txAttribute.codedBitLen_c_trun +1];
         v_c[0] = txAttribute.codedBitLen_c_trun;
       
         int j = 1;
         for (int i=1;i<=txAttribute.codedBitLen_c_trun;i++)
         {
            v_c[i] = _v_c[j];
            
            j++;
            if (j > txAttribute.codedBitLen_c) { j = 1; }
         }
         
         delete [] _v_c;
      }
      // Case 2, truncation or codeword length = matching size
      else { v_c = _v_c; }
      
// Debug
      static int init2 = 0;
      if (init2 == 0)
      {
         cout << "RPDCCH FL = " << rpdcch[0] << endl;
         cout << "codedBitLen = " << txAttribute.codedBitLen_c << endl;
         cout << "codedBitLen_truncated = " << txAttribute.codedBitLen_c_trun << endl;
         cout << "capacity = " << capacity << endl;
         init2 = 1;
      }

// Debug
      if (capacity < rpdcch[0])
      { cout << "FL too long" << endl; getchar(); }

      // Reset some counters.
      nBitTx_c = 0;
   }
   
   // Calculate symbol energy for modulation purpose.
   // Ratio of power of RS RE to data RE. Applies to all RSs within a subframe.
   double pwrBoost = 1.0;
   double ECR = (double)rpdcch[0]/txAttribute.codedBitLen_c_trun;
   
//   txAttribute.E_car_data = E_b*bitPerSym*ECR;
//   txAttribute.E_car_data = E_b*bitPerSym;
   txAttribute.E_car_data = E_b; 
   
   txAttribute.E_car_RS = txAttribute.E_car_data*pwrBoost;

   // Map the pilot symbols.
   static int pilotBit[2] = {1,1};
   complex<double> pilotSym = txAttribute.psk.one_pskMod(2,txAttribute.E_car_RS,pilotBit);
   
   // x[] stores the OFDM symbols.
   complex<double> x[Nfft +1]; x[0] = Nfft;
   for (int i=1;i<=Nfft;i++) { x[i] = 0.0; }

   /* OFDM RE mapping */
   for (int iRB=0;iRB<NUM_RB;iRB++)
   {
      for (int iCar_RB=0;iCar_RB<N_SC;iCar_RB++)
      {
         int iCar = iRB*N_SC+iCar_RB;
         
         int RE_type = RE_mapping(OFDMSymIdx,iRB,iCar_RB);

         // Referece signals mapping.
         if (RE_type == RS) { x[iCar+1] = pilotSym; }
         
         // RPDCCH mapping. Note that a total of "NUM_PDCCH_SYM+2" OFDM symbols 
         // can't be used for RPDCCH transmissions.   
         else if (RE_type == RPDCCH)
         {
            // Check if the data has been consumed in the case of
            // codedBitLen_c < capacity
            if (nBitTx_c < txAttribute.codedBitLen_c_trun)
            {
               if (modFormat == PSK)
               { x[iCar+1] = txAttribute.psk.one_pskMod(bitPerSym,txAttribute.E_car_data,v_c+nBitTx_c+1); }
               else if (modFormat == QAM)
               { x[iCar+1] = txAttribute.qam.one_qamMod(bitPerSym,txAttribute.E_car_data,v_c+nBitTx_c+1); }
               
               nBitTx_c += bitPerSym;
            }
            else // Stuff some random data
            { x[iCar+1] = giveRandSym(txAttribute.E_car_data, rng); }
         }
         
         // Rel-8 PDCCH and user data mapping. Stuff some random data for now.
         else if (RE_type == PDCCH || RE_type == USER_DATA)
         { x[iCar+1] = giveRandSym(txAttribute.E_car_data, rng); }
      }
   }

   /* SFBC */
/*
   // Though this is STBC, we use it as SFBC here.
   STBC Alamouti;
   complex<double> *z = Alamouti.AlamoutiEncode(x);
*/

   // For single tx ANT.
   complex<double> *z = new complex<double> [Nfft+1]; z[0] = Nfft;
   for (int i=1;i<=Nfft;i++) { z[i] = x[i]; }

   /* IFFT; convert to time domain. */
   int signalLen = (int)real(z[0]);
   int numTxAnt = signalLen/Nfft;
      
   complex<double> *r = new complex<double> [(Nfft+CPLen)*numTxAnt+1];
   r[0] = (Nfft+CPLen)*numTxAnt;
   for (int iTxAnt=1;iTxAnt<=numTxAnt;iTxAnt++)
   {
      int offset1 = (Nfft+CPLen)*(iTxAnt-1), offset2 = Nfft*(iTxAnt-1);
      
      ifft(z+offset2,r+offset1,Nfft,CPLen);

      // Add CP.
      for (int i=1;i<=CPLen;i++) { r[i+offset1] = r[i+Nfft+offset1]; }
   }
   
   delete [] z;
 
   return r;
}
