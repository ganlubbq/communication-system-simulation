/* 
 * Noise simulation for ligand-receptor binding. 
 * This program simulates the Brownian motion behavior of the particles and find
 * the time until which the first collision between a receptor and a particle 
 * occurs. 
 * The parameters are from the original paper: Noise Analysis in Ligand-binding 
 * Reception for Molecular Communication in Nanonetworks. 
 * The purpose of writing this simulation program is to prove that the results 
 * in the aforementioned paper is wrong. 
 * The number of particles 2e6 is calculated by assuming a concentration of 
 * ~500 particles/um^3 and ro = 10um
 *
 * Based on my simulation result, ~40 time steps are required for the 1st 
 * colision to happen, i.e., for a receptor to become bound.
 *
 * Author: Ling-San Meng
 * Email: f95942117@gmail.com
 * Date: 2011/11/21
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

inline bool withinRx (double x, double y, double z, double ro_sq)
{
   if (x*x + y*y + z*z <= ro_sq) { return true; }
   else { return false; }
}

inline void getRandPos(LAB524Rand* rng, double ro, double ro_sq , double *x, double *y, double *z)
{
   double a, b, c;
   
   while (1)
   {
      a = ro*(2*rng->getUniform() - 1);
      b = ro*(2*rng->getUniform() - 1);
      c = ro*(2*rng->getUniform() - 1);

      if (withinRx(a,b,c,ro_sq)) { break; }
   }
   
   *x = a;
   *y = b;
   *z = c;
}

int main()
{
   // Simulation parameters
   const double T = 1000000000000; // Total simulation time, in sec
   const double delta = 0.001; // Simulation time step, in sec
   const int N = 500; // Total number of chemical receptors
   const double kp = 0.2; // micrometer sec
   const double km = 10; // sec
   const double ro = 10; // micrometer
   const double ro_sq = ro*ro; 
   const double D = 100; // micrometer^2/
   const double A = 500; 
   const double rxVol = (4.0*pi*ro*ro*ro)/3.0;
   const double ro_p =  getValue("ro_p", "config.txt");
   const double ro_u =  ro_p;
   const double ro_p_plus_ro_u = ro_p + ro_u;
   const double ro_p_plus_ro_u_sq = ro_p_plus_ro_u*ro_p_plus_ro_u;
   
   // Time, rand, and fileIO
   LAB524Rand rng(time(NULL));
   Timer timer;
   ofstream data;
   data.open("data.txt",ios::app);

   // Initialize receptors positions.
   double *Ku_x = new double [N+1];   
   double *Ku_y = new double [N+1];   
   double *Ku_z = new double [N+1];   
   for (int i=1;i<=N;i++)
   { getRandPos(&rng, ro, ro_sq , Ku_x+i, Ku_y+i, Ku_z+i); }

   // Declare particle postitions   
   const int P =  getValue("Number of particles", "config.txt");
   double *Kp_x = new double [P+1];   
   double *Kp_y = new double [P+1];   
   double *Kp_z = new double [P+1];   
   for (int i=1;i<=P;i++)
   { getRandPos(&rng, ro, ro_sq , Kp_x+i, Kp_y+i, Kp_z+i); }
   
   double std = sqrt(2*D*delta);
   unsigned int nMotion = 0;
   unsigned int numOutaRange = 0;
   
   // Run time index
   for (int i=2;i<=(int)(T/delta);i++)
   {     
      cout << "time step: " << i << endl;
  
      for (int j=1;j<=P;j++)
      {
        double d_x = rng.getRealGaussian() * std;
        double d_y = rng.getRealGaussian() * std;
        double d_z = rng.getRealGaussian() * std;
    
        // Move
        Kp_x[j] += d_x; Kp_y[j] += d_y; Kp_z[j] += d_z;
        
        if (Kp_x[j]*Kp_x[j] + Kp_y[j]*Kp_y[j] + Kp_z[j]*Kp_z[j] > ro_sq)
        {
          getRandPos(&rng, ro, ro_sq , Kp_x+j, Kp_y+j, Kp_z+j);
        }
        else
        {
          // Check collision with receptors
          for (int k=1;k<=N;k++)
          {
            // Get the distance from this receptor
            double a = Kp_x[j]-Ku_x[k];
            if ((a>ro_p_plus_ro_u) || (-a>ro_p_plus_ro_u)) continue;
            else
            {
              double b = Kp_y[j]-Ku_y[k];
              if ((b>ro_p_plus_ro_u) || (-b>ro_p_plus_ro_u)) continue;
              else
              {
                double c = Kp_z[j]-Ku_z[k];
                if ((c>ro_p_plus_ro_u) || (-c>ro_p_plus_ro_u)) continue;
                else if (a*a+b*b+c*c < ro_p_plus_ro_u_sq)
                {
                  cout << "First collision" << endl;
                  data << "First collision occurs at time step " << i << endl;
                  getchar();
                  break;
                }
              }
            }
          }
        }
    }
      
   }
    
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
