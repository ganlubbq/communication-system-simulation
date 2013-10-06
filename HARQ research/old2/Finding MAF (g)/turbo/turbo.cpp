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
   
    const int k = d[0];
    
    int i,j,l,t0,t1;

    int a[k+m+m];
    int b[k+m+m];
    for (int i=0;i<k+m+m;i++) { a[i] = 0;b[i] = 0; }
    
    int temp[k];
    int _d[k+m];
    for (int i=0;i<k;i++) { _d[i] = d[i +1];temp[i] = _d[i]; }
    
    for (i=0;i<k;i++){//encoding a
        for (j=1;j<m+1;j++){
            a[i+m]^= a[i-j+m]*g11[j];
        }
        a[i+m]^= _d[i];
    }

    //add tail bits so that the 1st shift register goes to 0 state
    for (i=k;i<k+m;i++){
        _d[i]=0;
        for (j=1;j<m+1;j++){
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
    
    for (i=0;i<k;i++){//encoding b
        for (j=1;j<m+1;j++){
            b[i+m]^= b[i-j+m]*g11[j];
        }
        b[i+m]^= temp[i];
    }

    int *v = new int [3*(k+m) +1];
    v[0] = 3*(k+m);
   
    for (i=0;i<(k+m);i++){//encoding
        t0= 0;
        t1= 0;
        
        for (j=0;j<m+1;j++){
            t0^=a[i-j+m]*g12[j];
            t1^=b[i-j+m]*g12[j];
        }
        
        v[3*i +1]  = _d[i];
        v[3*i+1 +1]= t0;
        v[3*i+2 +1]= t1;
    }

    return v;
}

void siso(int,float *,int,float,float *,float *,int);

int *bcjrDecode(float *r, float N0, int iteration)
{
    const int k = (int)r[0]/3 - m;

    float x[k+m];
    float y1[k+m];
    float y2[k+m];
    
    float x_metric[k+m];
    float LLR[k+m];
    float z[k+m];
    
    for (int i=0;i<k+m;i++)
    {   
       x[i]  = r[3*i +1];
       y1[i] = r[3*i+1 +1];
       y2[i] = r[3*i+2 +1];
       
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

void siso(int pass,float *y,int option,float N0,float *LLR,float *z,int k){
     int i,j,h,l,s,b,count0,count1;
     int out1,out2,temp1;
     float temp[2],t0[st_num],t1[st_num],test,temp2,temp3,m0,m1,b0,b1;
     float (*pI_sym)[4][2] = new float [k+m][4][2];
     float (*A)[st_num] = new float [k+m+1][st_num]; 
     float (*B)[st_num] = new float [k+m+1][st_num]; 
 
     for (j=0;j<st_num;j++){
         A[0][j]=-100000;
         B[k+m][j]=-100000;
         }
     
     A[0][0]=0;
     B[k+m][0]=0;
     
     //branch metric Pk[c(e);I]        
     for (j=0;j<k+m;j++){
          
          for (h=0;h<4;h++){
                 b0=2*(h>>1)-1;
                 b1=2*(h&1)-1;
              
                 //1 for complete path metric ,0 for extrisic compute
                 pI_sym[j][h][1]= b0*z[j]/2 + b0*LLR[j]/2 + b1*y[j]*2/N0;
                 pI_sym[j][h][0]= b1*y[j]*2/N0;
               }
          }

     //forward recursion A
     for (j=1;j<k+m+1;j++){
        for (l=0;l<st_num;l++){
         for (h=0;h<2;h++){//for input 0 , 1
             
             out1=from_out_table[l][0+2*h];
             out2=from_out_table[l][1+2*h];
             temp1=2*out1+out2;
             
             test=pI_sym[j-1][temp1][1];
             temp[h]=A[j-1][from_table[l][h]]+test;
             }
          A[j][l]=logsum(temp[0],temp[1]);
          //A[j][l]=max(temp,2);
          }
       } 
       
     //backward recursion B
     for (j=k+m-1;j>=0;j--){
        for (l=0;l<st_num;l++){
         for (h=0;h<2;h++){//for input 0 , 1
             
             out1=out_table[l][0+2*h];
             out2=out_table[l][1+2*h];
             temp1=2*out1+out2;
             
             test=pI_sym[j][temp1][1];
             temp[h]=B[j+1][next_table[l][h]]+test;
             }
          B[j][l]=logsum(temp[0],temp[1]);
          //B[j][l]=max(temp,2);
          }
        }
       
     //extrinsic information
     for (j=0;j<k;j++){ // K
          count0=0;count1=0;
          for (l=0;l<st_num;l++){    // state
               for (h=0;h<2;h++){
                   if (h == 0){        
                      out1=out_table[l][0+2*h];
                      out2=out_table[l][1+2*h];
                      
                      temp1=2*out1+out2;
                      
                      t0[count0]=A[j][l]+B[j+1][next_table[l][h]]+pI_sym[j][temp1][0];  
                      count0++;
                      }
                   else{
                      out1=out_table[l][0+2*h];
                      out2=out_table[l][1+2*h];
             
                      temp1=2*out1+out2;
                      
                      t1[count1]=A[j][l]+B[j+1][next_table[l][h]]+pI_sym[j][temp1][0];   
                      count1++;
                      }
                   }
               }
               
          temp2=logsum(t0[0],t0[1]);
          temp3=logsum(t1[0],t1[1]);
          
          for (s=2;s<st_num;s++){
              temp2=logsum(temp2,t0[s]);
              temp3=logsum(temp3,t1[s]);
              }
  
          if(option==1){
            LLR[j]=temp3-temp2;
            }
          
          else {
            z[j]=temp3-temp2;
            }
          }
  
     delete [] (*pI_sym);
     delete [] (*A);
     delete [] (*B);
     }


