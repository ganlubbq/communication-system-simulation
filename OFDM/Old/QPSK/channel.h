/* Simulate the channel effect
 * BPSK modultaion
 * r[] is the modulated baseband signal plus noise
 *
 */

void channel(struct complex *x,float snr){
     int i=0,j=0;
     float N0,sd;
    
     N0=N/snr;                           //Before OFDM, SNR = Es/N0 , original Eb=Es, Es is set to be 1
                                         //After OFDM, Es becomes N times larger, so N0 *= N
                                         
     sd=sqrt(N0/2);                      //standard deviation of gaussian r.v.
   
     while (i<S){
              
              for (j=0;j<N+CP;j++){
                  x[i*(N+CP)+j].re+=gaurand()*sd;//add noise
                  x[i*(N+CP)+j].im+=gaurand()*sd;
                  }
                  
              i++;
              }          
              
    }

