/* terminated , soft decision         */

#define repetition 100
#define k 100000                           //length of input bit sequence
#define m 6                                //memory order = 6
#define st_num (int)pow(2,m)
#define k_m (k+m)
#define path 100


#define lower 4.5//SNR                     //lower bound of SNR sampling value
#define upper 14//SNR                     //upper bound of SNR sampling value

#define start 0                           //there are 20 sample points         
#define end 2                           //loop runs for i = start ~ end-1 

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include"gaussian.h"
#include"transmitter.h"

#include"receiver_2.h"



int main(){                                //polar NRZ , A = +- 0.1   T = 20^-3.
    
    int i,j=0,t,h;
    int *u,*v,*y,error;
    int s,min,hour;
    double snr[20],ber[20];
    FILE	*fp_output;
    char data[20]={"data.txt"};
    
    long temptime1,temptime2,temptime3,temptime4,temptime5;                           //using system time to choose a seed
    srand(time(&temptime1)%60);
    
    
    u=(int *) malloc((k+m)*sizeof(int));
    v=(int *) malloc((2*(k+m))*sizeof(int));
    y=(int *) malloc((k+m)*sizeof(int));
    for(i=0;i<k;i++){
          y[i]=-1;
          }
    
    
    
    fp_output=fopen(data,"w");

    for (i=start;i<end;i++){                                //sample SNR value
        snr[i]=lower+(float(upper-lower)/19)*i;
        }

    for (i=start;i<end;i++){                  //主迴圈 
        
      printf("第%2d圈\n",i);
      error=0;  
      
      time(&temptime4);
      //for (t=0;error<10;t++){               //repeat
      for (t=0;t<repetition;t++){  
        
        
        
        input(u,v);
        
        /*for(h=0;h<k;h++){
              printf("%d",u[h]);
              }
        printf("\n");*/
        
        output(v,snr[i],y);
        
        /*for(h=0;h<k;h++){
              printf("%d",y[h]);
              }
        printf("\n");
        system("pause");*/
        j=0;
        while (j<k){
              if (u[j]!=y[j]) error++;
              j++;
              }
        //system("pause");
        printf("t = %d\n",t);
        //free(y);
        
        }
      time(&temptime5);
      
      ber[i]=error/(float)(k*t);
        
      printf("SNR = %f\n",snr[i]);
      fprintf(fp_output,"SNR = %f\n",snr[i]);
      printf("BER = %e",ber[i]); 
      fprintf(fp_output,"BER = %e\n",ber[i]);
      
      s=temptime5-temptime4;
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
      printf("...%2d小時 %2d分 %2d秒\n",hour,min,s); 
      }
    free(u);
    //free(v);
    //free(y);                                     // will cause crash?? 
    
    time(&temptime2);                            //calculate execution time
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
