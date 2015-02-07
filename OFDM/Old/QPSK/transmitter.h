/* Simulate the transmitter side
 * Encoding plus Modulation
 * u[] is the original data bits
 * v[] is the encoded data bits
 * r[] is the modulated baseband signal
 * BPSK modulation 
 */
 
void modulation(int *v,struct complex *r){
     int j=0;
     
     while (j<(K+m)){
              r[j].re=1.414213562*v[2*j]-0.707106781;             // I channel BPSK modulation, r.re = +- 1/sqrt(2)             
              r[j].im=1.414213562*v[2*j+1]-0.707106781;           // Q channel BPSK modulation, r.im = +- 1/sqrt(2)
              j++;
              }
    }

void OFDM(struct complex *r,struct complex *x){//In ofdm convention, r[] here = X[k]
     int i,n;
     
     ifft(r,x,N);
                                           
     for (i=0;i<S;i++){                    //Add Cyclic prefix
         for (n=0;n<CP;n++){           
             x[i*(N+CP)+n].re=x[i*(N+CP)+N+n].re;
             x[i*(N+CP)+n].im=x[i*(N+CP)+N+n].im;
             }
         
         }  
     } 

void input(int *u,int *v,struct complex *x){
     int i,j,temp1,temp2;
     struct complex r[K+m];
     
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
