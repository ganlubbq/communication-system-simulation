struct state{
       struct state *from;
       int in;                   //the input which results in this state
       //int ti;
       int st;
       
       double weight;
       struct state *next1;
       struct state *next0;
       double out1_0;               //given 1 is transmitted,the first bit of the output
       double out1_1;               //given 1 is transmitted,the second bit of the output
       double out0_0;               //given 0 is transmitted,the first bit of the output
       double out0_1;               //given 0 is transmitted,the second bit of the output
       };

struct state node[path+1][st_num];

void *output(int *,double,int *);                       //main function,producing the decoded sequence
void nodeinit(void);                                 //sub function,initialize the trellis diagram
void nodereset(double *,int);                                //sub function,reset some parts of the states
void nodeupdate(double *,int);                  //sub function,calculate the weights and optimal path
void decode(int *,int);                  //sub function,construct the decoded sequence using  optimal path

void *output(int *x_c,double snr,int *r_d){
    int i,j=0;
    double A,n0,Y,*r,temp;
    r=(double *) malloc((2*(k+m))*sizeof(double));
    //r_d=(int *) malloc((k+m)*sizeof(int));
    
    //a=(int)((k+m)/k_m);
    //b=(k+m)%k_m;
    
    while (j<(2*(k+m))){
              
              if (x_c[j]==1) A=0.1;
              else A=-0.1 ;
              n0=pow(10,-5)*2/pow(10,snr/10);
              Y=gaurand()*sqrt(n0/(2*0.001))+A;  //Y is the matched filter output
              //Y=A;
              r[j]=Y;                              //r is the received vector
              
              j++;
              }
    
    
    
    /*-----------------decoding--------------------*/ 
    
    nodeinit();
    
    for (i=0;i<k_m-path+1;i++){
       
       if ( i==0 ) {
            node[0][0].weight=0;
            nodeupdate(r,i);
            }
       
       else{
            nodereset(r,i);  
            //node[0][final_state].weight=0;
            
            }
       
       
       
       
       //printf("state = %d\n",final_state);
       
       decode(r_d,i);
        
       //printf("state = %d\n",final_state);
       //system("pause");
       }
    
    
    free(r);
    free(node);
    //return r_d;
   
    }


void nodeinit(void){
     int x,i,j,h,temp,s[6],t1,t2,t3,t4;

     for (i=0;i<path+1;i++){
            for (j=0;j<64;j++){
                x=j<<1;
                if (x>63) x=x-64;
                
                node[i][j].weight=-1;
                node[i][j].in=-1;
                node[i][j].from = NULL;
                //node[i][j].ti=i;
                node[i][j].st=j;
                
                temp=j;
                
                for(h=m-1;h>=0;h--){                //derive current state in binary form
                      s[h]=temp/(int)pow(2,h);
                      if(h!=0) temp=temp%(int)pow(2,h);
                      }
                
                if(i!=path){
                node[i][j].next1=&node[i+1][x+1];     
                node[i][j].next0=&node[i+1][x];
                }
                else{
                node[i][j].next1=NULL;     
                node[i][j].next0=NULL;
                }
                
                t1=1^s[1]^s[2]^s[4]^s[5];
                if(t1 == 1) node[i][j].out1_0 = 0.1;
                else node[i][j].out1_0 = -0.1;
                
                t2=1^s[0]^s[1]^s[2]^s[5];
                if(t2 == 1) node[i][j].out1_1 = 0.1;
                else node[i][j].out1_1 = -0.1;
                
                t3=0^s[1]^s[2]^s[4]^s[5];
                if(t3 == 1) node[i][j].out0_0 = 0.1;
                else node[i][j].out0_0 = -0.1;
                
                t4=0^s[0]^s[1]^s[2]^s[5];
                if(t4 == 1) node[i][j].out0_1 = 0.1;
                else node[i][j].out0_1 = -0.1;
                }
            }
     
     }




void nodeupdate(double *r,int l){
     int i,j,x;
     double w1,w0;
     
     x=path;
     
     for (i=0;i<x;i++){
        for(j=0;j<st_num;j++){
             if (node[i][j].weight!=-1)
             {
                                  
              w1 = node[i][j].out1_0*r[2*l+2*i] + node[i][j].out1_1*r[2*l+2*i+1];
              w0 = node[i][j].out0_0*r[2*l+2*i] + node[i][j].out0_1*r[2*l+2*i+1];
             
               if (( node[i][j].next1->weight == -1 || (node[i][j].weight + w1) > node[i][j].next1->weight) && (l+i < k)){
              
               node[i][j].next1->from=&node[i][j]; 
               node[i][j].next1->in=1;
               node[i][j].next1->weight= node[i][j].weight +  w1;

               }                       
               
               if (node[i][j].next0->weight == -1 || (node[i][j].weight + w0) > node[i][j].next0->weight){
            
               node[i][j].next0->from=&node[i][j]; 
               node[i][j].next0->in=0;                        
               node[i][j].next0->weight= node[i][j].weight +  w0;  
             
               }
             }
            }
            }
      }

void nodereset(double *r,int l){
     int i,j,temp;
     double w1,w0;
     /*for (j=0;j<st_num;j++){
            node[0][j].weight=0;
            node[0][j].in=-1;
            node[0][j].from = NULL;
            }*/
     
     for (i=0;i<path;i++){
            for (j=0;j<st_num;j++){
            
            node[i][j].weight=node[i+1][j].weight;
            node[i][j].in=node[i+1][j].in;
            
            if(node[i+1][j].from!=NULL){
            temp=node[i+1][j].from->st;
            node[i][j].from = &node[i-1][temp];}
            
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
         if (node[path-1][j].weight!=-1)
             {
                                  
              w1 = node[path-1][j].out1_0*r[2*l+2*(path-1)] + node[path-1][j].out1_1*r[2*l+2*(path-1)+1];
              w0 = node[path-1][j].out0_0*r[2*l+2*(path-1)] + node[path-1][j].out0_1*r[2*l+2*(path-1)+1];
             
              //if(l==6)printf("%f %f\n",w1,w0);
             
               if (( node[path-1][j].next1->weight == -1 || (node[path-1][j].weight + w1) > node[path-1][j].next1->weight) && (l+(path-1) < k)){
              
               node[path-1][j].next1->from=&node[path-1][j]; 
               node[path-1][j].next1->in=1;
               node[path-1][j].next1->weight = node[path-1][j].weight +  w1;

               }                       
               
               if (node[path-1][j].next0->weight == -1 || (node[path-1][j].weight + w0) > node[path-1][j].next0->weight){
            
               node[path-1][j].next0->from=&node[path-1][j]; 
               node[path-1][j].next0->in=0;                        
               node[path-1][j].next0->weight = node[path-1][j].weight +  w0;  
             
               }
             }
         
         
         }
     
     
     
     
     }



void decode(int *r_d,int i){
     
     struct state *temp1;
     int j,x,h,c0=0,c1=0,next_st;
     x=path;
     
     //printf("state = %d\n",f_s);
     
     /*next_st=f_s<<1;
     if (next_st>63) next_st=next_st-64;*/

     /*if(i==6){
              for(j=0;j<64;j++){
                                printf("%f\n",node[x][j].weight);
                                }
              }*/



     if(i!=k_m-path){
          
          for(j=0;j<64;j++){
             if (node[x][j].weight!=-1){
                temp1=&node[x][j];
                //if(i==6) system("pause");
                for(h=0;h<path-1;h++){
                    temp1=temp1->from;                  
                    }
                
                //printf("temp1 %d\n",temp1->from->st);
                if(temp1->in==1) c1++;
                else c0++;
              }
             
             }
          
          /*printf("i = %d\n",i);   
          printf("c1 = %d\n",c1);
          printf("c0 = %d\n",c0);
          system("pause");*/
          
          if (c1>c0){
               r_d[i]=1;
               //return next_st+1;
               }
          else if (c1<c0){
               r_d[i]=0;
               //return next_st;
               }
          else {
               r_d[i]=rand()%2;
               //return next_st+r_d[i];
               }
          
          }
     
     
     else{
          temp1=&node[x][0];
          for (j=x-1;j>=0;j--){
            r_d[j+i]=temp1->in;
            //r_d[j+i]=8;
            //printf("position = %d\n",j+i);
            temp1=temp1->from;
            
            }
            //return -1;
          }
     
     
}
