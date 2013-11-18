#define m 4                               //memory order = 4
#define st_num (int)pow(2,m)
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
int h_table[8][3];

int record1[(k+m)/2];
int record2[(k+m)/2];
int record3[(k+m)/2];

float lookup_table[100000]={};//the real delta = index/1000

int u[k+m],v[(k+m)/2*3],y[k+m];
//int y[k+m];

void declaration(void);
void table(void);
void lookup(void);

void declaration(){
     
     table(); 
     lookup();
      
     rand_permute((k+m)/2,record1);
     rand_permute((k+m)/2,record2);
     rand_permute((k+m)/2,record3);   
      
     }

void lookup(void){
     int i;
     for (i=0;i<100000;i++){
         
         lookup_table[i]=exp(-(double)i/1000);
         
         }
     }

void table(void){
     int x,i,j,h,temp,s[m],t[12];
     int t1,t2,t3;
     
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
     
     for (j=0;j<st_num;j++){
         
         temp=j;
         for(h=m-1;h>=0;h--){ //derive current state in binary form
             s[h]=temp/(int)pow(2,h);
             if(h!=0) temp=temp%(int)pow(2,h);
             }        
          
          next_table[j][0]=2*s[0]+8*s[2];      //table for the next states
          next_table[j][1]=2*s[0]+4+8*s[2];
          next_table[j][2]=1+2*s[0]+8*s[2];
          next_table[j][3]=1+2*s[0]+4+8*s[2];
          
          t[0]=s[0]^s[1]^s[2];   //table for the path outputs
          t[1]=s[1]^s[3];
          t[2]=s[2]^s[3];
                 
          t[3]=s[0]^s[1]^s[2]; 
          t[4]=1^s[1]^s[3];
          t[5]=1^s[2]^s[3];
          
          t[6]=1^s[0]^s[1]^s[2]; 
          t[7]=s[1]^s[3];
          t[8]=1^s[2]^s[3];
         
          t[9]=1^s[0]^s[1]^s[2];
          t[10]=1^s[1]^s[3];
          t[11]=s[2]^s[3];
         
         
          for(i=0;i<12;i++){
              out_table[j][i] = t[i];
              
              }        
         }
     }

float logsum(float a,float b){    //aided by table look ups
       float max,delta;
       max=a;
       if (b>max) max=b;
       delta=a-b;
       if (delta<0) delta=0-delta;
       if (delta>100) delta=99.999;
       delta=log(1+lookup_table[int(delta*1000)]);
       return max+delta;
       }
