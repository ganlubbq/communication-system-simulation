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

int *generateDataBits(int FL, LAB524Rand* rng)
{
   int *u = new int [FL +1]; u[0] = FL;
   
   for (int i=1;i<=FL;i++) { u[i] = rng->getRandBinary(); }
   
   return u;
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
      if (v_c != NULL) { delete [] v_c; }
      v_c = txAttribute.tbcc.encode(rpdcch);
      txAttribute.codedBitLen_c = v_c[0];

      // Block interleaving.
      int FL = v_c[0]/3;
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
      
      interleave(v_c+1,txAttribute.permutation,FL);
      interleave(v_c+FL+1,txAttribute.permutation,FL);
      interleave(v_c+(FL<<1)+1,txAttribute.permutation,FL);

      // Rate matching (RM) based on the available capacity.
      // There are 2 cases where truncation on the codeword is needed.
      // Case 1, no need to match rate; check if truncation is needed for mod.
      txAttribute.codedBitLen_c_trun =  72;
/*
      if (txAttribute.codedBitLen_c < capacity)
      { txAttribute.codedBitLen_c_trun =  txAttribute.codedBitLen_c
                                        - txAttribute.codedBitLen_c%bitPerSym; }
      // Case 2, RM is needed or the length of coded bits = capacity.
      else { txAttribute.codedBitLen_c_trun = capacity; }
*/

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
   txAttribute.E_car_data = E_b*(bitPerSym/3.0);
   txAttribute.E_car_RS = txAttribute.E_car_data*pwrBoost;

   // Map the pilot symbols.
   static int pilotBit[2] = {1,1};
   complex<double> pilotSym = txAttribute.psk.one_pskMod(2,txAttribute.E_car_RS,pilotBit);
   
   // x[] stores the OFDM symbols.
   complex<double> *x = new complex<double> [Nfft +1];
   x[0] = Nfft;
   for (int i=1;i<=Nfft;i++) { x[i] = 0; }

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
         }
         
         // Rel-8 RPDCCH and user data mapping. Stuff some random data for now.
         else if (RE_type == PDCCH || RE_type == USER_DATA)
         {
            int *randBits = generateDataBits(bitPerSym, rng);

            if (modFormat == PSK)
            { x[iCar+1] = txAttribute.psk.one_pskMod(bitPerSym,txAttribute.E_car_data,randBits+1); }
            else if (modFormat == QAM)
            { x[iCar+1] = txAttribute.qam.one_qamMod(bitPerSym,txAttribute.E_car_data,randBits+1); }

            delete [] randBits;
         }
      }
   }
   
   
   /* SFBC */
/*
   // Though this is STBC, we use it as SFBC here.
   STBC Alamouti;
   complex<double> *z = Alamouti.AlamoutiEncode(x);
   delete [] x;
*/

   /* IFFT; convert to time domain. */
   int symLen = (int)real(x[0]);
   complex<double> *r;
   
   // 1 tx antenna only.
   if (symLen == Nfft)
   {
      // Perform IFFT
      r = new complex<double> [Nfft+CPLen+1];
      r[0] = Nfft+CPLen;
      ifft(x,r,Nfft,CPLen);
      
      // Add CP
      for (int i=1;i<=CPLen;i++) { r[i] = r[i+Nfft]; }
   }

   // 2 tx antennas.
   else if (symLen == (Nfft<<1))
   {
      // Perform IFFT
      r = new complex<double> [((Nfft+CPLen)<<1)+1];
      r[0] = (Nfft+CPLen)<<1;
      ifft(x,r,Nfft,CPLen);
// Caution!
      ifft(x+Nfft,r+Nfft+CPLen,Nfft,CPLen);
      
      // Add CP
      for (int i=1;i<=CPLen;i++) 
      { r[i] = r[i+Nfft]; r[i+Nfft+CPLen] = r[i+(Nfft<<1)+CPLen]; }      
   }
   
   delete [] x;
/*
   for (int i=1;i<=Nfft+CPLen;i++)
   {
       cout << i << " " << r[i] << endl;
   }getchar();
*/
   return r;
}
