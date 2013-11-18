void output(int *,double,int *);
void llr_comput(struct complex *,double,int *,int);
void viterbi(void);
void decode(int *);//reconstruct the decoded sequence using optimal path

void output(int *v,double snr,int *r_d){
    int i,j=0,x;
    double N0,sd,temp=1;                             //temp = sqrt(Es)
    struct complex r[(k+m)/2];      
    int id[(k+m)/2*3]={};
    
    modulation(v,r);
    
    N0=temp/(2*snr);                                    //SNR = Eb/N0  2Es = 3Eb
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
        
        /*compute the bit based metric*/
        llr_comput(r,N0,id,i);
    
        deinterleave(llr);
        
        viterbi();
        
        decode(r_d);
       
        input(r_d,id,1);//coding+interleaving
        
       /* printf("%d %d %d\n",id[0],id[1],id[2]);
        
        system("pause");*/
        }
    
    
    }

double max(double *a){
      double temp;
      int i;
      
      temp=a[0];
      for(i=0;i<4;i++){
           if(a[i]>temp) temp=a[i];
           }
      
      return temp;
      }

void llr_comput(struct complex *r,double N0,int* id,int pass){     //compute the bit based metrics
     int i,j,h,l,x=0,b0,b1,b2,temp1;
     double temp[8],ttemp[4];
     double temp_llr[2][(k+m)*2];
     
     for (i=0;i<(k+m)/2;i++){
         
         for (j=0;j<8;j++){//the usual 8 symbol metric
             temp[j]=mapping[j].re*r[i].re + mapping[j].im*r[i].im;
             //temp[j]=-(pow(mapping[j].re-r[i].re,2) + pow(mapping[j].im-r[i].im,2))/N0;
             //temp[j]=mapping[j].re*r[i].re*ray[i] + mapping[j].im*r[i].im*ray[i];
             }
         
         if(pass==0){
         /*bit based metric ; "metric a_b" ,a is the bit position,b is the bit value(0 or 1)*/
         for (j=0;j<3;j++){
                 for (l=0;l<2;l++){
                    
                     x=0;
                     for (h=0;h<8;h++){
                         
                         if (h_table[h][j]==l){
                            ttemp[x]=temp[h];
                            x++;
                            }
                         }
                     temp_llr[l][4*i+j]=max(ttemp);
                     }   
             // Log Likelihood Ratio = logP1 - logP0
             llr[4*i+j]=temp_llr[1][4*i+j]-temp_llr[0][4*i+j];
             }
         }
        
         else{//reselection
                  
         b0=id[i*3];
         b1=id[i*3+1];
         b2=id[i*3+2];
         
         temp1=4*0+2*b1+b2;
         llr[4*i]  =temp[temp1+4]-temp[temp1];
        
         temp1=4*b0+2*0+b2;
         llr[4*i+1]=temp[temp1+2]-temp[temp1];
         
         temp1=4*b0+2*b1+0;
         llr[4*i+2]=temp[temp1+1]-temp[temp1];
         }
         }
     }


void viterbi(void){
     int i,i_1,j,next1,next0;
     double w1,w0,cost[64]={},temp[64]={};
     
     cost[0]=1;
     
     for (i=0;i<k_m;i++){
        
         for(j=0;j<st_num;j++){
             
             next1 = next_table[j][1];
             next0 = next_table[j][0];
             
             if (cost[j]!=0){
               
               
               //for punctured codes
            /*   if(i%2==1){
                  w0=(2*out_table[j][0]-1)*llr[2*i];
                  w1=(2*out_table[j][2]-1)*llr[2*i];
                  }
               else{*/
                  w0=(2*out_table[j][0]-1)*llr[2*i]+(2*out_table[j][1]-1)*llr[2*i+1]*((i+1)%2);
                  w1=(2*out_table[j][2]-1)*llr[2*i]+(2*out_table[j][3]-1)*llr[2*i+1]*((i+1)%2);
                //  }
                  
               if (( temp[next1] == 0 || (cost[j] + w1) > temp[next1]) && (i < k)){
                  node[i+1][next1].from=&node[i][j]; 
                  node[i+1][next1].in=1;
                  temp[next1]=cost[j] + w1;
                  }                       
               
               if ( temp[next0] == 0 || (cost[j] + w0) > temp[next0] ){
                  node[i+1][next0].from=&node[i][j]; 
                  node[i+1][next0].in=0;
                  temp[next0]=cost[j] + w0;
                  }
               }
             }
 
             for(j=0;j<64;j++) {
                 cost[j]=temp[j];
                 temp[j]=0;
                 }
         }
      }

void decode(int *r_d){
     
     struct state *temp1;
     int i,x;
     
     x=k+m;

     temp1=&node[x][0];
     for (i=x-1;i>=0;i--){
            r_d[i]=temp1->in;
            temp1=temp1->from;
            }
     }
