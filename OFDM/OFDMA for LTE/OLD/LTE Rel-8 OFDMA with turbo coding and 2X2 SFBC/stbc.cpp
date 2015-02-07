#include "stbc.h"

#include <cmath>

#include <iostream>
using namespace std;

const double sqrt_1_2 = 0.707106781186548;
   
STBC::STBC()
{

}

STBC::~STBC()
{
                             
}

// Alamouti by defn. is 2XN. Structured as a double-sized 1-d array.
complex<double> *STBC::AlamoutiEncode(complex<double> *x)
{
   int symLen = (int)real(x[0]);
   
   // Check if the symLen is even.
   if ((symLen&1)==1)
   {
      cout << "Error in AlamoutiEncode(); symLen is not even" << endl;
      getchar();
   }
   
   complex<double> *z = new complex<double> [symLen*2 +1];
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

complex<double> *STBC::AlamoutiDecode(complex<double> *r, complex<double> *channelGain)
{
   int symLen = (int)real(r[0]);
   
   complex<double> *x = new complex<double> [symLen +1];
   x[0] = symLen;

   for (int i=1;i<=(symLen>>1);i++)
   {
      x[2*i-1] = conj(channelGain[2*i])*r[2*i-1] + channelGain[2*i+symLen]*conj(r[2*i]);
      x[2*i  ] = conj(channelGain[2*i+symLen])*r[2*i-1] - channelGain[2*i]*conj(r[2*i]);
   }

   // Remake channel gain due to DeSTBC. Don't care the other half (originally
   // used to store the chGain for the 2nd Tx antenna).
   channelGain[0] = symLen;
   for (int i=1;i<=(symLen>>1);i++)
   {
      // chGain for both x0 and x1 is set to (|h0|^2 + |h1|^2)/sqrt(2)
      // Note that the factor sqrt(2) is added to compensate for the halved
      // signal energy at the transmitter s.t. the symbol energy after deSTBC
      // is the same as that before.
      //    (|h0|^2 + |h1|^2) * x0 + h0*n0+h1*n1 (don't care conjucates)
      // => (|h0|^2 + |h1|^2)/sqrt(2) * sqrt(2)*x0 + h0*n0+h1*n1
      // => channel_gain * signal + noise
      channelGain[2*i-1] = (channelGain[2*i]*conj(channelGain[2*i]) 
                          + channelGain[2*i+symLen]*conj(channelGain[2*i+symLen]))
                          * sqrt_1_2;
      channelGain[2*i  ] = channelGain[2*i-1];
   }

   return x;
}
