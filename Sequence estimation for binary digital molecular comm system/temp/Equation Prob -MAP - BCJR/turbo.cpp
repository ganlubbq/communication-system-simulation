#include "turbo.h"
#include "fileIO.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>
 
#include <iostream>
using namespace std;

//#define BCJR 1
//#define APRIORI 1

const int turboIteration = (int)getValue("Iteration","config.txt");

const int m = 4;//(int)getValue("Memory order","config.txt");
const int st_num = (1<<m);

int next_table[st_num][2];                   //next states correspond to input 0, 1

int from_table[st_num][2];                   //previous states correspond to incoming 0, 1

long double lookup_table[100000]={0};              //the real delta = index/1000

const long double sd = getValue("Noise std", "config.txt");

const long double pi = 3.1415926;

#include "turboSub.h"


int *turboEncode(int *d)
{
}
 
extern const int N;
extern long double a[];

// output the log likelihood 
long double ll(long double Z, int state, int _m, int bit)
{
   static int hasInit = 0;
   static long double var = 0;
   static long double mean = 0;
   if (hasInit==0)
   {
      for (int i=_m+1;i<=N;i++) { mean += a[i]; var += a[i]*a[i]; }
      long double p1 = 0.5;
      mean *= p1;
      var *= (p1-p1*p1);
      var += sd*sd;
      hasInit = 1;
   }
   
   long double mean1 = mean, mean0;

   for (int j=1;j<=_m;j++)
   {
      mean1 += ((state>>(_m-j))&1)*a[j];
   }
   mean0 = mean1;
   mean1 += a[0];

   if (bit == 0 )
        { return exp((-(Z-mean0)*(Z-mean0))/2.0/var); }
   else { return exp((-(Z-mean1)*(Z-mean1))/2.0/var); }
}


// Must output the llr between bit 1 and 0
inline long double test(long double Z,int bit)
{  
   static int hasInit = 0;
   static long double var = 0;
   static long double mean = 0;
   if (hasInit==0)
   {
      for (int i=1;i<=N;i++) { mean += a[i]; var += a[i]*a[i]; }
      long double p1 = 0.5;
      mean *= p1;
      var *= (p1-p1*p1);
      var += sd*sd;      
      hasInit = 1;
   }

   if (bit == 0 )
        { return (-(Z-mean)*(Z-mean))/2.0/var; }
   else { return (-(Z-mean-a[0])*(Z-mean-a[0]))/2.0/var; }
}


void siso(long double *y,long double *aPriori0,long double *aPriori1,long double *extrinsic0,long double *extrinsic1,int k)
{
   long double (*A)[st_num] = new long double [k+1][st_num]; 
   long double (*B)[st_num] = new long double [k+1][st_num]; 
 
   for (int j=0;j<st_num;j++)
   {
      A[0][j] = 1.0/st_num;
      B[k][j] = 1.0/st_num;
   }

   //forward recursion A
   for (int j=0;j<k;j++)
   {
      long double sum = 0;
      
      for (int l=0;l<st_num;l++)
      {
         // Here term1, term2 are the WEIGHTs, larger the better
         int currentBit = (l>>(m-1))&1;        
         int lastState0 = from_table[l][0];
         int lastState1 = from_table[l][1];
         
         long double Pa; 
         if (currentBit == 0) { Pa = aPriori0[j]; }
         else { Pa = aPriori1[j]; }

         long double pathProb0 = ll(y[j], lastState0, m, currentBit);
         long double pathProb1 = ll(y[j], lastState1, m, currentBit);
         if (pathProb0 == 0 || pathProb1 == 0) { cout << "precision problem" << endl; }
         long double term1 = A[j][lastState0] * pathProb0 * Pa;
         long double term2 = A[j][lastState1] * pathProb1 * Pa;
         
         A[j+1][l] = term1 + term2;
         
         sum += A[j+1][l];
      }
      
      // Normalization
      for (int l=0;l<st_num;l++) { A[j+1][l] /= sum; }
   } 
       
   //backward recursion B
   for (int j=k-1;j>=0;j--)
   {
      long double sum = 0;
      
      for (int l=0;l<st_num;l++)
      {
         // Here term1, term2 are the WEIGHTs, larger the better
         int nextState0 = next_table[l][0];
         int nextState1 = next_table[l][1];

         long double pathProb0 = ll(y[j], l, m, 0);
         long double pathProb1 = ll(y[j], l, m, 1);
         if (pathProb0 == 0 || pathProb1 == 0) { cout << "precision problem" << endl; }
         long double term1 = B[j+1][nextState0] * pathProb0 * aPriori0[j];
         long double term2 = B[j+1][nextState1] * pathProb1 * aPriori1[j];
         
         B[j][l] = term1 + term2;
         
         sum += B[j][l];
      }
      
      // Normalization
      for (int l=0;l<st_num;l++) { B[j][l] /= sum; }
   }
       
   //extrinsic information
   for (int j=0;j<k;j++)
   {
      long double t0[st_num],t1[st_num];
      long double logSumOf_t0 = 0, logSumOf_t1 = 0;

      for (int l=0;l<st_num;l++)
      {  
         int nextState0 = next_table[l][0];
         int nextState1 = next_table[l][1];

#ifdef BCJR
         t0[l] = A[j][l] * B[j+1][nextState0]* ll(y[j], l, m, 0) *  aPriori0[j];
         t1[l] = A[j][l] * B[j+1][nextState1]* ll(y[j], l, m, 1) *  aPriori1[j]; 
#else
         t0[l] = A[j][l] * ll(y[j], l, m, 0)*  aPriori0[j];
         t1[l] = A[j][l] * ll(y[j], l, m, 1)*  aPriori1[j];
#endif         
         logSumOf_t0 = logSumOf_t0 + t0[l];
         logSumOf_t1 = logSumOf_t1 + t1[l];
      }
  
      extrinsic0[j] = logSumOf_t0;
      extrinsic1[j] = logSumOf_t1;
   }
   
   delete [] (*A);delete [] (*B);
}

int *bcjrDecode(long double *r, long double N0)
{
   static int hasInit = 0;
   if (hasInit == 0)
   {
      initialize();
      hasInit = 1;
   }
   
   const int k = (int)r[0];

   long double z[k];
   long double aPriori0[k];
   long double aPriori1[k];
   long double extrinsic0[k];
   long double extrinsic1[k];
   
   for (int i=0;i<k;i++)
   {   
      z[i]  = r[i+1];
      
      aPriori0[i] = 1.0;
      aPriori1[i] = 1.0;
   }
   
   /*-----------------decoding--------------------*/ 

#ifdef APRIORI   
   // Test apriori, not sure
   for(int j=0;j<k;j++)
   {
      aPriori0[j] = test(z[j],0);
      aPriori1[j] = test(z[j],1);
   }
#endif
   
   for (int i=0;i<turboIteration;i++)
   {
      siso(z,aPriori0,aPriori1,extrinsic0,extrinsic1,k);
/*   
      if (turboIteration != 1)
      {
         for(int j=0;j<k  ;j++)
         { aPriori0[j] = extrinsic0[j]+x0[j]; aPriori1[j] = extrinsic1[j]+x1[j]; }
      }
*/
   }

   
   int *r_d = new int [k +1];
   r_d[0] = k;
   
   for (int i=0;i<k;i++) { r_d[i +1] = ((extrinsic1[i]/extrinsic0[i]) > 1.0) ? 1 : 0; }
   
   return r_d;
}
