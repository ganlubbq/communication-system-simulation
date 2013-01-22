#include "transmitter.h"

#include "LAB524Rand.h"

#include "cc.h"

int *generateDataBits(int FL, LAB524Rand* rng)
{
   int *u = new int [FL +1];
   u[0] = FL;
   
   // Generate random bits
   for (int i=1;i<=FL;i++) { u[i] = rng->getRandBinary(); }
   
   return u;
}

float *tx(int *u, float Eb)
{
   // Encoding
   int *v = ccEncode(u);
   const int codedBitsLen = v[0];
   
   // BPSK modulation
   float Es = Eb;
   const int symbolLen = codedBitsLen;
   
   // Take squre root
   if (Es != 1.0) { Es = sqrt(Es); } 

   float *x = new float [symbolLen+1];
   
   // Mapping
   x[0] = symbolLen;
   for (int i=1;i<=symbolLen;i++)
   {
      x[i] = 2.0*Es*v[i] - Es;
   }
   
   delete [] v;
   return x;
}
