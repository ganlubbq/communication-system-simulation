void process1(int,double *,double);
void process2(void);
void puncture(double *,double *);

void siso(int,double *,int,double);
void decode(int *);

void initialize(){
     int i,j;
     
     for(i=0;i<k+m+1;i++){
         if(i != k+m){
              L[i]=100000;
              }
         
         for(j=0;j<st_num;j++){
             node[i][j].st=j;
             node[i][j].delta=0;
             node[i][j].from1=NULL;
             node[i][j].from2=NULL;
             }
        }
     }

int output(int *c,double snr,int *r_d){
    int i,j=0;
    double N0,sd;
    double r[(k+m)*3],x[k+m]={},y1[k+m]={},y2[k+m]={};
    
    N0=2/snr;//SNR = (3*Eb)/N0
    sd=sqrt(N0/2);//standard deviation of gaussian r.v.
   
    while (j<(k+m)*3){
              if (c[j]==1) r[j]=1;//temp = sqrt(Eb)
              else r[j]=-1;
              
              r[j] += gaurand()*sd;//add noise     
              
              if (j%3==0) x[(j/3)]=r[j];
              if (j%3==1)y1[(j/3)]=r[j];
              if (j%3==2)y2[(j/3)]=r[j];                        
              
              //ray[j]=rayrand();
              //r[j] = r[j]*ray[j];                              
              j++;
              }
  
   puncture(y1,y2);

   /*-----------------decoding--------------------*/ 
    
    for(i=0;i<iteration;i++){        
       
        process1(i,x,N0);
        
        initialize();
        siso(i,y1,1,N0);

        process2();

        Minterleave(LLR);
        
        initialize();
        siso(i,y2,2,N0);

        Mdeinterleave(z);
        }
    Mdeinterleave(L);
    
    decode(r_d);
    deinterleave(r_d);
    return 1;
    }

void puncture(double *y1,double *y2){
     int i;
     
     for(i=0;i<k+m;i++){
             if(i%2==0)y1[i]=0;
             else y2[i]=0;
             }
     }

void process1(int pass,double *x,double N0){
     int i; 
 
     if(pass==0){
          for(i=0;i<k+m;i++){
             x_metric[i]=4*x[i]/N0;
            
             LLR[i]=x_metric[i];
             
             z[i]=0;//z initialize
             }
          }
     
     else{
          for(i=0;i<k+m;i++){
             LLR[i]=x_metric[i];
             }
          for(i=k;i<k+m;i++){
             z[i]=0;
             }
          }
     }
     
void process2(void){
     int i;
     
     for(i=0;i<k;i++){
          LLR[i]= LLR[i] + x_metric[i];
          }
     for(i=k;i<k+m;i++){
          LLR[i]=0;
          }
     for(i=0;i<k+m;i++){//z歸0 
          z[i]=0;
          }
     }

double fmin(double a,double b){
      if(a<b) return a;
      else return b;
      }
      
double abs(double a){
       if (a>0) return a;
       else return -a;
       
       }
      
void siso(int pass,double *y,int option,double N0){
     int i,j,next1,next0;
     double w1,w0,cost[st_num]={},temp[st_num]={},b0,b1,b2,b3;
     double sum,C,var,temp4,mean,temp5[k+m],temp6;
     //viterbi part
     cost[0]=1;
     
     for (i=0;i<k+m;i++){
        for(j=0;j<st_num;j++){
             
             next1 = next_table[j][1];
             next0 = next_table[j][0];
             
             if (cost[j]!=0){
                
               b0=2*out_table[j][0]-1; b1=2*out_table[j][1]-1;
               b2=2*out_table[j][2]-1; b3=2*out_table[j][3]-1;
               
               w0=b0*z[i]/2 + b0*LLR[i]/2 + b1*y[i]*2/N0;
               w1=b2*z[i]/2 + b2*LLR[i]/2 + b3*y[i]*2/N0;
             
               if ( temp[next1] == 0){
                  node[i+1][next1].from1=&node[i][j]; 
                  node[i+1][next1].in1=1;
                  temp[next1]=cost[j] + w1;
                  }                       
               else if ((cost[j] + w1) > temp[next1]){
                    node[i+1][next1].from2=node[i+1][next1].from1;
                    node[i+1][next1].from1=&node[i][j];
                    node[i+1][next1].in2=node[i+1][next1].in1;
                    node[i+1][next1].in1=1;
                    node[i+1][next1].delta = (cost[j] + w1)- temp[next1];
                    temp[next1]=cost[j] + w1;
                    }
                    
               else {
                    node[i+1][next1].in2=1;
                    node[i+1][next1].from2=&node[i][j]; 
                    node[i+1][next1].delta=temp[next1]-(cost[j] + w1);
                    }
               
               if ( temp[next0]==0){
                  node[i+1][next0].from1=&node[i][j]; 
                  node[i+1][next0].in1=0;
                  temp[next0]=cost[j] + w0;
                  }
               
               else if ((cost[j] + w0) > temp[next0]){
                    node[i+1][next0].from2=node[i+1][next0].from1;
                    node[i+1][next0].from1=&node[i][j];
                    node[i+1][next0].in2=node[i+1][next0].in1;
                    node[i+1][next0].in1=0;
                    node[i+1][next0].delta = (cost[j] + w0)- temp[next0];
                    temp[next0]=cost[j] + w0;
                    }
                    
               else {
                    node[i+1][next0].in2=0;
                    node[i+1][next0].from2=&node[i][j]; 
                    node[i+1][next0].delta=temp[next0]-(cost[j] + w0);
                    }
        
               }
             }
 
             for(j=0;j<st_num;j++) {
                 cost[j]=temp[j];
                 temp[j]=0;
                 }
         }
     
     //sova part
     int s,states[k+m+1],x,count;
     struct state *temp1,*temp2,*temp3;
     int input1,input2;
     
     temp1=&node[k+m][0];
     for (i=k+m;i>0;i--){
            states[i]=temp1->st;
            temp1=temp1->from1;
            }
     states[0]=0;
     
     for (i=k+m-1;i>m;i--){
         temp1=&node[i+1][states[i+1]];
         temp2=&node[i+1][states[i+1]];
             x=0;
             do{
                 //for path 2,僅第一次吃from2 
                 if(x==0){
                     input2=temp2->in2;
                     
                     temp2=temp2->from2;
                     }    
                 else{
                      input2=temp2->in1;
                      
                      temp2=temp2->from1;
                      }
                 
                 //for path 1
                 input1=temp1->in1;
                 //update
                 if(input1!=input2){
                    L[i-x]=fmin(L[i],temp1->delta);
                   }
                 
             temp1=temp1->from1;//update完再往前    
                 
             x++;
             }while((temp2->st != temp1->st)&&(i-x)>=m);
             //}while((x < depth)&&(i-x)>=m);
             }
    
     //LLR part
     temp1=&node[k+m][0];
     for (i=k+m-1;i>=0;i--){
         input1=temp1->in1;
         temp1=temp1->from1;
        
         b1=2*input1-1;
         
         temp5[i] = (b1*L[i]-(LLR[i]+z[i]))*mul;
         
         if(option==1)   LLR[i] = temp5[i];
         else if(option==2)z[i] = temp5[i];
         }
    
     /*-----------------------------------------------------------*/
      /*sum=0;
      count=0;
      temp4=0;
      for (i=0;i<k+m;i++){
         //temp6=abs(temp5[i]);
         if(temp5[i]>0){
            temp6=temp5[i];
            }
         else temp6=0;
         
         if(temp6<100){ 
            count++;
            sum+=temp6;
            temp4+=temp6*temp6;
            } 
         }
     mean=sum/count;
     var=temp4/count-mean*mean;
     C=2*mean/var;
     C=1;
     printf("mean = %f\n",mean);
     printf("var  = %f\n",var);
     printf("C    = %f\n",2*mean/var);
     system("pause");
     
     
     if(option==1){
        for (i=0;i<k+m;i++){
         LLR[i]=temp5[i]*C*mul2;
         }
        }
     
     else if(option==2){
        for (i=0;i<k+m;i++){
         z[i]=temp5[i]*C*mul2;
         }
        }*/
     
     }

void decode(int *r_d){
     struct state *temp1;
     int i,;

     temp1=&node[k+m][0];
     for (i=k+m-1;i>=0;i--){
            r_d[i]=temp1->in1;
            
            temp1=temp1->from1;
            }
     }
     
