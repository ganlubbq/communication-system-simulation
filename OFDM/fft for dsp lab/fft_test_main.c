/* 
 */
 
#include <math.h>                                            
#include <stdio.h>
#include <stdlib.h>
                                                    
#define pi (3.14159265358979323846)
#define Nfft 8
#define CP 2
#define S 2                                
#define maskfft (Nfft-1)
#define stage (int)((float)log(Nfft)/log(2))

#define Qa 14
#define Qb 8

#include "fft.h"
//#include "dft.h"

int main(){
    struct complex x[S*Nfft+S*CP]={0};//x is the ofdm symbol samples x[n], y is the constellation points X[k]
    struct complex y[S*Nfft]={0};
    struct complex test[S*Nfft]={0};
    int i,n;
    
    /*printf("% 5d\n",(Qa-((6>>1)+2)-1));
    system("pause");*/
    
    
    fft_coef(w,Nfft);
  
    
    y[0].im=16383;
    y[1].im=16383;
    y[2].im=-16384;
    y[3].im=-16384;
    y[4].im=-16384;
    y[5].im=16383;
    y[6].im=16383;
    y[12].im=16383;
    y[15].im=16383;
    y[Nfft-1].im=-16384;
    
    for (i=0;i<S;i++){
         ifft(y,x,Nfft,i);
         }
    
    
    //ifft(y,x,Nfft);
    
    //Add Cyclic prefix
    /*for (i=0;i<S;i++){
         for (n=0;n<CP;n++){           
             x[i*(Nfft+CP)+n].re=x[i*(Nfft+CP)+Nfft+n].re;
             x[i*(Nfft+CP)+n].im=x[i*(Nfft+CP)+Nfft+n].im;
             }
         }*/
         
    for (i=0;i<S*Nfft+S*CP;i++){
        printf("% 5d ",x[i].re);
        printf("% 5d\n",x[i].im);
        }
    
    system("pause");
    
    fft(x,test,Nfft);

    for (i=0;i<S*Nfft;i++){
        printf("% d\n",test[i].im);
        }
    
    system("pause");
    }
    
    
    
