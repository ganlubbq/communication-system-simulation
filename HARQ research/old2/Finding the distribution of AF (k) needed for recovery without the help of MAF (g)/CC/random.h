/* Modify the normal r.v. generator
 * Date : 2007/4/14
 * --------------------------------
 * Change to .h and .cpp
 * Date : 2007/7/23
 */

#ifndef RANDOM_H
#define RANDOM_H

// Returns an uniform [0,1] r.v.
inline float uniformRV(void);

// Returns a normal r.v.
float normalRV(float mean = 0, float variance = 1.0);

// Returns a Rayleigh r.v.
// Mean power = 2*sigma^2
float rayleighRV(float meanPower = 1.0);

// Generate an exponential r.v.
float expRV(float rate);

#endif
