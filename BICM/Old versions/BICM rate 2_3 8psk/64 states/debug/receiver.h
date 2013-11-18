void table(void);
void output(int *,float,int *);                     //main function,producing the decoded sequence
void metric_comput(struct complex *,float *,float *,float *,float *,float *,float *);
void viterbi(struct complex *,float *,float *,float *,float *,float *,float *);                              //sub function,calculate the weights and optimal path
void decode(int *);                                  //sub function,construct the decoded sequence using  optimal path
float max(float,float,float,float);

void output(int *v,float snr,int *r_d){
    int i,j=0;
    float N0,sd,temp=1;                                                 //temp = sqrt(Es)
    float metric1_0[(k+m)/2],metric2_0[(k+m)/2],metric3_0[(k+m)/2];
    float metric1_1[(k+m)/2],metric2_1[(k+m)/2],metric3_1[(k+m)/2];
    struct complex *r;
    
    r=(struct complex *) malloc((k+m)/2*sizeof(struct complex));
    modulation(v,r);
    
    N0=temp/(2*snr);                                    //SNR = Eb/N0 , Es = 3Eb
    sd=sqrt(N0/2);                                    //standard deviation of gaussian r.v.
   
    /*while (j<(k+m)/2){
              r[j].re += gaurand()*sd;                 //r is the received vector                                 
              r[j].im += gaurand()*sd;
              j++;
              }*/

   /*-----------------decoding--------------------*/ 
   
    /*for(i=0;i<(k+m)/2+1;i++){
          for(j=0;j<64;j++){
             node[i][j].weight=0;             
             }
          }*/
    
    
    metric_comput(r,metric1_0,metric1_1,metric2_0,metric2_1,metric3_0,metric3_1);
    
    /*deinterleave1(metric1_0);
    deinterleave1(metric1_1);
    deinterleave2(metric2_0);
    deinterleave2(metric2_1);
    deinterleave3(metric3_0);
    deinterleave3(metric3_1);*/
    
    viterbi(r,metric1_0,metric1_1,metric2_0,metric2_1,metric3_0,metric3_1);
        
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
          
          t[0]=s[0]^s[2]^s[3];   //table for the path outputs
          t[1]=s[0]^s[3]^s[5];
          t[2]=s[0]^s[2]^s[3]^s[4]^s[5];
                 
          t[3]=s[0]^s[2]^1^s[3]; 
          t[4]=s[0]^1^s[3]^s[5];
          t[5]=s[0]^s[2]^1^s[3]^s[4]^s[5];
          
          t[6]=1^s[0]^s[2]^s[3]; 
          t[7]=1^s[0]^s[3]^s[5];
          t[8]=1^s[0]^s[2]^s[3]^s[4]^s[5];
         
          t[9]=s[0]^s[2]^s[3];
          t[10]=s[0]^s[3]^s[5];
          t[11]=s[0]^s[2]^s[3]^s[4]^s[5];
         
         
          for(i=0;i<12;i++){
              out_table[j][i] = t[i];
              
              }        
         }
     }

float max(float a,float b,float c,float d){
      float temp;
      
      temp=a;
      if(b>temp) temp=b;
      if(c>temp) temp=c;
      if(d>temp) temp=d;
      return temp;
      }

void metric_comput(struct complex *r,float *metric1_0,float *metric1_1,float *metric2_0,float *metric2_1,float *metric3_0,float *metric3_1){
     int i,j;
     float temp[8];
     
     for (i=0;i<(k+m)/2;i++){
         
         for (j=0;j<8;j++){
             temp[j]=mapping[j].re*r[i].re + mapping[j].im*r[i].im;
             }
         
         metric1_0[i]=max(temp[0],temp[2],temp[6],temp[4]);
         metric1_1[i]=max(temp[1],temp[3],temp[5],temp[7]);
         metric2_0[i]=max(temp[0],temp[1],temp[5],temp[4]);
         metric2_1[i]=max(temp[2],temp[3],temp[6],temp[7]);
         metric3_0[i]=max(temp[0],temp[1],temp[2],temp[3]);
         metric3_1[i]=max(temp[4],temp[5],temp[6],temp[7]);
         
         }
     }



void viterbi(struct complex *r,float *metric1_0,float *metric1_1,float *metric2_0,float *metric2_1,float *metric3_0,float *metric3_1){
     int i,j,next00,next01,next10,next11,t00,t01,t10,t11;
     float w00,w01,w10,w11,metric[8],cost[64]={},temp[64]={};
     float temp0,temp1,temp2,temp3,temp4,temp5,temp6,temp7,temp8,temp9,temp10,temp11,temp12;
     //temp? is the actual cost
     
     cost[0]=1;
     //node[0][0].weight=1;
     
     for (i=0;i<(k+m)/2;i++){
        metric[0]=metric1_0[i]+metric2_0[i]+metric3_0[i];
        metric[1]=metric1_1[i]+metric2_0[i]+metric3_0[i];
        metric[2]=metric1_0[i]+metric2_1[i]+metric3_0[i];
        metric[3]=metric1_1[i]+metric2_1[i]+metric3_0[i];
        metric[4]=metric1_0[i]+metric2_0[i]+metric3_1[i];
        metric[5]=metric1_1[i]+metric2_0[i]+metric3_1[i];
        metric[6]=metric1_0[i]+metric2_1[i]+metric3_1[i];
        metric[7]=metric1_1[i]+metric2_1[i]+metric3_1[i];
        
       
       for(j=0;j<64;j++) {
                 printf("%.1f ",cost[j]);
                }
       printf("\n");
       
        for(j=0;j<st_num;j++){
             
             t00 = out_table[j][0]+2*out_table[j][1]+4*out_table[j][2];
             t01 = out_table[j][3]+2*out_table[j][4]+4*out_table[j][5];
             t10 = out_table[j][6]+2*out_table[j][7]+4*out_table[j][8];
             t11 = out_table[j][9]+2*out_table[j][10]+4*out_table[j][11];
             
             
             next00 = next_table[j][0];
             next01 = next_table[j][1];
             next10 = next_table[j][2];
             next11 = next_table[j][3];
             
             if (cost[j]!=0){
             //if (node[i][j].weight!=0){

               w00 = metric[t00];   //path cost
               w01 = metric[t01];
               w10 = metric[t10];
               w11 = metric[t11];
 
               if ( temp[next00] == 0 || (cost[j] + w00) > temp[next00] ){
               //if ( node[i+1][next00].weight == 0 || (node[i][j].weight + w00) > node[i+1][next00].weight ){  
                  node[i+1][next00].from=&node[i][j]; 
                  node[i+1][next00].in[0]=0;
                  node[i+1][next00].in[1]=0;
                  temp[next00]=cost[j] + w00;
                  //node[i+1][next00].weight=node[i][j].weight + w00;
                  }                       
               if (( temp[next01] == 0 || (cost[j] + w01) > temp[next01] )&& (i < k/2)){
                  //if (( node[i+1][next01].weight == 0 || (node[i][j].weight + w01) > node[i+1][next01].weight )&& (i < k/2)){
                  node[i+1][next01].from=&node[i][j]; 
                  node[i+1][next01].in[0]=0;
                  node[i+1][next01].in[1]=1;
                  temp[next01]=cost[j] + w01;
                  //node[i+1][next01].weight=node[i][j].weight + w01;
                  }
               if (( temp[next10] == 0 || (cost[j] + w10) > temp[next10] )&& (i < k/2)){
                  //if (( node[i+1][next10].weight == 0 || (node[i][j].weight + w10) > node[i+1][next10].weight )&& (i < k/2)){
                  node[i+1][next10].from=&node[i][j]; 
                  node[i+1][next10].in[0]=1;
                  node[i+1][next10].in[1]=0;
                  temp[next10]=cost[j] + w10;
                  //node[i+1][next10].weight=node[i][j].weight + w10;
                  }
               if (( temp[next11] == 0 || (cost[j] + w11) > temp[next11] )&& (i < k/2)){
                  //if (( node[i+1][next11].weight == 0 || (node[i][j].weight + w11) > node[i+1][next11].weight )&& (i < k/2)){
                  node[i+1][next11].from=&node[i][j]; 
                  node[i+1][next11].in[0]=1;
                  node[i+1][next11].in[1]=1;
                  temp[next11]=cost[j] + w11;
                  //node[i+1][next11].weight=node[i][j].weight + w11;
                  }
              
              
              printf("state=%d\n",j);
              printf("next00=%2d  ",next00);   printf("out=%d%d%d  ",out_table[j][0],out_table[j][1],out_table[j][2]);   printf("cost=%3.1f\n",w00);
              printf("next01=%2d  ",next01);   printf("out=%d%d%d  ",out_table[j][3],out_table[j][4],out_table[j][5]);   printf("cost=%3.1f\n",w01);
              printf("next10=%2d  ",next10);   printf("out=%d%d%d  ",out_table[j][6],out_table[j][7],out_table[j][8]);   printf("cost=%3.1f\n",w10);
              printf("next11=%2d  ",next11);   printf("out=%d%d%d  ",out_table[j][9],out_table[j][10],out_table[j][11]); printf("cost=%3.1f\n",w11);
              system("pause");
              
              
              
               }
            }
            

            
            
            if(i==(k+m)/2-1){
               for(j=0;j<64;j++) {
                 printf("%.1f ",temp[j]);
                }
                printf("\n");         
                  system("pause");           
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

