#ifndef DEFINE_H
#define DEFINE_H

#include "fileIO.h"
#include "tbcc.h"
#include "modem.h"

#define PSK 0
#define QAM 1

#define THEORETICAL_BER 1

const int NUM_PDCCH_SYM = 2;

// See p. 173 in "LTE for 4G mobile broadband"
const int Nfft = (int)getValue("Nfft", "config.txt");
const int NUM_RB = (int)getValue("Number of physical resource blocks", "config.txt");
const int NUM_USEDCAR = NUM_RB*12;
const int CPLen = (int)((double)Nfft/4.0);

// Number of subcarriers per resource block.
const int N_SC = 12;

// Number of ODFM symbols per subframe. Assuming normal CP length.
const int N_SYM = 14;

// "NUM_RS_PER_RB" = 8, 16, 20, 24 for Tx antennas = 1, 2, 3, 4 respectively.
// See p. 201 in "LTE for 4G mobile broadband".
const int NUM_RS_PER_RB = 16;

// Some constant definition for OFDM RE mapping.
const int RS = 0;
const int USER_DATA = 1;
const int PDCCH = 2;
const int RPDCCH = 3;

struct TxAttribute
{
   // Modulation formats.
   int modFormat;
   int bitPerSym;
   
   // Symbol energy. Assume the energy of the carriers except the RSs are
   // all the same.
   double E_car_RS;
   double E_car_data;
   
   // Modems.
   PskModulator psk;
   QamModulator qam;
   
   // Tail-biting codec.
   TBCC tbcc;

   // Interleaving permutation of the coded bits.
   int *permutation;

   // Length of coded bits.
   int codedBitLen_c;
   // Length of coded bits after truncation (due to mod. or rate matching).
   int codedBitLen_c_trun;       
};

#endif

