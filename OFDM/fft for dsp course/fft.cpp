#include "fft.h"

#include <cmath>
#include <iostream>
using namespace std;

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

void fft(complex<double> *x,complex<double> *X,int npoints, int M, int KIND)
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
            if (KIND == 1)
            { temp2 = input[offset+branch+k] * w[k<<(stage-i-1)]; }
            else if (KIND == -1)
            { temp2 = input[offset+branch+k] * conj(w[k<<(stage-i-1)]); }

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
