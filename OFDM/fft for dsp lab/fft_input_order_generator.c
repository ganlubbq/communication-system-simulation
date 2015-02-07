/*  As Title!
 */
#include<stdio.h>
#include<stdlib.h>
#include <math.h>

#define pi (3.14159265358979323846)
#define Nfft 256
#define stage (int)((float)log(Nfft)/log(2))

int main(){
    int i,j,n,k;
    int temp;
    int position,bit;
    int x[Nfft],input[Nfft];
    int array[Nfft];
    
    FILE *fp_output;
    char data[20]={"data.txt"};
    fp_output=fopen(data,"w");

    for (i=0;i<Nfft;i++){
        position=0;
        for (j=0;j<stage;j++){
            position+=(((i>>j)&1)<<(stage-1-j));
            }
        array[i]=position;
        //input[i]=x[position];
        }    
    
    printf("----------------for IFFT-------------\n");
    fprintf(fp_output,"----------------for IFFT-------------\n");
    
    for (i=0;i<Nfft;i++){
        printf("input[%3d]=x[s*npoints+%3d];\n",i,array[i]);
        fprintf(fp_output,"input[%3d]=x[s*npoints+%3d];\n",i,array[i]);
        }   
        
    printf("----------------for FFT--------------\n"); 
    fprintf(fp_output,"----------------for FFT--------------\n");
       
    for (i=0;i<Nfft;i++){
        printf("input[%3d].re=(int)x[s*npoints+(s+1)*CP+%3d].re;\n",i,array[i]);
        printf("input[%3d].im=(int)x[s*npoints+(s+1)*CP+%3d].im;\n",i,array[i]);
        fprintf(fp_output,"input[%3d].re=(int)x[s*npoints+(s+1)*CP+%3d].re;\n",i,array[i]);
        fprintf(fp_output,"input[%3d].im=(int)x[s*npoints+(s+1)*CP+%3d].im;\n",i,array[i]);
        } 
            
    getchar();
    
    
    
    
    }
