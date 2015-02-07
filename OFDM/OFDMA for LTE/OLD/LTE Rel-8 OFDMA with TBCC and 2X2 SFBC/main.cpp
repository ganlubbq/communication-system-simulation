/*
 * LTE DL OFDMA structure with 2 Tx antennas and 2 Rx antennas.
 *
 * Author: Ling-San Meng
 * Email: f95942117@ntu.edu.tw
 *
 * Date: 2009/12/23
 */

#include "define.h"
#include "LAB524Rand.h"
#include "timer.h"
#include "transmitter.h"
#include "channel.h"
#include "receiver.h"
#include "fileIO.h"

#include <iostream>
#include <fstream>

using namespace std;

int main()
{
   const double start = getValue("From SNR", "config.txt");
   const double end = getValue("To SNR", "config.txt");
   const int frameError = (int)getValue("Accumulated frame errors", "config.txt");
   const double step = getValue("SNR step", "config.txt");

   // RPDCCH length
   const int FL = 101;

   const int points = (int)round((end-start)/step + 1);
   double snr[points], snrdB[points], ber[points], fer[points], thr[points];
    
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
      unsigned int totFLSimulated = 0;

//      double totalNumSubcarrierUsed = 0;
      
      // E_tot is the total eNB power
      double E_tot = 1.0; 
      double N0 = E_tot/snr[iSNR];
      
      // Note that a Pac is a radio frame
      for (iPac=0;nPacError<frameError;iPac++)
      {
         int *rpdcch = generateDataBits(FL, &rng);
             
         int *y;// Processed data bits
         
         for (int iOFDMSym=0;iOFDMSym<N_SYM;iOFDMSym++)
         {
            complex<double> *x = tx(rpdcch,E_tot,iOFDMSym,&rng);

            complex<double> **r = Dummy(x,N0,&rng);
//            complex<double> **r = blockRayleigh(x,N0,2,&rng);

            delete [] x;

            y = rx(r,E_tot,N0,iOFDMSym,&rng);
            
            delete [] r[0]; delete [] r[1]; delete [] r;
         }

         unsigned int temp = nBitError;

         int iBit = 1;
         while (iBit <= FL) 
         {
            if (rpdcch[iBit] != y[iBit]) { nBitError++; }
            iBit++;
         }
    
         if(nBitError > temp)
         {
            nPacError++;
            cout << "packet Error = " << nPacError << endl;
         }

         totFLSimulated += FL;         
         delete [] rpdcch; delete [] y;
      }

      ber[iSNR] = (double)nBitError/totFLSimulated;
      fer[iSNR] = (double)frameError/iPac;
//      thr[iSNR] = (double)(iPac-frameError)*FL/totalNumSubcarrierUsed;
      
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
//      cout << "Thr = " << fixed << thr[iSNR] << endl;
//      data << "Thr = " << fixed << thr[iSNR] << endl;
      
      // End simulation upon BER < 1e-5
      if (ber[iSNR] < 1e-5) { nLoop = iSNR+1; break; }
      
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
/*
   data << "--------------------------" << endl;
   cout << "--------------------------" << endl;
   
   for (int i=0;i<nLoop;i++)
   {
      data << fixed << thr[i] << endl;
      cout << fixed << thr[i] << endl;
   }
*/
   data << "--------------------------" << endl;
   cout << "--------------------------" << endl;
   cout << "Simulation started at  : "; cout << timer.getStartTime() << endl;
   cout << "Simulation finished at : "; cout << timer.getCurrentTime() << endl;
   cout << "Time elapsed : "; cout << timer.getElapsedTime() << " sec" << endl;
   cout << "Time per pac : "; cout << (double)timer.getElapsedTime()/iPac << " sec" << endl;
   
   data << "Simulation finished at : "; data << timer.getCurrentTime() << endl;
   data << "Time elapsed : "; data << timer.getElapsedTime() << " sec" << endl;
   
   data << "********************************************" << endl;
   data.close();

   getchar();
   
   return 0;
}
