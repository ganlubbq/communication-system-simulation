#define repetition 1000
#define k 4000                          //length of input bit sequence
#define m 6                               //memory order = 6
#define st_num (int)pow(2,m)

#define lower 4//SNR                     //lower bound of SNR sampling value
#define upper 9.5//SNR                     //upper bound of SNR sampling value

#define start 0                           //there are 20 sample points         
#define end 1                            //loop runs for i = start ~ end-1  

#define pi 3.1415926
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include"declaration.h"
#include"interleaver.h"
#include"gaussian.h"
#include"transmitter.h"
#include"modulation.h"
#include"receiver.h"

int main(){
    
    int i,j=0,t,temp;
    int bit_error,packet_error;
    declaration();
    table();

        
    input(u,u1,u2,v,v1,v2,v3);
 
    output(v,snr[i],y);    
    
    
    
    
    
    
    
    
    
    
    
    
    
    }
