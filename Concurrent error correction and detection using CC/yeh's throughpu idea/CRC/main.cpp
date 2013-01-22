/*msb ~~ lsb*/
/*CRC + CC HARQ*/
#define G11 0133
#define G12 0171

#define m 6                                  //memory order = m
#define st_num 64                            //number of states = 2^m

#define r 5                                  //CRC-r
#define GCRC 0xB                            //CRC generator

#define N 5                                 //partition size
#define FRAME 100                             //frames to send

#define k 500                               //pac size
#define pacERROR 100                         //accumulated pac errors

#define lower 1.5//SNR                         //lower bound of SNR sampling value
#define upper 11//SNR                         //upper bound of SNR sampling value

#define start 0                              //there are 20 sample points         
#define end 10                               //loop runs for i = start ~ end-1  

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include"declaration.h"
#include"crc.h"
#include"gaussian.h"
#include"transmitter.h"
#include"receiver_2.h"

int main(){
    
    int i,j=0,t,temp,n,frame,fail=0,resend;
    int u[(k+m)],v[2*(k+m)],y[(k+m)],bit_error,packet_error;
    int s,min,hour;
    float snr[20],snr_db[20],ber[20],T[20];
    FILE	*fp_output;
    char data[20]={"data.txt"};
    declaration();
    
    long temptime1,temptime2,temptime3,temptime4,temptime5;
    srand(time(&temptime1)%60);                             //seed
 
    fp_output=fopen(data,"w");

    for (i=start;i<end;i++){                                //sample SNR value
        snr_db[i]=lower+(float(upper-lower)/19)*i;
        snr[i]=pow(10,snr_db[i]/10);  
        }
    
    for (i=start;i<end;i++){
        
      printf("第%2d圈 SNR=%.2f\n",i,snr_db[i]);
      bit_error=0;  
      packet_error=0;
      resend=0;
      frame=0;
      n=0;
      
      for (t=0;frame<FRAME;t++){
        input(u,v);                          //generate encoded sequece
        output(v,snr[i],y);                  //generate output under a SNR
        
        if(retransmit==1){
          resend++;
          continue;
          }
        
        temp = bit_error;
        
        j=0;
        while (j<k-r){
              if (u[j]!=y[j]) bit_error++;
              j++;
              }
        
        if(bit_error > temp) {
           packet_error++;
           fail=1;
           //printf("packet error = %d\n",packet_error);
           }
           
        n++;
        if  (n==N){
            
            if  (fail==1){
                fail=0; 
                }
        
            else{
                 frame++;
                 printf("frame transmitted = %d\n",frame);
                 }
            n=0;
            }     
        }
      
      ber[i]=bit_error/(float)((k-r)*(t-resend));
      T[i]=N*FRAME/(float)t;//t為傳送端傳送過的subpac數 
      
      printf("SNR = %f\n",snr_db[i]);
      fprintf(fp_output,"SNR = %f\n",snr_db[i]);
      printf("BER = %e\n",ber[i]); 
      fprintf(fp_output,"BER = %e\n",ber[i]);
      printf("THR = %f\n",T[i]); 
      fprintf(fp_output,"THR = %f\n",T[i]);
      }

    time(&temptime2);                              //calculate execution time
    s=temptime2-temptime1;
    hour=s/3600;
    s=s%3600;
    min=s/60;
    s=s%60;
    
    fprintf(fp_output,"--------------------------\n");
    printf("--------------------------\n");
    
    for (i=start;i<end;i++){                          // print SNR
        fprintf(fp_output,"%f\n",snr_db[i]);
        printf("%f\n",snr_db[i]);
        }
    
    fprintf(fp_output,"--------------------------\n");
    printf("--------------------------\n");
    for (i=start;i<end;i++){
        fprintf(fp_output,"%e\n",ber[i]);             // print BER
        printf("%e\n",ber[i]);
        }
    
    fprintf(fp_output,"--------------------------\n");
    printf("--------------------------\n");
    for (i=start;i<end;i++){
        fprintf(fp_output,"%f\n",T[i]);      // print Throughput
        printf("%f\n",T[i]);
        }
     
    fprintf(fp_output,"--------------------------\n");
    printf("--------------------------\n");
    
    fprintf(fp_output,"你用了%2d小時 %2d分 %2d秒\n",hour,min,s);
    printf("你用了%2d小時 %2d分 %2d秒\n",hour,min,s);
    
    fclose(fp_output);
    
    system("pause");
    }
