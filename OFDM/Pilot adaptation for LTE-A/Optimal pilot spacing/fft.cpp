#include <cmath>

#include "fft.h"

#define S 1            

const double pi = 3.141592653589793;

complex<double> *w;

int hasInit = 0;

short *order;

short stage; // = log(npoints)/log(2));

complex<double> sqrt_npoints_1; // = 1/sqrt(npoints)

void fft_coef(complex<double> *w,int npoints)
{
     short i,j,position;

     for (i=0;i<npoints>>1;i++){
         real(w[i])=cos(2*pi*i/npoints);
         imag(w[i])=-sin(2*pi*i/npoints);
         }
         
      for (i=0;i<npoints;i++){
        position=0;
        for (j=0;j<stage;j++){
            position+=(((i>>j)&1)<<(stage-1-j));
            }
        
        order[i]=position;
        }
     }

void ifft(complex<double> *x,complex<double> *r,int npoints, int CPLen)
{
     short s,i,j,k,n;
     short offset,branch;
     short position;
     complex<double> *input,*output,*temp,temp1,temp2;
  
     if (hasInit == 0)
     {
        w = new complex<double> [npoints>>1];
        order = new short [npoints];
        stage = (int)((double)log(npoints)/log(2));
        sqrt_npoints_1 = 1.0/sqrt(npoints);
        fft_coef(w,npoints);
        hasInit = 1;
     }
 
     input  = new complex<double> [npoints];
     output = new complex<double> [npoints];

     for (s=0;s<S;s++){
     
     /*
     for (i=0;i<npoints;i++){
        position=0;
        for (j=0;j<stage;j++){
            position+=(((i>>j)&1)<<(stage-1-j));
            }
        
        input[i]=x[s*npoints+position];
        }*/
     for (i=0;i<npoints;i++){
        input[i]=x[s*npoints+order[i] +1];
        }
        
        
     for (i=0;i<stage;i++){//loop for the main 3 stages
     
         for (j=0;j<npoints>>(i+1);j++){//loop for the processing blocks
             
             branch=(1<<i);//half branch
             offset=j*(branch<<1);//offset to the branch number now
             
             for (k=0;k<branch;k++){
                 temp1=input[offset+k];
                 
//                 real(temp2) = real(input[offset+branch+k])*real(w[k<<(stage-i-1)])+imag(input[offset+branch+k])*imag(w[k<<(stage-i-1)]);
//                 imag(temp2) = -real(input[offset+branch+k])*imag(w[k<<(stage-i-1)])+imag(input[offset+branch+k])*real(w[k<<(stage-i-1)]);
                 temp2 = input[offset+branch+k] * conj(w[k<<(stage-i-1)]);
                 
                 //upper branches 
                 real(output[offset+k])=real(temp1+temp2);
                 imag(output[offset+k])=imag(temp1+temp2);
                 
                 //lower branches
                 real(output[offset+branch+k])=real(temp1-temp2);//complex add
                 imag(output[offset+branch+k])=imag(temp1-temp2);
                 }
             }
             
         //handoff in out
         temp=input;
         input=output;
         output=temp;
         }

     for (i=0;i<npoints;i++)
         {
         r[s*npoints+(s+1)*CPLen+i +1] = input[i]*sqrt_npoints_1;
         }
     }
     
     delete [] input;
     delete [] output;
}


void fft(complex<double> *x,complex<double> *r,int npoints, int CPLen)
{
     short s,i,j,k,n;
     short offset,branch;
     short position;
     complex<double> *input,*output,*temp,temp1,temp2;

     if (hasInit == 0)
     {
        w = new complex<double> [npoints>>1];
        order = new short [npoints];
        stage = (int)((double)log(npoints)/log(2));
        sqrt_npoints_1 = 1.0/sqrt(npoints);
        fft_coef(w,npoints);
        hasInit = 1;
     }
 
     input  = new complex<double> [npoints];
     output = new complex<double> [npoints];
     
     for (s=0;s<S;s++){
         
     /*for (i=0;i<npoints;i++){
        position=0;
        for (j=0;j<stage;j++){
            position+=(((i>>j)&1)<<(stage-1-j));
            }
        
        input[i]=x[s*npoints+(s+1)*CPLen+position];
        }*/
     for (i=0;i<npoints;i++){
        input[i]=x[s*npoints+(s+1)*CPLen+order[i] +1];
        }
               
     for (i=0;i<stage;i++){//loop for the main 3 stages
     
         for (j=0;j<npoints>>(i+1);j++){//loop for the processing blocks
             
             branch=(1<<i);//half branch
             offset=j*(branch<<1);//offset to the branch number now
             
             for (k=0;k<branch;k++){
                 temp1=input[offset+k];
                 
//                 real(temp2) = real(input[offset+branch+k])*real(w[k<<(stage-i-1)])-imag(input[offset+branch+k])*imag(w[k<<(stage-i-1)]);
//                 imag(temp2) = real(input[offset+branch+k])*imag(w[k<<(stage-i-1)])+imag(input[offset+branch+k])*real(w[k<<(stage-i-1)]);
                 temp2 = input[offset+branch+k] * w[k<<(stage-i-1)];
                 
                 //upper branches 
                 output[offset+k]=temp1+temp2;
                 
                 //lower branches
                 output[offset+branch+k]=temp1-temp2;//complex add
                 }
             }
       
         //handoff in out
         temp=input;
         input=output;
         output=temp;
         }

     for (i=0;i<npoints;i++)
         {
//         r[s*npoints+(s+1)*CPLen+i +1] = input[i]*temp;
         r[s*npoints+i +1] = input[i]*sqrt_npoints_1;
         }
     
     }
     
     delete [] input;
     delete [] output;
}
     
