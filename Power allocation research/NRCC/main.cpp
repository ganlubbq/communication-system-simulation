/* This is a simulation of the power allocation of ECC
 * Date : 2007/3/6
 */
#define G11 035
#define G12 023

#define m 4                                  //memory order = m
#define st_num 16                            //number of states = 2^m

#define k 500                       //frame length

#define start 4                     //starting SNR value
#define end 4                      //ending SNR value

#define ERROR 6000                    //number of accumulated packet errors 
#define step 0.2                     //spacing of SNR values

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include"declaration.h"
#include"gaussian.h"
#include"transmitter.h"
#include"receiver_2.h"

int main(){
    int i,j=0,t,temp,temp1;
    int u[(k+m)],v[2*(k+m)],y[(k+m)],bit_error,packet_error;
    int points=(int)((float)(end-start)/step) + 1;
    int record[k]={0};
    float error_rate[k]={0};
//    float r[k+h];
    float *snr,*snr_db,*ber;
    int s,min,hour;
    FILE	*fp_output;
    char data[20]={"data.txt"};
    long temptime1,temptime2;
    declaration();
    
    snr=(float*)malloc(points*sizeof(float));
    snr_db=(float*)malloc(points*sizeof(float));
    ber=(float*)malloc(points*sizeof(float));
    srand(time(&temptime1)%60);//seed
    fp_output=fopen(data,"w");
      
    ber[-1]=1;
    
    for (i=0;i<points;i++){//sample SNR value
        snr_db[i]=start+i*step;
        snr[i]=pow(10,snr_db[i]/10);  
    }                                                
    
    //for (i=0;i<points;i++){
    for (i=0;i<points && ber[i-1]>1e-5 ;i++){//simulate till BER ~= 1e-5   
        printf("第%2d圈\n",i);
        bit_error=0;  
        packet_error=0;
        for (j=0;j<k;j++) {record[j]=0; error_rate[j]=0;}
        
        for (t=0;packet_error<ERROR;t++){
         
            
            input(u,v);                          //generate encoded sequece
            output(v,snr[i],y);                  //generate output under a SNR
        
            temp = bit_error;
            
            j=0;
            while (j<k){
                  if (u[j]!=y[j]) {
                     bit_error++;
                     record[j]++;
                  }
                  j++;
            }
        
            if(bit_error > temp) {
              packet_error++;
              printf("packet error = %d\n",packet_error);
            }
        
        }
        ber[i]=bit_error/(float)(k*t);
        
        printf("SNR = %f\n",snr_db[i]);
        fprintf(fp_output,"SNR = %f\n",snr_db[i]);
        printf("BER = %e\n",ber[i]); 
        fprintf(fp_output,"BER = %e\n",ber[i]);
        
        for (j=0;j<k;j++) error_rate[j]=(float)record[j]/bit_error;
        
        for (j=0;j<k;j++){
            printf("Pe[%3d] = %f\n",j,error_rate[j]);
        }
        for (j=0;j<k;j++){
            fprintf(fp_output,"%3d\n",j);
        }
        fprintf(fp_output,"--------------------------\n");
        for (j=0;j<k;j++){
            fprintf(fp_output,"%f\n",error_rate[j]);
        }
        system("pause");
        
        temp1=i+1;
    }
    
    time(&temptime2);//calculate execution time
    s=temptime2-temptime1;
    hour=s/3600;
    s=s%3600;
    min=s/60;
    s=s%60;
    
    fprintf(fp_output,"--------------------------\n");
    printf("--------------------------\n");
    
    for (i=0;i<temp1;i++){// print SNR & BER
        fprintf(fp_output,"%f\n",snr_db[i]);
        printf("%f\n",snr_db[i]);
    }
    
    fprintf(fp_output,"--------------------------\n");
    printf("--------------------------\n");
    for (i=0;i<temp1;i++){
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
