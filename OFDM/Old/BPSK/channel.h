/* Simulate the channel effect
 * BPSK modultaion
 * r[] is the modulated baseband signal plus noise
 *
 */

void channel(struct complex *x,float snr){
     int i=0,j=0,S;
     float N0,sd,temp;
     
     S=(K+m)*2/N;
     
     N0=N*2/snr;                         //Before OFDM, SNR = (Es*2)/N0 , original Eb=2*Es, Es is set to be 1
                                         //After OFDM, Es becomes N times larger, so N0 *= N
      
     sd=sqrt(N0/2);                      //standard deviation of gaussian r.v.
   
     while (i<S){
              
              for (j=0;j<N+CP;j++){
                  temp=gaurand()*sd;
                  //x[i*(N+CP)+j].re+=temp;//add noise
                  //x[i*(N+CP)+j].im+=temp;
                  }
                  
              /*ray[i]=rayrand();
              for (j=0;j<N+CP;j++){
                  temp=gaurand()*sd;
                  x[i*(N+CP)+j].re=temp+x[i*(N+CP)+j]*ray[i];
                  x[i*(N+CP)+j].im=temp+x[i*(N+CP)+j]*ray[i];
                  }*/
     
              i++;
              }          
              
    }

