#include "turbo.h"
#include "fileIO.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>
 
#include <iostream>
using namespace std;

const int turboIteration = (int)getValue("Iteration","config.txt");

const int m = 3;//(int)getValue("Memory order","config.txt");
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

// Output the log likelihood
inline double ll(double Z, int state, int _m, int bit, int *survivor)
{
   int survivorLen = survivor[0];
   double var = 0;
   double mean = 0;
   
   // Still around the beginning of a packet. 
   // It turns out that for the unknown interfering bits, it's better not to
   // their variance into account.
   if (survivorLen != (N-_m))
   {
      for (int i=_m+survivorLen+1;i<=N;i++) { mean += a[i]; }
      float p1 = 0.5;
      mean *= p1;
   }

   var += sd*sd;
   if (var == 0) { var = 1e-5; }

   // This is the bit driving the transition.
   if (bit == 1) { mean += a[0]; }
   
   for (int j=1;j<=_m;j++)
   { mean += ((state>>(_m-j))&1)*a[j]; }

   for (int j=_m+1;j<=_m+survivorLen;j++)
   { mean += survivor[j-_m]*a[j]; }

   return (-(Z-mean)*(Z-mean))/2.0/var;
}

void siso(double *y,double *aPriori0,double *aPriori1,double *extrinsic0,double *extrinsic1,int k)
{
   double (*A)[st_num] = new double [k+1][st_num]; 
   double (*B)[st_num] = new double [k+1][st_num]; 

   // Record the surviving state entering this state
   // survivor[i][10] = 15. The surviving state entering state 10
   // at time index i is 15.
   int (*survivor)[st_num] = new int [k+1][st_num]; 
  
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
         int currentBit = l >> (m-1);
         int lastState0 = from_table[l][0];
         int lastState1 = from_table[l][1];

         // There are 2 survivor sequences. One for lastState0, the other one
         // for lastState1.
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

         // Well, here is something that I'm not sure. The paper says the
         // apriori for the forward recusion should be the last bit of the
         // starting state of the transition, not the driving bit of the
         // transition. But I don't know why.
/*
         double Pa; 
         if (currentBit == 0) { Pa = aPriori0[j]; }
         else { Pa = aPriori1[j]; }
*/
         double Pa0 = 0, Pa1 = 0;
         int idx = j-m;
         if (idx >=0 )
         {
            if ((lastState0&1) == 0) { Pa0 = aPriori0[idx]; Pa1 = aPriori1[idx]; }
                                else { Pa0 = aPriori1[idx]; Pa1 = aPriori0[idx]; }

         }
         double term1 = A[j][lastState0] + ll(y[j], lastState0, m, currentBit, survivorSeq0) + Pa0;
         double term2 = A[j][lastState1] + ll(y[j], lastState1, m, currentBit, survivorSeq1) + Pa1;
         
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
   for (int j=k-1;j>=0;j--)
   {
      double avg = 0;
      
      for (int l=0;l<st_num;l++)
      {
         // Survivor sequence.
         int survivorLen = min(N-m,j);
         int *survivorSeq = new int [survivorLen +1]; survivorSeq[0] = survivorLen;

         int state = survivor[j][l];
         for (int i=0;i<survivorLen;i++)
         {
            survivorSeq[i+1] = (state)&1;
            int index = j-1-i;
            if (index >= 0)
            { state = survivor[j-1-i][state]; }
         }

         int nextState0 = next_table[l][0];
         int nextState1 = next_table[l][1];

         double term1 = B[j+1][nextState0] + ll(y[j], l, m, 0, survivorSeq) + aPriori0[j];
         double term2 = B[j+1][nextState1] + ll(y[j], l, m, 1, survivorSeq) + aPriori1[j];
         
         B[j][l] = logsum(term1,term2);
         
         avg += B[j][l];
         
         delete [] survivorSeq;
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

         // Calculate the state probability
         double PState = 0;
         for (int i=1;i<=m;i++)
         {
            int idx = j-i;
            if (idx >= 0)
            {
               if (((l>>(m-i))&1) == 0)
               { PState += aPriori0[idx]; }
               else
               { PState += aPriori1[idx]; }
            }
         }

         t0[l] = A[j][l] + B[j+1][nextState0]+ ll(y[j], l, m, 0, survivorSeq) +  aPriori0[j] + PState;
         t1[l] = A[j][l] + B[j+1][nextState1]+ ll(y[j], l, m, 1, survivorSeq) +  aPriori1[j] + PState;

         delete [] survivorSeq;
  
         logSumOf_t0 = logsum(logSumOf_t0,t0[l]);
         logSumOf_t1 = logsum(logSumOf_t1,t1[l]);
      }
  
      extrinsic0[j] = logSumOf_t0;
      extrinsic1[j] = logSumOf_t1;
   }
   
   delete [] (*A);delete [] (*B);delete [] (*survivor);
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
