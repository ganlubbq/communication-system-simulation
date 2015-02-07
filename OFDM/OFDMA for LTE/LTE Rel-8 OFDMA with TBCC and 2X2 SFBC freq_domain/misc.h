#ifndef MISC_H
#define MISC_H

#include "define.h"

#include <cstdlib>

const double sqrt_1_2 = 0.707106781186548;
const double pi = 3.141592653589793;

inline double logSum(double a,double b)
{
   double max = (a>b) ? a : b; 
   double delta = (a>b) ? (a-b) : (b-a);
    
   if (delta>100) { delta = 100; }
   delta = log(1+exp(-delta));
    
   return max+delta;

//   return log(exp(a)+exp(b));
}


inline int RE_mapping(int OFDMSymIdx, int iRB, int iCar_RB)
{
   static int iCarLimit = codedBitLenM/2/freqDiv;

   // RE for reference signals.
   // See p. 201 in "LTE for 4G mobile broadband"/
   if ((OFDMSymIdx==0 || OFDMSymIdx==4 || OFDMSymIdx==7  || OFDMSymIdx==11) &&
       (iCar_RB==0 || iCar_RB==3 ||iCar_RB==6 ||iCar_RB==9)) { return RS; }

   // RE for Rel-8 PDCCH.
   else if (OFDMSymIdx < NUM_PDCCH_SYM) { return PDCCH; }

   // RE for RPDCCH.
   else if ((OFDMSymIdx>NUM_PDCCH_SYM) && (OFDMSymIdx!=(N_SYM-1)) && (iCar_RB<iCarLimit))
   {
      extern int *RBIdx;
      
      for (int i=0;i<freqDiv;i++)
      { if (iRB == RBIdx[i]) { return RPDCCH; } }
      
      return USER_DATA;
   }

   // The remaining are assumed to be user data.
   else { return USER_DATA; }
}

const int tbccColPermute[32] = {1,17,9,25,5,21,13,29,3,19,11,27,7,23,15,31,0,16,8,
                              24,4,20,12,28,2,18,10,26,6,22,14,30};

// Block interleaver for TBCC in LTE Rel-8.
inline void tbccInterleave(int *a,int len)
{
   int numDummyBit = 32 - (len&31);
   int numRow = (len + numDummyBit) >> 5;
   int interleaver[numRow][32];

   for (int jCol=0;jCol<32;jCol++)
   {
      if (jCol<numDummyBit) { interleaver[0][jCol] = -1; }
      else { interleaver[0][jCol] = jCol-numDummyBit; }
   }
   if (numRow>1)
   {
      for (int iRow=1;iRow<numRow;iRow++)
      {
         for (int jCol=0;jCol<32;jCol++)
         {            
            interleaver[iRow][jCol] = (iRow<<5) + jCol - numDummyBit;
         }
      }
   }

   int bitIdx = 0;   
   for (int i=0;i<32;i++)
   {
      int colIdx = tbccColPermute[i];
      
      for (int iRow=0;iRow<numRow;iRow++)
      {
         if (interleaver[iRow][colIdx] == -1) {}
         else
         {
            a[bitIdx] = interleaver[iRow][colIdx];
            bitIdx++;
         }
      }
   }
/*
   for (int i=0;i<len;i++)
   { cout << a[i] << " "; }
   cout << endl; getchar();
*/
}


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


inline void interleave(int *x, int *interleavingOrder, int k)
{
   int temp[k];
     
   for (int i=0;i<k;i++) { temp[i] = x[interleavingOrder[i]]; }
     
   for (int i=0;i<k;i++) { x[i] = temp[i]; }  
}

inline void deinterleave(double *x1,int *interleavingOrder, int k)
{
   double temp1[k];
      
   for (int i=0;i<k;i++) { temp1[interleavingOrder[i]] = x1[i]; }
     
   for (int i=0;i<k;i++) { x1[i] = temp1[i]; }
}

#endif
