struct state{
       struct state *from;
       int in;
       };

struct state node[k+1][st_num];

int next_table[64][2];
float out_table[64][4];

void table(void);
void *output(int *,float,int *);                     //main function,producing the decoded sequence
int cost_cal(float *);                              //sub function,calculate the weights and optimal path
void decode(int *,int);                                  //sub function,construct the decoded sequence using  optimal path

void *output(int *x_c,float snr,int *r_d){
    int i,j=0,x,final_state;
    float N0,sd,*r,temp=1;                             //temp = sqrt(Eb)
      
    r=(float *) malloc(2*k*sizeof(float));
    
    N0=temp*2/snr;                                    //SNR = Eb*2/N0
    sd=sqrt(N0/2);                                    //standard deviation of gaussian r.v.
   
    while (j<2*k){
    
              if (x_c[j]==1) temp=1;                  //temp = sqrt(Eb)
              else temp=-1;
              r[j]=gaurand()*sd+temp;                 //r is the received vector                                 
              
              j++;
              }

   /*-----------------decoding--------------------*/ 
   
    final_state=cost_cal(r);
        
    decode(r_d,final_state);

    free(r);
    }

void table(void){
     int x,j,h,temp,s[6],t1,t2,t3,t4;
     for (j=0;j<64;j++){
         x=j<<1;
         if (x>63) x=x-64;
         next_table[j][0] = x;              //table for the next states
         next_table[j][1] = x+1;
         
         temp=j;
         
         for(h=m-1;h>=0;h--){ //derive current state in binary form
             s[h]=temp/(int)pow(2,h);
             if(h!=0) temp=temp%(int)pow(2,h);
             }
         
                t1=1^s[1]^s[2]^s[4]^s[5];   //table for the path outputs
                if(t1 == 1) out_table[j][0] = 1;
                else out_table[j][0] = -1;
                
                t2=1^s[0]^s[1]^s[2]^s[5];
                if(t2 == 1) out_table[j][1] = 1;
                else out_table[j][1] = -1;
                
                t3=0^s[1]^s[2]^s[4]^s[5];
                if(t3 == 1) out_table[j][2] = 1;
                else out_table[j][2] = -1;
                
                t4=0^s[0]^s[1]^s[2]^s[5];
                if(t4 == 1) out_table[j][3] = 1;
                else out_table[j][3] = -1;
         }
     }


int cost_cal(float *r){
     int i,j,next1,next0,final_state;
     float w1,w0,temp1,cost[64]={},temp[64]={};
     
     cost[0]=1;
     
     for (i=0;i<k;i++){
        for(j=0;j<st_num;j++){
             
             next1 = next_table[j][1];
             next0 = next_table[j][0];
             
             if (cost[j]!=0){
                                  
               w1 = out_table[j][0]*r[2*i] + out_table[j][1]*r[2*i+1];      //path costs
               w0 = out_table[j][2]*r[2*i] + out_table[j][3]*r[2*i+1];
             
               if ( temp[next1] == 0 || (cost[j] + w1) > temp[next1]){
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
             //printf("i=%d\n",i);
             //system("pause");
             for(j=0;j<64;j++) {
                 cost[j]=temp[j];
                 temp[j]=0;
                 }
             
             if(i==k-1){
                        temp1=cost[0];
                        for (i=0;i<64;i++){
                            if (cost[i] >= temp1){
                               temp1=cost[i];
                               final_state=i;
                               }
                            }
                        //printf("i=%d\n",final_state);
                        //system("pause");
                        return final_state;
                        
                        }
             
             
         }
      }


void decode(int *r_d,int final_state){
     
     struct state *temp1;
     int i,x;
     
     x=k;

     temp1=&node[x][final_state];
     for (i=x-1;i>=0;i--){
            r_d[i]=temp1->in;
            temp1=temp1->from;
            }
     }
