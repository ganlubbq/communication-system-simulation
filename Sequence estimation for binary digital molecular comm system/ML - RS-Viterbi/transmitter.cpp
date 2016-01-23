#include "transmitter.h"
#include "LAB524Rand.h"
#include "cc.h"
#include "modulator.h"
#include "fileIO.h"

#include <cstdlib>

int *generateDataBits(int FL, LAB524Rand* rng)
{
   int *u = new int [FL +1];
   u[0] = FL;
   
   // Generate random bits
   for (int i=1;i<=FL;i++) { u[i] = rng->getRandBinary(); }

// Tail bits
int _m = 6;
//for (int i=FL-_m;i<=FL;i++) { u[i] = 0; }

   return u;
}

// Such that rx can find us.
CC *cc = NULL;
Modulator *bpsk = NULL;

// a0 a1 a2 ~ aN 
float a[] = {
 4.6974,
 2.3205,
 1.7859,
 1.5074,
 1.3289,
 1.2019,
 1.1056,
 1.0293,
 0.9669,
 0.9146,
 0.8700,
 0.8313,
 0.7974,
 0.7673,
 0.7404,
 0.7162,
 0.6941,
 0.6740,
 0.6556,
 0.6385,
 0.6228
};

int N = 20;//(int)getValue("Memory order", "config.txt");

float *tx(int *u, float Eb)
{
   static int hasInit = 0;
   if (hasInit == 0) 
   {
      for (int i=0;i<=N;i++) { a[i] /= 10; }
      hasInit = 1;
   }

   // Pre-append N random bits
   int FL = u[0];
   int _u[FL+N+1]; _u[0] = FL+N;
   
   for (int i=1;i<=N;i++) { _u[i] = rand()&1; }
   for (int i=N+1;i<=N+FL;i++) { _u[i] = u[i-N]; }
               
   // Return an array of observation variables {Z_i}
   float *x = new float [FL+1];
   x[0] = FL;
   
   for (int i=N+1;i<=N+FL;i++)
   {
      float Z = 0;
      for (int j=0;j<=N;j++)
      {
         Z += _u[i-j]*a[j];
      }
      
      x[i-N] = Z;
   }
   
   return x;
}
