#include "transmitter.h"

#include "LAB524Rand.h"

#include "cc.h"

int *generateDataBits(int FL, LAB524Rand* rng)
{
   int *u = new int [FL +1];
   u[0] = FL;
   
   // Generate random bits
   for (int i=0;i<u[0];i++) { u[i +1] = rng->getRandBinary(); }
   
   return u;
}

double *tx(int *u, double Eb)
{  
//   cout << "before encode"; getchar();
         
   // Encoding
   int *v = ccEncode(u);
   int codedBitsLen = v[0];
//   cout << "after encode"; getchar();
         
   // BPSK modulation
   double Es = Eb;
   int symbolLen = codedBitsLen;
   
   // Take squre root
   if (Es != 1.0) { Es = sqrt(Es); } 

   double *x = new double [symbolLen+1];
   
   // Mapping
   x[0] = symbolLen;
   for (int i=0;i<symbolLen;i++)
   {
      x[i+1] = 2.0*Es*v[i+1] - Es;
   }
   
   delete [] v;
   return x;
}
