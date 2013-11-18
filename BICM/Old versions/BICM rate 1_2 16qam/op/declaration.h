#define z 4
#define M (int)pow(2,z)

#define m 6                                //memory order = 6
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

struct complex mapping[M];

int next_table[64][2];             //table for the 2 next states corresponding to 0,1
int out_table[64][4];              //table for the 4 output bits,2 bits each
int path[64][4][2];                //record of the 2 states in one of the 4 paths
int path_out[64][4][4];            //record of the 4 output bits during one of the 4 paths
int h_table[4][2];                 //binary table for the number "4",leftmost bit=2^2
int M_table[16][4];
double ray[(k+m)*2/z];


//double bit_metric[4][2][(k+m)/2]; //bit_metric[bit position 0 1 2][0 or 1][symbol position]
double bit_metric[2][(k+m)*2];

int u[k+m],v[(k+m)*2],y[k+m];

int record1[(k+m)/2];
int record2[(k+m)/2];
int record3[(k+m)/2];
int record4[(k+m)/2];

void declaration(void);
void table(void);

void declaration(){
     
     table();

     rand_permute((k+m)/2,record1);
     rand_permute((k+m)/2,record2);
     rand_permute((k+m)/2,record3);
     rand_permute((k+m)/2,record4);
     }
     

void table(void){
     int x,i,j,h,temp,s[6],t[z];
    
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
         
         out_table[j][0]=0^s[1]^s[2]^s[4]^s[5];   //table for the path outputs
         out_table[j][1]=0^s[0]^s[1]^s[2]^s[5];
         out_table[j][2]=1^s[1]^s[2]^s[4]^s[5];
         out_table[j][3]=1^s[0]^s[1]^s[2]^s[5];
         }
     
     
     for (j=0;j<4;j++){
             temp=j;
             t[0]=(int)temp/2;
             temp=temp%2;
             t[1]=temp;
             
          
          for (h=0;h<2;h++){
              h_table[j][h]=t[h];
              }
        
          }


   
     
     for (j=0;j<16;j++){
             temp=j;
             
             t[0]=(int)temp/8;
             temp=temp%8;
             t[1]=(int)temp/4;
             temp=temp%4;
             t[2]=(int)temp/2;
             temp=temp%2;
             t[3]=temp;
             
          
          for (h=0;h<4;h++){
              M_table[j][h]=t[h];
              }
          }
 
 
     /*printf("%d\n",M_table[9][0]);
     printf("%d\n",M_table[9][1]);
     printf("%d\n",M_table[9][2]);
     printf("%d\n",M_table[9][3]);
    
     system("pause"); */    
 
    
     for (i=0;i<64;i++){
         for (j=0;j<4;j++){
             
             path[i][j][0]=next_table[i][h_table[j][0]];
             path[i][j][1]=next_table[path[i][j][0]][h_table[j][1]];
             //path[i][j][2]=next_table[path[i][j][1]][h_table[j][2]];
             }
         }
         
         
     for (i=0;i<64;i++){
         for (j=0;j<4;j++){
             if(h_table[j][0]==0){
             path_out[i][j][0]= out_table[i][0];
             path_out[i][j][1]= out_table[i][1];
             }
             else{
             path_out[i][j][0]= out_table[i][2];
             path_out[i][j][1]= out_table[i][3];
             }
             
             if(h_table[j][1]==0){
             path_out[i][j][2]= out_table[path[i][j][0]][0];
             path_out[i][j][3]= out_table[path[i][j][0]][1];
             }
             else{
             path_out[i][j][2]= out_table[path[i][j][0]][2];
             path_out[i][j][3]= out_table[path[i][j][0]][3];
             }
             }
         }
    
     }
double logsum(double a,double b){
       double max,delta;
       max=a;
       if (b>max) max=b;
       delta=a-b;
       if (delta<0) delta=0-delta;
       if (delta>100) {
       //printf("logsum overflow\n");
       //system("pause");
       delta=100;
       }
       delta=log(1+exp(-delta));
       return max+delta;
       }
