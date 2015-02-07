#ifndef MISC_H
#define MISC_H

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

inline void deinterleave(double *x1,int *interleavingOrder)
{
   int k = (int)x1[0];
   double temp1[k +1];
      
   for (int i=1;i<=k;i++) { temp1[interleavingOrder[i-1]+1] = x1[i]; }
     
   for (int i=1;i<=k;i++) { x1[i] = temp1[i]; }
}

#endif
