/*convolutional code encoding*/

#define G11 015
#define G12 017

#define m 3                                 //memory order = m
#define st_num 8                            //number of states = 2^m

#define mask 0x00000001

struct state{
       struct state *from;
       int in;
       };

struct state node[B*N+m+1][st_num];
int nextstate[st_num][2];
int nextoutput[st_num][4];
int g11[m+1],g12[m+1];                       //generator sequence

void table(void){
     int x,i,j,h,temp,s[m];
     int temp1,temp2,check;
     
     temp1=G11;temp2=G12;
     check=1<<(m+1);
     
     //generator check
     if ( temp1>=check ) temp1-=check;
     if ( temp2>=check ) temp2-=check;
     
     for (i=0;i<m+1;i++){
         g11[i]=temp1/(1<<(m-i));
         temp1=temp1%(1<<(m-i));
         
         g12[i]=temp2/(1<<(m-i));
         temp2=temp2%(1<<(m-i));
         }
         
     for (i=0;i<st_num;i++){
         x=i>>1;
         nextstate[i][0] = x;
         nextstate[i][1] = x + (1<<(m-1));
         
         temp=i;
         
         //derive current state in binary form
         for(h=m-1;h>=0;h--){
             s[m-1-h]=temp/(1<<h);
             if(h!=0) temp=temp%(1<<h);
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
void cc(short *sample,short *v,short *test){
     short i,j,h,l,temp,temp1,temp2,B2,idx;
     short memory[m]={};
     
     /*encode from the lsb
       note the "32bit" correction*/
     B2=B;
     for (h=0;h<N;h++){ 
     
         if(h==N-1) B2=B+m;
     
         for (i=0;i<B2;i++){
         
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
             v[2*i+2*B*h+1]=temp2;
             
             
             
             //B=16 
             idx=2*h + i/8;
             
             l=i%8;
             
             test[idx]=test[idx] | ((temp1<<(2*l)) + (temp2<<(2*l+1)));
             }
         }   
     }

/*apply viterbi to decode r[], then put the decoded bits in y[]*/
void viterbi(float *r,short *y,short *z){
     short i,j,next1,next0;
     float w1,w0,cost[st_num]={0},temp[st_num]={0};
     struct state *temp1;
   
     cost[0]=65536;
     for (i=0;i<B*N+m;i++){
        
        for(j=0;j<st_num;j++){
             
             //if (cost[j]!=0){
               
               next0 = nextstate[j][0];
               next1 = nextstate[j][1];
                   
               w0 = r[2*i]*nextoutput[j][0] + r[2*i+1]*nextoutput[j][1];     //path costs
               w1 = r[2*i]*nextoutput[j][2] + r[2*i+1]*nextoutput[j][3];
               
               if ( cost[j] + w1 > temp[next1] ){
                  node[i+1][next1].from=&node[i][j]; 
                  node[i+1][next1].in=1;
                  temp[next1]=cost[j] + w1;
                  }                       
               
               if ( cost[j] + w0 > temp[next0] ){
                  node[i+1][next0].from=&node[i][j]; 
                  node[i+1][next0].in=0;
                  temp[next0]=cost[j] + w0;
                  }
               }
             //}
 
             for(j=0;j<st_num;j++) {
                 cost[j]=temp[j];
                 temp[j]=0;
                 }
         }
      
     temp1=&node[B*N+m][0];
     i=0;
     while (i<m){
           temp1=temp1->from;
           i++;
           }
     /*for (i=B*N-1;i>=0;i--){
            y[i]=temp1->in;
            temp1=temp1->from;
            }*/
      for (i=N-1;i>=0;i--){
          z[i]=0;
          for (j=B-1;j>=0;j--){
              z[i] |= (temp1->in)<<j;
              y[i*B+j]=temp1->in;
              temp1=temp1->from;
              }
          }
      }
      
      
      
