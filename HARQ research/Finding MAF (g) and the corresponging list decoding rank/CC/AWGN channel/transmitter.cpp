#include "transmitter.h"
#include "LAB524Rand.h"
#include "cc.h"
#include "modulator.h"

int *generateDataBits(int FL, LAB524Rand* rng)
{
   int *u = new int [FL +1];
   u[0] = FL;
   
   // Generate random bits
   for (int i=0;i<u[0];i++) { u[i +1] = rng->getRandBinary(); }
   
   return u;
}

// Such that rx can find us.
CC *cc = NULL;
MODULATOR *bpsk = NULL;

double *tx(int *u, double Eb)
{  
   // Encoding
   if (cc==NULL) { cc = new CC(); }
   int *v = cc->encode(u);
   const int codedBitsLen = v[0];
   Eb *= cc->getCodeRate();
   
   // Modulation
   if (bpsk==NULL) { bpsk = new MODULATOR(Eb,1); }
   
   double *x = bpsk->bpskMod(v);
   delete [] v;
   
   return x;
}
