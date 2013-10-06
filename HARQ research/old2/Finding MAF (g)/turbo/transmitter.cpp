#include "transmitter.h"

#include <cstdlib>
#include <cmath>
#include <iostream>
using namespace std;
#include "turbo.h"

int *generateDataBits(int FL)
{
   int *u = new int [FL +1];
   u[0] = FL;
   
   // Generate random bits
   for (int i=0;i<u[0];i++) { u[i +1] = rand()&1; }
   
   return u;
}

float *tx(int *u, float Eb)
{  
//   cout << "before encode"; getchar();
         
   // Encoding
   int *v = turboEncode(u);
   int codedBitsLen = v[0];
//   cout << "after encode"; getchar();
         
   // BPSK modulation
   float Es = Eb;
   int symbolLen = codedBitsLen;
   
   // Take squre root
   if (Es != 1.0) { Es = sqrt(Es); } 

   float *x = new float [symbolLen+1];
   
   // Mapping
   x[0] = symbolLen;
   for (int i=0;i<symbolLen;i++)
   {
      x[i+1] = 2.0*Es*v[i+1] - Es;
   }
   
   delete [] v;
   return x;
}
