//void output(int *,float,int *);                     //main function,producing the decoded sequence
void bit_metric_comput(struct complex *,int *,int);
void viterbi(void);                              //sub function,calculate the weights and optimal path
void decode(int *);                                  //sub function,construct the decoded sequence using  optimal path

int output(int *v,float snr,int *r_d){
    int i,j=0;
    float N0,sd,temp=1;                                                 //temp = sqrt(Es)
    struct complex r[(k+m)/2];
    
    int id[(k+m)/2*3]={};
    
    modulation(v,r);
    
    N0=temp/(2*snr);                                    //SNR = Eb/N0 , Es = 3Eb
    sd=sqrt(N0/2);                                    //standard deviation of gaussian r.v.
    
    while (j<(k+m)/2){
              ray[j]=rayrand();
              r[j].re = r[j].re + gaurand()*sd;                 //r is the received vector                                 
              r[j].im = r[j].im + gaurand()*sd;
              
              //r[j].re = r[j].re*ray[j] + gaurand()*sd;                 //r is the received vector                                 
              //r[j].im = r[j].im*ray[j] + gaurand()*sd;
              
              j++;
              }
   
    /*-------------------decoding------------------------------*/

    
    for(i=0;i<iteration;i++){
       
        bit_metric_comput(r,id,i);
        
        deinterleave(bit_metric[0],bit_metric[1]);
        
        viterbi();
        
        decode(r_d);
       
        input(r_d,id,1);//coding+interleaving
             
        }
   
    return 1;
    }



float max(float a,float b,float c,float d){
      float temp;
      
      temp=a;
      if(b>temp) temp=b;
      if(c>temp) temp=c;
      if(d>temp) temp=d;
      return temp;
      }

void bit_metric_comput(struct complex *r,int *id,int pass){
     int i,j,l,h,b0,b1,b2,temp1;
     float temp[8];
     
     for (i=0;i<(k+m)/2;i++){
         //the usual 8 symbol metric
         for (j=0;j<8;j++){
             temp[j]=mapping[j].re*r[i].re + mapping[j].im*r[i].im;
             //temp[j]=mapping[j].re*r[i].re*ray[i] + mapping[j].im*r[i].im*ray[i];
             //temp[j]=-(pow(mapping[j].re*ray[i]-r[i].re,2) + pow(mapping[j].im*ray[i]-r[i].im,2));
             
             }
         
         if(pass==0){//suboptimal bit metric
        
        
         bit_metric[0][3*i]  =max(temp[0],temp[2],temp[6],temp[4]);
         bit_metric[1][3*i]  =max(temp[1],temp[3],temp[5],temp[7]);
         bit_metric[0][3*i+1]=max(temp[0],temp[1],temp[5],temp[4]);
         bit_metric[1][3*i+1]=max(temp[2],temp[3],temp[6],temp[7]);
         bit_metric[0][3*i+2]=max(temp[0],temp[1],temp[2],temp[3]);
         bit_metric[1][3*i+2]=max(temp[4],temp[5],temp[6],temp[7]);
         }
         
         else{//reselection
                  
         b0=id[i*3];
         b1=id[i*3+1];
         b2=id[i*3+2];
         
         temp1=0+2*b1+4*b2;
         bit_metric[0][3*i]=temp[temp1];
         
         temp1=1+2*b1+4*b2;
         bit_metric[1][3*i]=temp[temp1];
         
         temp1=b0+2*0+4*b2;
         bit_metric[0][3*i+1]=temp[temp1];
         temp1=b0+2*1+4*b2;
         bit_metric[1][3*i+1]=temp[temp1];
         temp1=b0+2*b1+4*0;
         bit_metric[0][3*i+2]=temp[temp1];
         temp1=b0+2*b1+4*1;
         bit_metric[1][3*i+2]=temp[temp1];
              }
         }
     }

void viterbi(void){
     int i,j,next00,next01,next10,next11,t00,t01,t10,t11;
     float w00,w01,w10,w11,metric[8],cost[st_num]={},temp[st_num]={};
    // float temp0,temp1,temp2,temp3,temp4,temp5,temp6,temp7,temp8,temp9,temp10,temp11,temp12;
     //temp? is the actual cost
     
     cost[0]=1;
     
     for (i=0;i<(k+m)/2;i++){
        metric[0]=bit_metric[0][3*i]+bit_metric[0][3*i+1]+bit_metric[0][3*i+2];
        metric[1]=bit_metric[1][3*i]+bit_metric[0][3*i+1]+bit_metric[0][3*i+2];
        metric[2]=bit_metric[0][3*i]+bit_metric[1][3*i+1]+bit_metric[0][3*i+2];
        metric[3]=bit_metric[1][3*i]+bit_metric[1][3*i+1]+bit_metric[0][3*i+2];
        metric[4]=bit_metric[0][3*i]+bit_metric[0][3*i+1]+bit_metric[1][3*i+2];
        metric[5]=bit_metric[1][3*i]+bit_metric[0][3*i+1]+bit_metric[1][3*i+2];
        metric[6]=bit_metric[0][3*i]+bit_metric[1][3*i+1]+bit_metric[1][3*i+2];
        metric[7]=bit_metric[1][3*i]+bit_metric[1][3*i+1]+bit_metric[1][3*i+2];
        
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

               w00 = metric[t00];   //path cost
               w01 = metric[t01];
               w10 = metric[t10];
               w11 = metric[t11];
 
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

            for(j=0;j<st_num;j++) {
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
