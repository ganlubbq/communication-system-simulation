/* LDPC code simulation
 * Due to the memory limitation, array size is a problem
 * H generation is another problem
 * SPA decoding is not verified yet, problem too
 * Date : 2007/3/19
 * ------------------------------------------------------
 * Array size isn't a problem now, using dynamic malloc 
 * can declare an arbitrarily large array.
 * But I still can't find a full rank H. Even Gallager's H
 * is not full rank!
 * Date : 2007/3/21
 * ------------------------------------------------------
 * Fix a bug in extrinsic().
 * Change all the array declaration to malloc, there is no 
 * array size limit now.
 * Date : 2007/3/30
 * ------------------------------------------------------
 * There is still a decoding problem with the parity check matrix PA1H,
 * GallagerH and HammingH are both OK. Besides, the decoding performance
 * of Hamming is pretty good.
 * Date : 2007/3/31
 * PS : It seems that the identity part of H should be placed 
 * on the left side, or the decoding result will be reversed?
 * ------------------------------------------------------
 * Test large H from Alist
 * I think my code is not correct...
 * From a program develop perspective, this one is dead...
 * Date : 2007/4/4
 */
#define Imax 5

#define k 504                          //pac length
#define h 504                         //parity bits

#define start 0                      //starting SNR value
#define end 5                        //ending SNR value

#define ERROR 100                   //number of accumulated packet errors 
#define step 1                       //spacing of SNR values

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>


#include"declaration.h"
#include"gaussian.h"
#include"0transmitter.h"
#include"channel.h"
#include"decoder.h"


int main(){
    int i,j=0,t,temp,temp1;
    int *u,*v,*y;
    int bit_error,packet_error;
    int points=(int)((float)(end-start)/step) + 1;
    float *r;
    float *snr,*snr_db,*ber;
    int s,min,hour;
    FILE	*fp_output;
    char data[20]={"data.txt"};
    long temptime1,temptime2;
    
    declare();
    
    u=(int*)malloc(k*sizeof(int));
    v=(int*)malloc((k+h)*sizeof(int));
    y=(int*)malloc(k*sizeof(int));
    r=(float*)malloc((k+h)*sizeof(float));
    
    snr=(float*)malloc(points*sizeof(float));
    snr_db=(float*)malloc(points*sizeof(float));
    ber=(float*)malloc(points*sizeof(float));
    srand(time(&temptime1)%60);//seed
    fp_output=fopen(data,"w");
      
    
    for (i=0;i<points;i++){//sample SNR value
        snr_db[i]=start+i*step;
        snr[i]=pow(10,snr_db[i]/10);  
    }                                                
    
    for (i=0;i<points;i++){
        
        printf("第%2d圈\n",i);
        bit_error=0;  
        packet_error=0;
      
        for (t=0;packet_error<ERROR;t++){
            
            encode(u,v);
            
            awgn(v,snr[i],r);
            
            decode(r,y,(float)(k+h)/k/snr[i],H);
        
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
        }
     
        ber[i]=bit_error/(float)(k*t);
        
        printf("SNR = %f\n",snr_db[i]);
        fprintf(fp_output,"SNR = %f\n",snr_db[i]);
        printf("BER = %e\n",ber[i]); 
        fprintf(fp_output,"BER = %e\n",ber[i]);
      
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
    
    for (i=0;i<h;i++){
        free(*(H+i));
    }

    free(H);
    free(u);free(v);free(y);free(r);
    free(snr);free(snr_db);free(ber);
    
    system("pause");
}
