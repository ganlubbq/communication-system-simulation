#include <math.h>                                            
#include <stdio.h>
#include <stdlib.h>
                                                    
#define pi (3.14159265358979323846)
#define Nfft 64
#define CP 0
#define S 1                                

float maxre,maxim;

#include "fft.h"



int main(){
    struct complex x[S*Nfft+S*CP]={0};//x is the ofdm symbol samples x[n], y is the constellation points X[k]
    struct complex y[S*Nfft]={0};
    struct complex test[S*Nfft]={0};
    long temptime1,temptime2,temptime3,temptime4,temptime5;
    srand(time(&temptime1)%60);                             //seed
 
    int i,n;
    
    fft_coef(w,Nfft);
    
    /*
    y[0].re=1;
    y[1].re=-1;
    y[2].re=-1;
    y[3].re=-1;
    y[4].re=1;
    y[5].re=1;
    y[6].re=-1;
    
    y[15].re=-1;
    y[24].re=1;
    y[27].re=1;
    y[Nfft-1].re=1;*/
    
    
    for (i=0;i<Nfft*S;i++){
        y[i].re=(2*(rand()%2)-1);
        }
    
    ifft(y,x,Nfft);
    

    
    //Add Cyclic prefix
    for (i=0;i<S;i++){
         for (n=0;n<CP;n++){           
             x[i*(Nfft+CP)+n].re=x[i*(Nfft+CP)+Nfft+n].re;
             x[i*(Nfft+CP)+n].im=x[i*(Nfft+CP)+Nfft+n].im;
             }
         
         }
    
    maxre=x[0].re;
    maxim=x[0].im;     
    for (i=0;i<S*Nfft+S*CP;i++){
        if(x[i].re>maxre)maxre=x[i].re;
        if(x[i].im>maxim)maxim=x[i].im;
        printf("% 7.2f ",x[i].re);
        printf("% 7.2f\n",x[i].im);
        }
    
    system("pause");
    printf("%max.re is : %7.2f ",maxre);
    printf("%max.im is : %7.2f ",maxim);
    system("pause");
    
    fft(x,test,Nfft);

    for (i=0;i<S*Nfft;i++){
        printf("% 7.2f\n",test[i].re);
        }
    
    system("pause");
    }
    
    
    
