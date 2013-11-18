#define m 4                                //memory order = 4
#define st_num 16
#define k_m (k+m)

struct state{
       struct state *from;
       int in;
       };

struct state node[k+m+1][st_num];

int next_table[16][2];
int out_table_index[16][2];
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
     int x,j,h,temp,s[4],t1,t2,t3,t4;
     for (j=0;j<16;j++){
         /*x=j<<1;
         if (x>15) x=x-16;
         next_table[j][0] = x;              //table for the next states
         next_table[j][1] = x+1;*/
         
         temp=j;
         for(h=m-1;h>=0;h--){ //derive current state in binary form
             s[h]=temp/(int)pow(2,h);
             if(h!=0) temp=temp%(int)pow(2,h);
             }
        
        next_table[j][0]=(0^s[0]^s[1]^s[3])+2*s[0]+4*s[1]+8*s[2];
        next_table[j][1]=(1^s[0]^s[1]^s[3])+2*s[0]+4*s[1]+8*s[2];
            
        /*t1=0^s[0]^s[1]^s[3];
        t2=0^s[2]^s[3];
        t3=1^s[0]^s[1]^s[3];   //table for the path outputs
        t4=1^s[2]^s[3];*/
        
        t1=0;
        t2=(0^s[0]^s[1]^s[3])^s[2]^s[3];
        t3=1;   //table for the path outputs
        t4=(1^s[0]^s[1]^s[3])^s[2]^s[3];
        
        
        out_table_index[j][0]=2*t1+t2;
        out_table_index[j][1]=2*t3+t4;
        
        }
     

}
