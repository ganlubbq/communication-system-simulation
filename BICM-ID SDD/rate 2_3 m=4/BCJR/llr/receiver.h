void bit_metric_comput(struct complex *,double,int);
void siso(int);
void decode(int *,double *,double *);                                  //sub function,construct the decoded sequence using  optimal path

int output(int *c,double snr,int *r_d){
    int i,j=0;
    double N0,sd,temp=1;                                                 //temp = sqrt(Es)
    struct complex r[(k+m)/2];
    
    
    modulation(c,r);
    
    N0=temp/(2*snr);                                    //SNR = Eb/N0 , Es = 3Eb
    sd=sqrt(N0/2);                                    //standard deviation of gaussian r.v.
   
    while (j<(k+m)/2){
              r[j].re += gaurand()*sd;                 //r is the received vector                                 
              r[j].im += gaurand()*sd;
              
              ray[j]=rayrand();
              //r[j].re = r[j].re*ray[j] + gaurand()*sd;                 //r is the received vector                                 
              //r[j].im = r[j].im*ray[j] + gaurand()*sd;
              
              j++;
              }

   /*-----------------decoding--------------------*/ 
       
    for(i=0;i<iteration;i++){
       
        bit_metric_comput(r,N0,i);
        
        deinterleave(bit_metric_llr);
            
        siso(i);
            
        //interleave(id[0],id[1]);     
        interleave(llr);
        }
   
    decode(r_d,out[0],out[1]);

    return 1;
    }

double max(double *a,int l){
      double temp;
      int i;
      
      temp=a[0];
      for(i=0;i<l;i++){
           if(a[i]>temp) temp=a[i];
           }
      
      return temp;
      }

void bit_metric_comput(struct complex *r,double N0,int pass){
     int i,l,j,x,h,b0,b1,b2,temp1;
     double temp[8],ttemp[4],test1,test2,temp2;
     static double cache[8][(k+m)/2];
     
     for (i=0;i<(k+m)/2;i++){
         
         if(pass==0){
             
             for (j=0;j<8;j++){
                 //cache[j][i]=-(pow(mapping[j].re-r[i].re,2) + pow(mapping[j].im-r[i].im,2))/N0;
                 //cache[j][i]=-(pow(mapping[j].re-r[i].re,2) + pow(mapping[j].im-r[i].im,2));
                 cache[j][i]=2*(mapping[j].re*r[i].re + mapping[j].im*r[i].im)/N0;
                 }

             //bit based metric ; "metric a_b" ,a is the bit position,b is the bit value(0 or 1)
             for (j=0;j<3;j++){
                 for (l=0;l<2;l++){
                    
                     x=0;
                     for (h=0;h<8;h++){
                         
                         if (h_table[h][j]==l){
                            ttemp[x]=cache[h][i];
                            x++;
                            }
                         }
                     temp2=logsum(ttemp[0],ttemp[1]);
                     temp2=logsum(temp2,ttemp[2]);
                     bit_metric[l][3*i+j]=logsum(temp2,ttemp[3]);
                     //bit_metric[l][3*i+j]=max(ttemp,4);
                     }   
                bit_metric_llr[3*i+j]=bit_metric[1][3*i+j]-bit_metric[0][3*i+j];
                }
            }
         
         else{
             
              //the usual 8 symbol metric
             for (j=0;j<8;j++){
             
                 b0=2*h_table[j][0]-1;
                 b1=2*h_table[j][1]-1;
                 b2=2*h_table[j][2]-1;
                 
                 test1=b0*llr[i*3]+b1*llr[i*3+1]+b2*llr[i*3+2];
     
                 temp[j]=cache[j][i]+test1*mul;
                 }
            
             for (j=0;j<3;j++){
                 for (l=0;l<2;l++){
                    
                     x=0;
                     for (h=0;h<8;h++){
                         
                         if (h_table[h][j]==l){
                            ttemp[x]=temp[h];
                            x++;
                            }
                         }
                     temp2=logsum(ttemp[0],ttemp[1]);
                     temp2=logsum(temp2,ttemp[2]);
                     bit_metric[l][3*i+j]=logsum(temp2,ttemp[3])-(2*l-1)*llr[3*i+j]*mul;
                     }   
                 bit_metric_llr[3*i+j]=bit_metric[1][3*i+j]-bit_metric[0][3*i+j];
                 }
             }
         }
     }

double fmax(double x1,double x2){
       if(x1<x2) return x2;
       else return x1;
       }

void siso(int pass){
     
     int i,j,s,h,l,b,count0,count1;;
     int out1,out2,out3,temp1;
     double temp[4],t0[32],t1[32],H,test,h0,h1,p0,temp2,temp3;
     double pI_sym[(k+m)/2][8];
     double A[(k+m)/2+1][16]={},B[(k+m)/2+1][16]={};
     
     for (j=0;j<16;j++){
         A[0][j]=-pow(10,10);
         B[(k+m)/2][j]=-pow(10,10);
         
         }
     
     A[0][0]=0;
     B[(k+m)/2][0]=0;
     
     //branch metric Pk[c(e);I]        
     for (j=0;j<(k+m)/2;j++){
          for (h=0;h<8;h++){
                 pI_sym[j][0]=-bit_metric_llr[3*j]-bit_metric_llr[3*j+1]-bit_metric_llr[3*j+2];
                 pI_sym[j][1]=-bit_metric_llr[3*j]-bit_metric_llr[3*j+1]+bit_metric_llr[3*j+2];
                 pI_sym[j][2]=-bit_metric_llr[3*j]+bit_metric_llr[3*j+1]-bit_metric_llr[3*j+2];
                 pI_sym[j][3]=-bit_metric_llr[3*j]+bit_metric_llr[3*j+1]+bit_metric_llr[3*j+2];
                 pI_sym[j][4]=bit_metric_llr[3*j]-bit_metric_llr[3*j+1]-bit_metric_llr[3*j+2];
                 pI_sym[j][5]=bit_metric_llr[3*j]-bit_metric_llr[3*j+1]+bit_metric_llr[3*j+2];
                 pI_sym[j][6]=bit_metric_llr[3*j]+bit_metric_llr[3*j+1]-bit_metric_llr[3*j+2];
                 pI_sym[j][7]=bit_metric_llr[3*j]+bit_metric_llr[3*j+1]+bit_metric_llr[3*j+2];
            
               }
          }
     
     //forward recursion A
     for (j=1;j<(k+m)/2+1;j++){
        for (l=0;l<16;l++){
        
         //sum over 4 edges such that the ending state of that edge is j
         //temp=0;
         for (h=0;h<4;h++){//from input 00 to 11
             
             out1=from_out_table[l][0+3*h];
             out2=from_out_table[l][1+3*h];
             out3=from_out_table[l][2+3*h];
             temp1=4*out1+2*out2+out3;
             
             test=pI_sym[j-1][temp1];
             temp[h]=A[j-1][from_table[l][h]]+test;
             }
          temp2=logsum(temp[0],temp[1]);
          temp2=logsum(temp2,temp[2]);
          A[j][l]=logsum(temp2,temp[3]);
          //A[j][l]=max(temp,4);
          }
       } 
       
     //backward recursion B
     for (j=(k+m)/2-1;j>=0;j--){
         for (l=0;l<16;l++){
         //sum over 4 edges such that the starting state of that edge is j
         //temp=0;
         for (h=0;h<4;h++){//from input 00 to 11
             
             out1=out_table[l][0+3*h];
             out2=out_table[l][1+3*h];
             out3=out_table[l][2+3*h];
             temp1=4*out1+2*out2+out3;
             
             test=pI_sym[j][temp1];
             temp[h]=B[j+1][next_table[l][h]]+test;
             }
          temp2=logsum(temp[0],temp[1]);
          temp2=logsum(temp2,temp[2]);
          B[j][l]=logsum(temp2,temp[3]);
          //B[j][l]=max(temp,4);
          }
        }
     
     //P(c;O)
    if(pass!=iteration-1){ 
     for(b=0;b<3;b++){
      for (j=0;j<(k+m)/2;j++){ // K
          count0=0;count1=0;
          for (l=0;l<16;l++){    // state
               for (h=0;h<4;h++){
                   
                   if (out_table[l][b+3*h]==0){
                      
                      out1=out_table[l][0+3*h];
                      out2=out_table[l][1+3*h];
                      out3=out_table[l][2+3*h];
             
                      temp1=4*out1+2*out2+out3;
                      
                      t0[count0]=A[j][l]+pI_sym[j][temp1]+B[j+1][next_table[l][h]]
                                 -bit_metric[0][3*j+b];

                      count0++;
                      }
                   else{
                      out1=out_table[l][0+3*h];
                      out2=out_table[l][1+3*h];
                      out3=out_table[l][2+3*h];
             
                      temp1=4*out1+2*out2+out3;
             
                      t1[count1]=A[j][l]+pI_sym[j][temp1]+B[j+1][next_table[l][h]]
                                 -bit_metric[1][3*j+b];
                      
                      count1++;
                      }
                   }
               }
          temp2=logsum(t0[0],t0[1]);
          temp3=logsum(t1[0],t1[1]);
          for (s=2;s<32;s++){
              temp2=logsum(temp2,t0[s]);
              temp3=logsum(temp3,t1[s]);
              }
          
          id[0][3*j+b]=temp2;
          id[1][3*j+b]=temp3;
                   
          p0=fmax(id[0][3*j+b],id[1][3*j+b]);
            
          id[0][3*j+b]-=p0;
          id[1][3*j+b]-=p0;
          
          llr[3*j+b]=id[1][3*j+b]-id[0][3*j+b];
          }
      }
      }
       
     if(pass==iteration-1){ 
       
       //P(u;O)
      for(b=0;b<2;b++){ 
       for (j=0;j<(k+m)/2;j++){ // K
          //t0=0;t1=0;
          count0=0;count1=0;
          for (l=0;l<16;l++){    // state
               for (h=0;h<4;h++){
                   //if (h/2 == 0){
                   if (h_table[h][b+1] == 0){        
                      out1=out_table[l][0+3*h];
                      out2=out_table[l][1+3*h];
                      out3=out_table[l][2+3*h];
             
                      temp1=4*out1+2*out2+out3;
                      
                      t0[count0]=A[j][l]+pI_sym[j][temp1]+B[j+1][next_table[l][h]];   
                      count0++;
                      }
                   else{
                      out1=out_table[l][0+3*h];
                      out2=out_table[l][1+3*h];
                      out3=out_table[l][2+3*h];
             
                      temp1=4*out1+2*out2+out3;
                      
                      t1[count1]=A[j][l]+pI_sym[j][temp1]+B[j+1][next_table[l][h]];  
                      count1++;
                      }
                   }
               }
          
          temp2=logsum(t0[0],t0[1]);
          temp3=logsum(t1[0],t1[1]);
          for (s=2;s<32;s++){
              temp2=logsum(temp2,t0[s]);
              temp3=logsum(temp3,t1[s]);
              }
          
          out[0][2*j+b]=temp2;
          out[1][2*j+b]=temp3;
          
          }
       }
       }
     
     }


void decode(int *r_d,double *out0,double *out1){
     int i,x;
     
     for (i=0;i<k+m;i++){
         if(out0[i]>out1[i]) r_d[i]=0;
         else r_d[i]=1;
         }
      }

