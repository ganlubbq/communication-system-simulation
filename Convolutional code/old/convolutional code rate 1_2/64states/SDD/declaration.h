struct state{
       struct state *from;
       int in;
       };


struct state node[k+m+1][st_num];



int next_table[64][2];
//float out_table[64][4];
int out_table_index[64][2];
float path_metric[k+m][4];
float ray[2*k_m]; //record or the rayleigh r.v.


int *u,*v,*y;

void declaration(void);
void table(void);

void declaration(){
     
     table();
     
     u=(int *) malloc(k_m*sizeof(int));
     v=(int *) malloc(k_m*2*sizeof(int));
     y=(int *) malloc(k_m*sizeof(int));
     
     }

void table(void){
     int x,j,h,temp,s[6],t1,t2,t3,t4;
     for (j=0;j<64;j++){
         x=j<<1;
         if (x>63) x=x-64;
         next_table[j][0] = x;              //table for the next states
         next_table[j][1] = x+1;
         
         temp=j;
         
         for(h=m-1;h>=0;h--){ //derive current state in binary form
             s[h]=temp/(int)pow(2,h);
             if(h!=0) temp=temp%(int)pow(2,h);
             }
            
        t1=0^s[1]^s[2]^s[4]^s[5];
        /*if(t1 == 1) out_table[j][0] = 1;
        else out_table[j][0] = -1;*/
                
        t2=0^s[0]^s[1]^s[2]^s[5];
       /* if(t2 == 1) out_table[j][1] = 1;
        else out_table[j][1] = -1;*/
        
        t3=1^s[1]^s[2]^s[4]^s[5];   //table for the path outputs
        /*if(t3 == 1) out_table[j][2] = 1;
        else out_table[j][2] = -1;*/
                
        t4=1^s[0]^s[1]^s[2]^s[5];
        /*if(t4 == 1) out_table[j][3] = 1;
        else out_table[j][3] = -1;*/
        
        
        out_table_index[j][0]=2*t1+t2;
        out_table_index[j][1]=2*t3+t4;
        
        }
     

}
