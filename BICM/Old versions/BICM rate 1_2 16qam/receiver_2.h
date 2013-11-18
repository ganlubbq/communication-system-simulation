void output(int *,double,int *);
void bit_metric_comput(struct complex *);
void viterbi(void);
void decode(int *);//reconstruct the decoded sequence using optimal path

void output(int *v,double snr,int *r_d){
    int i,j=0,x;
    double N0,sd,temp=1,Eav;                             //temp = sqrt(E0)
    double temp1;
    struct complex *r;
    
    //compute Eav
    Eav=2*(M-1)/3;
         
    r=(struct complex *) malloc((k+m)*2/z*sizeof(struct complex));
        
    modulation(v,r);
    
    N0=Eav/(2*snr);                                    //SNR = Eb/N0  Eav = 2Eb
    sd=sqrt(N0/2);                                    //standard deviation of gaussian r.v.
   
    while (j<(k+m)*2/z){
              temp1=gaurand()*sd;
              r[j].re += temp1;                 //r is the received vector                                 
              r[j].im += temp1;
              
              ray[j]=rayrand();
              //r[j].re = r[j].re*ray[j] + temp1;                 //r is the received vector                                 
              //r[j].im = r[j].im*ray[j] + temp1;
              
              j++;
              }

    /*-----------------decoding--------------------*/ 

    /*compute the bit based metric*/
    bit_metric_comput(r);
    
    deinterleave(bit_metric[0],bit_metric[1]);
     
    viterbi();
    
    decode(r_d);

    free(r);
    }

double max(double *a){
      int i;
      double temp;
      
      temp=a[0];
      for(i=0;i<8;i++){
          if(a[i]>temp) temp=a[i];
           }
      
      return temp;
      }

void bit_metric_comput(struct complex *r){     //compute the bit based metrics
     int i,j,h,l,x=0;
     double temp[16],ttemp[8];
     
     for (i=0;i<(k+m)/2;i++){
         
         for (j=0;j<M;j++){//the usual 16 symbol metric
 
            temp[j]=-(pow(mapping[j].re-r[i].re,2) + pow(mapping[j].im-r[i].im,2));
            //temp[j]=-(pow(mapping[j].re*ray[i]-r[i].re,2) + pow(mapping[j].im*ray[i]-r[i].im,2));
             }
         
         for (j=0;j<4;j++){
             for (l=0;l<2;l++){
                    
                     x=0;
                     for (h=0;h<16;h++){
                         
                         if (M_table[h][j]==l){
                            ttemp[x]=temp[h];
                            x++;
                            }
                         }
                     //bit_metric[j][l][i]=max(ttemp);
                     bit_metric[l][4*i+j]=max(ttemp);
                     }   
             }
         }
     }


void viterbi(void){
     int i,j,h,t1,t0,b0,b1,b2,b3;
     double sym_metric[16],cost[64]={},temp[64]={};
     double w[4];
     
     cost[0]=1;
     
     for (i=0;i<k_m/2;i++){
        
        /*compute symbol metric*/
        
            for (h=0;h<16;h++){
                b3= M_table[h][3];
                b2= M_table[h][2];//binary
                b1= M_table[h][1];
                b0= M_table[h][0];
                 
                /*sym_metric[h]=bit_metric[3][b3][i]+
                              bit_metric[2][b2][i]+
                              bit_metric[1][b1][i]+
                              bit_metric[0][b0][i];*/
                
                sym_metric[h]=bit_metric[b3][4*i+3]+
                              bit_metric[b2][4*i+2]+
                              bit_metric[b1][4*i+1]+
                              bit_metric[b0][4*i];
               
               }
            
       
            for(j=0;j<st_num;j++){
             if (cost[j]!=0){
               
               /*path cost*/
                for(h=0;h<4;h++){
                   //index
                   t0=8*path_out[j][h][0]+4*path_out[j][h][1]+2*path_out[j][h][2]+path_out[j][h][3];
                   w[h]= sym_metric[t0];
                   }
                         
                for(h=0;h<4;h++){
                    if ((temp[path[j][h][1]] == 0 || (cost[j] + w[h]) > temp[path[j][h][1]]) && (i<k/2)){
                       
                       node[i*2+2][path[j][h][1]].from=&node[i*2+1][path[j][h][0]]; 
                       node[i*2+1][path[j][h][0]].from=&node[i*2][j]; 
                       
                       node[i*2+2][path[j][h][1]].in=h_table[h][1];
                       node[i*2+1][path[j][h][0]].in=h_table[h][0];
                       
                       temp[path[j][h][1]]=cost[j] + w[h];
                       }
                    }
                    
                 /*last 3 loops for a terminated code*/
                if ((temp[path[j][0][1]] == 0 || (cost[j] + w[0]) > temp[path[j][0][1]]) && (i>=k/2)){
                       
                       node[i*2+2][path[j][0][1]].from=&node[i*2+1][path[j][0][0]]; 
                       node[i*2+1][path[j][0][0]].from=&node[i*2][j]; 
                       
                       node[i*2+2][path[j][0][1]].in=0;
                       node[i*2+1][path[j][0][0]].in=0;
                       
                       temp[path[j][0][1]]=cost[j] + w[0];
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
