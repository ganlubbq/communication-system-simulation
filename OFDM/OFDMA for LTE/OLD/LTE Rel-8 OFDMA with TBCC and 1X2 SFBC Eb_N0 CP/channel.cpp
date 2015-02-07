#include "channel.h"

complex<double> **Dummy(complex<double> *z, int numTxANT, int numRxANT)
{
   const int signalLen = (int)real(z[0]);
   
   complex<double> **r = new complex<double>* [2];
   r[0] = new complex<double> [signalLen +1];
   r[1] = new complex<double> [signalLen +1];
   r[0][0] = signalLen;
   r[1][0] = signalLen;
   
   // No channel effect.
   for (int j=1;j<=signalLen;j++)
   {
      r[0][j] = z[j];
      r[1][j] = z[j];
   }
   
   return r;
}
