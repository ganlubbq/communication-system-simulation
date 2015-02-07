/* FFT and IFFT functions
 * w[] is the trigonometric coefficient table, and should be global
 * order[] is the fft input order generator
 * function arguments are orderd as (input, output, Nfft size)
 *
 * PS : some decalaration ,defines, and fft_coef should be placed 
 *      to the decalaration header file for consistency
 */

#define maskfft Nfft-1
#define stage (int)((float)log(Nfft)/log(2))//for efficiency, please calculate it out !

struct complex{
       float re;
       float im;
       };

struct complex w[Nfft>>1];
short order[Nfft];

void fft_coef(struct complex *w,int npoints){
     short i,j,position;
     
     for (i=0;i<npoints>>1;i++){
         w[i].re=cos(2*pi*i/npoints);
         w[i].im=-sin(2*pi*i/npoints);
         }
         
      for (i=0;i<npoints;i++){
        position=0;
        for (j=0;j<stage;j++){
            position+=(((i>>j)&1)<<(stage-1-j));
            }
        
        order[i]=position;
        }
     }



void ifft(struct complex *x,struct complex *r,int npoints){
     short s,i,j,k,n;
     short offset,branch;
     short position;
     struct complex *input,*output,*temp,temp1,temp2;
     
     input=(struct complex *) malloc( npoints*sizeof(struct complex));
     output=(struct complex *) malloc(npoints*sizeof(struct complex));
     
     for (s=0;s<S;s++){
     
     /*
     for (i=0;i<npoints;i++){
        position=0;
        for (j=0;j<stage;j++){
            position+=(((i>>j)&1)<<(stage-1-j));
            }
        
        input[i]=x[s*npoints+position];
        }*/
     for (i=0;i<npoints;i++){
        input[i]=x[s*npoints+order[i]];
        }
        
        
     for (i=0;i<stage;i++){//loop for the main 3 stages
     
         for (j=0;j<npoints>>(i+1);j++){//loop for the processing blocks
             
             branch=(1<<i);//half branch
             offset=j*(branch<<1);//offset to the branch number now
             
             for (k=0;k<branch;k++){
                 temp1=input[offset+k];
                 
                 temp2.re=input[offset+branch+k].re*w[k<<(stage-i-1)].re+input[offset+branch+k].im*w[k<<(stage-i-1)].im;
                 temp2.im=-input[offset+branch+k].re*w[k<<(stage-i-1)].im+input[offset+branch+k].im*w[k<<(stage-i-1)].re;
                 
                 //upper branches 
                 output[offset+k].re=temp1.re+temp2.re;
                 output[offset+k].im=temp1.im+temp2.im;
                 
                 //lower branches
                 output[offset+branch+k].re=temp1.re-temp2.re;//complex add
                 output[offset+branch+k].im=temp1.im-temp2.im;
                 }
             }
             
         maxre=output[0].re;
         maxim=output[0].im;
         for (j=0;j<npoints;j++){
        if(output[j].re>maxre)maxre=output[j].re;
        if(output[j].im>maxim)maxim=output[j].im;
        }
    printf("stage : %d ",i);
    printf("%max.re is : %7.2f ",maxre);
    printf("%max.im is : %7.2f\n",maxim);
    system("pause");
    
         
         //handoff in out
         temp=input;
         input=output;
         output=temp;
         }

     for (i=0;i<npoints;i++){
         r[s*npoints+(s+1)*CP+i].re=input[i].re/sqrt(npoints);
         r[s*npoints+(s+1)*CP+i].im=input[i].im/sqrt(npoints);
         }
     }
     
     free(input);
     free(output);
     }


void fft(struct complex *x,struct complex *r,int npoints){
     short s,i,j,k,n;
     short offset,branch;
     short position;
     struct complex *input,*output,*temp,temp1,temp2;
     
     input=(struct complex *) malloc( npoints*sizeof(struct complex));
     output=(struct complex *) malloc(npoints*sizeof(struct complex));
     
     for (s=0;s<S;s++){
         
     /*for (i=0;i<npoints;i++){
        position=0;
        for (j=0;j<stage;j++){
            position+=(((i>>j)&1)<<(stage-1-j));
            }
        
        input[i]=x[s*npoints+(s+1)*CP+position];
        }*/
     for (i=0;i<npoints;i++){
        input[i].re=x[s*npoints+(s+1)*CP+order[i]].re;
        input[i].im=x[s*npoints+(s+1)*CP+order[i]].im;
        }
               
     for (i=0;i<stage;i++){//loop for the main 3 stages
     
         for (j=0;j<npoints>>(i+1);j++){//loop for the processing blocks
             
             branch=(1<<i);//half branch
             offset=j*(branch<<1);//offset to the branch number now
             
             for (k=0;k<branch;k++){
                 temp1=input[offset+k];
                 
                 temp2.re=input[offset+branch+k].re*w[k<<(stage-i-1)].re-input[offset+branch+k].im*w[k<<(stage-i-1)].im;
                 temp2.im=input[offset+branch+k].re*w[k<<(stage-i-1)].im+input[offset+branch+k].im*w[k<<(stage-i-1)].re;
                 
                 //upper branches 
                 output[offset+k].re=temp1.re+temp2.re;
                 output[offset+k].im=temp1.im+temp2.im;
                 
                 //lower branches
                 output[offset+branch+k].re=temp1.re-temp2.re;//complex add
                 output[offset+branch+k].im=temp1.im-temp2.im;
                 }
             }
             
          maxre=output[0].re;
         maxim=output[0].im;
         for (j=0;j<npoints;j++){
        if(output[j].re>maxre)maxre=output[j].re;
        if(output[j].im>maxim)maxim=output[j].im;
        }
    printf("stage : %d ",i);
    printf("%max.re is : %7.2f ",maxre);
    printf("%max.im is : %7.2f\n",maxim);
    system("pause");
       
         //handoff in out
         temp=input;
         input=output;
         output=temp;
         }

     for (i=0;i<npoints;i++){
         r[s*npoints+i].re=input[i].re/sqrt(npoints);
         r[s*npoints+i].im=input[i].im/sqrt(npoints);
         }
     
     }
     
     free(input);
     free(output);
     }
     
