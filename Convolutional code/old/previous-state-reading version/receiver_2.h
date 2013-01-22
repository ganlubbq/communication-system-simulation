void viterbi(float *);                                 //sub function,calculate the weights and optimal path
void decode(int *);                                 //sub function,construct the decoded sequence using  optimal path

void output(int *v,float snr,int *y){
    int i,j=0,x;
    float N0,sd,r[2*(k+m)];
    
    N0=2/snr;                                         //SNR = Eb*2/N0 , Eb=1
    sd=sqrt(N0/2);                                    //standard deviation of gaussian r.v.
   
    while (j<(2*(k+m))){
    
              r[j]=2*v[j]-1;
              
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
     int i,j,pre0,pre1,b0,b1,b2,b3;
     float w1,w0,cost[st_num]={},temp[st_num]={};
     
     cost[0]=1;
     
     for (i=1;i<=(k+m);i++){
        
        for(j=0;j<st_num;j++){
           
           pre0=prestate[j][0];pre1=prestate[j][1];
           //b0=2*preoutput[j][0]-1;b1=2*preoutput[j][1]-1;b2=2*preoutput[j][2]-1; b3=2*preoutput[j][3]-1;
           b0=preoutput[j][0];b1=preoutput[j][1];b2=preoutput[j][2];b3=preoutput[j][3];
           
           node[i][j].in=inputbit[j];
           
           if (cost[pre0]!=0){
               w0 = r[2*(i-1)]*b0 + r[2*(i-1)+1]*b1;     //path costs
               
               node[i][j].from=&node[i-1][pre0]; 
               temp[j]=cost[pre0] + w0;
               }
             
           if (cost[pre1]!=0){
               w1 = r[2*(i-1)]*b2 + r[2*(i-1)+1]*b3;
               
               if ( cost[pre1] + w1 > temp[j] ){
                  node[i][j].from=&node[i-1][pre1]; 
                  temp[j]=cost[pre1] + w1;
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

     temp1=&node[k+m][0];
     for (i=(k+m)-1;i>=0;i--){
            y[i]=temp1->in;
            
            //printf("%d ",i);
            
            temp1=temp1->from;
            }
     }
