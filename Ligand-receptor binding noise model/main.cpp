/* 
 * Noise simulation for ligand-receptor binding. 
 * This simulation is extremely computational intensive.
 * Simulate the noise of ligand-receptor using the ligand-receptor kinetics
 * model proposed in Pierobon, M. and Akyildiz, I. F., “Noise Analysis in 
 * Ligand-binding Reception for Molecular Communication in Nanonetworks,” 
 * IEEE Transactions on Signal Processing, vol. 59, no. 9, pp. 4168–4182, 
 * September 2011. 
 * The key parameters claimed to be used in the paper are as follows: 
 * T=5, delta=0.001, ro_p=ro_u=0.001, A=500, and D=100
 *
 * Originally this simulation is written in Matlab. I was hoping that in C it 
 * could be a lot faster, which turns out to be quite the opposite. I guess it 
 * is because of the "list" class. I wonder how dynamic arrays are handled in 
 * Matlab.
 *
 * Author: Ling-San Meng
 * Email: f95942117@gmail.com
 * Date: 2011/11/15
 */
 
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <list>

using namespace std;

const double pi = 3.1415926;

#include "LAB524Rand.h"
#include "timer.h"
#include "fileIO.h"
#include "subfunction.h"

int main()
{
//   const float start = getValue("From SNR", "config.txt");
   
   // Simulation parameters
   const double T = 5.0; // Total simulation time, in sec
   const double delta = 0.001; // Simulation time step, in sec
   const int N = 500; // Total number of chemical receptors
   const double kp = 0.2; // micrometer sec
   const double km = 10; // sec
   const double ro = 10; // micrometer
   const double ro_sq = ro*ro; 
   const double D = 100; // micrometer^2/
   const double A = 1; 
   const double rxVol = (4.0*pi*ro*ro*ro)/3.0;
   
   // Time, rand, and fileIO
   LAB524Rand rng(time(NULL));
   Timer timer;
   ofstream data;
   data.open("data.txt",ios::app);
   
   // Concentration function goes here
   double *Cr = new double [(int)(T/delta)+1];
   for (int i=1;i<=(int)(T/delta);i++)
   { Cr[i] = A*sin(2*pi*0.8*(i*delta)) + A; }
   
   // Initialization
   double *n_b = new double [(int)(T/delta)+1];
   int *n_u = new int [(int)(T/delta)+1];
   n_b[1] = 0; // Number of bound receptors. This is continuous
   n_u[1] = N; // Number of unbound receptors. This is discrete
   
   // Initialize receptors positions.
   double *Ku_x = new double [N+1];   
   double *Ku_y = new double [N+1];   
   double *Ku_z = new double [N+1];   
   for (int i=1;i<=N;i++)
   { getRandPos(&rng, ro, ro_sq , Ku_x+i, Ku_y+i, Ku_z+i); }
   
   // Declare particle postitions
   list<double> Kp_x;
   list<double> Kp_y;
   list<double> Kp_z;
      
   // Receptor bound indicator
   int *bound_ind = new int [N+1];
   for (int i=1;i<=N;i++) { bound_ind[i] = 0; }

   // Stores the unbound receptor indices
   list<int> unbound_idx;
   for (int i=1;i<=N;i++) { unbound_idx.push_back(i); }
   // Stores the bound receptor indices
   list<int> bound_idx;

   // Number of particles inside the receptor
   int *P = new int [(int)(T/delta)+1];
   P[1] = 0; 
   
   double diff_n_b = 0;
   
   // Run time index
   for (int i=2;i<=(int)(T/delta);i++)
   {     
cout << i << endl;
      Cr[i] = Cr[i] - diff_n_b/rxVol; // Rectify the concentration based on the previous bounding situation
      P[i] = round(Cr[i] * rxVol); // Get the number of particles inside receptor  
cout << P[i] << endl; getchar();
      // Update particle position list (Random generation, random deletion)
      if (P[i] > P[i-1])
      {            
         //random generation
         int n = P[i] - P[i-1];
         for (int j=1;j<=n;j++)
         {
           int p_idx = P[i-1]+j;
           getRandPos(&rng, ro, ro_sq , &Kp_x, &Kp_y, &Kp_z, p_idx);
         }
      }
      else if (P[i] < P[i-1]) 
      {
         // random deletion
         int n = P[i-1] - P[i];
         for (int j=1;j<=n;j++)
         {
            int p_idx = ceil(rng.getUniform()*(Kp_x.size()));
            listErase(&Kp_x, p_idx);
            listErase(&Kp_y, p_idx);
            listErase(&Kp_z, p_idx);
         }
      } 
      else {} 

      // Update the particles positions after Brownian motion
      for (int j=1;j<=P[i];j++)
      {
         double a = listInc(&Kp_x, j, rng.getRealGaussian()*2*D*delta);
         double b = listInc(&Kp_y, j, rng.getRealGaussian()*2*D*delta);
         double c = listInc(&Kp_z, j, rng.getRealGaussian()*2*D*delta);
         
         // if (outOfRange) regenerate a new one;
         if (a*a+b*b+c*c > ro_sq)
         { getRandPos(&rng, ro, ro_sq , &Kp_x, &Kp_y, &Kp_z, j); }
      }
      
      //   Check for collisions -> differential increment
      //   n_u(i-1) unbound receptors and positions, P(i) particles and positions
      
       
      double diff_inc = 0;
      double diff_dec = km*n_b[i-1]*delta; // Compute k- * n_b * timeStep -> differential decrement
      diff_n_b = diff_inc - diff_dec; // Take their difference -> d n_b
      n_b[i] = n_b[i-1] + diff_n_b; // Update number of bound
      
      // Update number of unbound based on "d n_b"
      // Update receptor bound position list
      n_u[i] = round(N - n_b[i]); 
      
      if (n_u[i] > n_u[i-1]) // some receptors are being freed
      {   
        // Randomly select n from bound_idx to become unbound
        int n = n_u[i] - n_u[i-1];
        for (int j=1;j<=n;j++)
        {
           int idx = ceil(rng.getUniform()*(N-n_u[i-1])); // Pick an array idx from bound list
           
           int receptor_num = listErase(&bound_idx, idx); // Pop out a receptor num from bound list 
           unbound_idx.push_back( receptor_num ); // Append the receptor idx to the unbound list
           bound_ind[receptor_num] = 0; // Set the indicator corresponding to the receptor idx to 0/unbound
        }
      }
      else if (n_u[i] < n_u[i-1]) // some receptors are being occupied
      {
         // Randomly select n from unbound_idx to become bound
         int n = - n_u[i] + n_u[i-1];
         
         for (int j=1;j<=n;j++)
         {
            int idx = ceil(rng.getUniform()*(n_u[i-1])); // Pick an array idx from unbound list 
            
            int receptor_num = listErase(&unbound_idx, idx); // Pop out a receptor num from unbound list 
            bound_idx.push_back( receptor_num ); // Append the receptor idx to the bound list
            bound_ind[receptor_num] = 1; // Set the indicator corresponding to the receptor idx to 1/bound
         }
      }
        
      else {}
   }
    
/*
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
*/    
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
