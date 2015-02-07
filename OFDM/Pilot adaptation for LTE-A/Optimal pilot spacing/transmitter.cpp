#include "define.h"
#include "transmitter.h"
#include "LAB524Rand.h"
#include "modem.h"
#include "fft.h"
#include "turbo.h"

int *generateDataBits(int FL, LAB524Rand* rng)
{
   int *u = new int [FL +1];
   u[0] = FL;
   
   // Generate random bits
   for (int i=1;i<=FL;i++) { u[i] = rng->getRandBinary(); }
//   for (int i=1;i<=FL;i++) { u[i] = 0; }
   
   return u;
}

// Declare as global such that rx can find them.
//PskModulator *bpsk = NULL;
PskModulator *psk_pilot = NULL;
PskModulator *psk = NULL;
QamModulator *qam = NULL;
int *interleavingOrder = 0;

inline void randPermute(int *a,int len)
{
   int temp[len];
     
   for (int i=0;i<len;i++) { temp[i] = i; }
     
   for (int i=0;i<len;i++)
   {
      int x = (rand()%(len-i));
         
      a[i] = temp[x];
         
      for (int j=x;j<len-1-i;j++)
      {
         temp[j] = temp[j+1];
      }
   }
}

inline void interleave(int *x, int *interleavingOrder)
{
   int k = x[0];
   int temp[k +1];
     
   for (int i=1;i<=k;i++) { temp[i] = x[interleavingOrder[i-1] +1]; }
     
   for (int i=1;i<=k;i++) { x[i] = temp[i]; }  
}

// For use in rx(), lame
double E_subcar_pilot;
int codedBitsLen;

// This for the multipath() in channel.cpp, lame
int numOFDMSymWithoutPilot;

// For use in main(), lame
double codeRate;

complex<double> *tx(int *u, double E_tot, int modFormat, int bitsPerSym, double pwrBoost, double *totalNumSubcarrierUsed)
{
   // Encode u into v
   int *v = turboEncode(u);
   codedBitsLen = v[0];
   codeRate = (double)u[0]/v[0];

   // Random interleaving
   if (interleavingOrder == 0) { interleavingOrder = new int [codedBitsLen]; }
   randPermute(interleavingOrder, codedBitsLen);
   interleave(v,interleavingOrder);

   // Compute variables needed for OFDM modulation
   extern int Nfft;
   extern int pilotSpacing;
   int numPilot = (pilotSpacing==0) ? 0 : (int)((double)Nfft/pilotSpacing+0.9999999999);
   int numDataBitsPerOFDMSym = (Nfft-numPilot)*bitsPerSym;
   
   int FL = (int)((double)codedBitsLen/numDataBitsPerOFDMSym+0.9999999999)*numDataBitsPerOFDMSym;
   int paddingLen = FL - codedBitsLen;
   int numOFDMSym = FL/numDataBitsPerOFDMSym;
   
   // This for the multipath() in channel.cpp, lame
   numOFDMSymWithoutPilot = ((int)((double)codedBitsLen/Nfft/bitsPerSym+0.9999999999)*numDataBitsPerOFDMSym)/numDataBitsPerOFDMSym;

   
   double E_subcar_data = E_tot/(numPilot*(pwrBoost-1.0)+Nfft);
   E_subcar_pilot = E_subcar_data*pwrBoost;
   
   // This if for the throughput calculation in main(), lame.
   *totalNumSubcarrierUsed += (double)numOFDMSym*Nfft - (double)paddingLen/bitsPerSym;
   
   // Initialize modulator
   static int hasInit = 0;
   if (hasInit == 0)
   {
      // Use QPSK for pilot
      psk_pilot = new PskModulator(2);  
      if (modFormat==PSK && psk==NULL) { psk = new PskModulator(bitsPerSym); }
      else if (modFormat==QAM && qam==NULL) { qam = new QamModulator(bitsPerSym); }

   cout << "Eb = " << scientific << E_subcar_data/((double)bitsPerSym*codeRate) << endl;
   cout << "Total codeword length = "<< FL << endl;
   cout << "numPaddingBit = "<< paddingLen << endl;
   cout << "numPaddingSym = "<< paddingLen/bitsPerSym << endl;
   cout << "numOFDMSym = "<< numOFDMSym << endl << endl;
//   cout << "Press enter to continue" << endl;getchar();

      hasInit = 1;
   }
 
   // Prepare the pilot symbols
   static int pilotBit[2] = {1,1};
   complex<double> pilotSym = psk_pilot->one_pskMod(E_subcar_pilot,pilotBit);

   // Prepare the padding symbols
   int *paddingBits = new int [bitsPerSym];
   for (int i=0;i<bitsPerSym;i++) { paddingBits[i] = 0; }
   complex<double> paddingSym;
   if (modFormat == PSK) { paddingSym = psk->one_pskMod(E_subcar_data,paddingBits); }
   else if (modFormat == QAM) { paddingSym = qam->one_qamMod(E_subcar_data,paddingBits); }
  
   delete [] paddingBits;
   
   // Main modulation loop
   complex<double> *x = new complex<double> [Nfft*numOFDMSym +1];
   x[0] = Nfft*numOFDMSym;
   int bitIdx = 1;

   for (int iOFDMSym=0;iOFDMSym<numOFDMSym;iOFDMSym++)
   {   
      for (int iSubcar=1;iSubcar<=Nfft;iSubcar++)
      {            
         // Place to insert pilot
         if ((iSubcar-1)%pilotSpacing == 0) { x[iSubcar+iOFDMSym*Nfft] = pilotSym; }
         
         // Data carrier
         else
         {
            // Add padding bits
            if (bitIdx+bitsPerSym > codedBitsLen+1)
            {
               // Padding bits totally
               if (bitIdx > codedBitsLen) { x[iSubcar+iOFDMSym*Nfft] = paddingSym; }
               else
               {
                  // Partially padding for the last goddamn data symbol                   
                  int *_paddingBits = new int [bitsPerSym];
                  for (int i=0;i<bitsPerSym;i++) { _paddingBits[i] = 0; }
                  for (int i=0;i<=(codedBitsLen-bitIdx);i++) { _paddingBits[i] = v[bitIdx+i]; }
                  // Modulate
                  if (modFormat == PSK) { x[iSubcar+iOFDMSym*Nfft] = psk->one_pskMod(E_subcar_data,_paddingBits); }
                  else if (modFormat == QAM) { x[iSubcar+iOFDMSym*Nfft] = qam->one_qamMod(E_subcar_data,_paddingBits); }
  
                  delete [] paddingBits;
               }
            }
            else
            {
               if (modFormat == PSK)
               { x[iSubcar+iOFDMSym*Nfft] = psk->one_pskMod(E_subcar_data,v+bitIdx); }
               else if (modFormat == QAM)
               { x[iSubcar+iOFDMSym*Nfft] = qam->one_qamMod(E_subcar_data,v+bitIdx); }
            }
            
            bitIdx += bitsPerSym;
         }
      }
   }
   
   delete [] v;
   
/*
   // IFFT; convert to time domain
   extern int Nfft;
   extern int CPLen;
   complex<double> *r = new complex<double> [Nfft+CPLen+1];
   r[0] = Nfft+CPLen;

   ifft(x,r,Nfft,CPLen);
   delete [] x;
   return r;
*/
   return x;
}
