void output(int *,double,int *);
void llr_comput(struct complex *,double,double *,int);
void viterbi(void);
void sova(double a);
void soft_id_comput(double *);
void decode(int *);//reconstruct the decoded sequence using optimal path

void initialize(){
     int i,j;
     
     for(i=0;i<(k+m)/2+1;i++){
         if(i!=(k+m)/2){
              L[3*i]  =10000000;
              L[3*i+1]=10000000;
              L[3*i+2]=10000000;
              }
         
         for(j=0;j<16;j++){
             node[i][j].st=j;
             node[i][j].delta=0;
             }
        }
     }

void output(int *v,double snr,int *r_d){
    int i,j=0,x;
    double N0,sd,temp=1,a;                             //temp = sqrt(Es)
    struct complex r[(k+m)/2];      
    int id[(k+m)/2*3]={};
    double soft_id[(k+m)/2*3]={};
     
    modulation(v,r);
    
    N0=temp/(2*snr);                                    //SNR = Eb/N0  2Es = 3Eb
    sd=sqrt(N0/2);                                    //standard deviation of gaussian r.v.
   
    a=4*4/N0;                                             // 4*d*Es/N0
   
    while (j<(k+m)/2){
             
              r[j].re += gaurand()*sd;                 //r is the received vector                                 
              r[j].im += gaurand()*sd;
              ray[j]=rayrand();
              //r[j].re = r[j].re*ray[j] + gaurand()*sd;                 //r is the received vector                                 
              //r[j].im = r[j].im*ray[j] + gaurand()*sd;
              
              j++;
              }
   
    /*-----------------decoding--------------------*/ 
    for(i=0;i<iteration;i++){  
        initialize();
        /*compute the bit based metric*/
        
        llr_comput(r,N0,soft_id,i);
        
        deinterleave(M[0],M[1]);
        
        viterbi();
       
        sova(a);
        
        soft_id_comput(soft_id);
        
        interleave(soft_id);
       }
    
    decode(r_d);
    }

double fmin(double x1,double x2){
       if(x1>x2) return x2;
       else return x1;
       }

double min(double *a){
      double temp;
      int i;
      
      temp=a[0];
      for(i=0;i<4;i++){
           if(a[i]<temp) temp=a[i];
           }
      
      return temp;
      }

void llr_comput(struct complex *r,double N0,double *soft_id,int pass){     //compute the bit based metrics
     int i,j,h,l,x=0,b0,b1,b2,temp1;
     double temp[8],ttemp[4],test,temp2;
      
     for (i=0;i<(k+m)/2;i++){
         
         if(pass==0){
         
         for (j=0;j<8;j++){//the usual 8 symbol metric
             //temp[j]=2*(mapping[j].re*r[i].re + mapping[j].im*r[i].im)/N0;
             temp[j]=(pow(mapping[j].re-r[i].re,2) + pow(mapping[j].im-r[i].im,2))/N0;
             //temp[j]=mapping[j].re*r[i].re*ray[i] + mapping[j].im*r[i].im*ray[i];
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
                     temp2=logsum(-ttemp[0],-ttemp[1]);
                     temp2=logsum(temp2,-ttemp[2]);
                     M[l][3*i+j]=-logsum(temp2,-ttemp[3]);
                     }   
             }
         }
        
         else{
       
         for (j=0;j<8;j++){//the usual 8 symbol metric
         
             b0=2*h_table[j][0]-1;
             b1=2*h_table[j][1]-1;
             b2=2*h_table[j][2]-1;
             
             test=(b0*soft_id[3*i]+b1*soft_id[3*i+1]+b2*soft_id[3*i+2])/2;
             
             temp[j]=(pow(mapping[j].re-r[i].re,2) + pow(mapping[j].im-r[i].im,2))/N0
                     -test;
            
             }
         
         for (j=0;j<3;j++){
                 for (l=0;l<2;l++){
                    
                     x=0;
                     for (h=0;h<8;h++){
                         
                         b0=2*h_table[h][j]-1;

                         
                         if (h_table[h][j]==l){
                            
                            ttemp[x]=temp[h]+b0*soft_id[3*i+j]/2;
                            //ttemp[x]=temp[h];
                            x++;
                            }
                         }
                     temp2=logsum(-ttemp[0],-ttemp[1]);
                     temp2=logsum(temp2,-ttemp[2]);
                     M[l][3*i+j]=-logsum(temp2,-ttemp[3]);
                     }   
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
               
               w00 = M[out_table[j][0]][3*i]+M[out_table[j][1]][3*i+1]+M[out_table[j][2]][3*i+2];   //path cost
               w01 = M[out_table[j][3]][3*i]+M[out_table[j][4]][3*i+1]+M[out_table[j][5]][3*i+2];
               w10 = M[out_table[j][6]][3*i]+M[out_table[j][7]][3*i+1]+M[out_table[j][8]][3*i+2];
               w11 = M[out_table[j][9]][3*i]+M[out_table[j][10]][3*i+1]+M[out_table[j][11]][3*i+2];
               
               if (temp[next00] == 0){  
                  node[i+1][next00].from1=&node[i][j]; 
                  node[i+1][next00].in1[0]=0;
                  node[i+1][next00].in1[1]=0;
                  temp[next00]=cost[j] + w00;
                  }                       
               else if ( (cost[j] + w00) < temp[next00] ){
                    node[i+1][next00].from2=node[i+1][next00].from1; 
                    node[i+1][next00].from1=&node[i][j]; 
                    node[i+1][next00].in2[0]=node[i+1][next00].in1[0];
                    node[i+1][next00].in2[1]=node[i+1][next00].in1[1];
                    node[i+1][next00].in1[0]=0;
                    node[i+1][next00].in1[1]=0;
                    node[i+1][next00].delta = temp[next00] - (cost[j] + w00);
                    temp[next00]=cost[j] + w00;
                    }
               else if ( (cost[j] + w00)-temp[next00]<node[i+1][next00].delta  || node[i+1][next00].delta==0){
                    node[i+1][next00].in2[0]=0;
                    node[i+1][next00].in2[1]=0;
                    node[i+1][next00].from2=&node[i][j]; 
                    node[i+1][next00].delta=(cost[j] + w00)-temp[next00];
                    }  
               
               if(i < k/2){
                    
               if (temp[next01] == 0){  
                  node[i+1][next01].from1=&node[i][j]; 
                  node[i+1][next01].in1[0]=0;
                  node[i+1][next01].in1[1]=1;
                  temp[next01]=cost[j] + w01;
                  }                       
               else if ( (cost[j] + w01) < temp[next01] ){
                    node[i+1][next01].from2=node[i+1][next01].from1; 
                    node[i+1][next01].from1=&node[i][j]; 
                    node[i+1][next01].in2[0]=node[i+1][next01].in1[0];
                    node[i+1][next01].in2[1]=node[i+1][next01].in1[1];
                    node[i+1][next01].in1[0]=0;
                    node[i+1][next01].in1[1]=1;
                    node[i+1][next01].delta = temp[next01] - (cost[j] + w01);
                    temp[next01]=cost[j] + w01;
                    }
               else if ( (cost[j] + w01)-temp[next01]<node[i+1][next01].delta  || node[i+1][next01].delta==0){
                    node[i+1][next01].in2[0]=0;
                    node[i+1][next01].in2[1]=1;
                    node[i+1][next01].from2=&node[i][j]; 
                    node[i+1][next01].delta=(cost[j] + w01)-temp[next01];
                    }  
                    
               if (temp[next10] == 0){  
                  node[i+1][next10].from1=&node[i][j]; 
                  node[i+1][next10].in1[0]=1;
                  node[i+1][next10].in1[1]=0;
                  temp[next10]=cost[j] + w10;
                  }                       
               else if ( (cost[j] + w10) < temp[next10] ){
                    node[i+1][next10].from2=node[i+1][next10].from1; 
                    node[i+1][next10].from1=&node[i][j]; 
                    node[i+1][next10].in2[0]=node[i+1][next10].in1[0];
                    node[i+1][next10].in2[1]=node[i+1][next10].in1[1];
                    node[i+1][next10].in1[0]=1;
                    node[i+1][next10].in1[1]=0;
                    node[i+1][next10].delta = temp[next10] - (cost[j] + w10);
                    temp[next10]=cost[j] + w10;
                    }
               else if ( (cost[j] + w10)-temp[next10]<node[i+1][next10].delta  || node[i+1][next10].delta==0){
                    node[i+1][next10].in2[0]=1;
                    node[i+1][next10].in2[1]=0;
                    node[i+1][next10].from2=&node[i][j]; 
                    node[i+1][next10].delta=(cost[j] + w10)-temp[next10];
                    }  
                    
               if (temp[next11] == 0){  
                  node[i+1][next11].from1=&node[i][j]; 
                  node[i+1][next11].in1[0]=1;
                  node[i+1][next11].in1[1]=1;
                  temp[next11]=cost[j] + w11;
                  }                       
               else if ( (cost[j] + w11) < temp[next11] ){
                    node[i+1][next11].from2=node[i+1][next11].from1; 
                    node[i+1][next11].from1=&node[i][j]; 
                    node[i+1][next11].in2[0]=node[i+1][next11].in1[0];
                    node[i+1][next11].in2[1]=node[i+1][next11].in1[1];
                    node[i+1][next11].in1[0]=1;
                    node[i+1][next11].in1[1]=1;
                    node[i+1][next11].delta = temp[next11] - (cost[j] + w11);
                    temp[next11]=cost[j] + w11;
                    }
               else if ( (cost[j] + w11)-temp[next11]<node[i+1][next11].delta  || node[i+1][next11].delta==0){
                    node[i+1][next11].in2[0]=1;
                    node[i+1][next11].in2[1]=1;
                    node[i+1][next11].from2=&node[i][j]; 
                    node[i+1][next11].delta=(cost[j] + w11)-temp[next11];
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

void sova(double a){
     int i,j,s,states[(k+m)/2+1],x;
     struct state *temp1,*temp2,*temp;
     int path1_0,path1_1,path1_2,path2_0,path2_1,path2_2,input1,input2,tm;
     
     temp1=&node[(k+m)/2][0];
     for (i=(k+m)/2;i>=0;i--){
            states[i]=temp1->st;
            temp1=temp1->from1;
            }
     
     for (i=k/2-1;i>m;i--){
         temp1=&node[i+1][states[i+1]];
         temp2=&node[i+1][states[i+1]];
             x=0;
             do{
                 //for path 2,僅第一次吃from2 
                 if(x==0){
                     input2=2*(temp2->in2[0])+(temp2->in2[1]);
                     
                     temp2=temp2->from2;
                     tm=temp2->st;
                     path2_0=out_table[tm][0+3*input2];
                     path2_1=out_table[tm][1+3*input2];
                     path2_2=out_table[tm][2+3*input2];
                     }    
                 else{
                      input2=2*(temp2->in1[0])+(temp2->in1[1]);
                      temp2=temp2->from1;
                      tm=temp2->st;
                      path2_0=out_table[tm][0+3*input2];
                      path2_1=out_table[tm][1+3*input2];
                      path2_2=out_table[tm][2+3*input2];
                      }
                 
                 
                 //for path 1
                 input1=2*(temp1->in1[0])+(temp1->in1[1]);
                 //temp1=temp1->from1;
                 temp=temp1->from1;
                 tm=temp->st;
                 path1_0=out_table[tm][0+3*input1];
                 path1_1=out_table[tm][1+3*input1];
                 path1_2=out_table[tm][2+3*input1];
                
                 //temp1=&node[i+1][states[i+1]];//go home
                
                 //update
                 if(path1_0!=path2_0){
                    L[3*(i-x)]  =fmin(L[3*(i-x)],temp1->delta);
                  }
                 if(path1_1!=path2_1){
                    L[3*(i-x)+1]=fmin(L[3*(i-x)+1],temp1->delta);
                  }
                 if(path1_2!=path2_2){
                    L[3*(i-x)+2]=fmin(L[3*(i-x)+2],temp1->delta);
                  }
                
             temp1=temp1->from1;//update玩再往前    
                 
             x++;
             //}while((temp2->st != temp1->st)&&(i-x)>=m);
             }while((x < depth)&&(i-x)>=m);
        }
     }

void soft_id_comput(double *soft_id){
     int i,x1,x2,x3,j,tm,b1,b2,b3,input;
     struct state *temp;
     double E1,E2,E3,var1,var2,var3,temp1=0,temp2=0,temp3=0,C1,C2,C3;
     
     temp=&node[(k+m)/2][0];
     for (i=(k+m)/2-1;i>=0;i--){
            
            input=2*(temp->in1[0])+(temp->in1[1]);
            temp=temp->from1;
            tm=temp->st;
            
            b1=out_table[tm][0+3*input];
            b2=out_table[tm][1+3*input];
            b3=out_table[tm][2+3*input];
            
            //hardoutput*reliability = LLR
            soft_id[3*i]  =(2*b1-1)*  L[3*i];
            soft_id[3*i+1]=(2*b2-1)*L[3*i+1];
            soft_id[3*i+2]=(2*b3-1)*L[3*i+2];
            
            soft_id[3*i]  *=mul;
            soft_id[3*i+1]*=mul;
            soft_id[3*i+2]*=mul;
         }
     /*---------------------------------------------------*/
    
    /* x1=0; x2=0; x3=0;
     for (i=0;i<(k+m)/2;i++){
         if(soft_id[3*i]<pow(10,9) && soft_id[3*i]> -pow(10,9)){
            temp1+=L[3*i];
            x1++;
            }
         if(soft_id[3*i+1]<pow(10,9) && soft_id[3*i+1]> -pow(10,9)){
            temp2+=L[3*i+1];
            x2++;
            }
         if(soft_id[3*i+2]<pow(10,9) && soft_id[3*i+2]> -pow(10,9)){
            temp3+=L[3*i+2];
            x3++;
            }
         }
     
     E1=temp1/x1; E2=temp2/x2; E3=temp3/x3;
     
    x1=0; x2=0; x3=0; temp1=0; temp2=0; temp3=0;
     for (i=0;i<(k+m)/2;i++){
         if(soft_id[3*i]<pow(10,9) && soft_id[3*i]> -pow(10,9)){
            temp1+=pow(L[3*i],2);
            x1++;
            }
         if(soft_id[3*i+1]<pow(10,9) && soft_id[3*i+1]> -pow(10,9)){
            temp2+=pow(L[3*i+1],2);
            x2++;
            }
         if(soft_id[3*i+2]<pow(10,9) && soft_id[3*i+2]> -pow(10,9)){
            temp3+=pow(L[3*i+2],2);
            x3++;
            }
         }
     
 
      
     var1=temp1/x1-E1*E1;
     var2=temp2/x2-E2*E2;
     var3=temp3/x3-E3*E3;
     
     C1=2*E1/var1;
     C2=2*E2/var2;
     C3=2*E3/var3;
     
     
     for (i=0;i<(k+m)/2;i++){
         if(soft_id[3*i]<pow(10,9) && soft_id[3*i]> -pow(10,9)){
            //soft_id[3*i]*=C1;
            }
         if(soft_id[3*i+1]<pow(10,9) && soft_id[3*i+1]> -pow(10,9)){
            //soft_id[3*i+1]*=C2;
            }
         if(soft_id[3*i+2]<pow(10,9) && soft_id[3*i+2]> -pow(10,9)){
            //soft_id[3*i+2]*=C3;
            }
         }*/
     
     /*printf("E%+f ",E1);
     printf("C%+f\n",C1);
     printf("E%+f ",E2);
     printf("C%+f\n",C2);
     printf("E%+f ",E3);
     printf("C%+f\n",C3);
     system("pause");*/
     
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

