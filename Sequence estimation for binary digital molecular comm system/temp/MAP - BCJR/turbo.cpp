#include "turbo.h"
#include "fileIO.h"

#include <cstdlib>
#include <cmath>
 
#include <iostream>
using namespace std;

const int turboIteration = 1;//(int)getValue("Iteration","config.txt");

const int m = 4;//(int)getValue("Memory order","config.txt");
const int st_num = (1<<m);

int next_table[st_num][2];                   //next states correspond to input 0, 1

int from_table[st_num][2];                   //previous states correspond to incoming 0, 1

float lookup_table[100000]={0};              //the real delta = index/1000

extern float sd;

#include "turboSub.h"

int *turboEncode(int *d)
{
}
 
 // a0 a1 a2 ~ aN 
const int N = 20;
float a[N+1] = {
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


// Must output the llr between bit 1 and 0
inline float test(float Z,int bit)
{  
   float mean = 0, var = 0;
   for (int i=1;i<=N;i++) { mean += a[i]; var += a[i]*a[i]; }
   float p1 = 0.5;
   mean *= p1;
   var *= (p1-p1*p1);

   var += sd*sd;

      if (bit == 0 )
        { return (-(Z-mean)*(Z-mean))/2.0/var; }
   else { return (-(Z-mean-a[0])*(Z-mean-a[0]))/2.0/var; }
}

// Must output the llr between bit 1 and 0
inline float distance(float Z, int state, int _m)
{  
   float mean1 = 0, mean0 = 0, var = 0;
   for (int i=_m+1;i<=N;i++) { mean1 += a[i]; var += a[i]*a[i]; }
   float p1 = 0.5;
   mean1 *= p1;
   mean0 = mean1;
   var *= (p1-p1*p1);

   var += sd*sd;
   
   mean1 += a[0];
 
   for (int j=1;j<=_m;j++)
   {
      mean1 += ((state>>(_m-j))&1)*a[j];
      mean0 += ((state>>(_m-j))&1)*a[j];
   }
   
   return ((Z-mean0)*(Z-mean0) - (Z-mean1)*(Z-mean1))/2.0/var;
}

   
// output the log likelihood 
inline float ll(float Z, int state, int _m, int bit)
{
   static int hasInit = 0;
   static float var = 0;
   static float mean = 0;
   if (hasInit==0)
   {
      for (int i=_m+1;i<=N;i++) { mean += a[i]; var += a[i]*a[i]; }
      float p1 = 0.5;
      mean *= p1;
      var *= (p1-p1*p1);

      var += sd*sd;
   
      hasInit = 1;
   }
   
   float mean1 = mean, mean0;

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

void siso(float *y,float *aPriori0,float *aPriori1,float *extrinsic,int k)
{
   float (*A)[st_num] = new float [k+1][st_num]; 
   float (*B)[st_num] = new float [k+1][st_num]; 
 
   for (int j=0;j<st_num;j++)
   {
      A[0][j] = 0;//-1.0e10;
      B[k][j] = 0;//-1.0e10;
   }
//   A[0][0]=0;
//   B[k][1]=10;

   //forward recursion A
   for (int j=0;j<k;j++)
   {
      for (int l=0;l<st_num;l++)
      {
         // Here term1, term2 are the WEIGHTs, larger the better
         // How aPriori is utilized needs some thoughts
         int currentBit = (l>>(m-1))&1;        
         int lastState0 = from_table[l][0];
         int lastState1 = from_table[l][1];
         
         float Pa; 
         if (currentBit == 0) { Pa = aPriori0[j]; }
         else { Pa = aPriori1[j]; }
         
         float term1 = A[j][lastState0] + ll(y[j], lastState0, m, currentBit) + Pa;
         float term2 = A[j][lastState1] + ll(y[j], lastState1, m, currentBit) + Pa;
         
//       cout << term1 << endl << term2 << endl << logsum(term1,term2) << endl<<  endl;getchar();         

         A[j+1][l] = logsum(term1,term2);
      }
   } 
       
   //backward recursion B
   for (int j=k-1;j>=0;j--)
   {
      for (int l=0;l<st_num;l++)
      {
         // Here term1, term2 are the WEIGHTs, larger the better
         int nextState0 = next_table[l][0];
         int nextState1 = next_table[l][1];
         
         float term1 = B[j+1][nextState0] + ll(y[j], l, m, 0);
         float term2 = B[j+1][nextState1] + ll(y[j], l, m, 1);
         
//       cout << term1 << endl << term2 << endl << logsum(term1,term2) << endl<<  endl;;
       
         B[j][l] = logsum(term1,term2);
      }
   }
       
   //extrinsic information
   for (int j=0;j<k;j++)
   {
      float t0[st_num],t1[st_num];
      float logSumOf_t0 = -1.0e30, logSumOf_t1 = -1.0e30;

      for (int l=0;l<st_num;l++)
      {  
         int nextState0 = next_table[l][0];
         int nextState1 = next_table[l][1];
         
//         t0[l] = A[j][l] + B[j+1][nextState0]+ ll(y[j], l, m, 0);
//         t1[l] = A[j][l] + B[j+1][nextState1]+ ll(y[j], l, m, 1);         

         t0[l] = A[j][l] + ll(y[j], l, m, 0);
         t1[l] = A[j][l] + ll(y[j], l, m, 1);
         
         logSumOf_t0 = logsum(logSumOf_t0,t0[l]);
         logSumOf_t1 = logsum(logSumOf_t1,t1[l]);  
      }
  
      extrinsic[j] = logSumOf_t1-logSumOf_t0;
   }
   
   delete [] (*A);delete [] (*B);
}

int *bcjrDecode(float *r, float N0)
{
   static int hasInitialized = 0;
   if (hasInitialized == 0)
   {
    for (int i=0;i<=N;i++) { a[i] /= 10.0; }
    initialize();
    hasInitialized = 1;
   }
   
   const int k = (int)r[0];
   
   float x[k];
   float aPriori0[k];
   float aPriori1[k];
   float extrinsic[k];
   
   for (int i=0;i<k;i++)
   {   
    x[i]  = r[i+1];
    
    extrinsic[i] = 0;
   }
   
   /*-----------------decoding--------------------*/ 
   
   // Test apriori, not sure
   for(int j=0;j<k;j++) { aPriori0[j] = test(x[j],0); aPriori1[j] = test(x[j],1); }
   //    for(int j=0;j<k;j++) { aPriori0[j] = 0; aPriori1[j] = 0; }
   
   for (int i=0;i<turboIteration;i++)
   {
     siso(x,aPriori0,aPriori1,extrinsic,k);
   
   //        for(int j=0;j<k  ;j++) { aPriori[j] = extrinsic[j]; }
   //        for(int j=k;j<k+m;j++) { aPriori[j] = 0; }
     
   //        Minterleave(aPriori,k);
   
   //        siso(y2,aPriori,extrinsic,k);
   
   //        deinterleave(extrinsic,k);
   }
   //    deinterleave(aPriori,k);
   
   int *r_d = new int [k +1];
   r_d[0] = k;
   
   for (int i=0;i<k;i++) { r_d[i +1] = (extrinsic[i] > 0) ? 1 : 0; }
   
   return r_d;
}
