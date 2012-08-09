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
   for (int i=1;i<=FL;i++) { u[i] =0;}// = rng->getRandBinary(); }
   
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

complex<float> *tx(int *u, float Eb, int modFormat, int modOrder)
{

   // Modulate
   complex<float> *x = new complex<float> [u[0]/4 +1];
   x[0] = u[0]/4;

   int bitIdx = 1;

   if (modFormat == QAM)
   {                 
      if (qam==NULL) { qam = new QamModulator(modOrder); }
      
      for (int i=1;i<=(int)real(x[0]);i++)
      {
          x[i] = qam->one_qamMod(Eb*4, u+bitIdx);

          bitIdx += 4;
      }      
   }

   return x;
}
