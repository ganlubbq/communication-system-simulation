#include "fft.h"

#include <cmath>
#include <iostream>
using namespace std;

#define S 1            

const double pi = 3.141592653589793;

complex<double> *w;

int hasInit = 0;

int *order;

int stage; // = log(npoints)/log(2));

double sqrt_npoints_1; // = 1/sqrt(npoints)

void fft_coef(complex<double> *w,int npoints)
{
   int i,j,position;

   for (i=0;i<npoints>>1;i++)
   {
       real(w[i]) =  cos(2*pi*i/npoints);
       imag(w[i]) = -sin(2*pi*i/npoints);
   }

   for (i=0;i<npoints;i++)
   {
      position=0;

      for (j=0;j<stage;j++) { position+=(((i>>j)&1)<<(stage-1-j)); }

      order[i]=position;
   }  
}

void init(int npoints)
{
   w = new complex<double> [npoints>>1];
   order = new int [npoints];

   stage = 0;
   int temp = npoints;
   while ((temp&1) == 0) { temp >>= 1; stage++; }

   sqrt_npoints_1 = 1.0/sqrt(npoints);

   fft_coef(w,npoints);

   hasInit = 1;
}

void ifft(complex<double> *x,complex<double> *r,int npoints, int CPLen)
{
   int i,j,k,n;
   int offset,branch;
   int position;
   complex<double> *input,*output,*temp,temp1,temp2;

   if (hasInit == 0) { init(npoints); }

   input  = new complex<double> [npoints];
   output = new complex<double> [npoints];

   for (i=0;i<npoints;i++) { input[i]=x[order[i] +1]; }

   //loop for the main 3 stages
   for (i=0;i<stage;i++)
   {
      //loop for the processing blocks
      for (j=0;j<npoints>>(i+1);j++)
      {
         branch=(1<<i);//half branch
         offset=j*(branch<<1);//offset to the branch number now

         for (k=0;k<branch;k++)
         {
            temp1 = input[offset+k];
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
      r[CPLen+i +1] = input[i]*sqrt_npoints_1;
   }

   delete [] input;
   delete [] output;
}


void fft(complex<double> *x,complex<double> *X,int npoints, int CPLen)
{
   int i,j,k,n;
   int offset,branch;
   int position;
   complex<double> *input,*output,*temp,temp1,temp2;

   if (hasInit == 0) { init(npoints); }
   
   input  = new complex<double> [npoints];
   output = new complex<double> [npoints];

   for (i=0;i<npoints;i++) { input[i]=x[CPLen+order[i] +1]; }

   //loop for the main 3 stages
   for (i=0;i<stage;i++)
   {
      //loop for the processing blocks
      for (j=0;j<npoints>>(i+1);j++)
      {
         branch=(1<<i);//half branch
         offset=j*(branch<<1);//offset to the branch number now

         for (k=0;k<branch;k++)
         {
            temp1=input[offset+k];

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
      X[i +1] = input[i]*sqrt_npoints_1;
   }

   delete [] input;
   delete [] output;
}


complex<double> *w_dft;

void initCoef(int Nfft)
{
   const double sqrt_Nfft = sqrt((double)Nfft);
   
   w_dft = new complex<double> [Nfft];
   for (int i=0;i<Nfft;i++)
   {
      real(w_dft[i]) =  cos(2.0*pi*i/Nfft)/sqrt_Nfft;
      imag(w_dft[i]) = -sin(2.0*pi*i/Nfft)/sqrt_Nfft;
   }
}

void dft(complex<double> *x,complex<double> *X,int Nfft)
{
   static int hasInit = 0;
   if (hasInit == 0)
   {
      initCoef(Nfft);
      hasInit = 1;
   }
   
   for (int k=1;k<=Nfft;k++)
   {
      X[k] = 0;
      for (int n=1;n<=Nfft;n++)
      {
          int coefIdx = (k-1)*(n-1);
          while (coefIdx >= Nfft) { coefIdx -= Nfft; }

          X[k] += x[n]*w_dft[coefIdx];
      }
   }
}
