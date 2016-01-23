/* 
 * Noise simulation for ligand-receptor binding. 
 * This program simulates the Brownian motion behavior of one particle and find
 * how many times of the brownian motion are required for the first collision 
 * between a receptor and the particle to occur. The number of times for the 
 * particle to diffuse out of the receptor space and then randomly regenerated 
 * is also found.
 * The parameters are from the original paper: Noise Analysis in Ligand-binding 
 * Reception for Molecular Communication in Nanonetworks. 
 * The purpose of writing this simulation program is to prove that the results 
 * in the aforementioned paper is wrong. 
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
   
   //// Run time index

double std = sqrt(2*D*delta);
unsigned int nMotion = 0;
unsigned int numOutaRange = 0;
double Kp_x, Kp_y, Kp_z;

for (int i=2;i<=(int)(T/delta);i++)
{     
    if (i%10000 == 0) { cout << i*delta << " sec" << endl; }  

    double d_x = rng.getRealGaussian() * std;
    double d_y = rng.getRealGaussian() * std;
    double d_z = rng.getRealGaussian() * std;

    // Move
    Kp_x = Kp_x+d_x; Kp_y = Kp_y+d_y; Kp_z = Kp_z+d_z;
    nMotion = nMotion + 1;
    
    if (Kp_x*Kp_x + Kp_y*Kp_y + Kp_z*Kp_z > ro_sq)
    {
       numOutaRange = numOutaRange + 1;

      getRandPos(&rng, ro, ro_sq , &Kp_x, &Kp_y, &Kp_z);
    }
    else
    {
      // Check collision with receptors
      for (int k=1;k<=N;k++)
      {
        // Get the distance from this receptor
        double a = Kp_x-Ku_x[k];
        if ((a>ro_p_plus_ro_u) || (-a>ro_p_plus_ro_u)) continue;
        else
        {
          double b = Kp_y-Ku_y[k];
          if ((b>ro_p_plus_ro_u) || (-b>ro_p_plus_ro_u)) continue;
          else
          {
            double c = Kp_z-Ku_z[k];
            if ((c>ro_p_plus_ro_u) || (-c>ro_p_plus_ro_u)) continue;
            else if (a*a+b*b+c*c < ro_p_plus_ro_u_sq)
            {
              cout << "nMotion: " << nMotion << endl;
              cout << "numOutaRange: " << numOutaRange << endl;
              getchar();
              break;
            }
          }
        }
      }
    }
}
   
   
  getchar();
  return 0;  
}
