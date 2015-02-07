void fft(struct complex *,struct complex *,int);
void viterbi(struct complex *);                              //sub function,calculate the weights and optimal path
void decode(int *);                                 //sub function,construct the decoded sequence using  optimal path

void output(struct complex *x,int *y){
     struct complex r[K+m];
     
     //"de" ofdm here 
     fft(x,r,N);
         
     viterbi(r);
        
     decode(y);
     }
    
    
/*void fft(struct complex *x,struct complex *r,int npoints){
     int i,k,n,temp;
     float real,imag;
     
     for (i=0;i<S;i++){
         for (k=0;k<npoints;k++){
         
             real=0;
             imag=0;
             for (n=CP;n<npoints+CP;n++){
                 temp=(k*n)%npoints;
                 real+=x[i*(npoints+CP)+n].re*w[temp].re + x[i*(npoints+CP)+n].im*-w[temp].im;
                 imag+=-x[i*(npoints+CP)+n].re*-w[temp].im + x[i*(npoints+CP)+n].im*w[temp].re;
                 }
             
             r[i*npoints+k].re=real/npoints;
             r[i*npoints+k].im=imag/npoints;
             }
         }
     }*/


     
void viterbi(struct complex *r){
     int i,j,next1,next0,b0,b1,b2,b3,temp2;
     float w1,w0,cost[st_num]={},temp[st_num]={};
     
     temp2=1<<(m-1);
     
     cost[0]=65536;
     
     for (i=0;i<(K+m);i++){
        
        for(j=0;j<st_num;j++){
             
             //if (cost[j]!=0){
               
               next0 = j>>1;
               next1 = next0+temp2;
               
               //b0=2*nextoutput[j][0]-1;b1=2*nextoutput[j][1]-1;b2=2*nextoutput[j][2]-1; b3=2*nextoutput[j][3]-1;
               b0=nextoutput[j][0];b1=nextoutput[j][1];b2=nextoutput[j][2]; b3=nextoutput[j][3];
           
                   
               w0 = r[i].re*b0 + r[i].im*b1;     //path costs
               w1 = r[i].re*b2 + r[i].im*b3;
               
               /*w0 = r[2*i]*b0*ray[i] + r[2*i+1]*b1*ray[i];
               w1 = r[2*i]*b2*ray[i] + r[2*i+1]*b3*ray[i];*/
               
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
 
             for(j=0;j<st_num;j++) {
                 //printf("cost: %f\n",cost[j]);
                 cost[j]=temp[j];
                 temp[j]=0;
                 }
         }
      }


void decode(int *y){
     struct state *temp1;
     int i;

     temp1=&node[(K+m)][0];
     for (i=(K+m)-1;i>=0;i--){
            y[i]=temp1->in;
            temp1=temp1->from;
            }
     }
