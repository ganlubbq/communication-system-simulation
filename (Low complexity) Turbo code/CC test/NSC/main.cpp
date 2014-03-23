/* This is a turbo code simulation of generalized setup
 * terminated, SDD, rate 1/3 or 1/2
 * Date : 2007/2/16
 */

#define G11 035
#define G12 023

#define m 4                                  //memory order = m
#define st_num 16                            //number of states = 2^m

#define k 1000                       //frame length

#define start 1                     //starting SNR value
#define end 1                      //ending SNR value

#define ERROR 9999999                   //number of accumulated packet errors 
#define step 0.2                     //spacing of SNR values

#define X 250

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
    int u[k+m],v[(k+m)*3],y[k],bit_error,packet_error;
    int points=(int)((float)(end-start)/step) + 1;
    float *snr,*snr_db,*ber;
    int pac[2][X]={0};//2 row X column
    float Pc[X]={0};//probability of correctness
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
      
        for (t=0;packet_error<ERROR;t++){
        //for (t=0;t<repetition;t++){  
            
            sum=0;
            input(u,v);                          //generate encoded sequece
            output(v,snr[i],y);                  //generate output under a SNR
        
            temp = bit_error;
        
            j=0;
            while (j<k){
                  if (u[j]!=y[j]) bit_error++;
                  j++;
            }
            
            if(bit_error > temp) {//this pac is erroneous
              packet_error++;
              printf("packet error = %d, ",packet_error);
              printf("sum = %f\n",sum);
              pac[1][(int)sum/10]++;
            }
            else{                //this pac is correct
              printf("packet correct, ");
              printf("sum = %f\n",sum);
              pac[0][(int)sum/10]++;
            }
        
        }
        
        //check
        /*printf("simulated pac = %d\n",t);
        counter=0;
        for (j=0;j<X;j++){
             counter+=pac[0][j];
             counter+=pac[1][j];
        }  
        printf("check = %d\n",counter);
        getchar();*/
        
        //cal Pc
        for (j=0;j<X;j++){
             if (pac[0][j]+pac[1][j]==0) Pc[j]=-0.01;
             else Pc[j]=(float)pac[0][j]/(pac[0][j]+pac[1][j]);
        }  
        
        ber[i]=bit_error/(float)(k*t);
        
        printf("SNR = %f\n",snr_db[i]);
        fprintf(fp_output,"SNR = %f\n",snr_db[i]);
        printf("BER = %e\n",ber[i]); 
        fprintf(fp_output,"BER = %e\n",ber[i]);
      
        temp1=i+1;
    }
    
    for (j=0;j<X;j++){
        printf("%4d ~ %4d   ",j*10,j*10+9);
        printf("%4.1f%%\n",Pc[j]*100);
    }  
    
    
    getchar();
    
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
