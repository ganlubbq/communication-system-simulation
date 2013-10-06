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
   const float step = getValue("SNR step", "config.txt");
   const int FL = (int)getValue("Frame length", "config.txt");

   const float codeRate = 1.0/2.0;

   const int SNRpoints=(int)((end-start)/step) + 1;
   float snr[SNRpoints], snrdB[SNRpoints], ber[SNRpoints], fer[SNRpoints];
   // Sample SNR value
   for (int iSNR=0;iSNR<SNRpoints;iSNR++)
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

   const int nRetran = (int)getValue("Number of retransmission for avg","config.txt");

   const float startAF = getValue("From retransmission AF", "config.txt");
   const float endAF = getValue("To retransmission AF", "config.txt");
   const float AFstep = getValue("Retransmission AF step", "config.txt");

   const int AFpoints=(int)((endAF-startAF)/AFstep) + 1;
   float AF[AFpoints];     
   float Pout[SNRpoints][AFpoints],meanMAF[SNRpoints][AFpoints],
         meanSquareMAF[SNRpoints][AFpoints],varianceMAF[SNRpoints][AFpoints];
         
   for (int i=0;i<AFpoints;i++)
   {
      AF[i] = startAF + i*AFstep;
   }
    
     
   data << "********************************************" << endl;
   data << "Simulation started at  : "; data << timer.getStartTime(); data << endl;
      
   for (int iSNR=0;iSNR<SNRpoints;iSNR++)
   {
      cout << "Loop " << iSNR+1 << ", SNR = " << fixed << snrdB[iSNR] << endl;

      // Eb is the energy per bit of the modulated signal
      float Eb = 1.0;
      float N0 = Eb/snr[iSNR]/codeRate;         

      for (int iAF=0;iAF<AFpoints;iAF++)
      {
         cout << "Retransmission AF = " << fixed << AF[iAF] << endl;

         float MAFrecord[nRetran]; 
  
         int nRetranError = 0;
         
         int iRetran;
         for (iRetran=0;iRetran<nRetran;)
         {
            int *u = generateDataBits(FL);
          
            float *x = tx(u,Eb);
     
            // Valid for BPSK 
            const int symLen = (int)x[0];
            float noiseBuff[symLen];
            
            float *r = AWGN(x,N0,noiseBuff);
//            float *ray = Rayleigh(float *x, float N0, float mP);
      
            int *y = rx(r,N0);
     
            bool decodable = !(diffBits(u,y,FL));
            delete [] y;
         
            // If 1st decoding succeeds, continue
            if (decodable == true)
            {  
               delete [] r;delete [] u;delete [] x;
               
               continue;
            }
         
         /****************MAF*********************************************/
         // Assign an initial value to start the search
         float g = 1.0;
         
         float r2[symLen +1];
         r2[0] = symLen;
         
         // Double the initial value repeatedly untill >= the Answer
         while (1)
         {
            // Apply MAF to x[]
            for (int l=0;l<symLen;l++)
            { 
               r2[l +1] = (1+g)*x[l +1] + noiseBuff[l];
            }

            int *y = rx(r2,N0);
            
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
               r2[l +1] = (1+g)*x[l +1] + noiseBuff[l];
            }
            
            int *y = rx(r2,N0);
            
            bool decodable = !(diffBits(u,y,FL));
            delete [] y;
            
            if (decodable == true) { g -= gStep; }
            else { g += gStep; }
                
            if (gStep <= ACCURACY)
            {
               MAFrecord[iRetran] = g;
               break;
            }
            
            gStep /= 2;
         }
         /****************************************************************/
            
            
            
            /****************Retran******************************************/
            float retranFrame[symLen +1];
            retranFrame[0] = symLen;
            for (int l=0;l<symLen;l++)
            { 
               retranFrame[l +1] = (g+AF[iAF])*x[l +1];
            }
            
            float *retranFramePlusNoise = AWGN(retranFrame,N0,noiseBuff);
            retranFramePlusNoise[0] = symLen;
            
            // Combine two frames
            for (int l=0;l<symLen;l++)
            { 
               r[l +1] += retranFramePlusNoise[l +1];
            }
            
            y = rx(r,N0);
            
            decodable = !(diffBits(u,y,FL));
            delete [] y;

            if (decodable == false)
            {
               nRetranError++;
            }
            
            delete [] retranFramePlusNoise;
            /****************************************************************/
         
         
            delete [] u;delete [] x;delete [] r;
   //         delete [] ray;

            cout << "iRetran = " << iRetran << endl;
            iRetran++;
         }//end of Retran loop

         // Clalculate mean, mean square MAF
         float total1 = 0, total2 = 0;
         for (int j=0;j<nRetran;j++)
         {
            total1 += MAFrecord[j];
            total2 += MAFrecord[j]*MAFrecord[j];
         }
         meanMAF[iSNR][iAF] = total1/(float)nRetran;
         meanSquareMAF[iSNR][iAF] = total2/(float)nRetran;
         varianceMAF[iSNR][iAF] = meanSquareMAF[iSNR][iAF] - 
                                  meanMAF[iSNR][iAF]*meanMAF[iSNR][iAF];
      
         Pout[iSNR][iAF] = (float)nRetranError/nRetran;
      
         cout << "SNR = " << fixed << snrdB[iSNR] << endl;
         data << "SNR = " << fixed << snrdB[iSNR] << endl;
         cout << "AF = " << fixed << AF[iAF] << endl;
         data << "AF = " << fixed << AF[iAF] << endl;
         cout << "mean MAF = " << meanMAF[iSNR][iAF] << endl;
         data << "mean MAF = " << meanMAF[iSNR][iAF] << endl;
         cout << "mean square MAF = " << meanSquareMAF[iSNR][iAF] << endl;
         data << "mean square MAF = " << meanSquareMAF[iSNR][iAF] << endl;
         cout << "variance MAF = " << varianceMAF[iSNR][iAF] << endl;
         data << "variance MAF = " << varianceMAF[iSNR][iAF] << endl;
         cout << "mean squrare AF = " << varianceMAF[iSNR][iAF]+(AF[iAF]+meanMAF[iSNR][iAF])*(AF[iAF]+meanMAF[iSNR][iAF]) << endl;
         data << "mean squrare AF = " << varianceMAF[iSNR][iAF]+(AF[iAF]+meanMAF[iSNR][iAF])*(AF[iAF]+meanMAF[iSNR][iAF]) << endl;
         cout << "Pout = " << Pout[iSNR][iAF] << endl << endl;
         data << "Pout = " << Pout[iSNR][iAF] << endl << endl;
             
      }//end of AF loop
         
      nLoop = iSNR+1;
      cout << endl << endl;;
   }
    
   // print SNR & BER
   for (int i=0;i<nLoop;i++)
   {
      data << "--------------------------" << endl;
      cout << "--------------------------" << endl;

      cout << "SNR = " << fixed << snrdB[i] << endl;
      data << "SNR = " << fixed << snrdB[i] << endl;      

      data << "squareK-------------------" << endl;
      cout << "squareK-------------------" << endl;
            
      for (int j=0;j<AFpoints;j++)
      {
         float a = AF[j];
         // squareK = Gvariance + (a + Gmean)^2
         float squareK = varianceMAF[i][j]+(a+meanMAF[i][j])*(a+meanMAF[i][j]);
         
         cout << fixed << squareK << endl;
         data << fixed << squareK << endl;
      }
      
      data << "Pout----------------------" << endl;
      cout << "Pout----------------------" << endl;
      
      for (int j=0;j<AFpoints;j++)
      {
         cout << Pout[i][j] << endl;
         data << Pout[i][j] << endl;
      }
      
      data << "mean MAF------------------" << endl;
      cout << "mean MAF------------------" << endl;
      
      for (int j=0;j<AFpoints;j++)
      {
         cout << meanMAF[i][j] << endl;
         data << meanMAF[i][j] << endl;
      }
      
      data << "mean square MAF-----------" << endl;
      cout << "mean square MAF-----------" << endl;
      
      for (int j=0;j<AFpoints;j++)
      {
         cout << meanSquareMAF[i][j] << endl;
         data << meanSquareMAF[i][j] << endl;
      }
      
      data << "variance MAF-------------" << endl;
      cout << "variance MAF--------------" << endl;
      
      for (int j=0;j<AFpoints;j++)
      {
         cout << varianceMAF[i][j] << endl;
         data << varianceMAF[i][j] << endl;
      }
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
