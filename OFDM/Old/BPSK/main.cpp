/*msb ~~ lsb*/
/*convolutional code rate 1/2*/
#define G11 0133
#define G12 0171

#define m 6                                  //memory order = m
#define st_num 64                            //number of states = 2^m

#define K 1018                               //pac size
#define pacERROR 100                         //number of pac errors to accumulate

#define N 128                                //OFDM subcarrier number
#define CP N/4                               //Cyclic prefix length, default to %25
 
#define lower 0//SNR in db                   //beginnig SNR value
#define upper 19//SNR in db                  //last SNR value, spacing = (upper-lower)/19

#define start 0                              //simulates from SNR = start, start+1*spacing..., start+(end-1)*spacing   
#define end 5                                

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include"declaration.h"
#include"gaussian.h"
#include"transmitter.h"
#include"channel.h"
#include"receiver_2.h"

int main(){
    
    int i,j=0,t,temp;
    int s,min,hour;
    int u[(K+m)],v[2*(K+m)],y[(K+m)],bit_error,packet_error;
    struct complex x[(K+m)*2 + (K+m)*2/N*CP];
    float snr[20],snr_db[20],ber[20];
    
    FILE *fp_output;
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
      bit_error=0;  
      packet_error=0;
      
      time(&temptime4);
      //for (t=0;bit_error<100;t++){
      for (t=0;packet_error<pacERROR;t++){
        
        input(u,v,x);                         //generate encoded sequece
        channel(x,snr[i]);                    //simulate channel effect
        output(x,y);                          //generate output under a SNR
        
        temp = bit_error;
        
        j=0;
        while (j<K){
              if (u[j]!=y[j]) bit_error++;
              j++;
              }
        
        if(bit_error > temp) {
           packet_error++;
           printf("packet error = %d\n",packet_error);
           }
        
        //printf("t = %d\n",t);
        
        
        /*if (i==0&&t==99){                          //time pre for fixed loop
            time(&temptime5);    
            s=temptime5-temptime4;
            s=s*((end-start)*repetition/100 - 1);
            hour=s/3600;
            s=s%3600;
            min=s/60;
            s=s%60;
            printf("預估 %2d小時 %2d分 %2d秒\n",hour,min,s);
            }*/
        }
      //time(&temptime5);                          //time cal for fixed loop
      
      ber[i]=bit_error/(float)(K*t);
        
      printf("SNR = %f\n",snr_db[i]);
      fprintf(fp_output,"SNR = %f\n",snr_db[i]);
      printf("BER = %e\n",ber[i]); 
      fprintf(fp_output,"BER = %e\n",ber[i]);
      
      /*s=temptime5-temptime4;                     //time cal for fixed loop
      hour=s/3600;
      s=s%3600;
      min=s/60;
      s=s%60;
      printf("...%2d小時 %2d分 %2d秒",hour,min,s); 
      
      s=temptime5-temptime1;
      hour=s/3600;
      s=s%3600;
      min=s/60;
      s=s%60;
      printf("...%2d小時 %2d分 %2d秒\n",hour,min,s);*/ 
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
