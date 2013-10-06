#ifndef DEFINE_H
#define DEFINE_H

//#define QUANTIZATION_BIT 1           // 1 corresponds to HDD

const int G11 = 035;
const int G12 = 023;

const int m = 4;                       // memory order = m
const int st_num = (1<<m);             // number of states = 2^m
       
const int k = 128;                     // frame length

const float gStep = 0.001;              // Additive increment of MAF
const float aStep = 0.001;              // Additive increment of aAF


#endif
