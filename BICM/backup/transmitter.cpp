#include "define.h"
#include "transmitter.h"
#include "LAB524Rand.h"
#include "cc.h"
#include "modem.h"

#include <iostream>

using namespace std;

int *generateDataBits(int FL, LAB524Rand* rng)
{
   int *u = new int [FL +1];
   u[0] = FL;
   
   // Generate random bits
   for (int i=1;i<=FL;i++) { u[i] = rng->getRandBinary(); }
   
   return u;
}

// Declare as global such that rx can find them.
CC *cc = NULL;
PskModulator *bpsk = NULL;
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

complex<float> *tx(int *u, float Eb, int modFormat, int bitsPerSym)
{
   // Encoding
   if (cc==NULL) { cc = new CC(u[0]); }
   int *v = cc->encode(u);
   const int codedBitsLen = v[0];
   Eb *= cc->getCodeRate();

   // Random interleaving
   if (interleavingOrder == 0) { interleavingOrder = new int [codedBitsLen]; }
   randPermute(interleavingOrder, codedBitsLen);
   interleave(v,interleavingOrder);

   // Modulate
   complex<float> *x;
   if (modFormat == PSK)
   {
      // There is special speed-up for BPSK modulation
      if (bitsPerSym == 1)
      {
         if (bpsk==NULL) { bpsk = new PskModulator(Eb,1); }
         x = bpsk->bpskMod(v);
      }
      else
      {
         if (psk==NULL) { psk = new PskModulator(Eb,bitsPerSym); }  
         x = psk->pskMod(v);
      }
      
   }   
   else if (modFormat == QAM)
   {
      if (qam==NULL) { qam = new QamModulator(Eb,bitsPerSym); }
      x = qam->qamMod(v);
   }

   delete [] v;
   
   return x;
}
