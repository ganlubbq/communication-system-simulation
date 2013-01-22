/* 
   This is a simple test program for the cc and viterbi functions
   Multisample encoding
   CC pac size = B*N
*/
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

#define B 32 //quantization bits
#define N 4  //number of samples to be encoded

#include "viterbi.h"

int x[N]={0xae58,0xea24,0x28a4};//sample1, sample2, sample3...
int v[(B*N+m)*2],y[B*N];
float r[(B*N+m)*2];
     
int main(){
    int i,j;
    /*print from the msb of the last element in x*/
    
    printf("<- sample3 <- sample2 <- sample1\n");
    
    for (i=N-1;i>=0;i--){
         printf("<-0x");
         for (j=0;j<B;j++){
              printf("%d",(x[i]>>(B-j-1))&mask);
              }
         }
    printf("\n");
    
    cc(x,v);
    
    /*idiot channel*/
    for (i=0;i<(B*N+m)*2;i++){
         r[i]=2.0*v[i]-1;
         }

    viterbi(r,y);
    
    
    for (i=0;i<B*N;i++){
         if(i%B==0)printf("<-0x");
         printf("%d",y[B*N-i-1]);
         }
    
    getchar();
    }
