/* DFT and IDFT functions
 * w is the trigonometric coefficient table, and should be global
 * function arguments are orderd as (input, output, Nfft size)
 * Qx = 7 can support up to Nfft size 256
 * the Q formats for real, and imag local variables,
 * are planned originally for Nfft size = 128,
 * but it turned out to have no overflow when Nfft is larger
 */

struct complex{
       short re;
       short im;
       };

struct complex w[Nfft];//Q15
  
void fft_coef(struct complex *w,int npoints){
     short i,j;
     
     for (i=0;i<npoints;i++){
         w[i].re=32767*cos(2*pi*i/npoints);
         w[i].im=(-1)*32767*sin(2*pi*i/npoints);
         }
     }
     
void ifft(struct complex *r,struct complex *x,int npoints){//r[] -> Qa, x[n] -> Qb
     short i,n,k,temp;
     
     int real,imag;//Qa+15-stage
     
     
     for (i=0;i<S;i++){                         //Index of ofdm symbol
         for (n=CP;n<npoints+CP;n++){
         
             real=0;
             imag=0;
             for (k=0;k<npoints;k++){
             
                 temp=(k*(n-CP)) & maskfft;
                 real=real + ((int)r[i*npoints+k].re*w[temp].re>>stage)-((int)r[i*npoints+k].im*-w[temp].im>>stage);
                 imag=imag + ((int)r[i*npoints+k].re*-w[temp].im>>stage)+((int)r[i*npoints+k].im*w[temp].re>>stage);
                 }
             x[i*(npoints+CP)+n].re=real>>(Qa-Qb+15-stage);
             x[i*(npoints+CP)+n].im=imag>>(Qa-Qb+15-stage);
             
             }
         }  
     }
     
void fft(struct complex *x,struct complex *r,int npoints){//x[n] -> Qb, r[] -> Qa
     short i,k,n,temp;
     
     int real,imag;//Qb+15-stage
     
     
     for (i=0;i<S;i++){
         for (k=0;k<npoints;k++){
         
             real=0;
             imag=0;
             for (n=CP;n<npoints+CP;n++){
                 temp=(k*(n-CP)) & maskfft;
                 real= real+((int)x[i*(npoints+CP)+n].re*w[temp].re>>stage) + ((int)x[i*(npoints+CP)+n].im*-w[temp].im>>stage);
                 imag= imag-((int)x[i*(npoints+CP)+n].re*-w[temp].im>>stage) + ((int)x[i*(npoints+CP)+n].im*w[temp].re>>stage);
                 }
             
             r[i*npoints+k].re=real>>(Qb+1);//real>>(4 + log2(Nfft))
             r[i*npoints+k].im=imag>>(Qb+1);
             }
         }
     }

           
