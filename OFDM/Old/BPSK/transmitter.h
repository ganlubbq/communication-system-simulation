/* Simulate the transmitter side
 * Encoding plus Modulation
 * u[] is the original data bits
 * v[] is the encoded data bits
 * r[] is the modulated baseband signal
 * BPSK modulation 
 */
void modulation(int *v,float *r){
     int j=0;
     
     while (j<(2*(K+m))){
    
              r[j]=2*v[j]-1;             //BPSK modulation             
              
              j++;
              }
    }
    
void ifft(float *r,struct complex *x){
     int i,n,k;
     int S;                               //number of ofdm symbols
     float real,imag;
     
     S=(K+m)*2/N;
     
     for (i=0;i<S;i++){                   //Index of ofdm symbol
         for (n=CP;n<N+CP;n++){           //IFFT
         
             real=0;
             imag=0;
             for (k=0;k<N;k++){
                 real+=r[i*N+k]*cos(k*2*pi*n/N);
                 imag+=r[i*N+k]*sin(k*2*pi*n/N);
             
                 }
             x[i*(N+CP)+n].re=real;
             x[i*(N+CP)+n].im=imag;
             printf("% 6.2f ",real);
             printf("% 6.2f \n",imag);
             }
         
         }
         getchar();    
     }
     
void OFDM(float *r,struct complex *x){    //In ofdm convention, r[] here = X[k]
     int i,n;
     int S;                               //number of ofdm symbols
     S=(K+m)*2/N;
     
     ifft(r,x);
                                           
     for (i=0;i<S;i++){                    //Add Cyclic prefix
         for (n=0;n<CP;n++){           
             x[i*(N+CP)+n].re=x[i*(N+CP)+N+n].re;
             x[i*(N+CP)+n].im=x[i*(N+CP)+N+n].im;
             }
         
         }  
     
     
     //FFT test
     /*for (i=0;i<S;i++){
         for (k=0;k<N;k++){
         
             real=0;
             for (n=CP;n<N+CP;n++){
                 real+=x[i*(N+CP)+n].re*cos(k*2*pi*n/N) + x[i*(N+CP)+n].im*sin(k*2*pi*n/N);
                 }
             
             r[i*N+k]=real/N;
             }
         }*/
      
     
     } 

void input(int *u,int *v,struct complex *x){
     int i,j,temp1,temp2;
     float r[2*(K+m)];
     
     for (i=-m;i<K+m;i++){
         if (-1<i && i<K) u[i]=rand()%2;
         else u[i]=0;
         }
  
     for (i=0;i<K+m;i++){                 //encoding
        
         temp1=u[i]*g11[0];
         temp2=u[i]*g12[0];
        
         for (j=1;j<=m;j++){
        
             temp1=temp1^u[i-j]*g11[j];
             temp2=temp2^u[i-j]*g12[j];
             }
        
         v[2*i]=temp1;
         v[(2*i)+1]=temp2;
         }
    
     modulation(v,r);
     
     OFDM(r,x);
     }
