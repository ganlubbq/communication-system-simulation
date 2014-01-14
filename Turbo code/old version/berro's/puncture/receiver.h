void process1(int,float *,float *,float);
void process2(int,float *,float);
void puncture(float *,float *);

void siso(int,int,float);
void decode(int *,float *,float *);

int output(int *c1,int *c2,int *c3,float snr,int *r_d){
    int i,j=0;
    float N0,sd;
    float x[k+m],y1[k+m],y2[k+m];
    float out[2][k+m]={};
    
    N0=3/snr;//SNR = (3*Eb)/N0
    sd=sqrt(N0/2);//standard deviation of gaussian r.v.
   
    while (j<k+m){
              if (c1[j]==1) x[j]=1;//temp = sqrt(Eb)
              else x[j]=-1;
              if (c2[j]==1) y1[j]=1;//temp = sqrt(Eb)
              else y1[j]=-1;
              if (c3[j]==1) y2[j]=1;//temp = sqrt(Eb)
              else y2[j]=-1;
                
              x[j] += gaurand()*sd;//add noise     
              y1[j] += gaurand()*sd;
              y2[j] += gaurand()*sd;
              
              //ray[j]=rayrand();
              //r[j] = r[j]*ray[j];                              
              j++;
              }
    
    //puncture(y1,y2);
    
   /*-----------------decoding--------------------*/ 
       
    for(i=0;i<iteration;i++){
       
        process1(i,x,y1,N0);
            
        siso(i,1,N0);//will produce dec1_out[][]
        
        Minterleave(dec1_out[0],dec1_out[1]);
         
        process2(i,y2,N0);
        
        siso(i,2,N0);//will produce feed[][]
        
        deinterleave(feed[0],feed[1]);
        }
   
    decode(r_d,feed[0],feed[1]);

    return 1;
    }

float max(float *a,int l){
      float temp;
      int i;
      
      temp=a[0];
      for(i=0;i<l;i++){
           if(a[i]>temp) temp=a[i];
           }
      
      return temp;
      }

void puncture(float *y1,float *y2){
     int i;
     
     for(i=0;i<k+m;i++){
             if(i%2==0)y1[i]=0;
             else y2[i]=0;
             }
     }

void process1(int pass,float *x,float *y1,float N0){
     int i;
     static float cache[2][k+m],cache1[2][k+m];
     
     if(pass==0){
          for(i=0;i<k+m;i++){
             cache[0][i]=-(pow(x[i] - (-1),2))/N0;
             cache[1][i]=-(pow(x[i] - 1,2))/N0;
             
             bit_metric[0][i]=cache[0][i];
             bit_metric[1][i]=cache[1][i];
             
             cache1[0][i]=-(pow(y1[i] - (-1),2))/N0;
             cache1[1][i]=-(pow(y1[i] - 1,2))/N0;
             
             y1_metric[0][i]=cache1[0][i];
             y1_metric[1][i]=cache1[1][i];
             
             
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
         
         
         
         
         for(i=0;i<k+m;i++){
             y1_metric[0][i]=cache1[0][i] + feedy1[0][i];
             y1_metric[1][i]=cache1[1][i] + feedy1[1][i];
             }
    
         }
 
     
     }
     
void process2(int pass,float *y2,float N0){
     int i;
     static float cache[2][k+m];
     
     for(i=0;i<k;i++){
          bit_metric[0][i]=dec1_out[0][i];
          bit_metric[1][i]=dec1_out[1][i];
          }
     for(i=k;i<k+m;i++){
          bit_metric[0][i]=0;
          bit_metric[1][i]=0;
          }
     
     if(pass==0){
          for(i=0;i<k+m;i++){
             cache[0][i]=-(pow(y2[i] - (-1),2))/N0;
             cache[1][i]=-(pow(y2[i] - 1,2))/N0;
             
             y2_metric[0][i]=cache[0][i];
             y2_metric[1][i]=cache[1][i];
           
             }
          }
     
     else{

        /* for(i=0;i<k+m;i++){
             y2_metric[0][i]=cache[0][i] + feedy2[0][i];
             y2_metric[1][i]=cache[1][i] + feedy2[1][i];
             }*/
    
         }
     
     }

void siso(int pass,int option,float N0){
     int i,j,h,l,s,count0,count1,count2,count3;
     int out1,out2,temp1;
     float temp[2],t0[16],t1[16],t2[16],t3[16],test,temp2,temp3,m0,m1,fix;
     float pI_sym[k+m][4];
     float A[k+m+1][16],B[k+m+1][16];

     for (j=0;j<16;j++){
         A[0][j]=-pow(10,5);
         B[k+m][j]=-pow(10,5);
         }
     
     A[0][0]=0;
     B[k+m][0]=0;
     
     //branch metric Pk[c(e);I]        
     for (j=0;j<k+m;j++){
          //m0=-(pow(y[j] - (-1),2))/N0;
          //m1=-(pow(y[j] - 1,2))/N0;
          if(option==1){
             for (h=0;h<4;h++){
                 pI_sym[j][0]= bit_metric[0][j] + y1_metric[0][j];
                 pI_sym[j][1]= bit_metric[0][j] + y1_metric[1][j];
                 pI_sym[j][2]= bit_metric[1][j] + y1_metric[0][j];
                 pI_sym[j][3]= bit_metric[1][j] + y1_metric[1][j];
                 }
             }
          else{
               for (h=0;h<4;h++){
                 pI_sym[j][0]= bit_metric[0][j] + y2_metric[0][j];
                 pI_sym[j][1]= bit_metric[0][j] + y2_metric[1][j];
                 pI_sym[j][2]= bit_metric[1][j] + y2_metric[0][j];
                 pI_sym[j][3]= bit_metric[1][j] + y2_metric[1][j];
                 }
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
       
     //P(c;O)
       for (j=0;j<k;j++){ // K
          count0=0;count1=0;count2=0;count3=0;
          for (l=0;l<16;l++){    // state
               for (h=0;h<2;h++){
                   if (out_table[l][0+2*h] == 0){        
                      out1=out_table[l][0+2*h];
                      out2=out_table[l][1+2*h];
                      
                      temp1=2*out1+out2;
                      
                      if(option==1||(option==2 && pass==iteration-1))
                         t0[count0]=A[j][l]+B[j+1][next_table[l][h]]+pI_sym[j][temp1];  
                      else t0[count0]=A[j][l]+B[j+1][next_table[l][h]]+pI_sym[j][temp1]
                           - bit_metric[0][j];
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
               for (h=0;h<2;h++){//bit2
                   if (out_table[l][1+2*h] == 0){        
                      out1=out_table[l][0+2*h];
                      out2=out_table[l][1+2*h];
                      
                      temp1=2*out1+out2;
                      
                      t2[count2]=A[j][l]+B[j+1][next_table[l][h]]+pI_sym[j][temp1]
                            -(-(pow(y[j] - (-1),2))/N0);
                      count2++;
                      }
                   else{
                      out1=out_table[l][0+2*h];
                      out2=out_table[l][1+2*h];
             
                      temp1=2*out1+out2;
                      
                      t3[count3]=A[j][l]+B[j+1][next_table[l][h]]+pI_sym[j][temp1]
                            -(-(pow(y[j] - 1,2))/N0);  
                      count3++;
                      }
                   }
               
               }
               
          temp2=logsum(t0[0],t0[1]);
          temp3=logsum(t1[0],t1[1]);
          
          for (s=2;s<16;s++){
              temp2=logsum(temp2,t0[s]);
              temp3=logsum(temp3,t1[s]);
              }
  
          fix=fmax(temp2,temp3);
          temp2-=fix;
          temp3-=fix;
  
          if(option==1){
            dec1_out[0][j]=temp2;
            dec1_out[1][j]=temp3;
            }
          
          else if(option==2){
            feed[0][j]=temp2;
            feed[1][j]=temp3;
            }
          //-----------------------------
          temp2=logsum(t2[0],t2[1]);
          temp3=logsum(t3[0],t3[1]);
          
          for (s=2;s<16;s++){
              temp2=logsum(temp2,t2[s]);
              temp3=logsum(temp3,t3[s]);
              }
  
          fix=fmax(temp2,temp3);
          temp2-=fix;
          temp3-=fix;
  
          if(option==1){
            feedy1[0][j]=temp2;
            feedy1[1][j]=temp3;
            }
          
          else if(option==2){
            feedy2[0][j]=temp2;
            feedy2[1][j]=temp3;
            }
  
          
          }
      
     }

void decode(int *r_d,float *out0,float *out1){
     int i;
     
     for (i=0;i<k;i++){
         if(out0[i]>out1[i]) r_d[i]=0;
         else r_d[i]=1;
         }
      }

