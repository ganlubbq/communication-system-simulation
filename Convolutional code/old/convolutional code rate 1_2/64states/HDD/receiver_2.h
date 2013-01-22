#define st_num (int)pow(2,m)

struct state{
       struct state *from;
       int in;                   //the input which results in this state
       int weight;
       //struct state *next1;
       //struct state *next0;
       //float out1_0;               //given 1 is transmitted,the first bit of the output
       //float out1_1;               //given 1 is transmitted,the second bit of the output
       //float out0_0;               //given 0 is transmitted,the first bit of the output
       //float out0_1;               //given 0 is transmitted,the second bit of the output
       };

struct state node[k_m+1][st_num];

int next_table[64][2];
int out_table[64][4];


void table1(void);
void *output(int *,float,int *);                       //main function,producing the decoded sequence
void nodeinit(void);                                 //sub function,initialize the trellis diagram
//void nodereset(void);                                //sub function,reset some parts of the states
void nodeupdate(int *);                  //sub function,calculate the weights and optimal path
void decode(int *);                  //sub function,construct the decoded sequence using  optimal path




void *output(int *x_c,float snr,int *r_d){
    int i,j=0,x,*r;
    float A,n0,Y;
    
    r=(int *) malloc((2*(k+m))*sizeof(int));
    //r_d=(int *) malloc((k+m)*sizeof(int));
    
    //a=(int)((k+m)/k_m);
    //b=(k+m)%k_m;
    
    while (j<(2*(k+m))){
              
              if (x_c[j]==1) A=0.1;
              else A=-0.1 ;
              n0=pow(10,-5)*2/pow(10,snr/10);
              Y=gaurand()*sqrt(n0/(2*0.001))+A;  //Y is the matched filter output
              if (Y>0) r[j]=1;                   //r is the estimation of the input bit
              else if (Y<0) r[j]=0;
              else r[j]=rand()%2;
              j++;
              }
    
    
    
    /*-----------------decoding--------------------*/ 
    
    /*for (j=0;j<64;j++){
         x=j<<1;
         if (x>63) x=x-64;
         
         table[j].next1=&node[i+1][x+1];     
         node[i][j].next0=&node[i+1][x];
         }
    */
    
    
    
    
    nodeinit();
    
    node[0][0].weight=0;
        
    nodeupdate(r);                                    //determine weight
        

    decode(r_d);


    free(r);
   
    //free(node);
    //return r_d;
    }


void table1(void){
     int x,j,h,temp,s[6];
     
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
         
                out_table[j][0]=1^s[1]^s[2]^s[4]^s[5];
                
                
                out_table[j][1]=1^s[0]^s[1]^s[2]^s[5];
                
                
                out_table[j][2]=0^s[1]^s[2]^s[4]^s[5];
                
                
                out_table[j][3]=0^s[0]^s[1]^s[2]^s[5];
                
         }
     }

void nodeinit(void){
     int i,j;

     for (i=0;i<k_m+1;i++){
            for (j=0;j<64;j++){
                
                node[i][j].weight=-1;
                node[i][j].in=-1;
                node[i][j].from = NULL;
                
                
                }
            }

     }




void nodeupdate(int *r){
     int i,j,x,next1,next0;
     int w1,w0;
     
     x=k+m;
     
     for (i=0;i<x;i++){
        for(j=0;j<st_num;j++){
             
             next1 = next_table[j][1];
             next0 = next_table[j][0];
             
             if (node[i][j].weight!=-1)
             {
                                  
              w1 = (out_table[j][0]^r[2*i]) + (out_table[j][1]^r[2*i+1]);
              w0 = (out_table[j][2]^r[2*i]) + (out_table[j][3]^r[2*i+1]);
             
               if (( node[i+1][next1].weight == -1 || (node[i][j].weight + w1) < node[i+1][next1].weight) && (i < k)){
               
               node[i+1][next1].from=&node[i][j]; 
               node[i+1][next1].in=1;
               node[i+1][next1].weight= node[i][j].weight +  w1;

               }                       
               
               
               if (node[i+1][next0].weight == -1 || (node[i][j].weight + w0) < node[i+1][next0].weight){
               node[i+1][next0].from=&node[i][j]; 
               node[i+1][next0].in=0;                        
               node[i+1][next0].weight= node[i][j].weight +  w0;  
             
               }
             }
            }
            }
      
      
      }


void decode(int *r_d){
     
     struct state *temp1;
     int i,x;
     
     x=k+m;

     temp1=&node[x][0];
     for (i=x-1;i>=0;i--){
            r_d[i]=temp1->in;
            temp1=temp1->from;
            
            }
     
}
