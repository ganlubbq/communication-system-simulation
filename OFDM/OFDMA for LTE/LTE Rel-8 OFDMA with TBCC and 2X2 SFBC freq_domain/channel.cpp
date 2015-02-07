#include "channel.h"
#include "fft.h"
#include "define.h"

// Define EPA or TU here. 
#define TU
#define IID

const double pi = 3.141592653589793;
const double k = 0.9; // a2/a1

const double a1 = sqrt(1.0/(1.0+k*k));
const double a2 = a1*k;

double corMatrix1_2[2][2] = {
a1,   a2,
a2,   a1};

const double corMatrix2_2[4][4] = {
0.591715976331361,   0.177514792899408,   0.177514792899408,   0.053254437869822,
0.177514792899408,   0.591715976331361,   0.053254437869822,   0.177514792899408,
0.177514792899408,   0.053254437869822,   0.591715976331361,   0.177514792899408,
0.053254437869822,   0.177514792899408,   0.177514792899408,   0.591715976331361};


#ifdef EPA

// Specify the path gain
#define pathGainNum 7 // For EPA

// Specify the channel length read from txt
#define chanLen 1000 // For EPA Unit: radioframe

// speccify the computation bound for sinc
#define bound 12 // For EPA

// For EPA
const double tau[7] = {0e-9,30e-9,70e-9,90e-9,110e-9,190e-9,410e-9};

#endif

#ifdef TU

// Specify the path gain
#define pathGainNum 20 // For TU

// Specify the channel length read from txt
#define chanLen 400 // For TU

// speccify the computation bound for sinc
#define bound 40 // For TU

// For TU
const double tau[20] = {0, 2.1700e-07, 5.1200e-07, 5.1400e-07, \
5.1700e-07, 6.7400e-07, 8.8200e-07, 1.2300e-06, \
1.28700e-06, 1.31100e-06, 1.34900e-06, 1.53300e-06, \
1.53500e-06, 1.62200e-06, 1.81800e-06, 1.83600e-06, \
1.88400e-06, 1.94300e-06, 2.04800e-06, 2.1400e-06};

#endif

/************Above variable must be modified for different scenarios************/

const double Ts = 1.00/15000/1024;

complex<double> *channelGain1 = NULL; // 1 tx ANT - 1 rx ANT
complex<double> *channelGain2;        // 1 tx ANT - 2 rx ANT

double Sinc(const double &val)
{
    if(val == 0)
        return 1.00000;
    else
        return ((double)sin(pi*val))/(pi*val);
}

void readFile(const char* filename, complex<double>** buffer, const int &bufLen)
{
    FILE* infile;
    infile = fopen(filename, "r");
        
    for(int i=0; i<bufLen; i++)
        buffer[i] = new complex<double> [pathGainNum];
        
    for(int i=0; i<bufLen; i++)
    {
        for(int j=0; j<pathGainNum; j++)
        {
            fscanf(infile,"%le",&buffer[i][j].real());
            fscanf(infile,"%le",&buffer[i][j].imag());
        }
    }       
    fclose(infile);
}


// x is supposed to be the freq. domain symbol complex values
complex<double> **FastFading_2(complex<double> *x, int numRxANT)
{  
   const int numSym = (int)(real(x[0]));
   const int numTxAnt = numSym/Nfft;
   
   // Allocate memory for channelGain[]
   if (channelGain1 == NULL)
   {
      channelGain1 = new complex<double> [Nfft*numTxAnt +1]; channelGain1[0] = Nfft*numTxAnt;
      channelGain2 = new complex<double> [Nfft*numTxAnt +1]; channelGain2[0] = Nfft*numTxAnt;
   }
   
   // Initialized the channel impulse response.
   static complex<double> *chImpulse1 = NULL;
   static complex<double> *chImpulse2;
   static complex<double> *chImpulse3;
   static complex<double> *chImpulse4;
   if (chImpulse1 == NULL)
   {
      chImpulse1 = new complex<double> [Nfft +1];
      chImpulse2 = new complex<double> [Nfft +1];
      chImpulse3 = new complex<double> [Nfft +1];
      chImpulse4 = new complex<double> [Nfft +1];
      chImpulse1[0] = Nfft; chImpulse2[0] = Nfft;
      chImpulse3[0] = Nfft; chImpulse4[0] = Nfft;
   }
   
   // Clear the impulse responses.
   for (int i=1;i<=Nfft;i++)
   {
      chImpulse1[i] = 0.0; chImpulse2[i] = 0.0;
      chImpulse3[i] = 0.0; chImpulse4[i] = 0.0;
   }

   // Read the channel impulse responses from files and load them into
   // chImpulse1, chImpulse2, chImpulse3, and chImpulse4. Sinc operation might
   // be needed.
   const int chanNum = numTxAnt*numTxAnt;
   const int bufLen = chanLen*14*10;
   static bool isRead = 0;
   static int ncount = 0;
   static complex<double>*** chanBuffer = new complex<double>** [4];
   
   //Initializing the channel buffer
   if(isRead == 0)
   {
      for(int i=0; i<4; i++) { chanBuffer[i] = new complex<double>* [bufLen]; }
      
      const char* file = getString("File name for channel data", "config.txt");
      string file_str = file;
      string path = "../channel_data/";
      string app1 = "_1.txt", app2 = "_2.txt", app3 = "_3.txt", app4 = "_4.txt";
      char file1[50] = "", file2[50] = "", file3[50] = "", file4[50] = "";
      
      strcpy (file1, (path+file+app1).c_str());
      strcpy (file2, (path+file+app2).c_str());
      strcpy (file3, (path+file+app3).c_str());
      strcpy (file4, (path+file+app4).c_str());
      delete [] file;
      
      readFile(file1, chanBuffer[0], bufLen); cout << "File 1 loaded" << endl;
      readFile(file2, chanBuffer[1], bufLen); cout << "File 2 loaded" << endl;
      readFile(file3, chanBuffer[2], bufLen); cout << "File 3 loaded" << endl;
      readFile(file4, chanBuffer[3], bufLen); cout << "File 4 loaded" << endl;
      
      isRead = 1;
   }
   
   for(int i=1; i<=bound; i++)
   {
      for(int ngain=0; ngain<pathGainNum; ngain++)
            chImpulse1[i] += chanBuffer[0][ncount][ngain]*((complex<double>)Sinc((tau[ngain])/Ts-((double)i-1.000)));

      for(int ngain=0; ngain<pathGainNum; ngain++)
            chImpulse2[i] += chanBuffer[1][ncount][ngain]*((complex<double>)Sinc((tau[ngain])/Ts-((double)i-1.000)));
    
      for(int ngain=0; ngain<pathGainNum; ngain++)
            chImpulse3[i] += chanBuffer[2][ncount][ngain]*((complex<double>)Sinc((tau[ngain])/Ts-((double)i-1.000)));

      for(int ngain=0; ngain<pathGainNum; ngain++)
            chImpulse4[i] += chanBuffer[3][ncount][ngain]*((complex<double>)Sinc((tau[ngain])/Ts-((double)i-1.000)));
   }

   ncount++;   
   if(ncount==bufLen)
      ncount=0;
   
/*
double temp1 = 0;     
double temp2 = 0; 
for (int i = 1;i<=Nfft;i++)
{
    temp1 += (double)(  abs(chImpulse1[i])*abs(chImpulse1[i])   ); 
    temp2 += (double)(  abs(chImpulse2[i])*abs(chImpulse2[i])   );
}
cout <<temp1 << endl;
cout <<temp2 << endl;
system("pause");
*/

   // Convert the ch. impulse to ch. frequence response.
   fft(chImpulse1,channelGain1,Nfft,0);
   fft(chImpulse2,channelGain2,Nfft,0);
   if (numTxAnt == 2) 
   {
      fft(chImpulse3,channelGain1+Nfft,Nfft,0);
      fft(chImpulse4,channelGain2+Nfft,Nfft,0);
   }
   
   // Correct the amplitude after fft.
   // Note that for those subcarriers > NUM_USEDCAR, we don't care.
   static double sqrtNfft = sqrt(Nfft);
   for (int iTxAnt=1;iTxAnt<=numTxAnt;iTxAnt++)
   {
      int offset = Nfft*(iTxAnt-1);
   
      for (int i=1;i<=NUM_USEDCAR;i++)
      {
         channelGain1[i+offset] *= sqrtNfft;
         channelGain2[i+offset] *= sqrtNfft;
      }   
   }

/*
   double meanP1 = 0;
   double meanP2 = 0;
   double meanP3 = 0;
   double meanP4 = 0;
   for (int i=1;i<=NUM_USEDCAR;i++)
   {
       meanP1 += abs(channelGain1[i])*abs(channelGain1[i]);
       meanP2 += abs(channelGain1[i+Nfft])*abs(channelGain1[i+Nfft]);
       meanP3 += abs(channelGain2[i])*abs(channelGain2[i]);
       meanP4 += abs(channelGain2[i+Nfft])*abs(channelGain2[i+Nfft]);
   }

   meanP1 /= (NUM_USEDCAR);
   meanP2 /= (NUM_USEDCAR);
   meanP3 /= (NUM_USEDCAR);
   meanP4 /= (NUM_USEDCAR);
   cout << fixed << "meanP1 = " << meanP1 << endl;
   cout << fixed << "meanP2 = " << meanP3 << endl;
   cout << fixed << "meanP3 = " << meanP2 << endl;
   cout << fixed << "meanP4 = " << meanP4 << endl;
   getchar();
*/

   // Correlation
#ifdef IID

#else

#ifdef SFBC
   for (int i=1;i<=NUM_USEDCAR;i++)
   {
      complex<double> ch1, ch2, ch3, ch4;
      ch1 = corMatrix2_2[0][0]*channelGain1[i]      + corMatrix2_2[0][1]*channelGain2[i] + 
            corMatrix2_2[0][2]*channelGain1[i+Nfft] + corMatrix2_2[0][3]*channelGain2[i+Nfft];
      ch2 = corMatrix2_2[1][0]*channelGain1[i]      + corMatrix2_2[1][1]*channelGain2[i] + 
            corMatrix2_2[1][2]*channelGain1[i+Nfft] + corMatrix2_2[1][3]*channelGain2[i+Nfft];
      ch3 = corMatrix2_2[2][0]*channelGain1[i]      + corMatrix2_2[2][1]*channelGain2[i] + 
            corMatrix2_2[2][2]*channelGain1[i+Nfft] + corMatrix2_2[2][3]*channelGain2[i+Nfft];
      ch4 = corMatrix2_2[3][0]*channelGain1[i]      + corMatrix2_2[3][1]*channelGain2[i] + 
            corMatrix2_2[3][2]*channelGain1[i+Nfft] + corMatrix2_2[3][3]*channelGain2[i+Nfft];

      channelGain1[i] = ch1;
      channelGain2[i] = ch2;
      channelGain1[i+Nfft] = ch3;
      channelGain2[i+Nfft] = ch4;
   }
#else

   for (int i=1;i<=NUM_USEDCAR;i++)
   {
      complex<double> ch1, ch2;
      ch1 = corMatrix1_2[0][0]*channelGain1[i] + corMatrix1_2[0][1]*channelGain2[i];
      ch2 = corMatrix1_2[1][0]*channelGain1[i] + corMatrix1_2[1][1]*channelGain2[i];

      channelGain1[i] = ch1;
      channelGain2[i] = ch2;
   }
#endif

#endif
/*
   meanP1 = 0;
   meanP2 = 0;
   meanP3 = 0;
   meanP4 = 0;
   for (int i=1;i<=NUM_USEDCAR;i++)
   {
       meanP1 += abs(channelGain1[i])*abs(channelGain1[i]);
       meanP2 += abs(channelGain1[i+Nfft])*abs(channelGain1[i+Nfft]);
       meanP3 += abs(channelGain2[i])*abs(channelGain2[i]);
       meanP4 += abs(channelGain2[i+Nfft])*abs(channelGain2[i+Nfft]);
   }

   meanP1 /= (NUM_USEDCAR);
   meanP2 /= (NUM_USEDCAR);
   meanP3 /= (NUM_USEDCAR);
   meanP4 /= (NUM_USEDCAR);
   cout << fixed << "meanP1 = " << meanP1 << endl;
   cout << fixed << "meanP2 = " << meanP3 << endl;
#ifdef SFBC
   cout << fixed << "meanP3 = " << meanP2 << endl;
   cout << fixed << "meanP4 = " << meanP4 << endl;
#endif
   getchar();
*/
   // r[][] is the returned signal.
   complex<double> **r = new complex<double>* [2];
   r[0] = new complex<double> [numSym +1];
   r[1] = new complex<double> [numSym +1];
   r[0][0] = numSym;
   r[1][0] = numSym;
        
   int j = 1;
   while (j <= numSym)
   {
      r[0][j] = x[j]*channelGain1[j];
      r[1][j] = x[j]*channelGain2[j];
      j++;
   }
   
   return r;
}

complex<double> **Dummy(complex<double> *z, int numRxANT)
{
   const int signalLen = (int)real(z[0]);
   const int numTxAnt = signalLen/Nfft;
   
   // Allocate memory for channelGain[]
   if (channelGain1 == NULL)
   {
      channelGain1 = new complex<double> [Nfft*numTxAnt +1]; channelGain1[0] = Nfft*numTxAnt;
      channelGain2 = new complex<double> [Nfft*numTxAnt +1]; channelGain2[0] = Nfft*numTxAnt;
      for (int i=1;i<=Nfft*numTxAnt;i++)
      { channelGain1[i] = 1.0; channelGain2[i] = 1.0; }
   }
   
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
