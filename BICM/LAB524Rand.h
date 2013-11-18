#ifndef __LAB524_RAND_H__
#define __LAB524_RAND_H__

#include <iostream>
#include <cmath>
#include <complex>

/**********************************************************************

  The LAB524Rand class

  This class is developed by Da-shan Shiu to provide speedy execution
  and precise control of random number generation for LAB524 
  simulation. Here speedy algorithms are chosen. Aggressive inlining
  are used to enhance simulation speed as well.

  The fundamental algorithms are:
  - the Tausworthy random number generator, to generate random numbers
    from 0 to 2^32-1;
  - the Zigurat formula, to generate random Gaussian numbers.
  Both of the formula are taken from GSL (Gnu Scientific Library).

**********************************************************************/

// Types and parameters required by the Taus algorithm
#define LAB524_RAND_MASK 0xffffffffUL
#define LAB524_TAUSWORTHE(s,a,b,c,d) (((s &c) <<d) & LAB524_RAND_MASK) ^ ((((s <<a) &LAB524_RAND_MASK)^s) >>b)
typedef struct
{
  unsigned long int s1, s2, s3;
}
LAB524RandState_t;

// Types and parameters required by the Zigurat algorithm
/* position of right-most step */
#define LAB524_PARAM_R 3.44428647676

using namespace std;

class LAB524Rand
{
 public: 

  LAB524Rand() { setSeed(1912); };
  LAB524Rand(unsigned long int seed) { setSeed(seed); };

  // Method: set seed, to set the seed of the random number generator
  void setSeed(unsigned long int seed);

  // Method: getRand()
  // It returns a random integer between 0 and 2^32-1.
  inline unsigned long getRand()
  {
    _state.s1 = LAB524_TAUSWORTHE (_state.s1, 13, 19, 4294967294UL, 12);
    _state.s2 = LAB524_TAUSWORTHE (_state.s2, 2, 25, 4294967288UL, 4);
    _state.s3 = LAB524_TAUSWORTHE (_state.s3, 3, 11, 4294967280UL, 17);
    
    unsigned long x = _state.s1 ^ _state.s2 ^ _state.s3;
    return x; 
  }

  // Method: getRandBinary()
  // It returns a random binary number, 0 or 1.
  inline unsigned long getRandBinary()
  { return (getRand() & 4)>>2; };

  // Method: getUniform()
  // It returns a double precision floating point number uniformly 
  // distributed in the range [0,1). The range includes 0.0 but excludes 1.0. 
  inline double getUniform()
  { return getRand()*oneOverMaxPlusOne; }

  // Method: getRealGaussian
  // Generate real N(0,1) Gaussian value using the Zigurat formula,
  // the fastest formula known to-date.
  inline double getRealGaussian()
  {
    unsigned long  U, sign, i, j;
    double  x, y;
    while (1) 
    {
      U = getRand();
      i = U & 0x0000007F;     /* 7 bit to choose the step */
      sign = U & 0x00000080;  /* 1 bit for the sign */
      j = U>>8;		      /* 24 bit for the x-value */

      x = j*wtab[i];
      if (j < ktab[i])  break;

      if (i<127) {
	double  y0, y1;
	y0 = ytab[i];
	y1 = ytab[i+1];
	y = y1+(y0-y1)*getUniform();
      } else {
	x = LAB524_PARAM_R - log(1.0-getUniform())/LAB524_PARAM_R;
	y = exp(-LAB524_PARAM_R*(x-0.5*LAB524_PARAM_R))*getUniform();
      }
      if (y < exp(-0.5*x*x))  break;
    } // end while()
    y = sign ? x : -x;
    return y;
  };

  inline complex<double> getCmplxGaussian()
  { return complex<double>(getRealGaussian(), getRealGaussian()); }

  void getCmplxGaussian(complex<double>* arrayOfWhiteNoise, 
			const int numNoise);

  static const double oneOverMaxPlusOne;
  static const double ytab[128];
  static const double wtab[128];
  static const unsigned long ktab[128];
 protected:
  LAB524RandState_t _state;
};

class UniformIntegerRV : public LAB524Rand
{
 public:

  // Constructor
  UniformIntegerRV(const int seed, 
		   const int inclusiveLowerBound, 
		   const int exclusiveUpperBound);

  ~UniformIntegerRV();

  // Modification of configuration
  void setLB(int lowerBound);
  void setUB(int upperBound);

  // Get a random number
  inline int getAndUpdate() 
  { 
    return _lb+ (getRand() % _range); 
  };
   
 private:
  int _lb, _ub, _range;
};

#endif // __LAB524_RAND_H__
