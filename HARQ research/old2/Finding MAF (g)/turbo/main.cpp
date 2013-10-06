/* 
 * Date : 2007/10/21
 */

#include <iostream> 
#include <cstdlib>
#include <cmath>
#include <fstream>

using namespace std;

#include "timer.h"
#include "transmitter.h"
#include "channel.h"
#include "receiver.h"
#include "fileIO.h"


const int nMAF = (int)getValue("Number of MAF for avg", "config.txt");
const float ACCURACY = getValue("Allowable deviation", "config.txt");

int diffBits(int *a, int *b, int len)
{
   int count = 0;
   for (int i=0;i<len;i++)
   {
      if (a[i +1] != b[i +1]) { count++; }
   }
   return count;   
}


int main()
{
   const float start = getValue("From SNR", "config.txt");
   const float end = getValue("To SNR", "config.txt");

   const int ERROR = (int)getValue("Accumulated frame errors", "config.txt");
   const float step = getValue("SNR step", "config.txt");

   const int FL = (int)getValue("Frame length", "config.txt");

   const float codeRate = 1.0/3.0;

   const int points=(int)((float)(end-start)/step) + 1;
   float snr[points], snrdB[points], ber[points], fer[points], meanMAF[points],
         meanSquareMAF[points], varianceMAF[points];
    
   // Sample SNR value
   for (int iSNR=0;iSNR<points;iSNR++)
   {
      snrdB[iSNR] = start + iSNR*step;
      snr[iSNR] = pow(10,snrdB[iSNR]/10);  
   }                                                

   // Choose random seed   
   srand(time(NULL)%60);

   Timer timer;
    
   ofstream data;
   data.open("data.txt",ios::app);

   int nLoop;

   for (int iSNR=0;iSNR<points;iSNR++)
   {
      cout << "Loop " << iSNR+1 << ", SNR = " << fixed << snrdB[iSNR] << endl;

      // Eb is the energy per bit of the modulated signal
      float Eb = 1.0;
      float N0 = Eb/snr[iSNR]/codeRate;         

      float MAFrecord[nMAF]; 
  
      int iMAF;
      for (iMAF=0;iMAF<nMAF;)
      {
         int *u = generateDataBits(FL);
        
         float *x = tx(u,Eb);
     
         // Valid for BPSK 
         const int symLen = (int)x[0];
         float noiseBuff[symLen];
  
         float *r = AWGN(x,N0,noiseBuff);
//         float *ray = Rayleigh(float *x, float N0, float mP);
      
         int *y = rx(r,N0);
     
         bool decodable = !(diffBits(u,y,FL));
         delete [] y;
         
         // If 1st decoding succeeds, continue
         if (decodable == true)
         {
            delete [] u;delete [] x;delete [] r;
            continue;
         }
         
         /****************MAF loop****************************************/
         // Assign an initial value to start the search
         float g = 1.0;
         
         // Double the initial value repeatedly untill >= the Answer
         while (1)
         {
            // Apply MAF to x[]
            for (int l=0;l<symLen;l++)
            { 
               r[l +1] = (1+g)*x[l +1] + noiseBuff[l];
            }

            int *y = rx(r,N0);
            
            bool decodable = !(diffBits(u,y,FL));
            delete [] y;
            
            // Check if can be decoded correctly
            if (decodable == true) { break; }
            else { g *= 2; }
         }

         g /= 2;
         float gStep = g/2;
         // Begin the binary search
         while (1)
         {
            // Apply MAF to x[]
            for (int l=0;l<symLen;l++)
            { 
               r[l +1] = (1+g)*x[l +1] + noiseBuff[l];
            }
            
            int *y = rx(r,N0);
            
            bool decodable = !(diffBits(u,y,FL));
            delete [] y;
            
            if (decodable == true) { g -= gStep; }
            else { g += gStep; }
                
            if (gStep <= ACCURACY)
            {
               MAFrecord[iMAF] = g;
               break;
            }
            
            gStep /= 2;
         }
 
         /****************************************************************/
         
         
         delete [] u;delete [] x;delete [] r;
//         delete [] ray;

         cout << "iMAF = " << iMAF << endl;
         iMAF++;
      }

      if (iSNR == 0)
      {     
         data << "********************************************" << endl;
         data << "Simulation started at  : "; data << timer.getStartTime(); data << endl;
      }
      
      float total1 = 0, total2 = 0;
      // Clalculate mean MAF
      for (int j=0;j<nMAF;j++)
      {
         total1 += MAFrecord[j];
         total2 += MAFrecord[j]*MAFrecord[j];
      }
      
      meanMAF[iSNR] = total1/(float)nMAF;
      meanSquareMAF[iSNR] = total2/(float)nMAF;
      varianceMAF[iSNR] = meanSquareMAF[iSNR] - meanMAF[iSNR]*meanMAF[iSNR];
    
      cout << "SNR = " << fixed << snrdB[iSNR] << endl;
      data << "SNR = " << fixed << snrdB[iSNR] << endl;
      cout << "mean MAF = " << meanMAF[iSNR] << endl;
      data << "mean MAF = " << meanMAF[iSNR] << endl;
      cout << "mean square MAF = " << meanSquareMAF[iSNR] << endl;
      data << "mean square MAF = " << meanSquareMAF[iSNR] << endl;
      cout << "variance MAF = " << varianceMAF[iSNR] << endl;
      data << "variance MAF = " << varianceMAF[iSNR] << endl;
         
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
      data << meanMAF[i] << "," << endl;
      cout << meanMAF[i] << endl;
   }    
   
   data << "--------------------------" << endl;
   cout << "--------------------------" << endl;
    
   for (int i=0;i<nLoop;i++)
   {
      data << meanSquareMAF[i] << "," << endl;
      cout << meanSquareMAF[i] << endl;
   }    
   
   data << "--------------------------" << endl;
   cout << "--------------------------" << endl;
    
   for (int i=0;i<nLoop;i++)
   {
      data << varianceMAF[i] << "," << endl;
      cout << varianceMAF[i] << endl;
   }    
   
   data << "--------------------------" << endl;
   cout << "--------------------------" << endl;
    
    
   cout << "Simulation started at  : "; timer.printStartTime(); cout << endl;
   cout << "Simulation finished at : "; timer.printCurrentTime(); cout << endl;
   cout << "Time elapsed : "; timer.printElapsedTime(); cout << endl;
   
   data << "Simulation finished at : "; data << timer.getCurrentTime(); data << endl;
   data << "Time elapsed : "; data << timer.getElapsedTime(); data << endl;

   data << "********************************************" << endl;
   data.close();

   system("pause");
   
   return 0;
}
