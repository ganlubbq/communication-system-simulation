/*convolutional code encoding*/

#define G11 015
#define G12 017

#define m 3                                  //memory order = m
#define st_num 8                            //number of states = 2^m

#define mask 0x00000001

short in[B*N+m+1][st_num];
short state[B*N+m+1][st_num];
short nextstate[st_num][2];
short nextoutput[st_num][4];
short g11[m+1],g12[m+1];                       //generator sequence

void table(void){
     int x,i,j,h,temp,s[m];
     int temp1,temp2,check;
     
     temp1=G11;temp2=G12;
     check=(int)pow(2,m+1);
     
     //generator check
     if ( temp1>=check ) temp1-=check;
     if ( temp2>=check ) temp2-=check;
     
     for (i=0;i<m+1;i++){
         g11[i]=temp1/(int)pow(2,m-i);
         temp1=temp1%(int)pow(2,m-i);
         
         g12[i]=temp2/(int)pow(2,m-i);
         temp2=temp2%(int)pow(2,m-i);
         }
         
     for (i=0;i<st_num;i++){
         x=i>>1;
         nextstate[i][0] = x;
         nextstate[i][1] = x + (int)pow(2,m-1);
         
         temp=i;
         
         //derive current state in binary form
         for(h=m-1;h>=0;h--){
             s[m-1-h]=temp/(int)pow(2,h);
             if(h!=0) temp=temp%(int)pow(2,h);
             }
            
         nextoutput[i][0]=0*g11[0];
         nextoutput[i][1]=0*g12[0];
         nextoutput[i][2]=1*g11[0];
         nextoutput[i][3]=1*g12[0];
         for (j=0;j<m;j++){
            nextoutput[i][0]=nextoutput[i][0]^s[j]*g11[j+1];
            nextoutput[i][1]=nextoutput[i][1]^s[j]*g12[j+1];
            nextoutput[i][2]=nextoutput[i][2]^s[j]*g11[j+1];
            nextoutput[i][3]=nextoutput[i][3]^s[j]*g12[j+1];
            }
         }
     }

/* convolutional encode sample(bit-packed), then put the codeword in v[] */
void cc(int *sample,int *v){
     int i,j,h,temp,temp1,temp2,B_;
     int memory[m]={0};

     table();
  
     /*encode from the lsb
       note the "32bit" correction*/
     B_=B;
     for (h=0;h<N;h++){ 
     
         if(h==N-1) B_=B+m;
     
         for (i=0;i<B_;i++){
         
             temp=(sample[h]>>i)&mask;//temp is the current input bit
         
             if(i>=B-m && h!=N-1) memory[B-1-i]=temp;
         
             if(i<32){
               temp1=temp * g11[0];
               temp2=temp * g12[0];
             }
             else{
                  temp1=0;
                  temp2=0;
                  }
             for (j=1;j<=m;j++){
            
                if  (i-j<0){
                    temp1=temp1^ memory[j-i-1] * g11[j];
                    temp2=temp2^ memory[j-i-1] * g12[j];
                    }
            
                else if (i-j<32){
                    temp1=temp1^ ((sample[h]>>(i-j)) & mask) * g11[j];
                    temp2=temp2^ ((sample[h]>>(i-j)) & mask) * g12[j];
                    }
                }
        
             v[2*i+2*B*h]=temp1;
             v[(2*i)+1+2*B*h]=temp2;
             }
         }   
     }

/*apply viterbi to decode r[], then put the decoded bits in y[]*/
void viterbi(float *r,int *y){
     int i,j,next1,next0;
     float w1,w0,cost[st_num]={0},temp[st_num]={0};
     int temp1;
   
     cost[0]=65536;
     for (i=0;i<B*N+m;i++){
        
        for(j=0;j<st_num;j++){
             
             if (cost[j]!=0){
               
               next0 = nextstate[j][0];
               next1 = nextstate[j][1];
                   
               w0 = r[2*i]*nextoutput[j][0] + r[2*i+1]*nextoutput[j][1];     //path costs
               w1 = r[2*i]*nextoutput[j][2] + r[2*i+1]*nextoutput[j][3];
               
               if ( cost[j] + w1 > temp[next1] ){
                  state[i+1][next1]=j; 
                  in[i+1][next1]=1;
                  temp[next1]=cost[j] + w1;
                  }                       
               
               if ( cost[j] + w0 > temp[next0] ){
                  state[i+1][next0]=j; 
                  in[i+1][next0]=0;
                  temp[next0]=cost[j] + w0;
                  }
               }
             }
 
             for(j=0;j<st_num;j++) {
                 cost[j]=temp[j];
                 temp[j]=0;
                 }
         }
      
     temp1=state[B*N+m][0];
     i=0;
     while (i<m){
           temp1=state[B*N+m-i][temp1];
           i++;
           }
     for (i=B*N-1;i>=0;i--){
            y[i]=in[i+1][temp1];
            temp1=state[i+1][temp1];
            
            }
      }
      
      
      
