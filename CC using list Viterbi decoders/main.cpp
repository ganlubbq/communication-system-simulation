/* Use LAB524Rand.h
 * Change the loop index range for efficiency
 * ------------------------------------------
 * Rewrite fading
 * ------------------------------------------
 * Demodulator outputs LLR.
 * Feed LLR to decoders.
 * 
 * Date: 2008/1/8
 */

#include <iostream>
#include <fstream>

using namespace std;

#include "LAB524Rand.h"
#include "timer.h"
#include "transmitter.h"
#include "channel.h"
#include "receiver.h"
#include "fileIO.h"

int main()
{
   const float start = getValue("From SNR", "config.txt");
   const float end = getValue("To SNR", "config.txt");

   const int ERROR = (int)getValue("Accumulated frame errors", "config.txt");
   const float step = getValue("SNR step", "config.txt");

   const int FL = (int)getValue("Frame length", "config.txt");

   const int nList = (int)getValue("Number of list decoding output", "config.txt");
   
   const int points = (int)round((end-start)/step + 1);
   float snr[points], snrdB[points], ber[points], fer[points];
    
   // Sample SNR value
   for (int iSNR=0;iSNR<points;iSNR++)
   {
      snrdB[iSNR] = start + iSNR*step;
      snr[iSNR] = pow(10,snrdB[iSNR]/10);  
   }                                                

   // Choose random seed   
   LAB524Rand rng(time(NULL));

   Timer timer;
    
   ofstream data;
   data.open("data.txt",ios::app);

   int nLoop;

   for (int iSNR=0;iSNR<points;iSNR++)
   {
      cout << "Loop " << iSNR+1 << ", SNR = " << fixed << snrdB[iSNR] << endl;
      unsigned int nBitError = 0;  
      unsigned int nPacError = 0;

      // Eb is the energy per bit of the original data
      float Eb = 1.0;
      float N0 = Eb/snr[iSNR];         

      int iPac;
      for (iPac=0;nPacError<ERROR;iPac++)
      {
         int *u = generateDataBits(FL,&rng);
         
         float *x = tx(u,Eb);
         
         float *r = AWGN(x,N0,&rng);
//         float *r = Rayleigh(x,N0,&rng);
         delete [] x;
         
         int **y = rx(r,N0,nList);
         delete [] r;
         
         unsigned int temp = nBitError;
        
         int iBit = 1;
         while (iBit <= FL)
         {
            if (u[iBit] != y[1][iBit]) { nBitError++; }
            iBit++;
         }
        
         if(nBitError > temp)
         {
            nPacError++;
            cout << "packet error = " << nPacError << endl;
         }
         
         delete [] u;
         for (int j=0;j<nList;j++) { delete [] y[j]; }
         delete [] y;
      }
     
      ber[iSNR] = nBitError/(float)(FL*iPac);
      fer[iSNR] = ERROR/(float)iPac;
      
      if (iSNR == 0)
      {     
         data << "********************************************" << endl;
         data << "Simulation started at  : "; data << timer.getStartTime(); data << endl;
      }
      
      cout << "SNR = " << fixed << snrdB[iSNR] << endl;
      data << "SNR = " << fixed << snrdB[iSNR] << endl;
      cout << "BER = " << scientific << ber[iSNR] << endl;
      data << "BER = " << scientific << ber[iSNR] << endl;
      cout << "FER = " << scientific << fer[iSNR] << endl;
      data << "FER = " << scientific << fer[iSNR] << endl;
      
      // End simulation upon BER < 1e-5
      if (ber[iSNR] < 1e-5)
      { 
         nLoop = iSNR+1;
         break;
      }
      nLoop = iSNR+1;
      
      cout << endl;
   }
    

   data << "--------------------------" << endl;
   cout << "--------------------------" << endl;
    
   // print SNR & BER
   for (int i=0;i<nLoop;i++)
   {
      data << fixed << snrdB[i] << endl;
      cout << fixed << snrdB[i] << endl;
   }
    
   data << "--------------------------" << endl;
   cout << "--------------------------" << endl;
    
   for (int i=0;i<nLoop;i++)
   {
      data << scientific << ber[i] << endl;
      cout << scientific << ber[i] << endl;
   }

   data << "--------------------------" << endl;
   cout << "--------------------------" << endl;
    
   for (int i=0;i<nLoop;i++)
   {
      data << scientific << fer[i] << endl;
      cout << scientific << fer[i] << endl;
   }
   
   data << "--------------------------" << endl;
   cout << "--------------------------" << endl;
    
   cout << "Simulation started at  : "; cout << timer.getStartTime() << endl;
   cout << "Simulation finished at : "; cout << timer.getCurrentTime() << endl;
   cout << "Time elapsed : "; cout << timer.getElapsedTime() << " sec" << endl;
   
   data << "Simulation finished at : "; data << timer.getCurrentTime() << endl;
   data << "Time elapsed : "; data << timer.getElapsedTime() << " sec" << endl;

   data << "********************************************" << endl;
   data.close();

   getchar();
   
   return 0;
}
