/* FFT and IFFT functions
 * w is the trigonometric coefficient table, and should be global
 * function arguments are orderd as (input, output, Nfft size)
 */
 

struct complex{
       short re;
       short im;
       };

struct intcomplex{
       int re;
       int im;
       };
   
struct complex w[Nfft>>1];//Q15
short order[Nfft];

void fft_coef(struct complex *w,int npoints){//w[] -> Q15 
     short i,j,position;
     
     for (i=0;i<npoints>>1;i++){
         w[i].re=32767*cos(2*pi*i/npoints);
         w[i].im=(-1)*32767*sin(2*pi*i/npoints);
         }
     
     for (i=0;i<npoints;i++){
        position=0;
        for (j=0;j<stage;j++){
            position+=(((i>>j)&1)<<(stage-1-j));
            }
        
        order[i]=position;
        }
     }
            
void ifft(struct complex *x,struct complex *r,int npoints){//x[n] -> Qa, r[] -> Qb
     short s,i,j,k;
     short offset,branch;
     struct complex tempw;
     struct intcomplex *input,*output,*temp,temp1,temp2,temp3;

      input=(struct intcomplex *) malloc(npoints*sizeof(struct intcomplex));
     output=(struct intcomplex *) malloc(npoints*sizeof(struct intcomplex));
     
     for (s=0;s<S;s++){
     
     
     for (i=0;i<npoints;i++){
        input[i].re=x[s*npoints+order[i]].re>>scale;//pre scaling to avoid overflow
        input[i].im=x[s*npoints+order[i]].im>>scale;
        }
      
     for (i=0;i<stage;i++){//loop for the main 3 stages
     
         for (j=0;j<npoints>>(i+1);j++){//loop for the processing blocks
             
             branch=(1<<i);//half branch
             offset=j*(branch<<1);//offset to the branch number now
             
             for (k=0;k<branch;k++){
                 temp1=input[offset+k];
                 
                 temp3=input[offset+branch+k];
                 tempw=w[k<<(stage-i-1)];
                 
                 temp2.re=(temp3.re*tempw.re+temp3.im*tempw.im)>>15;
                 temp2.im=(-temp3.re*tempw.im+temp3.im*tempw.re)>>15;
                 
                 //upper branches 
                 output[offset+k].re=temp1.re+temp2.re;
                 output[offset+k].im=temp1.im+temp2.im;
                 
                 //lower branches
                 output[offset+branch+k].re=temp1.re-temp2.re;//complex add
                 output[offset+branch+k].im=temp1.im-temp2.im;
                 }
             }
             
         //handoff in out
         temp=input;
         input=output;
         output=temp;
         }

     for (i=0;i<npoints;i++){
         r[s*npoints+(s+1)*CP+i].re=input[i].re>>1;//(Qa-Qb-scale)
         r[s*npoints+(s+1)*CP+i].im=input[i].im>>1;
         }
     }
     
     free(input);
     free(output);
     }


           

void fft(struct complex *x,struct complex *r,int npoints){//x[n] -> Qb, r[] -> Qa
     short s,i,j,k;
     short offset,branch;
     struct complex tempw;
     struct intcomplex *input,*output,*temp,temp1,temp2,temp3;///Q15
     
     input=(struct intcomplex *) malloc( npoints*sizeof(struct intcomplex));
     output=(struct intcomplex *) malloc(npoints*sizeof(struct intcomplex));
     
     for (s=0;s<S;s++){
     
     for (i=0;i<npoints;i++){
        input[i].re=(int)x[s*npoints+(s+1)*CP+order[i]].re>>scale;//divide by Nfft in advance, and shift to become a pure
        input[i].im=(int)x[s*npoints+(s+1)*CP+order[i]].im>>scale;
        
        }
            
     for (i=0;i<stage;i++){//loop for the main 3 stages
     
         for (j=0;j<npoints>>(i+1);j++){//loop for the processing blocks
             
             branch=(1<<i);//half branch
             offset=j*(branch<<1);//offset to the branch number now
             
             for (k=0;k<branch;k++){
                 temp1=input[offset+k];
                 
                 temp3=input[offset+branch+k];
                 tempw=w[k<<(stage-i-1)];
                 
                 temp2.re=(temp3.re*tempw.re-temp3.im*tempw.im)>>15;
                 temp2.im=(temp3.re*tempw.im+temp3.im*tempw.re)>>15;
                 
                 //upper branches 
                 output[offset+k].re=temp1.re+temp2.re;
                 output[offset+k].im=temp1.im+temp2.im;
                 
                 //lower branches
                 output[offset+branch+k].re=temp1.re-temp2.re;//complex add
                 output[offset+branch+k].im=temp1.im-temp2.im;
                 }
             }
             
         //handoff in out
         temp=input;
         input=output;
         output=temp;
         }

     for (i=0;i<npoints;i++){
         r[s*npoints+i].re=input[i].re<<(5-(stage&1));//(scale-stage+Qa-Qb)
         r[s*npoints+i].im=input[i].im<<(5-(stage&1));
         }
     
     }
     
     free(input);
     free(output);
     }
     

     
