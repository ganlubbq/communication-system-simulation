void output(int *,float,int *);                     //main function,producing the decoded sequence
void viterbi(float *);                                 //sub function,calculate the weights and optimal path
void decode(int *);                                 //sub function,construct the decoded sequence using  optimal path

void output(int *x_c,float snr,int *r_d){
    int i,j=0;
    float N0,sd,*r;                             //temp = sqrt(Eb)
      
    r=(float *) malloc((2*k_m)*sizeof(float));
    
    N0=2/snr;                                    //SNR = Eb*2/N0
    sd=sqrt(N0/2);                                    //standard deviation of gaussian r.v.
   
   
    while (j<(2*k_m)){
    
              r[j]=2*x_c[j]-1;                 //r is the received vector                                 
              
              r[j]+=gaurand()*sd;        //add noise
              
              //ray[j]=rayrand();
              //r[j]=gaurand()*sd+temp*ray[j];
              j++;
              }
   
     j=0;  
     while (j<k_m){ //compute path cost
              
              path_metric[j][0]= -1*r[2*j] + -1*r[2*j+1];  //cost of output 00
              path_metric[j][1]= -1*r[2*j] +  1*r[2*j+1];  //cost of output 01
              path_metric[j][2]=  1*r[2*j] + -1*r[2*j+1];  //cost of output 10
              path_metric[j][3]=  1*r[2*j] +  1*r[2*j+1];  //cost of output 11
              
              
             /* path_metric[j][0]= -1*r[2*j]*ray[2*j] + -1*r[2*j+1]*ray[2*j+1];  //cost of output 00
              path_metric[j][1]= -1*r[2*j]*ray[2*j] +  1*r[2*j+1]*ray[2*j+1];  //cost of output 01
              path_metric[j][2]=  1*r[2*j]*ray[2*j] + -1*r[2*j+1]*ray[2*j+1];  //cost of output 10
              path_metric[j][3]=  1*r[2*j]*ray[2*j] +  1*r[2*j+1]*ray[2*j+1];  //cost of output 11*/
              
              j++;
              }
   
   /*-----------------decoding--------------------*/ 
   
    viterbi(r);
        
    decode(r_d);

    free(r);
    }

void viterbi(float *r){
     int i,j,next1,next0;
     float w1,w0;
     float *cost,*temp,*pass;
     
     cost=(float *) malloc(st_num*sizeof(float));
     temp=(float *) malloc(st_num*sizeof(float)); 
     
     for (j=0;j<st_num;j++){
         cost[j]=0;
         temp[j]=0;
     }
     
     cost[0]=65536;
     
     for (i=0;i<k_m;i++){
        
        for(j=0;j<st_num;j++){
             
             next1 = next_table[j][1];
             next0 = next_table[j][0];
                   
             /*w0=path_metric[i][out_table_index[j][0]];      //path costs
             w1=path_metric[i][out_table_index[j][1]];*/
               
             w0 = r[2*i]*(2*out_table[j][0]-1) + r[2*i+1]*(2*out_table[j][1]-1);     //path costs
             w1 = r[2*i]*(2*out_table[j][2]-1) + r[2*i+1]*(2*out_table[j][3]-1);
               
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

}


void decode(int *r_d){
     
     struct state *temp1;
     int i;

     temp1=&node[k_m][0];
     for (i=k_m-1;i>=0;i--){
            r_d[i]=temp1->in;
            temp1=temp1->from;
            }
     }
