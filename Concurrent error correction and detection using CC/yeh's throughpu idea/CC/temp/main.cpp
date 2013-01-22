/*msb ~~ lsb*/
/*convolutional code rate 1/2*/
#define G11 0133
#define G12 0171

#define N 5                                  //partition number
#define pacSUCCESS 100                       //successfully received big packets

#define m 6                                  //memory order = m
#define st_num 64                            //number of states = 2^m

#define k 500                               //pac size
#define pacERROR 100                         //accumulated pac errors

#define lower 2//SNR                         //lower bound of SNR sampling value
#define upper 19//SNR                         //upper bound of SNR sampling value

#define start 0                              //there are 20 sample points         
#define end 1                                //loop runs for i = start ~ end-1  

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include"declaration.h"
#include"gaussian.h"
#include"transmitter.h"
#include"receiver_2.h"

int main(){
    
    int i,j=0,t,temp,subpac_processed;
    int u[(k+m)],v[2*(k+m)],y[(k+m)],bit_error,packet_error,pac_success;
    int s,min,hour;
    float snr[20],snr_db[20],ber[20];
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
        
      printf("第%2d圈\n",i);
      subpac_processed=0;
      
      //for (t=0;bit_error<100;t++){
      //for (t=0;packet_error<pacERROR;t++){
      for (pac_success=0;pac_success<pacSUCCESS;pac_success++){
        
         
        //packet_error=0;
        
        for (t=0;t<N;t++){
        
        bit_error=0; 
        
        input(u,v);                          //generate encoded sequence
        output(v,snr[i],y);                  //generate output
        
        //temp = bit_error;
        
        j=0;
        while (j<k){
              if (u[j]!=y[j]) bit_error++;
              j++;
              }
        
        if(bit_error > 0) {                 //wrong subpac!
           //packet_error++;
           //printf("packet error = %d\n",packet_error);
           subpac_processed=subpac_processed-t+N;
           t=;
           }
           
        subpac_processed++;
        }
        }
      
      //ber[i]=bit_error/(float)(k*t);
        
      printf("SNR = %f\n",snr_db[i]);
      fprintf(fp_output,"SNR = %f\n",snr_db[i]);
      printf("BER = %e\n",ber[i]); 
      fprintf(fp_output,"BER = %e\n",ber[i]);
      
      }

    time(&temptime2);                              //calculate execution time
    s=temptime2-temptime1;
    hour=s/3600;
    s=s%3600;
    min=s/60;
    s=s%60;
    
    fprintf(fp_output,"--------------------------\n");
    printf("--------------------------\n");
    
    for (i=start;i<end;i++){                          // print SNR & BER
        fprintf(fp_output,"%f\n",snr_db[i]);
        printf("%f\n",snr_db[i]);
        }
    
    fprintf(fp_output,"--------------------------\n");
    printf("--------------------------\n");
    for (i=start;i<end;i++){
        fprintf(fp_output,"%e\n",ber[i]);
        printf("%e\n",ber[i]);
        }
    
    fprintf(fp_output,"--------------------------\n");
    printf("--------------------------\n");
    
    fprintf(fp_output,"你用了%2d小時 %2d分 %2d秒\n",hour,min,s);
    printf("你用了%2d小時 %2d分 %2d秒\n",hour,min,s);
    
    fclose(fp_output);
    
    system("pause");
    }
