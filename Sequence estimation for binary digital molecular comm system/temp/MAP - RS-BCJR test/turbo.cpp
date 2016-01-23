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

const int m = 2;//(int)getValue("Memory order","config.txt");
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
inline double ll(double Z, int state, int _m, int bit, int *survivor)
{
   int survivorLen = survivor[0];
   double var = 0;
   double mean = 0;
   
   // Still around the beginning of a packet. 
   if (survivorLen != (N-_m))
   {
      for (int i=_m+survivorLen+1;i<=N;i++) { mean += a[i]; var += a[i]*a[i]; }
      float p1 = 0.5;
      mean *= p1;
      var *= (p1-p1*p1);
   }
   
   var += sd*sd;
   if (var == 0) { var = 1e-5; }
   
   if (bit == 1) { mean += a[0]; }
   
   for (int j=1;j<=_m;j++)
   {
      mean += ((state>>(_m-j))&1)*a[j];
   }
   for (int j=_m+1;j<=_m+survivorLen;j++)
   {
      mean += survivor[j-_m]*a[j];
   }

   return (-(Z-mean)*(Z-mean))/2.0/var;
}


// output the log likelihood
inline double ll2(double Z, int state, int _m, int bit, int *survivor)
{
   int survivorLen = survivor[0];
   double var = 0;
   double mean = 0;

   // Still around the end of a packet.
   if (survivorLen != (N-_m))
   {
      for (int i=0;i<=N-_m-survivorLen-1;i++) { mean += a[i]; var += a[i]*a[i]; }

      float p1 = 0.5;
      mean *= p1;
      var *= (p1-p1*p1);
   }

   var += sd*sd;
   if (var == 0) { var = 1e-5; }

   for (int j=N-_m-survivorLen;j<=N-m-1;j++)
   {
      mean += survivor[N-_m-j]*a[j];
   }

   if (bit == 1) { mean += a[N-m]; }

   for (int j=N-m+1;j<=N;j++)
   {
      mean += ((state>>(N-j))&1)*a[j];
   }
   
   return (-(Z-mean)*(Z-mean))/2.0/var;
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

   // Record the surviving state entering this state
   // survivor[i][10] = 15. The surviving stating entering state 10
   // at time index i is 15.
   int (*survivor)[st_num] = new int [k+1][st_num]; 
   int (*Bsurvivor)[st_num] = new int [k+1][st_num];
  
   for (int l=0;l<st_num;l++)
   {
      A[0][l] = 0;
      B[k][l] = 0;
   }

   int L2 = N-m;
   for (int j=k-L2;j<=k;j++)
   {
      for (int l=0;l<st_num;l++)
      { B[j][l] = 0; }
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

         // Survivor sequence.
         int survivorLen = min(N-m,j);
         int *survivorSeq0 = new int [survivorLen +1]; survivorSeq0[0] = survivorLen;
         int *survivorSeq1 = new int [survivorLen +1]; survivorSeq1[0] = survivorLen;
         
         int state0, state1;
         state0 = survivor[j][lastState0];
         state1 = survivor[j][lastState1];
         for (int i=0;i<survivorLen;i++) 
         {
            survivorSeq0[i+1] = (state0)&1;
            survivorSeq1[i+1] = (state1)&1;
            int index = j-1-i;
            if (index >= 0)
            {
               state0 = survivor[j-1-i][state0];
               state1 = survivor[j-1-i][state1];
            }
         }

         double Pa; 
         if (currentBit == 0) { Pa = aPriori0[j]; }
         else { Pa = aPriori1[j]; }
         
         double term1 = A[j][lastState0] + ll(y[j], lastState0, m, currentBit, survivorSeq0) + Pa;
         double term2 = A[j][lastState1] + ll(y[j], lastState1, m, currentBit, survivorSeq1) + Pa;
         
         A[j+1][l] = logsum(term1,term2);
         
         if (term1 > term2) { survivor[j+1][l] = lastState0; }
         else               { survivor[j+1][l] = lastState1; }
         
         avg += A[j+1][l];
         
         delete [] survivorSeq0; delete [] survivorSeq1;
      }
      
      // Normalization
      avg /= st_num;
      for (int l=0;l<st_num;l++) { A[j+1][l] -= avg; }
   }
   
   
   //backward recursion B
   for (int j=k-1-L2;j>=0;j--)
   {
      double avg = 0;
      
      for (int l=0;l<st_num;l++)
      {
         // Here term1, term2 are the WEIGHTs, larger the better
         int nextState0 = next_table[l][0];
         int nextState1 = next_table[l][1];

         // Survivor sequence.
         int survivorLen = min(L2,k-1-L2-j);
         int *survivorSeq0 = new int [survivorLen +1]; survivorSeq0[0] = survivorLen;
         int *survivorSeq1 = new int [survivorLen +1]; survivorSeq1[0] = survivorLen;

         int state0 = Bsurvivor[j+1][nextState0];
         int state1 = Bsurvivor[j+1][nextState1];
         for (int i=0;i<survivorLen;i++)
         {
            survivorSeq0[i+1] = (state0)>>(m-1);
            survivorSeq1[i+1] = (state1)>>(m-1);
            int index = j+i+2;
            if (index <= k)
            {
               state0 = Bsurvivor[index][state0];
               state1 = Bsurvivor[index][state1];
            }
         }

         double term1 = B[j+1][nextState0] + ll2(y[j+L2], l, m, 0, survivorSeq0) + aPriori0[j];
         double term2 = B[j+1][nextState1] + ll2(y[j+L2], l, m, 1, survivorSeq1) + aPriori1[j];
         
         B[j][l] = logsum(term1,term2);

         if (term1 > term2) { Bsurvivor[j][l] = nextState0; }
         else               { Bsurvivor[j][l] = nextState1; }

         avg += B[j][l];
         
         delete [] survivorSeq0; delete [] survivorSeq1;
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
         // Survivor sequence.
         int survivorLen = min(N-m,j);
         int *survivorSeq = new int [survivorLen +1]; survivorSeq[0] = survivorLen;

         int state;
         state = survivor[j][l];
         for (int i=0;i<survivorLen;i++)
         {
            survivorSeq[i+1] = (state)&1;
            int index = j-1-i;
            if (index >= 0)
            { state = survivor[j-1-i][state]; }
         }

         int nextState0 = next_table[l][0];
         int nextState1 = next_table[l][1];

#ifdef BCJR
         t0[l] = A[j][l] + B[j+1][nextState0]+ ll(y[j], l, m, 0, survivorSeq) +  aPriori0[j];
         t1[l] = A[j][l] + B[j+1][nextState1]+ ll(y[j], l, m, 1, survivorSeq) +  aPriori1[j];
#else
         t0[l] = A[j][l] + ll(y[j], l, m, 0, survivorSeq)+  aPriori0[j];
         t1[l] = A[j][l] + ll(y[j], l, m, 1, survivorSeq)+  aPriori1[j];
#endif         
         logSumOf_t0 = logsum(logSumOf_t0,t0[l]);
         logSumOf_t1 = logsum(logSumOf_t1,t1[l]);
         
         delete [] survivorSeq;
      }
  
      extrinsic0[j] = logSumOf_t0;
      extrinsic1[j] = logSumOf_t1;
   }
   
   delete [] (*A);delete [] (*B);delete [] (*survivor);delete [] (*Bsurvivor);
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
