#include "define.h"
#include "transmitter.h"
#include "LAB524Rand.h"
#include "modem.h"
#include "fft.h"
#include "turbo.h"
#include "misc.h"
#include "fileIO.h"
#include "stbc.h"

// "pwrBoost" applies to all the RSs within a subframe.
double pwrBoost = 1.0;

// Declare as global such that rx can find them.
int modFormat;
PskModulator *psk = new PskModulator();
QamModulator *qam = new QamModulator();

int *interleavingOrder = 0;

// For use in rx(), lame.
// Since the rx() only gets the what's after rate matching, there is no way for 
// the rx() to know the original codedBitsLen. 
int codedBitsLen;

int *generateDataBits(int FL, LAB524Rand* rng)
{
   int *u = new int [FL +1];
   u[0] = FL;
   
   for (int i=1;i<=FL;i++) { u[i] = rng->getRandBinary(); }
   
   return u;
}

complex<double> *tx(int *u, double E_tot, int OFDMSymIdx, LAB524Rand* rng)
{
   // Read and determine the modulation format
   modFormat = QAM;
   int bitsPerSym = 4;
   
   static int codedBitTxIdx = 0;
   static int *v = NULL;

   // Beginning of a new subframe
   if (OFDMSymIdx == 0)
   {
      // Calcuate the available bits after adding RS & control
      // Assuming 2 Tx antennas and "NUM_PDCCH_SYM" <= 4 (which is the case in 
      // Rel-8). See p. 201 in "LTE for 4G mobile broadband".
      int codedBitsLen_matched;
      if (NUM_PDCCH_SYM == 0)
      { codedBitsLen_matched = (NUM_USEDCAR*N_SYM - NUM_RB*NUM_RS_PER_RB)
                                *bitsPerSym; }
      else 
      { codedBitsLen_matched = (NUM_USEDCAR*(N_SYM-NUM_PDCCH_SYM) - NUM_RB
                                *(NUM_RS_PER_RB-4))*bitsPerSym; }

      /* Generate a block of data for this subframe? */

      /* Turbo encoding */
      if (v != NULL) { delete [] v; }
      v = turboEncode(u);
      codedBitsLen = v[0];
            
      // Rate matching based on "codedBitsLen_matched"
      v[0] = codedBitsLen_matched;

// For debug purpose
static int init = 0;
if (init == 0)
{
   cout << "FL = " << u[0] << endl;
   cout << "codedBitsLen = " << codedBitsLen << endl;
   cout << "codedBitsLen_matched = " << codedBitsLen_matched << endl;
   cout << "Punctured_len = " << codedBitsLen-codedBitsLen_matched << endl;
   init = 1;
}
      // Debug
      if ((codedBitsLen < codedBitsLen_matched) || (codedBitsLen_matched < u[0]))
      { cout << "Error in codedBitsLen" << endl; getchar(); }

      // Interleaving
      if (interleavingOrder != 0) { delete [] interleavingOrder; }
      interleavingOrder = new int [v[0]];
      randPermute(interleavingOrder, v[0]);
      interleave(v,interleavingOrder);

      codedBitTxIdx = 0;
   }
   
   // Calculate symbol energy for modulation purpose
   double E_car_data = N_SYM*E_tot/(NUM_RB*NUM_RS_PER_RB*(pwrBoost-1.0)+NUM_USEDCAR*N_SYM);
   double E_car_RS = E_car_data*pwrBoost;

   // Map the pilot symbols
   static int pilotBit[2] = {1,1};
   complex<double> pilotSym = psk->one_pskMod(2,E_car_RS,pilotBit);

   // OFDM RE mapping
   complex<double> *x = new complex<double> [Nfft +1];
   x[0] = Nfft;
   for (int i=1;i<=Nfft;i++) { x[i] = 0; }

   /* RS mapping. */
   // See p. 201 in "LTE for 4G mobile broadband"
   if (OFDMSymIdx==0 || OFDMSymIdx==7)
   {
      for (int iRB=0;iRB<NUM_RB;iRB++)
      {
         // R0
         x[iRB*N_SC+ 1] = pilotSym; x[iRB*N_SC+ 7] = pilotSym;
         // R1
         x[iRB*N_SC+ 4] = pilotSym; x[iRB*N_SC+ 10] = pilotSym;
      }
   }
   else if (OFDMSymIdx==4 || OFDMSymIdx==11)
   {
      for (int iRB=0;iRB<NUM_RB;iRB++)
      {         
         // R0
         x[iRB*N_SC+ 4] = pilotSym; x[iRB*N_SC+ 10] = pilotSym;
         // R1
         x[iRB*N_SC+ 1] = pilotSym; x[iRB*N_SC+ 7] = pilotSym;         
      }
   }
   
   /* Rel-8 PDCCH mapping. */
   // Stuff some random bits for now.
   if (OFDMSymIdx < NUM_PDCCH_SYM)
   {
      for (int iRB=0;iRB<NUM_RB;iRB++)
      {
         for (int iCar_RB=1;iCar_RB<=N_SC;iCar_RB++)
         {
            int iCar = iRB*N_SC + iCar_RB;

            // These are the RSs. Note that OFDMSymIdx must <= 3.
            // Keep the if conditions for consistency though.
            if ((OFDMSymIdx==0 || OFDMSymIdx==4 || OFDMSymIdx==7 || OFDMSymIdx==11)
                && (iCar_RB==1 || iCar_RB==4 || iCar_RB==7 || iCar_RB==10))
            {}
            // These are PDCCH symbols sent by eNB.
            else
            {
               int *randBits = generateDataBits(bitsPerSym, rng);
            
               if (modFormat == PSK)
               { x[iCar] = psk->one_pskMod(bitsPerSym,E_car_data,randBits+1); }
               else if (modFormat == QAM)
               { x[iCar] = qam->one_qamMod(bitsPerSym,E_car_data,randBits+1); }
               
               delete [] randBits; 
            }  
         }
      }
   }
   
   /* Data mapping. */
   else
   { 
      for (int iRB=0;iRB<NUM_RB;iRB++)
      {
         for (int iCar_RB=1;iCar_RB<=N_SC;iCar_RB++)
         {
            int iCar = iRB*N_SC+iCar_RB;

            // These are the RSs.
            if ((OFDMSymIdx==0 || OFDMSymIdx==4 || OFDMSymIdx==7 || OFDMSymIdx==11)
                && (iCar_RB==1 || iCar_RB==4 || iCar_RB==7 || iCar_RB==10))
            {}
            else
            {
               if (modFormat == PSK)
               { x[iCar] = psk->one_pskMod(bitsPerSym,E_car_data,v+codedBitTxIdx+1); }
               else if (modFormat == QAM)
               { x[iCar] = qam->one_qamMod(bitsPerSym,E_car_data,v+codedBitTxIdx+1); }
  
               codedBitTxIdx += bitsPerSym;
            }  
         }
      }
   }
   
   /* SFBC */
   // Though this is STBC, we use it as SFBC here.
   STBC Alamouti;
   complex<double> *z = Alamouti.AlamoutiEncode(x);
   delete [] x;
   
   return z;
}
