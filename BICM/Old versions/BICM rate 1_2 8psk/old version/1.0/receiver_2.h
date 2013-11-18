void output(int *,float,int *);
void bit_metric_comput(struct complex *);
void viterbi(void);
void decode(int *);//reconstruct the decoded sequence using optimal path

void output(int *v,float snr,int *r_d){
    int i,j=0,x;
    float N0,sd,temp=1;                             //temp = sqrt(Es)
    struct complex *r;
          
    r=(struct complex *) malloc((k+m)*2/3*sizeof(struct complex));
        
    modulation(v,r);
    
    N0=temp*2/(3*snr);                                    //SNR = Eb/N0  2Es = 3Eb
    sd=sqrt(N0/2);                                    //standard deviation of gaussian r.v.
   
    while (j<(k+m)*2/3){
              r[j].re += gaurand()*sd;                 //r is the received vector                                 
              r[j].im += gaurand()*sd;
              j++;
              }

    /*compute the bit based metric*/
    bit_metric_comput(r);
    
    deinterleave1(bit_metric1[0]);
    deinterleave1(bit_metric1[1]);
    deinterleave2(bit_metric2[0]);
    deinterleave2(bit_metric2[1]);
    deinterleave3(bit_metric3[0]);
    deinterleave3(bit_metric3[1]);
  
   /*-----------------decoding--------------------*/ 
      
    viterbi();
        
    decode(r_d);

    free(r);
    }


float max(float a,float b,float c,float d){
      float temp;
      
      temp=a;
      if(b>temp) temp=b;
      if(c>temp) temp=c;
      if(d>temp) temp=d;
      return temp;
      }


void bit_metric_comput(struct complex *r){     //compute the bit based metrics
     int i,j;
     float temp[8];
     
     for (i=0;i<(k+m)*2/3;i++){
         
         for (j=0;j<8;j++){//the usual 8 symbol metric
             temp[j]=mapping[j].re*r[i].re + mapping[j].im*r[i].im;
             }
         
         /*bit based metric ; "metric a_b" ,a is the bit position,b is the bit value(0 or 1)*/
         bit_metric1[0][i]=max(temp[0],temp[2],temp[6],temp[4]);
         bit_metric1[1][i]=max(temp[1],temp[3],temp[5],temp[7]);
         bit_metric2[0][i]=max(temp[0],temp[1],temp[5],temp[4]);
         bit_metric2[1][i]=max(temp[2],temp[3],temp[6],temp[7]);
         bit_metric3[0][i]=max(temp[0],temp[1],temp[2],temp[3]);
         bit_metric3[1][i]=max(temp[4],temp[5],temp[6],temp[7]);
         
         }
     }


void viterbi(void){
     int i,j,h,t1,t0,b0,b1,b2;
     float sym_metric[2][8],cost[64]={},temp[64]={};
     float w[8];
     
     cost[0]=1;
     
     for (i=0;i<k_m/3;i++){
        
        /*compute symbol metric, sym_metric[0][8] is the 1st part in one loop,
         sym_metric[1][8] is the 2nd part*/
        for (j=0;j<2;j++){
            for (h=0;h<8;h++){
                b0= h_table[h][0];b1= h_table[h][1];b2= h_table[h][2];//binary
                sym_metric[j][h]=bit_metric1[b0][2*i+j]+bit_metric2[b1][2*i+j]+bit_metric3[b2][2*i+j];
               }
            }
       
        for(j=0;j<st_num;j++){
             if (cost[j]!=0){
               
               /*path cost*/
                for(h=0;h<8;h++){
                   t0=path_out[j][h][2]+2*path_out[j][h][1]+4*path_out[j][h][0];//index of 1st part
                   t1=path_out[j][h][5]+2*path_out[j][h][4]+4*path_out[j][h][3];//index of 2nd part
                   w[h]= sym_metric[0][t0] + sym_metric[1][t1];
                   }
                         
                for(h=0;h<8;h++){
                    if ((temp[path[j][h][2]] == 0 || (cost[j] + w[h]) > temp[path[j][h][2]]) && (i<k_m/3-2)){
                       
                       node[i*3+3][path[j][h][2]].from=&node[i*3+2][path[j][h][1]]; 
                       node[i*3+2][path[j][h][1]].from=&node[i*3+1][path[j][h][0]]; 
                       node[i*3+1][path[j][h][0]].from=&node[i*3][j]; 
                       
                       node[i*3+3][path[j][h][2]].in=h_table[h][2];
                       node[i*3+2][path[j][h][1]].in=h_table[h][1];
                       node[i*3+1][path[j][h][0]].in=h_table[h][0];
                       
                       temp[path[j][h][2]]=cost[j] + w[h];
                       } 
         
                    }
                
                /*last few loops for a terminated code*/
                if (temp[path[j][0][2]] == 0 || (cost[j] + w[0]) > temp[path[j][0][2]]){
                       
                       node[i*3+3][path[j][0][2]].from=&node[i*3+2][path[j][0][1]]; 
                       node[i*3+2][path[j][0][1]].from=&node[i*3+1][path[j][0][0]]; 
                       node[i*3+1][path[j][0][0]].from=&node[i*3][j]; 
                       
                       node[i*3+3][path[j][0][2]].in=h_table[0][2];
                       node[i*3+2][path[j][0][1]].in=h_table[0][1];
                       node[i*3+1][path[j][0][0]].in=h_table[0][0];
                       
                       temp[path[j][0][2]]=cost[j] + w[0];
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
