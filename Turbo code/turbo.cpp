#include "turbo.h"
#include "fileIO.h"

#include <cstdlib>
#include <cmath>

const int turboIteration = (int)getValue("Iteration","config.txt");

const int G11 = 035;
const int G12 = 023;

const int m = 4;
const int st_num = (1<<m);

int next_table[st_num][2];                   //next states correspond to input 0, 1
int out_table[st_num][4];                    //output bits correspond to input 0, 1

int stateTransition[st_num][2];                   //next states correspond to input 0, 1
int stateOutput[st_num][2];                    //output bits correspond to input 0, 1

int from_table[st_num][2];                   //previous states correspond to incoming 0, 1
int from_out_table[st_num][4];

//float ray[k+m];                             //record or the rayleigh r.v.
float lookup_table[100000]={0};              //the real delta = index/1000

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
      
   const int FL = d[0];
/*
   // Do not delete record[] for the first time turboEncode() is called
   static int recordAllocated = 0;
   if (recordAllocated == 1) { delete [] record; }
   record = new int [FL];
   recordAllocated = 1;
*/
   static int *temp = new int [FL];
   record = temp;
   
   rand_permute(record,FL);

   int *v = new int [3*(FL+m) +1];
   v[0] = 3*(FL+m);

   int shiftRegisterA = 0,
       shiftRegisterB = 0;

   // Encode the data bits
   for (int i=1;i<=FL;i++)
   {
      int inputBitA = d[i];
      int inputBitB = d[record[i-1]+1];
      static int offset_1 = FL+m,
                 offset_2 = (FL+m) << 1;
        
      v[i] = inputBitA;
      v[i+offset_1] = stateOutput[shiftRegisterA][inputBitA];
      v[i+offset_2] = stateOutput[shiftRegisterB][inputBitB];
      
      shiftRegisterA = stateTransition[shiftRegisterA][inputBitA];
      shiftRegisterB = stateTransition[shiftRegisterB][inputBitB];
   }
   
   // Encode the tail bits
   // Both SR are terminated, though i'm not sure whether the 2nd SR has to be
   // terminate or not.
   for (int i=1;i<=m;i++)
   {
      int inputBitA = isNumOfBit1Odd(G11&shiftRegisterA,m);
      int inputBitB = isNumOfBit1Odd(G11&shiftRegisterB,m);
      
      static int offset_1 = FL+(FL+m),
                 offset_2 = FL+((FL+m)<<1);
      
      v[i+FL] = inputBitA;
      v[i+offset_1] = stateOutput[shiftRegisterA][inputBitA];
      v[i+offset_2] = stateOutput[shiftRegisterB][inputBitB];
      
      shiftRegisterA >>= 1;
      shiftRegisterB >>= 1;
   }

   return v;
}

void siso(float *y,float *aPriori,float *extrinsic,int k)
{
   float (*A)[st_num] = new float [k+m+1][st_num]; 
   float (*B)[st_num] = new float [k+m+1][st_num]; 
 
   for (int j=0;j<st_num;j++)
   {
      A[0][j] = -1.0e10;
      B[k+m][j] = -1.0e10;
   }
   A[0][0]=0;
   B[k+m][0]=0;

   //forward recursion A
   for (int j=0;j<k+m;j++)
   {
      for (int l=0;l<st_num;l++)
      {
         int bit00 = from_out_table[l][0];
         int bit01 = from_out_table[l][1];
         
         int bit10 = from_out_table[l][2];
         int bit11 = from_out_table[l][3];

         float term1 = A[j][from_table[l][0]]+ (bit00-0.5)*aPriori[j] + (bit01-0.5)*y[j];
         float term2 = A[j][from_table[l][1]]+ (bit10-0.5)*aPriori[j] + (bit11-0.5)*y[j];
         
         A[j+1][l] = logsum(term1,term2);
//         A[j+1][l]=max(temp,2);
      }
   } 
       
   //backward recursion B
   for (int j=k+m-1;j>=0;j--)
   {
      for (int l=0;l<st_num;l++)
      {
         int bit00 = out_table[l][0];
         int bit01 = out_table[l][1];
         
         int bit10 = out_table[l][2];
         int bit11 = out_table[l][3]; 
          
         float term1 = B[j+1][next_table[l][0]]+ (bit00-0.5)*aPriori[j] + (bit01-0.5)*y[j];
         float term2 = B[j+1][next_table[l][1]]+ (bit10-0.5)*aPriori[j] + (bit11-0.5)*y[j];
         
         B[j][l] = logsum(term1,term2);
//         B[j][l]=max(temp,2);
      }
   }
       
   //extrinsic information
   for (int j=0;j<k;j++)
   {
      float t0[st_num],t1[st_num];
      float logSumOf_t0 = -1.0e10, logSumOf_t1 = -1.0e10;
      
      for (int l=0;l<st_num;l++)
      {  
         int bit0 = out_table[l][1];
         int bit1 = out_table[l][3];
                         
         t0[l] = A[j][l]+B[j+1][next_table[l][0]]+ (bit0-0.5)*y[j];
         t1[l] = A[j][l]+B[j+1][next_table[l][1]]+ (bit1-0.5)*y[j];
         
         logSumOf_t0 = logsum(logSumOf_t0,t0[l]);
         logSumOf_t1 = logsum(logSumOf_t1,t1[l]);
      }
  
      extrinsic[j] = logSumOf_t1-logSumOf_t0;
   }
  
   delete [] (*A);delete [] (*B);
}


int *bcjrDecode(float *r, float N0)
{
    const int k = (int)r[0]/3 - m;

    float x[k+m];
    float y1[k+m];
    float y2[k+m];
    
    float aPriori[k+m];
    float extrinsic[k+m];
    
    for (int i=0;i<k+m;i++)
    {
       static int offset_1 = (k+m)+1,
                  offset_2 = ((k+m)<<1)+1;
       static float mul = 4/N0;
                  
       x[i]  = mul*r[i+1];
       y1[i] = mul*r[i+offset_1];
       y2[i] = mul*r[i+offset_2];
       
       extrinsic[i] = 0;
    }
    
    /*-----------------decoding--------------------*/ 
    
    for (int i=0;i<turboIteration;i++)
    {
        for(int j=0;j<k+m;j++) { aPriori[j] = extrinsic[j] + x[j]; }
        
        siso(y1,aPriori,extrinsic,k);

        for(int j=0;j<k  ;j++) { aPriori[j] = extrinsic[j] + x[j]; }
        for(int j=k;j<k+m;j++) { aPriori[j] = 0; }
        
        Minterleave(aPriori,k);

        siso(y2,aPriori,extrinsic,k);

        deinterleave(extrinsic,k);
    }
    deinterleave(aPriori,k);
    
    int *r_d = new int [k +1];
    r_d[0] = k;
   
    for (int i=0;i<k;i++) { r_d[i +1] = (extrinsic[i]+aPriori[i] > 0) ? 1 : 0; }
    
    return r_d;
}

/*float max(float *a,int l){
      float temp;
      int i;
      
      temp=a[0];
      for(i=0;i<l;i++){
           if(a[i]>temp) temp=a[i];
           }
      
      return temp;
      }
*/
