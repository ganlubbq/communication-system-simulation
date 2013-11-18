void output(int *,double,int *);
void llr_comput(struct complex *,double);
void viterbi(void);
void decode(int *);//reconstruct the decoded sequence using optimal path

void output(int *v,double snr,int *r_d){
    int i,j=0,x;
    double N0,sd,temp=1;                             //temp = sqrt(Es)
    struct complex *r;
          
    r=(struct complex *) malloc((k+m)*2/3*sizeof(struct complex));
        
    modulation(v,r);
    
    N0=temp*2/(3*snr);                                    //SNR = Eb/N0  2Es = 3Eb
    sd=sqrt(N0/2);                                    //standard deviation of gaussian r.v.
   
    while (j<(k+m)*2/3){
             
              //r[j].re += gaurand()*sd;                 //r is the received vector                                 
              //r[j].im += gaurand()*sd;
              ray[j]=rayrand();
              r[j].re = r[j].re*ray[j] + gaurand()*sd;                 //r is the received vector                                 
              r[j].im = r[j].im*ray[j] + gaurand()*sd;
              
              j++;
              }
    
    /*-----------------decoding--------------------*/ 
    
    /*compute the bit based metric*/
    llr_comput(r,N0);
    
    deinterleave(bit_metric[0],bit_metric[1]);
     
    viterbi();
        
    decode(r_d);

    free(r);
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

void llr_comput(struct complex *r,double N0){     //compute the bit based metrics
     int i,j,h,l,x=0;
     double temp[8],ttemp[4],temp2;
     
     for (i=0;i<(k+m)*2/3;i++){
         
         for (j=0;j<8;j++){//the usual 8 symbol metric
             //temp[j]=mapping[j].re*r[i].re + mapping[j].im*r[i].im;
             //temp[j]=-(pow(mapping[j].re-r[i].re,2) + pow(mapping[j].im-r[i].im,2))/N0;
             temp[j]=-(pow(mapping[j].re*ray[i]-r[i].re,2) + pow(mapping[j].im*ray[i]-r[i].im,2))/N0;
             }
         
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
                     temp2=logsum(ttemp[0],ttemp[1]);
                     temp2=logsum(temp2,ttemp[2]);
                     bit_metric[l][3*i+j]=logsum(temp2,ttemp[3]);
                     //bit_metric[l][3*i+j]=max(ttemp);
                     }   
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
               
                   
               w0=bit_metric[out_table[j][0]][2*i]+bit_metric[out_table[j][1]][2*i+1];
               w1=bit_metric[out_table[j][2]][2*i]+bit_metric[out_table[j][3]][2*i+1];
               
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
