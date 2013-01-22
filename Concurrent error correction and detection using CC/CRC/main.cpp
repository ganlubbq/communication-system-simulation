/*msb ~~ lsb*/
/*CRC HARQ*/
#define G11 0133
#define G12 0171

#define m 6                                  //memory order = m
#define st_num 64                            //number of states = 2^m

#define r 0                                  //CRC-r
#define GCRC 0xB                            //CRC generator

#define k 500                               //pac size
#define pacERROR 500                         //accumulated pac errors

#define lower 2//SNR                         //lower bound of SNR sampling value
#define upper 9.5//SNR                         //upper bound of SNR sampling value

#define start 0                              //there are 20 sample points         
#define end 1                               //loop runs for i = start ~ end-1  

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
    
    int i,j=0,t,temp,count=0;
    int u[(k+m)],v[2*(k+m)],y[(k+m)],bit_error,packet_error,bit_error1,wrong=0,resend=0;
    int s,min,hour;
    float snr[20],snr_db[20],ber[20],per[20],throughput[20],Pd[20],Pw[20];;
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
      resend=0;
      wrong=0;
      bit_error=0;  
      packet_error=0;
      count=0;
      
      time(&temptime4);
      //for (t=0;bit_error<100;t++){
      for (t=0;packet_error<pacERROR;t++){
        input(u,v);                          //generate encoded sequece
        output(v,snr[i],y);                  //generate output under a SNR
        
        if(retransmit==1){
          temp = bit_error1;
        
          j=0;
          while (j<k-r){
                if (u[j]!=y[j]) bit_error1++;
                j++;
                }
        
          if(bit_error1 == temp) {
             wrong++;
             }
     
          t--;
          count++;
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
           printf("packet error = %d\n",packet_error);
           }
        //printf("processed pac = %d\n",t); 
        
        count++;
        }
      
      ber[i]=bit_error/(float)((k-r)*t);
      per[i]=pacERROR/(float)t;
      Pd[i]=(resend-wrong)/(float)(resend-wrong+pacERROR);
      Pw[i]=wrong/(float)(t-pacERROR+wrong);
      throughput[i]=(k-r)*t/k/(float)count;
      
      printf("SNR = %f\n",snr_db[i]);
      fprintf(fp_output,"SNR = %f\n",snr_db[i]);
      printf("BER = %e\n",ber[i]); 
      fprintf(fp_output,"BER = %e\n",ber[i]);
      printf("PER = %f\n",per[i]); 
      fprintf(fp_output,"PER = %f\n",per[i]);
      printf("Throughput = %f\n",throughput[i]); 
      fprintf(fp_output,"Throughput = %f\n",throughput[i]);
      
      //Throughput = received / processed
      printf("重傳率 = %f\n",resend/(float)count);//重傳率=1-Throughput;要求重傳的比率   
      printf("抓錯率 = %f (辨識出錯誤封包的能力)\n",Pd[i]);
      printf("誤殺率 = %f (無誤卻要求重傳的比率)\n",Pw[i]); 
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
        fprintf(fp_output,"%f\n",per[i]);             // print PER
        printf("%f\n",per[i]);
        }
    
    fprintf(fp_output,"--------------------------\n");
    printf("--------------------------\n");
    for (i=start;i<end;i++){
        fprintf(fp_output,"%f\n",throughput[i]);      // print Throughput
        printf("%f\n",throughput[i]);
        }
    
    fprintf(fp_output,"--------------------------\n");
    printf("--------------------------\n");
    fprintf(fp_output,"抓錯率Pd\n");
    printf("抓錯率Pd\n");
    for (i=start;i<end;i++){
        fprintf(fp_output,"%f\n",Pd[i]);      // print Pd
        printf("%f\n",Pd[i]);
        }
        
    fprintf(fp_output,"--------------------------\n");
    printf("--------------------------\n");
    fprintf(fp_output,"誤殺率Pw\n");
    printf("誤殺率Pw\n");
    for (i=start;i<end;i++){
        fprintf(fp_output,"%f\n",Pw[i]);      // print Pw
        printf("%f\n",Pw[i]);
        }
    
    fprintf(fp_output,"--------------------------\n");
    printf("--------------------------\n");
    
    fprintf(fp_output,"你用了%2d小時 %2d分 %2d秒\n",hour,min,s);
    printf("你用了%2d小時 %2d分 %2d秒\n",hour,min,s);
    
    fclose(fp_output);
    
    system("pause");
    }
