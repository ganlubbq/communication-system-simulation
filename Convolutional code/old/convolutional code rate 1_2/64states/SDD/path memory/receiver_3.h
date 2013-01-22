struct state{
       struct state *from;
       
       int in;
       //int ti;
       int st;
       float weight;
       };

struct state node[path+1][st_num];

int next_table[64][2];
float out_table[64][4];

void table(void);
void *output(int *,float,int *);                       //main function,producing the decoded sequence
void nodeinit(float *);                                //sub function,initialize the trellis diagram
void nodereset(float *,int);                           //sub function,reset some parts of the states
void decode(int *,int);                                //sub function,construct the decoded sequence using  optimal path

void *output(int *x_c,float snr,int *r_d){
    int i=0,j=0;
    float N0,sd,*r,temp=1;
    
    r=(float *) malloc((2*(k+m))*sizeof(float));
    
    N0=temp*2/snr;                                    //SNR = Eb*2/N0
    sd=sqrt(N0/2);                                    //standard deviation of gaussian r.v.
    
    while (j<(2*(k+m))){
              
              if (x_c[j]==1) temp=1;                  //temp = sqrt(Eb)
              else temp=-1;
              r[j]=gaurand()*sd+temp;                 //r is the received vector  
              
              j++;
              }
    
    /*-----------------decoding--------------------*/ 
    
    nodeinit(r);
    
    do{
       if(i!=0){
               nodereset(r,i);
               }
       
       decode(r_d,i);
       
       i++;
    }while(i<k_m-path+1);
    
    free(r);
    }


void table(void){
     int x,j,h,temp,s[6],t1,t2,t3,t4;
     for (j=0;j<64;j++){
         x=j<<1;
         if (x>63) x=x-64;
         
         next_table[j][0] = x;
         next_table[j][1] = x+1;
         
         temp=j;
         
         for(h=m-1;h>=0;h--){ //derive current state in binary form
             s[h]=temp/(int)pow(2,h);
             if(h!=0) temp=temp%(int)pow(2,h);
             }
         
                t1=1^s[1]^s[2]^s[4]^s[5];
                if(t1 == 1) out_table[j][0] = 0.1;
                else out_table[j][0] = -0.1;
                
                t2=1^s[0]^s[1]^s[2]^s[5];
                if(t2 == 1) out_table[j][1] = 0.1;
                else out_table[j][1] = -0.1;
                
                t3=0^s[1]^s[2]^s[4]^s[5];
                if(t3 == 1) out_table[j][2] = 0.1;
                else out_table[j][2] = -0.1;
                
                t4=0^s[0]^s[1]^s[2]^s[5];
                if(t4 == 1) out_table[j][3] = 0.1;
                else out_table[j][3] = -0.1;
         }
     }


void nodeinit(float *r){
     int i,j,next1,next0;
     float w1,w0;
     
     for (i=0;i<path+1;i++){
            for (j=0;j<64;j++){
                node[i][j].weight=-1;
                node[i][j].in=-1;
                node[i][j].from = NULL;
                //node[i][j].ti=i;
                node[i][j].st=j;
                }
            }
    
    node[0][0].weight=0;
    
    for (i=0;i<path;i++){
        for(j=0;j<st_num;j++){
             
             next1 = next_table[j][1];
             next0 = next_table[j][0];
             
             if (node[i][j].weight!=-1)
             {
                                  
              w1 = out_table[j][0]*r[+2*i] + out_table[j][1]*r[+2*i+1];
              w0 = out_table[j][2]*r[+2*i] + out_table[j][3]*r[+2*i+1];
             
               if (( node[i+1][next1].weight == -1 || (node[i][j].weight + w1) > node[i+1][next1].weight) && (i < k)){
               node[i+1][next1].from=&node[i][j]; 
               node[i+1][next1].in=1;
               node[i+1][next1].weight= node[i][j].weight +  w1;
               }                       
               
               if (node[i+1][next0].weight == -1 || (node[i][j].weight + w0) > node[i+1][next0].weight){
               node[i+1][next0].from=&node[i][j]; 
               node[i+1][next0].in=0;                        
               node[i+1][next0].weight= node[i][j].weight +  w0;  
               }
             }
            }
            }
    }

void nodereset(float *r,int l){
     int i,j,temp,next1,next0;
     float w1,w0;
     
     for (i=0;i<path;i++){
            for (j=0;j<st_num;j++){
            
            node[i][j].weight=node[i+1][j].weight;
            node[i][j].in=node[i+1][j].in;
            
            if(node[i+1][j].from!=NULL){
            
            temp=node[i+1][j].from->st;
            node[i][j].from = &node[i-1][temp];
            //node[i][j].from=node[i+1][j].from;
            
            }
            
            else{
                 node[i][j].from = NULL;
                 }
            }
          }

   
     
     for (j=0;j<st_num;j++){
         node[0][j].from = NULL;
         }
     
     for (j=0;j<st_num;j++){
         node[path][j].weight=-1;
         node[path][j].in=-1;
         node[path][j].from = NULL;
         }
    
     for (j=0;j<st_num;j++){
         
         next1 = next_table[j][1];
         next0 = next_table[j][0];
         
         if (node[path-1][j].weight!=-1)
             {
                                  
              w1 = out_table[j][0]*r[2*l+2*(path-1)] + out_table[j][1]*r[2*l+2*(path-1)+1];
              w0 = out_table[j][2]*r[2*l+2*(path-1)] + out_table[j][3]*r[2*l+2*(path-1)+1];
             
               if (( node[path][next1].weight == -1 || (node[path-1][j].weight + w1) > node[path][next1].weight) && (l+(path-1) < k)){
               node[path][next1].from=&node[path-1][j]; 
               node[path][next1].in=1;
               node[path][next1].weight = node[path-1][j].weight +  w1;
               }                       
               
               if (node[path][next0].weight == -1 || (node[path-1][j].weight + w0) > node[path][next0].weight){
               node[path][next0].from=&node[path-1][j]; 
               node[path][next0].in=0;                        
               node[path][next0].weight = node[path-1][j].weight +  w0;  
               }
             }
            }
           }



void decode(int *r_d,int i){
     
     struct state *temp1;
     int j,x,h,c0=0,c1=0,next_st;
     x=path;

     if(i!=k_m-path){
          for(j=0;j<64;j++){
             if (node[x][j].weight!=-1){
                temp1=&node[x][j];
                
                for(h=0;h<path-1;h++){
                    temp1=temp1->from;                  
                    }

                if(temp1->in==1) c1++;
                else c0++;
              }
             }
          
          if (c1>c0) r_d[i]=1;
          else if (c1<c0) r_d[i]=0;
          else r_d[i]=rand()%2;
               
          }
     
     else{
          temp1=&node[x][0];
          for (j=x-1;j>=0;j--){
              r_d[j+i]=temp1->in;
              temp1=temp1->from;
             }
          }
     }
