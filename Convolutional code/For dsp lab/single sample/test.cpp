/*This is a simple test program for the cc and viterbi functions*/
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

#define B 32 //quantization bits, B'd better not exceed (32-m)...

#include "viterbi.h"

int main(){
    int x=0xad8ce58;
    int v[(B+m)*2],y[B],i;
    float r[(B+m)*2];
      
    cc(x,v);
 
    /*print from the mst*/
    printf("0x");
    for (i=0;i<B;i++){
         printf("%d",(x>>(B-i-1))&mask);
         }
    
    /*idiot channel*/
    for (i=0;i<(B+m)*2;i++){
         r[i]=2.0*v[i]-1;
         }
          
    printf("\n");
    
    viterbi(r,y);
    
    printf("0x");
    for (i=0;i<B;i++){
         printf("%d",y[B-i-1]);
         }
    
    getchar();
    }
