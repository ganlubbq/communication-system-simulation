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
              
              //ray[j]=rayrand();
              //r[j]=gaurand()*sd+r[j]*ray[j];
              
              j++;
              }
   
     /*-----------------decoding--------------------*/ 
     
    viterbi(r);
        
    decode(y);
    }

void viterbi(float *r){
     int i,j,next1,next0,b0,b1,b2,b3,temp2;
     float w1,w0,*cost,*temp,*pass;
     
     cost=(float *) malloc(st_num*sizeof(float));
     temp=(float *) malloc(st_num*sizeof(float)); 
     
     for (j=0;j<st_num;j++){
         cost[j]=0;
         temp[j]=0;
     }
                           
     temp2=1<<(m-1);
     
     cost[0]=65536;
     
     for (i=0;i<(k+m);i++){
        
        for(j=0;j<st_num;j++){
             
             //if (cost[j]!=0){
               
               next0 = j>>1;
               next1 = next0+temp2;
               
               b0=2*nextoutput[j][0]-1;b1=2*nextoutput[j][1]-1;b2=2*nextoutput[j][2]-1; b3=2*nextoutput[j][3]-1;
               //b0=nextoutput[j][0];b1=nextoutput[j][1];b2=nextoutput[j][2]; b3=nextoutput[j][3];
           
                   
               w0 = r[2*i]*b0 + r[2*i+1]*b1;     //path costs
               w1 = r[2*i]*b2 + r[2*i+1]*b3;
               
               /*w0 = r[2*i]*b0*ray[2*i] + r[2*i+1]*b1*ray[2*i+1];
               w1 = r[2*i]*b2*ray[2*i] + r[2*i+1]*b3*ray[2*i+1];*/
               
               //if (( temp[next1] == 0 || (cost[j] + w1) > temp[next1]) && (i < k)){
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
             //}
 
             pass=cost;
             cost=temp;
             temp=pass;
             for(j=0;j<st_num;j++) {
                 temp[j]=0;
             }
             
         }
         
         sum=cost[0]-65536;
         free(cost);
         free(temp);
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
