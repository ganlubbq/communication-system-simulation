#include "transmitter.h"
#include "LAB524Rand.h"
#include "cc.h"
#include "modulator.h"

int *generateDataBits(int FL, LAB524Rand* rng)
{
   int *u = new int [FL +1];
   u[0] = FL;
   
   // Generate random bits
   for (int i=1;i<=FL;i++) { u[i] = 0; }//rng->getRandBinary(); }

   return u;
}

// Such that rx can find us.
CC *cc = NULL;
MODULATOR *bpsk = NULL;

float *tx(int *u, float Eb)
{
   // Encoding
   if (cc==NULL) { cc = new CC(); }
   int *v = cc->encode(u);
   const int codedBitsLen = v[0];
   Eb *= cc->getCodeRate();
   
   // Modulation
   if (bpsk==NULL) { bpsk = new MODULATOR(Eb,1); }
   
   float *x = bpsk->bpskMod(v);
   delete [] v;
   
   return x;
}
