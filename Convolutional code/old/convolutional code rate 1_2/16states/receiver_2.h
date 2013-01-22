void viterbi(void);                                 //sub function,calculate the weights and optimal path
void decode(int *);                                 //sub function,construct the decoded sequence using  optimal path

void output(int *x_c,float snr,int *r_d){
    int i,j=0,x;
    float N0,sd,r[2*(k+m)],temp=1;                             //temp = sqrt(Eb)
    
    N0=temp*2/snr;                                    //SNR = Eb*2/N0
    sd=sqrt(N0/2);                                    //standard deviation of gaussian r.v.
   
    while (j<(2*(k+m))){
    
              if (x_c[j]==1) temp=1;                  //temp = sqrt(Eb)
              else temp=-1;
              r[j]=temp;                 //r is the received vector                                 
              //r[j]+=gaurand()*sd;        //add noise
              ray[j]=rayrand();
              r[j]=gaurand()*sd+temp*ray[j];
              j++;
              }
   
     j=0;  
     while (j<(k+m)){ //compute path cost
              
              i=2*j;
              x=i+1;
             /* path_metric[j][0]= -1*r[i] + -1*r[x];  //cost of output 00
              path_metric[j][1]= -1*r[i] +  1*r[x];  //cost of output 01
              path_metric[j][2]=  1*r[i] + -1*r[x];  //cost of output 10
              path_metric[j][3]=  1*r[i] +  1*r[x];  //cost of output 11*/
              
              path_metric[j][0]= -1*r[i]*ray[i] + -1*r[x]*ray[x];  //cost of output 00
              path_metric[j][1]= -1*r[i]*ray[i] +  1*r[x]*ray[x];  //cost of output 01
              path_metric[j][2]=  1*r[i]*ray[i] + -1*r[x]*ray[x];  //cost of output 10
              path_metric[j][3]=  1*r[i]*ray[i] +  1*r[x]*ray[x];  //cost of output 11
              
              j++;
              }
   
   /*-----------------decoding--------------------*/ 
   
    viterbi();
        
    decode(r_d);
    }

void viterbi(){
     int i,j,next1,next0;
     float w1,w0,cost[16]={},temp[16]={};
     
     cost[0]=1;
     
     for (i=0;i<(k+m);i++){
        
        for(j=0;j<st_num;j++){
             
             next1 = next_table[j][1];
             next0 = next_table[j][0];
             
             if (cost[j]!=0){
                   
               w0=path_metric[i][out_table_index[j][0]];      //path costs
               w1=path_metric[i][out_table_index[j][1]];
               
               if ( temp[next1] == 0 || (cost[j] + w1) > temp[next1] ){
                  node[(i+1)][next1].from=&node[i][j]; 
                  node[(i+1)][next1].in=1;
                  temp[next1]=cost[j] + w1;
                  }                       
               
               if ( temp[next0] == 0 || (cost[j] + w0) > temp[next0] ){
                  node[(i+1)][next0].from=&node[i][j]; 
                  node[(i+1)][next0].in=0;
                  temp[next0]=cost[j] + w0;
                  }
               }
             }
 
             for(j=0;j<16;j++) {
                 cost[j]=temp[j];
                 temp[j]=0;
                 }
         }
      }


void decode(int *r_d){
     
     struct state *temp1;
     int i;

     temp1=&node[(k+m)][0];
     for (i=(k+m)-1;i>=0;i--){
            r_d[i]=temp1->in;
            temp1=temp1->from;
            }
     }
