/* 
 * Use LAB524Rand.h
 * Change the loop index range for efficiency
 * ------------------------------------------
 * Rewrite fading
 * ------------------------------------------
 * Demodulator outputs LLR.
 * Feed LLR to decoders.
 * ------------------------------------------
 * BICM
 * Change the related data with type float to complex
 * 
 * Date: 2008/1/22
 * ------------------------------------------
 * Enhance fileIO.h to include the getString().
 * Add the function of reading modulation format in config.txt.
 *
 * Provide src to Antonio
 * Bit-interleaved coded modulation simulation program.
 * Convolutional codes with rate = 1/2, no puncturing.
 * Support M-PSK and M-QAM modulation formats. (Need verification)
 * Random interleaving for each transmission.
 *
 * Note that the parameter of "Frame length" should be consistent with the
 * modulation order.
 *
 * In main.cpp, replace line 117 with line 118 to simulate Rayleigh fading
 * environment.
 *
 * Author: Ling-San Meng
 * Email: f95942117@ntu.edu.tw
 *
 * Date: 2009/5/20
 */

#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

#include "define.h"
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

   const int frameError = (int)getValue("Accumulated frame errors", "config.txt");
   const float step = getValue("SNR step", "config.txt");

   const int FL = (int)getValue("Frame length", "config.txt");
   
   // Read and determine the modulation format
   char *modFormatChar = getString("Modulation format", "config.txt");
   string modFormatStr = modFormatChar;
   int modFormat = -1;
   if (modFormatStr.find("PSK") != string::npos) { modFormat = PSK; }
   else if (modFormatStr.find("QAM") != string::npos) { modFormat = QAM; }
   if (modFormat == -1)
   {
      cout << "Error for modulation format!" << endl;
      getchar();
      exit(1);
   }
   int constellationSize = atoi(modFormatChar);
   int bitsPerSym = 0;
   while ((constellationSize&1) != 1)
   {
      constellationSize >>= 1;
      bitsPerSym ++;
   }
   delete [] modFormatChar;
      
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
   int iPac;
   
   for (int iSNR=0;iSNR<points;iSNR++)
   {
      cout << "Loop " << iSNR+1 << ", SNR = " << fixed << snrdB[iSNR] << endl;
      unsigned int nBitError = 0;  
      unsigned int nPacError = 0;

      // Eb is the energy per bit of the original data
      float Eb = 1.0;
      float N0 = Eb/snr[iSNR];
            
      for (iPac=0;nPacError<frameError;iPac++)
      {
         int *u = generateDataBits(FL,&rng);
         
         complex<float> *x = tx(u,Eb,modFormat,bitsPerSym);
         
         complex<float> *r = AWGN(x,N0,&rng);
//         complex<float> *r = Rayleigh(x,N0,&rng);
         delete [] x;
         
         int *y = rx(r,N0,modFormat,bitsPerSym);
         delete [] r;
         
         unsigned int temp = nBitError;
        
         int iBit = 1;
         while (iBit <= FL)
         {
            if (u[iBit] != y[iBit]) { nBitError++; }
            iBit++;
         }
                 
         if(nBitError > temp)
         {
            nPacError++;
            cout << "packet Error = " << nPacError << endl;
         }
         
         delete [] u;delete [] y;
      }
     
      ber[iSNR] = nBitError/(float)(FL*iPac);
      fer[iSNR] = frameError/(float)iPac;
      
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
   cout << "Time per pac : "; cout << (float)timer.getElapsedTime()/iPac << " sec" << endl;
   
   data << "Simulation finished at : "; data << timer.getCurrentTime() << endl;
   data << "Time elapsed : "; data << timer.getElapsedTime() << " sec" << endl;
   
   data << "********************************************" << endl;
   data.close();

   getchar();
   
   return 0;
}
