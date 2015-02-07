/*
 * FFT & IFFT for digital signal processing comuputer exercise. 
 *
 * Author: Ling-San Meng
 * Email: f95942117@ntu.edu.tw
 *
 * Date: 2010/1/8
 */

#include "fft.h"

#include <iostream>
#include <cmath>
#include <fstream>
#include <complex>

using namespace std;

int main()
{
   const int N = 1024;
   const int M = 10;
   
   // Generate x.
   complex<double> x[N +1]; x[0] = N;
   for (int i=0;i<N;i++)
   {
      x[i+1] = sin((double)2.0*pi*i/N) +
               sin((double)2.0*pi*i/N*5) +
               sin((double)2.0*pi*i/N*10);
      x[i+1] /= 3.0;
   }
   
   // Perform ifft on x.
   complex<double> y[N +1]; y[0] = N;
   fft(x,y,N,M,1);

   // Perform fft on y.
   complex<double> z[N +1]; z[0] = N;
   fft(y,z,N,M,-1);
   
   // File output.   
   ofstream data;
   data.open("x.txt",ios::out);
   for (int i=1;i<=N;i++) { data << abs(x[i]) << endl; }

   ofstream data2;
   data2.open("y.txt",ios::out);
   for (int i=1;i<=N;i++) { data2 << abs(y[i]) << endl; }

   ofstream data3;
   data3.open("z.txt",ios::out);
   for (int i=1;i<=N;i++) { data3 << abs(z[i]) << endl; }
   
   cout << endl;
   cout << "Data printed. Press enter."<< endl;
   getchar();
   
   return 0;
}
