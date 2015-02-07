First, the main Q format used here are Qa and Qb
Qa is for the viterbi, and Qb is for the OFDM symbol time domain samples x[].
Why choose Q14, because Q15 will cause some problems...
and about the Qb, the gain of IFFT is about ((stage>>1)+2), which is observed in simulation program. Actually the gain of IFFT is quite irregular,
maybe it's good to list a table

Nfft stage gain
256  8     64
128  7     32
64   6     32
32   5     16
16   4     16
8    3     8
(observed in simulation program)
gain=(stage>>1)+2 roughly holds when Nfft is below 256

For convenience, just set a value for Qa, and then try Qb till there is no error in decoding
maybe it's good to list a table again

Nfft stage Qa-Qb
1024 10    8
512  9     7
256  8     6
128  7     
64   6     
32   5     
16   4     
8    3     