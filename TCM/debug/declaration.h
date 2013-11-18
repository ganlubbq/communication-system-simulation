#define m 12                                //memory order = 6
#define st_num 64
#define k_m (k+m)
#define pi 3.1415926

struct state{
       struct state *from;
       int in[2];
       };
            
struct complex{
       float re;
       float im;
       };

struct state node[k_m/2+1][st_num];

struct complex mapping[8];

int next_table[64][2];             //table for the 2 next states corresponding to 0,1
int out_table[64][4];              //table for the 4 output bits,2 bits each

float sym_metric[8][(k+m)/2];

int *u,*v,*y;

void declaration(void);
void table(void);

void declaration(){
     
     table();
     
     u=(int *) malloc((k+m)*sizeof(int));
     v=(int *) malloc((k+m)/2*3*sizeof(int));
     y=(int *) malloc((k+m)*sizeof(int));
     
     }
     

void table(void){
     int x,i,j,h,temp,s[6];
    
     for (j=0;j<64;j++){
         x=j<<1;
         if (x>63) x=x-64;
         next_table[j][0] = x;              //table for the next states
         next_table[j][1] = x+1;
         
         temp=j;
         
         for(h=6-1;h>=0;h--){ //derive current state in binary form
             s[h]=temp/(int)pow(2,h);
             if(h!=0) temp=temp%(int)pow(2,h);
             }
         
          
         
         
         out_table[j][0]=0^s[1]^s[2]^s[4]^s[5];   //table for the path outputs
         out_table[j][1]=0^s[0]^s[1]^s[2]^s[5];
         out_table[j][2]=1^s[1]^s[2]^s[4]^s[5];
         out_table[j][3]=1^s[0]^s[1]^s[2]^s[5];
         
         
         }
     
   
     }
