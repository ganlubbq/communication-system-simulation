#define m 4                                //memory order = 4
#define st_num (int)pow(2,m)
#define k_m (k+m)
#define pi 3.1415926

struct state{
       struct state *from;
       int in;
       };
            
struct complex{
       double re;
       double im;
       };

struct state node[k+m+1][st_num];

struct complex mapping[8];

int next_table[st_num][2];             //table for the 2 next states corresponding to 0,1
int out_table[st_num][4];              //table for the 4 output bits,2 bits each
int h_table[8][3];                 //binary table for the number "8",leftmost bit=2^2
double ray[(k+m)*2/3]; //record or the rayleigh r.v.

double llr[(k+m)*2]; //log(P1/P0)

int u[k+m],v[(k+m)*2],y[k+m];

void declaration(void);
void table(void);

void declaration(){
     
     table();
     
     }
     

void table(void){
     int x,i,j,h,temp,s[m],t1,t2,t3,t4;
    
     for (j=0;j<st_num;j++){
         x=j<<1;
         if (x>st_num-1) x=x-st_num;
         next_table[j][0] = x;              //table for the next states
         next_table[j][1] = x+1;
         
         temp=j;
         
         for(h=m-1;h>=0;h--){ //derive current state in binary form
             s[h]=temp/(int)pow(2,h);
             if(h!=0) temp=temp%(int)pow(2,h);
             }
         
         out_table[j][0]=0^s[0]^s[1]^s[3];   //table for the path outputs
         out_table[j][1]=0^s[2]^s[3];
         out_table[j][2]=1^s[0]^s[1]^s[3];
         out_table[j][3]=1^s[2]^s[3];
         }
     
     
     for (j=0;j<8;j++){
             temp=j;
             t1=(int)temp/4;
             temp=temp%4;
             t2=(int)temp/2;
             temp=temp%2;
             t3=temp;
          h_table[j][0]=t1;
          h_table[j][1]=t2;
          h_table[j][2]=t3;
          }
    
   
     }
