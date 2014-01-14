void process1(int,double *,double);
void process2(void);

void siso(int,double *,int,double);
void decode(int *,double *,double *);

int output(int *c,double snr,int *r_d){
    int i,j=0;
    double N0,sd;
    double r[(k+m)*3],x[k+m]={},y1[k+m]={},y2[k+m]={};
    double out[2][k+m]={};
    
    N0=3/snr;//SNR = (3*Eb)/N0
    sd=sqrt(N0/2);//standard deviation of gaussian r.v.
   
    while (j<(k+m)*3){
              if (c[j]==1) r[j]=1;//temp = sqrt(Eb)
              else r[j]=-1;
              
              r[j] += gaurand()*sd;//add noise     
              
              if (j%3==0) x[(j/3)]=r[j];
              if (j%3==1)y1[(j/3)]=r[j];
              if (j%3==2)y2[(j/3)]=r[j];                        
              
              ray[j]=rayrand();
              //r[j] = r[j]*ray[j];                              
              j++;
              }
  
   /*-----------------decoding--------------------*/ 
       
    for(i=0;i<iteration;i++){
       
        process1(i,x,N0);
            
        siso(i,y1,1,N0);//will produce dec1_out[][]
        
        Minterleave(dec1_out[0],dec1_out[1]);
        
        process2();
        
        siso(i,y2,2,N0);//will produce feed[][]
        
        deinterleave(feed[0],feed[1]);
        }
   
    decode(r_d,feed[0],feed[1]);

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

void process1(int pass,double *x,double N0){
     int i;
     static double cache[2][k+m];
     
     if(pass==0){
          for(i=0;i<k+m;i++){
             cache[0][i]=-(pow(x[i] - (-1),2))/N0;
             cache[1][i]=-(pow(x[i] - 1,2))/N0;
             
             bit_metric[0][i]=cache[0][i];
             bit_metric[1][i]=cache[1][i];
             }
          }
     
     else{
          for(i=0;i<k;i++){
             bit_metric[0][i]=cache[0][i] + feed[0][i];
             bit_metric[1][i]=cache[1][i] + feed[1][i];
             }
          for(i=k;i<k+m;i++){
             bit_metric[0][i]=cache[0][i];
             bit_metric[1][i]=cache[1][i];
             }
         }
     
     }
     
void process2(void){
     int i;
     
     for(i=0;i<k;i++){
          bit_metric[0][i]=dec1_out[0][i];
          bit_metric[1][i]=dec1_out[1][i];
          }
     for(i=k;i<k+m;i++){
          bit_metric[0][i]=0;
          bit_metric[1][i]=0;
          }
     }

void siso(int pass,double *y,int option,double N0){
     int i,j,h,l,s,b,count0,count1;;
     int out1,out2,temp1;
     double temp[2],t0[16],t1[16],test,temp2,temp3,m0,m1,temp4;
     float pI_sym[k+m][4];
     float A[(k+m)+1][16]={},B[(k+m)+1][16]={};
      
     for (j=0;j<16;j++){
         A[0][j]=-pow(10,5);
         B[k+m][j]=-pow(10,5);
         }
     
     A[0][0]=0;
     B[k+m][0]=0;
     
     //branch metric Pk[c(e);I]        
     for (j=0;j<k+m;j++){
          m0=-(pow(y[j] - (-1),2))/N0;
          m1=-(pow(y[j] - 1,2))/N0;
          for (h=0;h<4;h++){
                 pI_sym[j][0]= bit_metric[0][j] + m0;
                 pI_sym[j][1]= bit_metric[0][j] + m1;
                 pI_sym[j][2]= bit_metric[1][j] + m0;
                 pI_sym[j][3]= bit_metric[1][j] + m1;
               }
          }
  
     //forward recursion A
     for (j=1;j<k+m+1;j++){
        for (l=0;l<16;l++){
         //sum over 2 edges such that the ending state of that edge is j
         //temp=0;
         for (h=0;h<2;h++){//from input 0 to 1
             
             out1=from_out_table[l][0+2*h];
             out2=from_out_table[l][1+2*h];
             temp1=2*out1+out2;
             
             test=pI_sym[j-1][temp1];
             temp[h]=A[j-1][from_table[l][h]]+test;
             }
          A[j][l]=logsum(temp[0],temp[1]);
          //A[j][l]=max(temp,2);
          }
       } 
       
     //backward recursion B
     for (j=k+m-1;j>=0;j--){
         for (l=0;l<16;l++){
         //sum over 4 edges such that the starting state of that edge is j
         //temp=0;
         for (h=0;h<2;h++){//from input 00 to 11
             
             out1=out_table[l][0+2*h];
             out2=out_table[l][1+2*h];
             temp1=2*out1+out2;
             
             test=pI_sym[j][temp1];
             temp[h]=B[j+1][next_table[l][h]]+test;
             }
          B[j][l]=logsum(temp[0],temp[1]);
          //B[j][l]=max(temp,2);
          }
        }
       
     //P(u;O)
       for (j=0;j<k;j++){ // K
          count0=0;count1=0;
          for (l=0;l<16;l++){    // state
               for (h=0;h<2;h++){
                   if (h == 0){        
                      out1=out_table[l][0+2*h];
                      out2=out_table[l][1+2*h];
                      
                      temp1=2*out1+out2;
                      
                      if(option==1||(option==2 && pass==iteration-1))
                         t0[count0]=A[j][l]+B[j+1][next_table[l][h]]+pI_sym[j][temp1];  
                      else t0[count0]=A[j][l]+B[j+1][next_table[l][h]]+pI_sym[j][temp1]
                           -bit_metric[0][j];
                      count0++;
                      }
                   else{
                      out1=out_table[l][0+2*h];
                      out2=out_table[l][1+2*h];
             
                      temp1=2*out1+out2;
                      
                      if(option==1||(option==2 && pass==iteration-1))
                         t1[count1]=A[j][l]+B[j+1][next_table[l][h]]+pI_sym[j][temp1];   
                      else t1[count1]=A[j][l]+B[j+1][next_table[l][h]]+pI_sym[j][temp1]
                           -bit_metric[1][j];  
                      count1++;
                      }
                   }
               }
               
          temp2=logsum(t0[0],t0[1]);
          temp3=logsum(t1[0],t1[1]);
          
          for (s=2;s<16;s++){
              temp2=logsum(temp2,t0[s]);
              temp3=logsum(temp3,t1[s]);
              }
  
          temp4=fmax(temp2,temp3);
          temp2-=temp4;
          temp3-=temp4;
  
          if(option==1){
            dec1_out[0][j]=temp2;
            dec1_out[1][j]=temp3;
            }
          
          else if(option==2){
            feed[0][j]=temp2;
            feed[1][j]=temp3;
            }
          }
      
     }

void decode(int *r_d,double *out0,double *out1){
     int i;
     
     for (i=0;i<k;i++){
         if(out0[i]>out1[i]) r_d[i]=0;
         else r_d[i]=1;
         }
      }

