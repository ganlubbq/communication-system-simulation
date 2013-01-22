void table(void);
void output(int *,float,int *);                     //main function,producing the decoded sequence
void cost_cal(float *);                              //sub function,calculate the weights and optimal path
void decode(int *);                                  //sub function,construct the decoded sequence using  optimal path

void output(int *x_c,float snr,int *r_d){
    int i,j=0,x;
    float N0,sd,*r,temp=1;                             //temp = sqrt(Eb)
      
    r=(float *) malloc((k+m)/2*3*sizeof(float));
    
    N0=temp*1.5/snr;                                    //SNR = Eb*1.5/N0
    sd=sqrt(N0/2);                                    //standard deviation of gaussian r.v.
   
    while (j<(k+m)/2*3){
    
              if (x_c[j]==1) temp=1;                  //temp = sqrt(Eb)
              else temp=-1;
              //r[j]=gaurand()*sd+temp;                 //r is the received vector                                 
              ray[j]=rayrand();
              r[j]=gaurand()*sd+temp*ray[j];
              j++;
              }

   /*-----------------decoding--------------------*/ 
   
    cost_cal(r);
        
    decode(r_d);

    free(r);
    }

void table(void){
     int x,i,j,h,temp,s[6],t[12];
     
     for (j=0;j<64;j++){
         
         temp=j;
         for(h=m-1;h>=0;h--){ //derive current state in binary form
             s[h]=temp/(int)pow(2,h);
             if(h!=0) temp=temp%(int)pow(2,h);
             }
         
          
          next_table[j][0]=2*s[0]+4*s[1]+16*s[3]+32*s[4];      //table for the next states
          next_table[j][1]=2*s[0]+4*s[1]+8+16*s[3]+32*s[4];
          next_table[j][2]=1+2*s[0]+4*s[1]+16*s[3]+32*s[4];
          next_table[j][3]=1+2*s[0]+4*s[1]+8+16*s[3]+32*s[4];
          
          /*for(i=0;i<4;i++){
              printf("%d ",next_table[j][i]);
              }
           printf("\n");
           system("pause");*/
          
          t[0]=s[0]^s[2]^s[3]^s[4];   //table for the path outputs
          t[1]=s[0]^s[1]^s[3]^s[5];
          t[2]=s[0]^s[2]^s[3]^s[4]^s[5];
                 
          t[3]=s[0]^s[2]^s[3]^s[4]; 
          t[4]=s[0]^s[1]^1^s[3]^s[5];
          t[5]=s[0]^s[2]^1^s[3]^s[4]^s[5];
          
          t[6]=1^s[0]^s[2]^s[3]^s[4]; 
          t[7]=s[0]^s[1]^s[3]^s[5];
          t[8]=1^s[0]^s[2]^s[3]^s[4]^s[5];
         
          t[9]=1^s[0]^s[2]^s[3]^s[4];
         t[10]=1^s[0]^s[1]^s[3]^s[5];
         t[11]=s[0]^s[2]^s[3]^s[4]^s[5];
         
         
          for(i=0;i<12;i++){
              if(t[i] == 1) out_table[j][i] = 1;
              else out_table[j][i] = -1;
              }        
         }
     }


void cost_cal(float *r){
     int i,j,next00,next01,next10,next11;
     float w00,w01,w10,w11,cost[64]={},temp[64]={};
     
     cost[0]=1;
     
     for (i=0;i<(k+m)/2;i++){
        for(j=0;j<st_num;j++){
             
             next00 = next_table[j][0];
             next01 = next_table[j][1];
             next10 = next_table[j][2];
             next11 = next_table[j][3];
             
             if (cost[j]!=0){
                                  
               //w00 = out_table[j][0]*r[3*i] + out_table[j][1]*r[3*i+1] + out_table[j][2]*r[3*i+2]; 
               //w01 = out_table[j][3]*r[3*i] + out_table[j][4]*r[3*i+1] + out_table[j][5]*r[3*i+2]; 
               //w10 = out_table[j][6]*r[3*i] + out_table[j][7]*r[3*i+1] + out_table[j][8]*r[3*i+2]; 
               //w11 = out_table[j][9]*r[3*i] + out_table[j][10]*r[3*i+1]+ out_table[j][11]*r[3*i+2]; 
 
               w00 = out_table[j][0]*r[3*i]*ray[3*i] + out_table[j][1]*r[3*i+1]*ray[3*i+1] + out_table[j][2]*r[3*i+2]*ray[3*i+2]; 
               w01 = out_table[j][3]*r[3*i]*ray[3*i] + out_table[j][4]*r[3*i+1]*ray[3*i+1] + out_table[j][5]*r[3*i+2]*ray[3*i+2]; 
               w10 = out_table[j][6]*r[3*i]*ray[3*i] + out_table[j][7]*r[3*i+1]*ray[3*i+1] + out_table[j][8]*r[3*i+2]*ray[3*i+2]; 
               w11 = out_table[j][9]*r[3*i]*ray[3*i] + out_table[j][10]*r[3*i+1]*ray[3*i+1]+ out_table[j][11]*r[3*i+2]*ray[3*i+2];
               
               if ( temp[next00] == 0 || (cost[j] + w00) > temp[next00] ){
                  node[i+1][next00].from=&node[i][j]; 
                  node[i+1][next00].in[0]=0;
                  node[i+1][next00].in[1]=0;
                  temp[next00]=cost[j] + w00;
                  }                       
               if (( temp[next01] == 0 || (cost[j] + w01) > temp[next01] )&& (i < k/2)){
                  node[i+1][next01].from=&node[i][j]; 
                  node[i+1][next01].in[0]=0;
                  node[i+1][next01].in[1]=1;
                  temp[next01]=cost[j] + w01;
                  }
               if (( temp[next10] == 0 || (cost[j] + w10) > temp[next10] )&& (i < k/2)){
                  node[i+1][next10].from=&node[i][j]; 
                  node[i+1][next10].in[0]=1;
                  node[i+1][next10].in[1]=0;
                  temp[next10]=cost[j] + w10;
                  }
               if (( temp[next11] == 0 || (cost[j] + w11) > temp[next11] )&& (i < k/2)){
                  node[i+1][next11].from=&node[i][j]; 
                  node[i+1][next11].in[0]=1;
                  node[i+1][next11].in[1]=1;
                  temp[next11]=cost[j] + w11;
                  }
               }
             }
 
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
