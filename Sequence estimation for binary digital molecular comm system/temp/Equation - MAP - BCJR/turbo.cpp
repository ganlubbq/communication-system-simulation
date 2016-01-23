#include "turbo.h"
#include "fileIO.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>
 
#include <iostream>
using namespace std;

#define BCJR 1
//#define APRIORI 1

const int turboIteration = (int)getValue("Iteration","config.txt");

const int m = 5;//(int)getValue("Memory order","config.txt");
const int st_num = (1<<m);

int next_table[st_num][2];                   //next states correspond to input 0, 1

int from_table[st_num][2];                   //previous states correspond to incoming 0, 1

double lookup_table[100000]={0};              //the real delta = index/1000

const double sd = getValue("Noise std", "config.txt");

const double pi = 3.1415926;

#include "turboSub.h"

double logsum(double a,double b)
{
   double max = (a>b) ? a : b;
   
   double delta = (a>b) ? (a-b) : (b-a);
   
   if (delta > 44)
   {
//      cout << "precision problem" << endl;
      return max;
   }
   else
   {
      long double temp = 1.0+exp(-delta);
      delta = (double)log(temp);
   
      return max+delta;
   }
}

int *turboEncode(int *d)
{
}
 
extern const int N;
extern double a[];

// output the log likelihood 
inline double ll(double Z, int state, int _m, int bit)
{
   static int hasInit = 0;
   static double var = 0;
   static double mean = 0;
   if (hasInit==0)
   {
      for (int i=_m+1;i<=N;i++) { mean += a[i]; var += a[i]*a[i]; }
      double p1 = 0.5;
      mean *= p1;
      var *= (p1-p1*p1);
      var += sd*sd;
      
      if (var == 0) { var = 1e-5; }
      hasInit = 1;
   }
   
   double mean1 = mean, mean0;

   for (int j=1;j<=_m;j++)
   {
      mean1 += ((state>>(_m-j))&1)*a[j];
   }
   mean0 = mean1;
   mean1 += a[0];
   
   if (bit == 0 )
        { return (-(Z-mean0)*(Z-mean0))/2.0/var; }
   else { return (-(Z-mean1)*(Z-mean1))/2.0/var; }
}


// Must output the llr between bit 1 and 0
inline double test(double Z,int bit)
{  
   static int hasInit = 0;
   static double var = 0;
   static double mean = 0;
   if (hasInit==0)
   {
      for (int i=1;i<=N;i++) { mean += a[i]; var += a[i]*a[i]; }
      double p1 = 0.5;
      mean *= p1;
      var *= (p1-p1*p1);
      var += sd*sd;
      
      if (var == 0) { var = 1e-5; }
      hasInit = 1;
   }

   if (bit == 0 )
        { return (-(Z-mean)*(Z-mean))/2.0/var; }
   else { return (-(Z-mean-a[0])*(Z-mean-a[0]))/2.0/var; }
}


void siso(double *y,double *aPriori0,double *aPriori1,double *extrinsic0,double *extrinsic1,int k)
{
   double (*A)[st_num] = new double [k+1][st_num]; 
   double (*B)[st_num] = new double [k+1][st_num]; 
 
   for (int j=0;j<st_num;j++)
   {
      A[0][j] = 0;
      B[k][j] = 0;
   }

   //forward recursion A
   for (int j=0;j<k;j++)
   {
      double avg = 0;
      
      for (int l=0;l<st_num;l++)
      {
         // Here term1, term2 are the WEIGHTs, larger the better
         int currentBit = (l>>(m-1))&1;        
         int lastState0 = from_table[l][0];
         int lastState1 = from_table[l][1];
         
         double Pa; 
         if (currentBit == 0) { Pa = aPriori0[j]; }
         else { Pa = aPriori1[j]; }
         
         double term1 = A[j][lastState0] + ll(y[j], lastState0, m, currentBit) + Pa;
         double term2 = A[j][lastState1] + ll(y[j], lastState1, m, currentBit) + Pa;
         
         A[j+1][l] = logsum(term1,term2);
         
         avg += A[j+1][l];
      }
      
      // Normalization
      avg /= st_num;
      for (int l=0;l<st_num;l++) { A[j+1][l] -= avg; }
   } 
       
   //backward recursion B
   for (int j=k-1;j>=0;j--)
   {
      double avg = 0;
      
      for (int l=0;l<st_num;l++)
      {
         // Here term1, term2 are the WEIGHTs, larger the better
         int nextState0 = next_table[l][0];
         int nextState1 = next_table[l][1];

         double term1 = B[j+1][nextState0] + ll(y[j], l, m, 0) + aPriori0[j];
         double term2 = B[j+1][nextState1] + ll(y[j], l, m, 1) + aPriori1[j];
         
         B[j][l] = logsum(term1,term2);
         
         avg += B[j][l];
      }
      
      // Normalization
      avg /= st_num;
      for (int l=0;l<st_num;l++) { B[j][l] -= avg; }
   }
       
   //extrinsic information
   for (int j=0;j<k;j++)
   {
      double t0[st_num],t1[st_num];
      double logSumOf_t0 = -1.0e30, logSumOf_t1 = -1.0e30;

      for (int l=0;l<st_num;l++)
      {  
         int nextState0 = next_table[l][0];
         int nextState1 = next_table[l][1];

#ifdef BCJR
         t0[l] = A[j][l] + B[j+1][nextState0]+ ll(y[j], l, m, 0) +  aPriori0[j];
         t1[l] = A[j][l] + B[j+1][nextState1]+ ll(y[j], l, m, 1) +  aPriori1[j]; 
#else
         t0[l] = A[j][l] + ll(y[j], l, m, 0)+  aPriori0[j];
         t1[l] = A[j][l] + ll(y[j], l, m, 1)+  aPriori1[j];
#endif         
         logSumOf_t0 = logsum(logSumOf_t0,t0[l]);
         logSumOf_t1 = logsum(logSumOf_t1,t1[l]);
      }
  
      extrinsic0[j] = logSumOf_t0;
      extrinsic1[j] = logSumOf_t1;
   }
   
   delete [] (*A);delete [] (*B);
}

int *bcjrDecode(double *r, double N0)
{
   static int hasInit = 0;
   if (hasInit == 0)
   {
      initialize();
      hasInit = 1;
   }
   
   const int k = (int)r[0];

   double z[k];
   double aPriori0[k];
   double aPriori1[k];
   double extrinsic0[k];
   double extrinsic1[k];
   
   for (int i=0;i<k;i++)
   {   
      z[i]  = r[i+1];
      
      aPriori0[i] = 0;
      aPriori1[i] = 0;
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
   
   for (int i=0;i<k;i++) { r_d[i +1] = ((extrinsic1[i]-extrinsic0[i]) > 0) ? 1 : 0; }
   
   return r_d;
}
