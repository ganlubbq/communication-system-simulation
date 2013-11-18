void output(int *,float,int *);
void sym_metric_comput(struct complex *);
void viterbi(void);
void decode(int *);//reconstruct the decoded sequence using optimal path

void output(int *v,float snr,int *r_d){
    int i,j=0,x;
    float N0,sd,temp=1;                             //temp = sqrt(Es)
    struct complex *r;
            
    r=(struct complex *) malloc((k+m)/2*sizeof(struct complex));
       
    modulation(v,r);
    
    N0=temp/(2*snr);                                    //SNR = Eb/N0  Es = 2Eb
    sd=sqrt(N0/2);                                    //standard deviation of gaussian r.v.
   
    /*while (j<(k+m)/2){
              r[j].re += gaurand()*sd;                 //r is the received vector                                 
              r[j].im += gaurand()*sd;
              j++;
              }*/

    
    sym_metric_comput(r);
    
    /*deinterleave0();
    deinterleave1();
    deinterleave2();*/
  
   /*-----------------decoding--------------------*/ 
      
    viterbi();
        
    decode(r_d);

    free(r);
    }


void sym_metric_comput(struct complex *r){     //compute the bit based metrics
     int i,j,h,l,x=0;
     
     for (i=0;i<(k+m)/2;i++){
         
         for (j=0;j<8;j++){//the usual 8 symbol metric
             sym_metric[j][i]=mapping[j].re*r[i].re + mapping[j].im*r[i].im;
             }
         }
     }


void viterbi(void){
     int i,j,h,t1,t0,next0,next1;
     float cost[64]={},temp[64]={};
     
     cost[0]=1;
     
     for (i=0;i<k_m/2;i++){
        /*for(j=0;j<64;j++) {
                 printf("%.1f ",cost[j]);
                }
        printf("\n");*/
       
        for(j=0;j<st_num;j++){
           
           next0 = next_table[j][0];   
           next1 = next_table[j][1];
           
           t0=2*out_table[j][0]+out_table[j][1];
           t1=2*out_table[j][2]+out_table[j][3];
           
           if (cost[j]!=0){
               
               if ( temp[next0] == 0 || (cost[j] + sym_metric[t0][i]) > temp[next0] ){
                  node[i+1][next0].from=&node[i][j]; 
                  node[i+1][next0].in[0]=0;
                  node[i+1][next0].in[1]=0;
                  //printf("%2d\n",t0);system("pause");
                  temp[next0]=cost[j] + sym_metric[t0][i];
                  }
               
               if ( temp[next0] == 0 || (cost[j] + sym_metric[4+t0][i]) > temp[next0]){
                  node[i+1][next0].from=&node[i][j]; 
                  node[i+1][next0].in[0]=1;
                  node[i+1][next0].in[1]=0;
                  temp[next0]=cost[j] + sym_metric[4+t0][i];
                  }
               
               if (( temp[next1] == 0 || (cost[j] + sym_metric[t1][i]) > temp[next1]) && (i < k/2)){
                  node[i+1][next1].from=&node[i][j]; 
                  node[i+1][next1].in[0]=0;
                  node[i+1][next1].in[1]=1;
                  temp[next1]=cost[j] + sym_metric[t1][i];
                  }                       
               if (( temp[next1] == 0 || (cost[j] + sym_metric[4+t1][i]) > temp[next1]) && (i < k/2)){
                  node[i+1][next1].from=&node[i][j]; 
                  node[i+1][next1].in[0]=1;
                  node[i+1][next1].in[1]=1;
                  temp[next1]=cost[j] + sym_metric[4+t1][i];
                  }     
              /*printf("state=%d\n",j);
              printf("next00=%2d  ",next0);   printf("out=0%d%d  ",out_table[j][0],out_table[j][1]);   printf("cost=%3.1f\n",sym_metric[t0][i]);
              printf("next10=%2d  ",next0);   printf("out=1%d%d  ",out_table[j][0],out_table[j][1]);   printf("cost=%3.1f\n",sym_metric[4+t0][i]);
              
              printf("next01=%2d  ",next1);   printf("out=0%d%d  ",out_table[j][2],out_table[j][3]);   printf("cost=%3.1f\n",sym_metric[t1][i]);
              printf("next11=%2d  ",next1);   printf("out=1%d%d  ",out_table[j][2],out_table[j][3]); printf("cost=%3.1f\n",sym_metric[t1+4][i]);
              system("pause");*/
              
               }
             
             }

            /*if(i==(k+m)/2-1){
               for(j=0;j<64;j++) {
                 printf("%.1f ",temp[j]);
                }
                printf("\n");         
                  system("pause");       
                  }*/
             
             for(j=0;j<64;j++) {
                 cost[j]=temp[j];
                 temp[j]=0;
                 }
         }
      }


void decode(int *r_d){
     
     struct state *temp1;
     int i,x;
     
     x=(k+m)/2;

     temp1=&node[x][0];
     for (i=x-1;i>=0;i--){
            r_d[2*i]=temp1->in[0];
            r_d[2*i+1]=temp1->in[1];
            
            temp1=temp1->from;
            }
    
    
    
    
     }
