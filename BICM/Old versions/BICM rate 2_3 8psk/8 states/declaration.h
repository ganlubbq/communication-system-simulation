#define m 4                               //memory order = 3
#define st_num 16
#define pi 3.1415926

struct state{
       struct state *from;
       int in[2];
       //float weight;
       };
            
struct complex{
       float re;
       float im;
       };

struct state node[(k+m)/2+1][st_num];

struct complex mapping[8];

int next_table[st_num][4];
int out_table[st_num][12];
float ray[(k+m)/2]; //record or the rayleigh r.v.
float bit_metric[2][(k+m)/2*3];

int *u,*v;

int record1[(k+m)/2];
int record2[(k+m)/2];
int record3[(k+m)/2];

void declaration(void);
void table(void);

void declaration(){
     
     table(); 
     
     rand_permute((k+m)/2,record1);
     rand_permute((k+m)/2,record2);
     rand_permute((k+m)/2,record3);
     
     u=(int *) malloc((k+m)*sizeof(int));
     v=(int *) malloc((k+m)/2*3*sizeof(int));
     }

void table(void){
     int x,i,j,h,temp,s[3],t[12];
     
     for (j=0;j<st_num;j++){
         
         temp=j;
         for(h=m-1;h>=0;h--){ //derive current state in binary form
             s[h]=temp/(int)pow(2,h);
             if(h!=0) temp=temp%(int)pow(2,h);
             }        
          
          next_table[j][0]=4*s[1];      //table for the next states
          next_table[j][1]=2+4*s[1];
          next_table[j][2]=1+4*s[1];
          next_table[j][3]=1+2+4*s[1];
          
          t[0]=s[2];   //table for the path outputs
          t[1]=s[0];
          t[2]=s[0]^s[1]^s[2];
                 
          t[3]=s[2]; 
          t[4]=1^s[0];
          t[5]=1^s[0]^s[1]^s[2];
          
          t[6]=1^s[2];
          t[7]=s[0];
          t[8]=1^s[0]^s[1]^s[2];
         
          t[9]=1^s[2];
          t[10]=1^s[0];
          t[11]=s[0]^s[1]^s[2];
         
         
          for(i=0;i<12;i++){
              out_table[j][i] = t[i];
              
              }        
         }
     }
