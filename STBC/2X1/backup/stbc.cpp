#include "stbc.h"

#include <cmath>

#include <iostream>
using namespace std;

const float sqrt_1_2 = 0.707106781186548;
   
STBC::STBC()
{

}

STBC::~STBC()
{
                             
}

// Alamouti by defn. is 2X?. Structured as a double-sized 1-d array.
complex<float> *STBC::AlamoutiEncode(complex<float> *x)
{
   int symLen = (int)real(x[0]);
   
   // Check if the symLen is even.
   if ((symLen&1)==1)
   {
      cout << "Error in AlamoutiEncode(); symLen is not even" << endl;
      getchar();
   }
   
   complex<float> *z = new complex<float> [symLen*2 +1];
   z[0] = symLen*2;
   
   // Note that the symbol energy has to be halved since we use 2 antennas.
   for (int i=1;i<=(symLen>>1);i++)
   {
      z[2*i-1] = x[2*i-1]*sqrt_1_2;
      z[2*i  ] = -conj(x[2*i])*sqrt_1_2;
      
      z[2*i-1+symLen] = x[2*i]*sqrt_1_2;
      z[2*i  +symLen] = conj(x[2*i-1])*sqrt_1_2;
   }
   
   return z;
}

complex<float> *STBC::AlamoutiDecode(complex<float> *r, complex<float> *channelGain)
{
   int symLen = (int)real(r[0]);
   
   complex<float> *x = new complex<float> [symLen +1];
   x[0] = symLen;

   for (int i=1;i<=(symLen>>1);i++)
   {
      x[2*i-1] = conj(channelGain[2*i])*r[2*i-1] + channelGain[2*i+symLen]*conj(r[2*i]);
      x[2*i  ] = conj(channelGain[2*i+symLen])*r[2*i-1] - channelGain[2*i]*conj(r[2*i]);
   }
   
   return x;
}
