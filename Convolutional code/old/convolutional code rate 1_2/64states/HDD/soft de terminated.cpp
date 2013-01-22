/* terminated , hard decision*/

#define repetition 1
#define k 1000                          //length of input bit sequence
#define m 6                                //memory order = 6
#define k_m k+m

#define lower 0//SNR                     //lower bound of SNR sampling value
#define upper 9.5//SNR                     //upper bound of SNR sampling value

#define start 0                           //there are 20 sample points         
#define end 13                             //loop runs for i = start ~ end-1 

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include"gaussian.h"
#include"transmitter.h"
#include"receiver_2.h"

int main(){                                //polar NRZ , A = +- 0.1   T = 0.001
    
    int i,j=0,t,temp;
    int *u,*v,*y,bit_error,packet_error;
    int s,min,hour;
    double snr[20],ber[20];
    FILE	*fp_output;
    char data[20]={"data.txt"};
    
    long temptime1,temptime2,temptime3,temptime4,temptime5;
    srand(time(&temptime1)%60);                             //seed
    
    u=(int *) malloc((k+m)*sizeof(int));
    v=(int *) malloc((2*(k+m))*sizeof(int));
    y=(int *) malloc((k+m)*sizeof(int));
    
    fp_output=fopen(data,"w");

    for (i=start;i<end;i++){                                //sample SNR value
        snr[i]=lower+(float(upper-lower)/19)*i;
        }

    table1();                                               //initialize cache
    
    for (i=start;i<end;i++){
        
      printf("第%2d圈\n",i);
      bit_error=0;  
      packet_error=0;
      
      time(&temptime4);
      //for (t=0;bit_error<100;t++){          //3 選 1  謝謝 
      for (t=0;packet_error<100;t++){
      //for (t=0;t<repetition;t++){  
        
        input(u,v);                          //generate encoded sequece
        output(v,snr[i],y);                  //generate output under a SNR
        
        temp = bit_error;
        
        j=0;
        while (j<k){
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
      
      ber[i]=bit_error/(float)(k*t);
        
      printf("SNR = %f\n",snr[i]);
      fprintf(fp_output,"SNR = %f\n",snr[i]);
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
    free(u);
    //free(v);
    //free(y);                                     // will cause crash?? 
    
    time(&temptime2);                              //calculate execution time
    s=temptime2-temptime1;
    hour=s/3600;
    s=s%3600;
    min=s/60;
    s=s%60;
    
    fprintf(fp_output,"--------------------------\n");
    printf("--------------------------\n");
    
    for (i=start;i<end;i++){                          // print SNR & BER
        fprintf(fp_output,"%f\n",snr[i]);
        printf("%f\n",snr[i]);
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
