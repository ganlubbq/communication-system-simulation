/*
 * An example demonstrating how to use "turbo.h".
 *
 * Date: 2007/12/25
 *
 * Author: Ling-San Meng
 * Email : f95942117@ntu.edu.tw
 */
 
#include "turbo.h"

#include <iostream>

using namespace std;

const int INFINITY = 4;

int main()
{
   int frameLength = 6;
   
   /* Data generation */
   int dataBits[frameLength +1];
   dataBits[0] = frameLength;
   dataBits[1] = 0; dataBits[2] = 1; dataBits[3] = 1; dataBits[4] = 0;
   dataBits[5] = 1; dataBits[6] = 1;

   cout << "Data bits:" << endl;
   for (int i=1;i<=frameLength;i++)
   { cout << dataBits[i] << " "; }
   cout << endl << endl;
   
   /*/ Encoding */
   int *codedBits = turboEncode(dataBits);
   int codedBitsLen = codedBits[0];
   double codeRate = (double)frameLength/codedBitsLen;

   cout << "Code rate = " << codeRate << endl;
   cout << "Coded bits:" << endl;
   for (int i=1;i<=codedBitsLen;i++)
   { cout << codedBits[i] << " "; }
   cout << endl << endl;
   
   /* Set the LLR of the coded bits. */
   double LLR[codedBitsLen +1];
   LLR[0] = codedBitsLen;
   for (int i=1;i<=codedBitsLen;i++)
   { 
      if (codedBits[i] == 0) { LLR[i] = -INFINITY; }
                        else { LLR[i] =  INFINITY; }
   }
   
   /* Decoding */
   // Noise spectral density. Note that there is no real AWGN added and we set
   // it to 0.01 arbitrarily here.
   double N0 = 0.01;
   
   int *decodedBits = turboDecode(LLR,N0);

   cout << "Decoded bits:" << endl;
   for (int i=1;i<=frameLength;i++)
   { cout << decodedBits[i] << " "; }
   
   delete [] codedBits;
   delete [] decodedBits;
   
   getchar();
   return 0;    
}
