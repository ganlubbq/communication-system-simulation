void process(float *,float *,float *);
void puncture(float *,float *);

void viterbi(float *,float *,float *,float);
void decode(float *, float *,int *);

int A[k+m];

int output(int *c,float snr,float snr_db,int *r_d){
    int i,j=0;
    float N0,sd;
    float r[(k+m)*3],x[k+m]={0},y1[k+m]={0},y2[k+m]={0};
    float LLR[k+m]={0};
    float z[k+m]={0};

    /*--------------channel effect----------------*/ 
    
    N0=3/snr;//SNR = (3*Eb)/N0
    sd=sqrt(N0/2);//standard deviation of gaussian r.v.
   
    while (j<(k+m)*3){
          r[j]=2*c[j]-1;
              
          r[j] += gaurand()*sd;//add noise     
          
          //ray[j]=rayrand();
          //r[j] = r[j]*ray[j];                              
          j++;
    }
    
    j=0;
    while (j<k+m){
          x[j] =r[3*j];
          y1[j]=r[3*j+1];
          y2[j]=r[3*j+2];                        
          j++;
    }
  
   //puncture(y1,y2);

   /*-----------------decoding--------------------*/ 
    
    for(i=0;i<iteration;i++){
       
        process(LLR,x,z);//make LLR[]=z[]+x[]
        
        viterbi(LLR,y1,z,snr_db);
        
        process(LLR,x,z);//make LLR[]=z[]+x[]
        
        for (j=k;j<k+m;j++){
            LLR[j]=0;
        }
          
        Minterleave(LLR);

        viterbi(LLR,y2,z,snr_db);

        Mdeinterleave(z);
    }
    
    Mdeinterleave(LLR);
    
    deinterleave(A);
    
    decode(LLR,z,r_d);

    return 1;
}

void puncture(float *y1,float *y2){
     int i;
     
     for(i=0;i<k+m;i++){
             if((i&1)==0)y1[i]=0;
             else y2[i]=0;
             }
     }


void process(float *a,float *b,float *c){
     int i;
     
     for (i=0;i<k+m;i++){
         a[i]=b[i]+c[i];
     }
     
}
     
void viterbi(float *LLR,float *y,float *z,float snr_db){//dillema of whether 1st siso should substract a priori directly or whole LLR then add x, choose the later one for now
     /*
     use LLR and y to do viterbi
     store the hard result in say A[], and "sum" value
     use N0(snr) and sum to lookup in TABLE for reliability say 0.4
     cal the "difference" of A[] and LLR[], store in z[], then z[]*=0.4 (z[] ~ k)
     
     z[] now works as extrinsic information, */
     
     int i,j,next1,next0;
     float w1,w0;
     float *cost,*temp,*pass;
     //float A[k+m];
    
     cost=(float *) malloc(st_num*sizeof(float));
     temp=(float *) malloc(st_num*sizeof(float)); 
     
     for (j=0;j<st_num;j++){
         cost[j]=0;
         temp[j]=0;
     }
     
     cost[0]=65536;
     
     for (i=0;i<k+m;i++){
        
        for(j=0;j<st_num;j++){
             
             next1 = next_table[j][1];
             next0 = next_table[j][0];
                   
             /*w0=path_metric[i][out_table_index[j][0]];      //path costs
             w1=path_metric[i][out_table_index[j][1]];*/
               
             w0 = LLR[i]*(2*out_table[j][0]-1) + y[i]*(2*out_table[j][1]-1);     //path costs
             w1 = LLR[i]*(2*out_table[j][2]-1) + y[i]*(2*out_table[j][3]-1);
               
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
             
        pass=cost;
        cost=temp;
        temp=pass;
        for (j=0;j<st_num;j++) {
            temp[j]=0;
        }
             
     }
     
     sum=cost[0]-65536;
     free(cost);
     free(temp);
     
     //decode and store in A[]
     struct state *temp1;
     
     temp1=&node[k+m][0];
     for (i=k+m-1;i>=0;i--){
         A[i]=2*(temp1->in)-1;//modu
         temp1=temp1->from;
     }
     
     //find reliability
     float R;
     int xidx,yidx;
     
     xidx=(int)((float)snr_db/step);yidx=(int)((float)sum/10.0);
     R=TABLE[xidx][yidx];  
     
     /*printf("snr_db = %f\n",snr_db);
     printf("sum = %f\n",sum);
     printf("xidx = %d\n",xidx);
     printf("yidx = %d\n",yidx);
     system("pause");
     
     printf("R = %f\n",R);
     system("pause");*/
     
     //derive z
     for (i=0;i<k;i++){
         /*if (A[i]*LLR[i] < 1){
            z[i]=R*(A[i]-LLR[i]);
         }
         else z[i]=0;*/
         z[i]=R*(A[i]-LLR[i]);
         
     }

     
}

void decode(float *LLR, float *z,int *r_d){
     int i;
     
     for (i=0;i<k;i++){
        /* if((z[i]+LLR[i])<0) r_d[i]=0;
         else r_d[i]=1;*/
         r_d[i]=(A[i]+1)/2;
     }
}

