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
#include <iomanip>

using namespace std;

const int FERBUFF_SIZE = 20;

int ferBuff[FERBUFF_SIZE];

int main()
{        
   const double start = getValue("From SNR", "config.txt");
   const double end = getValue("To SNR", "config.txt");
   const int frameError = (int)getValue("Accumulated frame errors", "config.txt");
   const double step = getValue("SNR step", "config.txt");

   // RPDCCH length
   const int FL = (int)getValue("Frame length", "config.txt");

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
   
   data << endl;
   data << "********************************************" << endl;
   data << "Simulation started at  : "; data << timer.getStartTime(); data << endl;

   data << "--------------------------" << endl;
    
   // print SNR
   for (int i=0;i<points;i++)
   {
      data << fixed << snrdB[i] << endl;
   }
   
   data << "--------------------------" << endl;
   
   int nLoop;
   int iPac;
   for (int iSNR=0;iSNR<points;iSNR++)
   {
      cout << "Loop " << iSNR+1 << ", SNR = " << fixed << snrdB[iSNR] << endl;
      unsigned int nBitError = 0;  
      unsigned int nPacError = 0;
      unsigned int totFLSimulated = 0;

      double N0 = 1.0; 
      double E_b = snr[iSNR];
      
      // Note that a Pac is a sub frame
      for (iPac=0;nPacError<frameError;iPac++)
      {
         int *rpdcch = generateDataBits(FL, &rng);
         
         int *y;
         
         for (int iOFDMSym=0;iOFDMSym<N_SYM;iOFDMSym++)
         {
            complex<double> *x = tx(rpdcch,E_b,iOFDMSym,&rng);

//            complex<double> **r = Dummy(x,2);
            complex<double> **r = FastFading_2(x,2);

            y = rx(r,N0,iOFDMSym,&rng);

            delete [] x;

            delete [] r[0]; delete [] r[1]; delete [] r;
         }

         unsigned int temp = nBitError;

         int iBit = 1;
         while (iBit <= FL) 
         {
            if (rpdcch[iBit] != y[iBit]) { nBitError++; }
            iBit++;
         }
    
         totFLSimulated += FL;         
         delete [] rpdcch; delete [] y;
         
         double _FER;
         if(nBitError > temp)
         {
            nPacError++;
            _FER = (double)nPacError/(iPac+1);
            cout << setprecision(1) << fixed << snrdB[iSNR] << " dB";
            cout << ", nPacError = " << fixed << nPacError;
            cout << setprecision(6) << ", FER = " << scientific << _FER << endl;
/*
            // Termination check
            while (_FER < 10.0) { _FER *= 10.0; }
            int intFER = (int)_FER;

            static int p = 0;
            ferBuff[p] = intFER;
            p++;
            if (p == FERBUFF_SIZE) { p = 0; }

            int quit = 1;
            for (int i=1;i<FERBUFF_SIZE;i++)
            { if (ferBuff[i] != ferBuff[0]) { quit = 0; break; } }

            _FER = (double)nPacError/(iPac+1);
            if (((_FER>1e-2) && (quit==1) && (nPacError>=200)) || 
                ((_FER<1e-2) && (quit==1) && (nPacError>=100)) )
            { for (int i=0;i<FERBUFF_SIZE;i++) { ferBuff[i] = -1; } break; }
*/
         }

      }

      ber[iSNR] = (double)nBitError/totFLSimulated;
      fer[iSNR] = (double)nPacError/(iPac+1);//(double)frameError/iPac;
      
      cout << "SNR = " << fixed << snrdB[iSNR] << endl;
      cout << "BER = " << scientific << ber[iSNR] << endl;
      cout << "FER = " << scientific << fer[iSNR] << endl;
      data << scientific << fer[iSNR] << endl;
      
      // End simulation upon BER < 1e-5
//      if (ber[iSNR] < 1e-5) { nLoop = iSNR+1; break; }
      if (fer[iSNR] < 1e-4) { break; }
            
      nLoop = iSNR+1;
      
      cout << endl;
   }
   
/*
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
*/
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
   cout << "Time per subframe : "; cout << (double)timer.getElapsedTime()/iPac << " sec" << endl;
   
   data << "Simulation finished at : "; data << timer.getCurrentTime() << endl;
   data << "Time elapsed : "; data << timer.getElapsedTime() << " sec" << endl;
   
   data << "********************************************" << endl;
   data.close();

   getchar();
   
   return 0;
}
