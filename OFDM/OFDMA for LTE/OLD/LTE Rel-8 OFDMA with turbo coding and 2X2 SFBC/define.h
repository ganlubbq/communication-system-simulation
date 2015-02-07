#ifndef DEFINE_H
#define DEFINE_H

#include "fileIO.h"

#define PSK 0
#define QAM 1

#define THEORETICAL_BER 1

const int NUM_PDCCH_SYM = 0;

// See p. 173 in "LTE for 4G mobile broadband"
const int Nfft = (int)getValue("Nfft", "config.txt");
const int NUM_RB = (int)getValue("Number of physical resource blocks", "config.txt");
const int NUM_USEDCAR = NUM_RB*12;

// Number of subcarriers per resource block.
const int N_SC = 12;

// Number of ODFM symbols per subframe. Assuming normal CP length.
const int N_SYM = 14;

// "NUM_RS_PER_RB" = 8, 16, 20, 24 for Tx antennas = 1, 2, 3, 4 respectively.
// See p. 201 in "LTE for 4G mobile broadband".
const int NUM_RS_PER_RB = 16;

#endif

