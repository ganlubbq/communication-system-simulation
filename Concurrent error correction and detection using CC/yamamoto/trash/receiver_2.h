int viterbi(float *);                                 //sub function,calculate the weights and optimal path
void decode(int *);                                 //sub function,construct the decoded sequence using  optimal path

int output(int *v,float snr,int *y){
    int i,j=0,x;
    float N0,sd,r[2*(k+m)];
    
    N0=2/snr;                                         //SNR = Eb*2/N0 , Eb=1
    sd=sqrt(N0/2);                                    //standard deviation of gaussian r.v.
   
    while (j<(2*(k+m))){
    
              if (v[j]==1) r[j]=1;
              else r[j]=-1;
              
              //r[j]+=gaurand()*sd;        //add noise
              
              ray[j]=rayrand();
              r[j]=gaurand()*sd+r[j]*ray[j];
              j++;
              }
   
     /*-----------------decoding--------------------*/ 
     
    viterbi(r);
    
    if(retransmit==1) return 1;
    
    decode(y);
    }

int viterbi(float *r){
     int i,j,pre0,pre1,templabel[st_num]={},label[st_num]={},count,state=0,b0,b1,b2,b3;
     float w1,w0,cost[st_num]={},temp[st_num]={},diff=0;
     
     cost[0]=1;
     
     for (i=1;i<=(k+m);i++){
        for(j=0;j<st_num;j++){
           
           pre0=prestate[j][0];
           pre1=prestate[j][1];
           
           b0=2*preoutput[j][0]-1;
           b1=2*preoutput[j][1]-1;
           b2=2*preoutput[j][2]-1;
           b3=2*preoutput[j][3]-1;
           
           node[i][j].in=inputbit[j];
           
           if (cost[pre0]!=0){
               //w0 = r[2*(i-1)]*b0 + r[2*(i-1)+1]*b1;     //path costs
               w0 = r[2*(i-1)]*b0*ray[2*(i-1)] + r[2*(i-1)+1]*b1*ray[2*(i-1)+1];
               node[i][j].from=&node[i-1][pre0]; 
               temp[j]=cost[pre0] + w0;
               }
             
           if (cost[pre1]!=0){
               //w1 = r[2*(i-1)]*b2 + r[2*(i-1)+1]*b3;     //path costs
               w1 = r[2*(i-1)]*b2*ray[2*(i-1)] + r[2*(i-1)+1]*b3*ray[2*(i-1)+1];
               if ( cost[pre1] + w1 > temp[j] ){
                  node[i][j].from=&node[i-1][pre1]; 
                  diff=(cost[pre1] + w1)-temp[j];
                  if (label[pre1]==1 || diff<A) templabel[j]=1;
                  temp[j]=cost[pre1] + w1;
                  }  
               else {
                    diff=temp[j]-(cost[pre1] + w1);
                    if (label[pre0]==1 || diff<A) templabel[j]=1;
                    }
               } 
               
           }
       
        /*for(j=0;j<st_num;j++) {
           printf("%d",label[j]);
           }*/
       
        count=0;
        for(j=0;j<st_num;j++) {
           cost[j]=temp[j];
           temp[j]=0;
           count+=label[j];
           label[j]=templabel[j];
           templabel[j]=0;
           }
        
       
        /*printf(" %d",count);
        printf("\n");
        system("pause");*/
        
        if (count == st_num) {
           //printf("retran\n");
           retransmit=1;
           return 1;
           }
        }
     }


void decode(int *y){
     
     struct state *temp1;
     int i;

     temp1=&node[k+m][0];
     for (i=(k+m)-1;i>=0;i--){
            y[i]=temp1->in;
            temp1=temp1->from;
            }
     }
