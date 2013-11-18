void output(int *,double,int *);
void bit_metric_comput(struct complex *,double,double *,int);
void viterbi(void);
void sova(double,int);
void soft_id_comput(double *);
void decode(int *);//reconstruct the decoded sequence using optimal path

void initialize(){
     int i,j;
     
     for(i=0;i<(k+m)/2+1;i++){
         if(i!=(k+m)/2){
              L[3*i]  =100000;
              L[3*i+1]=100000;
              L[3*i+2]=100000;
              }
         
         for(j=0;j<st_num;j++){
             node[i][j].st=j;
             node[i][j].delta=0;
             }
        }
     }

void output(int *v,double snr,int *r_d){
    int i,j=0,x;
    double N0,sd,temp=1,a;                             //temp = sqrt(Es)
    struct complex r[(k+m)/2];      
    double soft_id[(k+m)/2*3]={};
     
    modulation(v,r);
    
    N0=temp/(2*snr);                                    //SNR = Eb/N0  2Es = 3Eb
    sd=sqrt(N0/2);                                    //standard deviation of gaussian r.v.
   
    a=4*4/N0;                                             // 4*d*Es/N0
   
    while (j<(k+m)/2){
              
              //r[j].re += gaurand()*sd;                 //r is the received vector                                 
              //r[j].im += gaurand()*sd;
              ray[j]=rayrand();
              r[j].re = r[j].re*ray[j] + gaurand()*sd;                 //r is the received vector                                 
              r[j].im = r[j].im*ray[j] + gaurand()*sd;
              
              j++;
              }
   
    /*-----------------decoding--------------------*/ 
    for(i=0;i<iteration;i++){  
        initialize();
        
        bit_metric_comput(r,N0,soft_id,i);
        
        deinterleave(LLR);
        
        viterbi();
       
        //sova(a,i);
 
        soft_id_comput(soft_id);
        
        interleave(soft_id);
       }
    
    decode(r_d);
    }
       
double fmin(double x1,double x2){
       if(x1>x2) return x2;
       else return x1;
       }

double max(double *a){
      double temp;
      int i;
      
      temp=a[0];
      for(i=0;i<4;i++){
           if(a[i]>temp) temp=a[i];
           }
      
      return temp;
      }

void bit_metric_comput(struct complex *r,double N0,double *soft_id,int pass){     //compute the bit based metrics
     int i,j,h,l,x=0,b0,b1,b2,temp1;
     double temp[8],ttemp[4],test,M[2],temp2;
     static double cache[(k+m)/2][8];
     
     for (i=0;i<(k+m)/2;i++){
         
         if(pass==0){
         
         for (j=0;j<8;j++){//the usual 8 symbol metric
             //temp[j]=-(pow(mapping[j].re-r[i].re,2) + pow(mapping[j].im-r[i].im,2))/N0;
             temp[j]=-(pow(mapping[j].re*ray[i]-r[i].re,2) + pow(mapping[j].im*ray[i]-r[i].im,2))/N0;
             cache[i][j]=temp[j];
            }
         
         /*bit based metric ; "metric a_b" ,a is the bit position,b is the bit value(0 or 1)*/
         for (j=0;j<3;j++){
                 for (l=0;l<2;l++){
                     x=0;
                     for (h=0;h<8;h++){
                         if (h_table[h][j]==l){
                            ttemp[x]=temp[h];
                            x++;
                            }
                         }
                     temp2=logsum(ttemp[0],ttemp[1]);
                     temp2=logsum(temp2,ttemp[2]);
                     M[l]=logsum(temp2,ttemp[3]);
                     }
                 LLR[3*i+j]=M[1]-M[0];
             }
         }
        
         else{
       
         for (j=0;j<8;j++){//the usual 8 symbol metric
             b0=2*h_table[j][0]-1;
             b1=2*h_table[j][1]-1;
             b2=2*h_table[j][2]-1;
             
             test=b0*soft_id[3*i]/2+b1*soft_id[3*i+1]/2+b2*soft_id[3*i+2]/2;
            
             temp[j]=cache[i][j]+test;
             }
         
         for (j=0;j<3;j++){
                 for (l=0;l<2;l++){
                     x=0;
                     for (h=0;h<8;h++){
                         
                         b0=2*h_table[h][j]-1;

                         if (h_table[h][j]==l){
                            
                            ttemp[x]=temp[h]-b0*soft_id[3*i+j]/2;
                            
                            x++;
                            }
                         }
                     temp2=logsum(ttemp[0],ttemp[1]);
                     temp2=logsum(temp2,ttemp[2]);
                     M[l]=logsum(temp2,ttemp[3]);
                     }
                 LLR[3*i+j]=M[1]-M[0];   
             }
         }
         }
     }


void viterbi(){
     int i,j,next00,next01,next10,next11,t00,t01,t10,t11;
     double w00,w01,w10,w11,testw00,testw01,testw10,testw11,cost[st_num]={},temp[st_num]={};
     
     cost[0]=1;
     
     for (i=0;i<(k+m)/2;i++){
        
        for(j=0;j<st_num;j++){
             
             next00 = next_table[j][0];
             next01 = next_table[j][1];
             next10 = next_table[j][2];
             next11 = next_table[j][3];
             
             if (cost[j]!=0){
               
               w00=(2*out_table[j][0]-1)*LLR[3*i]/2+(2*out_table[j][ 1]-1)*LLR[3*i+1]/2+(2*out_table[j][ 2]-1)*LLR[3*i+2]/2;
               w01=(2*out_table[j][3]-1)*LLR[3*i]/2+(2*out_table[j][ 4]-1)*LLR[3*i+1]/2+(2*out_table[j][ 5]-1)*LLR[3*i+2]/2;
               w10=(2*out_table[j][6]-1)*LLR[3*i]/2+(2*out_table[j][ 7]-1)*LLR[3*i+1]/2+(2*out_table[j][ 8]-1)*LLR[3*i+2]/2;
               w11=(2*out_table[j][9]-1)*LLR[3*i]/2+(2*out_table[j][10]-1)*LLR[3*i+1]/2+(2*out_table[j][11]-1)*LLR[3*i+2]/2;

               if (temp[next00] == 0){  
                  node[i+1][next00].from1=&node[i][j]; 
                  node[i+1][next00].in1[0]=0;
                  node[i+1][next00].in1[1]=0;
                  temp[next00]=cost[j] + w00;
                  }                       
               else if ( (cost[j] + w00) > temp[next00] ){
                    node[i+1][next00].from2=node[i+1][next00].from1; 
                    node[i+1][next00].from1=&node[i][j]; 
                    node[i+1][next00].in2[0]=node[i+1][next00].in1[0];
                    node[i+1][next00].in2[1]=node[i+1][next00].in1[1];
                    node[i+1][next00].in1[0]=0;
                    node[i+1][next00].in1[1]=0;
                    node[i+1][next00].delta = (cost[j] + w00)- temp[next00];
                    temp[next00]=cost[j] + w00;
                    }
               else if (temp[next00]- (cost[j] + w00) >node[i+1][next00].delta|| node[i+1][next00].delta==0){
                    node[i+1][next00].in2[0]=0;
                    node[i+1][next00].in2[1]=0;
                    node[i+1][next00].from2=&node[i][j]; 
                    node[i+1][next00].delta=temp[next00]-(cost[j] + w00);
                    }  
               
               if(i < k/2){
                    
               if (temp[next01] == 0){  
                  node[i+1][next01].from1=&node[i][j]; 
                  node[i+1][next01].in1[0]=0;
                  node[i+1][next01].in1[1]=1;
                  temp[next01]=cost[j] + w01;
                  }                       
               else if ( (cost[j] + w01) > temp[next01] ){
                    node[i+1][next01].from2=node[i+1][next01].from1; 
                    node[i+1][next01].from1=&node[i][j]; 
                    node[i+1][next01].in2[0]=node[i+1][next01].in1[0];
                    node[i+1][next01].in2[1]=node[i+1][next01].in1[1];
                    node[i+1][next01].in1[0]=0;
                    node[i+1][next01].in1[1]=1;
                    node[i+1][next01].delta =(cost[j] + w01)- temp[next01];
                    temp[next01]=cost[j] + w01;
                    }
               else if ( temp[next01] -(cost[j] + w01)>node[i+1][next01].delta || node[i+1][next01].delta==0){
                    node[i+1][next01].in2[0]=0;
                    node[i+1][next01].in2[1]=1;
                    node[i+1][next01].from2=&node[i][j]; 
                    node[i+1][next01].delta=temp[next01]-(cost[j] + w01);
                    }  
                    
               if (temp[next10] == 0){  
                  node[i+1][next10].from1=&node[i][j]; 
                  node[i+1][next10].in1[0]=1;
                  node[i+1][next10].in1[1]=0;
                  temp[next10]=cost[j] + w10;
                  }                       
               else if ( (cost[j] + w10) > temp[next10] ){
                    node[i+1][next10].from2=node[i+1][next10].from1; 
                    node[i+1][next10].from1=&node[i][j]; 
                    node[i+1][next10].in2[0]=node[i+1][next10].in1[0];
                    node[i+1][next10].in2[1]=node[i+1][next10].in1[1];
                    node[i+1][next10].in1[0]=1;
                    node[i+1][next10].in1[1]=0;
                    node[i+1][next10].delta = (cost[j] + w10)-temp[next10];
                    temp[next10]=cost[j] + w10;
                    }
               else if ( temp[next10]-(cost[j] + w10)>node[i+1][next10].delta  || node[i+1][next10].delta==0){
                    node[i+1][next10].in2[0]=1;
                    node[i+1][next10].in2[1]=0;
                    node[i+1][next10].from2=&node[i][j]; 
                    node[i+1][next10].delta=temp[next10]-(cost[j] + w10);
                    }  
                    
               if (temp[next11] == 0){  
                  node[i+1][next11].from1=&node[i][j]; 
                  node[i+1][next11].in1[0]=1;
                  node[i+1][next11].in1[1]=1;
                  temp[next11]=cost[j] + w11;
                  }                       
               else if ( (cost[j] + w11) > temp[next11] ){
                    node[i+1][next11].from2=node[i+1][next11].from1; 
                    node[i+1][next11].from1=&node[i][j]; 
                    node[i+1][next11].in2[0]=node[i+1][next11].in1[0];
                    node[i+1][next11].in2[1]=node[i+1][next11].in1[1];
                    node[i+1][next11].in1[0]=1;
                    node[i+1][next11].in1[1]=1;
                    node[i+1][next11].delta =(cost[j] + w11)- temp[next11];
                    temp[next11]=cost[j] + w11;
                    }
               else if ( temp[next11]-(cost[j] + w11)>node[i+1][next11].delta  || node[i+1][next11].delta==0){
                    node[i+1][next11].in2[0]=1;
                    node[i+1][next11].in2[1]=1;
                    node[i+1][next11].from2=&node[i][j]; 
                    node[i+1][next11].delta=temp[next11]-(cost[j] + w11);
                    }                   
                 }
              
             }
            }

           for(j=0;j<st_num;j++) {
                 cost[j]=temp[j];
                 temp[j]=0;
                 }
         }
      }

     
double abs(double x){
       if(x>0)return x;
       else return -x;
       }

void soft_id_comput(double *soft_id){
     int i,x1,x2,x3,j,tm,b1,b2,b3,input,count;
     struct state *temp;
     double sum,E,var,temp1,temp2,C;
     
     temp=&node[(k+m)/2][0];
     for (i=(k+m)/2-1;i>=0;i--){
            
            input=2*(temp->in1[0])+(temp->in1[1]);
            temp=temp->from1;
            tm=temp->st;
            
            b1=out_table[tm][0+3*input];
            b2=out_table[tm][1+3*input];
            b3=out_table[tm][2+3*input];
            
            //extrinsic informaiton comput
            soft_id[3*i]  =(2*b1-1)*R;
            soft_id[3*i+1]=(2*b2-1)*R;
            soft_id[3*i+2]=(2*b3-1)*R;

           /* soft_id[3*i]*=mul;
            soft_id[3*i+1]*=mul;
            soft_id[3*i+2]*=mul;*/
         }
     /*---------------------------------------------------*/
    /* sum=0;
     count=0;
     temp1=0;
     for (i=0;i<(k+m)/2;i++){
         temp2=abs(soft_id[3*i]);
         if(temp2<1000){sum+=temp2;temp1+=temp2*temp2;count++;}
         temp2=abs(soft_id[3*i+1]);
         if(temp2<1000){sum+=temp2;temp1+=temp2*temp2;count++;}
         temp2=abs(soft_id[3*i+2]);
         if(temp2<1000){sum+=temp2;temp1+=temp2*temp2;count++;}
         }
     E=sum/count;
     var=temp1/count-E*E;
     C=2*E/var;
     
     for (i=0;i<(k+m)/2;i++){
         soft_id[3*i]  *=(C*mul);
         soft_id[3*i+1]*=(C*mul);
         soft_id[3*i+2]*=(C*mul);
         }*/
     }

void decode(int *r_d){
     
     struct state *temp1;
     int i,x;
     
     x=(k+m)/2;

     temp1=&node[x][0];
     for (i=x-1;i>=0;i--){
            r_d[2*i]=temp1->in1[0];
            r_d[2*i+1]=temp1->in1[1];
            
            temp1=temp1->from1;
            }
     }

