#include "turbo.h"

#include <cstdlib>
#include <cmath>
#include <iostream>
using namespace std;

const int G11 = 035;
const int G12 = 023;

const int m = 4;
const int st_num = (1<<m);

int g11[m+1],g12[m+1];                       //generator sequence

int next_table[st_num][2];                   //next states correspond to input 0, 1
int out_table[st_num][4];                    //output bits correspond to input 0, 1
int from_table[st_num][2];                   //previous states correspond to incoming 0, 1
int from_out_table[st_num][4];

//double ray[k+m];                             //record or the rayleigh r.v.
double lookup_table[100000]={0};              //the real delta = index/1000

int *record;                               //record of interleaving order

#include "turboSub.h"

int *turboEncode(int *d)
{
   static int hasInitialized = 0;
   if (hasInitialized == 0)
   {
      initialize();
      hasInitialized = 1;
   }
   
   const int k = d[0];

   int a[k+m+m];
   int b[k+m+m];
   for (int i=0;i<k+m+m;i++) { a[i] = 0;b[i] = 0; }
    
   int temp[k];
   int _d[k+m];
   for (int i=0;i<k;i++) { _d[i] = d[i +1];temp[i] = _d[i]; }
    
   for (int i=0;i<k;i++)
   {  //encoding a
      for (int j=1;j<m+1;j++)
      {
         a[i+m]^= a[i-j+m]*g11[j];
      }
      a[i+m]^= _d[i];
   }

   //add tail bits so that the 1st shift register goes to 0 state
   for (int i=k;i<k+m;i++)
   {
      _d[i]=0;
      for (int j=1;j<m+1;j++)
      {
         _d[i]=_d[i]^a[i-j+m]*g11[j];
      }
   }

   // Do not delete record[] for the first time turboEncode() is called
   static int recordAllocated = 0;
   if (recordAllocated == 1) { delete [] record; }
   record = new int [k];
   recordAllocated = 1;
    
   rand_permute(record,k);
    
   interleave(temp,k);  
    
   for (int i=0;i<k;i++)
   {  //encoding b
      for (int j=1;j<m+1;j++)
      {
         b[i+m]^= b[i-j+m]*g11[j];
      }
      b[i+m]^= temp[i];
   }

   int *v = new int [3*(k+m) +1];
   v[0] = 3*(k+m);
   
   for (int i=0;i<(k+m);i++)
   {  //encoding
      int t0= 0;
      int t1= 0;
        
      for (int j=0;j<m+1;j++)
      {
         t0^=a[i-j+m]*g12[j];
         t1^=b[i-j+m]*g12[j];
      }
        
      v[3*i+1] = _d[i];
      v[3*i+2] = t0;
      v[3*i+3] = t1;
   }

   return v;
}

void siso(int,double *,int,double,double *,double *,int);

int *bcjrDecode(double *r, double N0, int iteration)
{
    const int k = (int)r[0]/3 - m;

    double sd = sqrt(N0/2);//standard deviation of gaussian r.v.

    double x[k+m];
    double y1[k+m];
    double y2[k+m];
    
    double x_metric[k+m];
    double LLR[k+m];
    double z[k+m];
    
    for (int i=0;i<k+m;i++)
    {   
       x[i]  = r[3*i+1];
       y1[i] = r[3*i+2];
       y2[i] = r[3*i+3];
       
       x_metric[i] = 4*x[i]/N0;
       z[i] = 0;
    }
    
    /*-----------------decoding--------------------*/ 
    
    for (int i=0;i<iteration;i++)
    {
        for (int j=0;j<k+m;j++){ LLR[j] = x_metric[j]; }
        
        siso(i,y1,1,N0,LLR,z,k);

        for(int j=0;j<k+m;j++)
        {
           if (j<k) { LLR[j] += x_metric[j]; }
           else { LLR[j]=0; }
        
           z[j]=0;
        }
        
        Minterleave(LLR,k);

        siso(i,y2,2,N0,LLR,z,k);//will produce feed[][]

        deinterleave(z,k);
    }
    deinterleave(LLR,k);
    
    int *r_d = new int [k +1];
    r_d[0] = k;
   
    for (int i=0;i<k;i++) { r_d[i +1] = (z[i]+LLR[i] > 0) ? 1 : 0; }
    
    return r_d;
}

/*double max(double *a,int l){
      double temp;
      int i;
      
      temp=a[0];
      for(i=0;i<l;i++){
           if(a[i]>temp) temp=a[i];
           }
      
      return temp;
      }
*/

void siso(int pass,double *y,int option,double N0,double *LLR,double *z,int k)
{
   double (*pI_sym)[4][2] = new double [k+m][4][2];
   double (*A)[st_num] = new double [k+m+1][st_num]; 
   double (*B)[st_num] = new double [k+m+1][st_num]; 
 
   for (int j=0;j<st_num;j++)
   {
      A[0][j] = -1.0e10;
      B[k+m][j] = -1.0e10;
   }
     
   A[0][0]=0;
   B[k+m][0]=0;
     
   //branch metric Pk[c(e);I]        
   for (int j=0;j<k+m;j++)
   {  // [][][1] for complete path metric ,[][][0] for extrisic compute
      pI_sym[j][0][1] = -z[j]/2 - LLR[j]/2 - y[j]*2/N0;
      pI_sym[j][0][0] = -y[j]*2/N0;
 
      pI_sym[j][1][1] = -z[j]/2 - LLR[j]/2 + y[j]*2/N0;
      pI_sym[j][1][0] = y[j]*2/N0;
      
      pI_sym[j][2][1] = z[j]/2 + LLR[j]/2 - y[j]*2/N0;
      pI_sym[j][2][0] = -y[j]*2/N0;
      
      pI_sym[j][3][1] = z[j]/2 + LLR[j]/2 + y[j]*2/N0;
      pI_sym[j][3][0] = y[j]*2/N0;
   }

   //forward recursion A
   for (int j=0;j<k+m;j++)
   {
      for (int l=0;l<st_num;l++)
      {
         int idxFor0 = 2*from_out_table[l][0]+from_out_table[l][1];
         int idxFor1 = 2*from_out_table[l][2]+from_out_table[l][3];
         
         double term1 = A[j][from_table[l][0]]+pI_sym[j][idxFor0][1];  
         double term2 = A[j][from_table[l][1]]+pI_sym[j][idxFor1][1];
         
         A[j+1][l] = logsum(term1,term2);
//         A[j+1][l]=max(temp,2);
      }
   } 
       
   //backward recursion B
   for (int j=k+m-1;j>=0;j--)
   {
      for (int l=0;l<st_num;l++)
      {
         int idxFor0 = 2*out_table[l][0]+out_table[l][1];
         int idxFor1 = 2*out_table[l][2]+out_table[l][3];
          
         double term1 = B[j+1][next_table[l][0]]+pI_sym[j][idxFor0][1];
         double term2 = B[j+1][next_table[l][1]]+pI_sym[j][idxFor1][1];
         
         B[j][l] = logsum(term1,term2);
//         B[j][l]=max(temp,2);
      }
   }
       
   //extrinsic information
   for (int j=0;j<k;j++)
   {
      double t0[st_num],t1[st_num];
      double logSumOf_t0 = -1.0e10, logSumOf_t1 = -1.0e10;
      
      for (int l=0;l<st_num;l++)
      {  
         int idxFor0 = 2*out_table[l][0]+out_table[l][1];
         int idxFor1 = 2*out_table[l][2]+out_table[l][3];
                         
         t0[l] = A[j][l]+B[j+1][next_table[l][0]]+pI_sym[j][idxFor0][0];
         t1[l] = A[j][l]+B[j+1][next_table[l][1]]+pI_sym[j][idxFor1][0];
         
         logSumOf_t0 = logsum(logSumOf_t0,t0[l]);
         logSumOf_t1 = logsum(logSumOf_t1,t1[l]);
      }
  
      if(option==1) { LLR[j] = logSumOf_t1-logSumOf_t0; }
      else { z[j] = logSumOf_t1-logSumOf_t0; }
   }
  
   delete [] (*pI_sym);delete [] (*A);delete [] (*B);
}


