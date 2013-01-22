void viterbi(float *);                                 //sub function,calculate the weights and optimal path
void decode(int *);                                 //sub function,construct the decoded sequence using  optimal path

void output(int *v,float snr,int *y){
    int i,j=0,x;
    float N0,sd,r[2*(k+m)];
    
    N0=2/snr;                                         //SNR = Eb*2/N0 , Eb=1
    sd=sqrt(N0/2);                                    //standard deviation of gaussian r.v.
   
    while (j<(2*(k+m))){
    
              if (v[j]==1) r[j]=1;
              else r[j]=-1;
              
              r[j]+=gaurand()*sd;        //add noise
              
              ray[j]=rayrand();
              //r[j]=gaurand()*sd+r[j]*ray[j];
              j++;
              }
   
     /*-----------------decoding--------------------*/ 
     
    viterbi(r);
        
    decode(y);
    }

void viterbi(float *r){
     int i,j,next1,next0,b0,b1,b2,b3;
     float w1,w0,cost[st_num]={},temp[st_num]={};
     
     cost[0]=1;
     
     for (i=0;i<(k+m);i++){
        
        for(j=0;j<st_num;j++){
             
             if (cost[j]!=0){
               
               next0 = nextstate[j][0];
               next1 = nextstate[j][1];
               b0=2*nextoutput[j][0]-1;b1=2*nextoutput[j][1]-1;b2=2*nextoutput[j][2]-1; b3=2*nextoutput[j][3]-1;
           
                   
               w0 = r[2*i]*b0 + r[2*i+1]*b1;     //path costs
               w1 = r[2*i]*b2 + r[2*i+1]*b3;
               
               if ( temp[next1] == 0 || (cost[j] + w1) > temp[next1] ){
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
 
             for(j=0;j<st_num;j++) {
                 cost[j]=temp[j];
                 temp[j]=0;
                 }
         }
      }


void decode(int *y){
     
     struct state *temp1;
     int i;

     temp1=&node[(k+m)][0];
     for (i=(k+m)-1;i>=0;i--){
            y[i]=temp1->in;
            temp1=temp1->from;
            }
     }
