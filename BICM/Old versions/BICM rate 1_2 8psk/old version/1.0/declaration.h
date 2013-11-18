struct state{
       struct state *from;
       int in;
       };
            
struct complex{
       float re;
       float im;
       };

struct state node[k+m+1][st_num];

struct complex mapping[8];

int next_table[64][2];             //table for the 2 next states corresponding to 0,1
int out_table[64][4];              //table for the 4 output bits,2 bits each
int path[64][8][3];                //record of the 3 states in one of the 8 paths
int path_out[64][8][6];            //record of the 6 output bits during one of the 8 paths
int h_table[8][3];                 //binary table for the number "8",leftmost bit=2^2

/*bit based metric ; "bit_metric?[0][]" is the metric of 0*/
float bit_metric1[2][(k+m)*2/3],bit_metric2[2][(k+m)*2/3],bit_metric3[2][(k+m)*2/3];
//float bit_metric[3][2][(k+m)*2/3];


int *u,*u1,*u2,*v,*v1,*v2,*v3,*y;

void declaration(void);
void table(void);

void declaration(){
     
     table();
     
     u=(int *) malloc((k+m)*sizeof(int));
    u1=(int *) malloc((k+m)/2*sizeof(int));
    u2=(int *) malloc((k+m)/2*sizeof(int));
     v=(int *) malloc((k+m)*2*sizeof(int));
    v1=(int *) malloc(((k+m))*2/3*sizeof(int));
    v2=(int *) malloc(((k+m))*2/3*sizeof(int));
    v3=(int *) malloc(((k+m))*2/3*sizeof(int));
     y=(int *) malloc((k+m)*sizeof(int));
     
     }
     

void table(void){
     int x,i,j,h,temp,s[6],t1,t2,t3,t4;
    
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
     
    
     for (i=0;i<64;i++){
         for (j=0;j<8;j++){
             
             path[i][j][0]=next_table[i][h_table[j][0]];
             path[i][j][1]=next_table[path[i][j][0]][h_table[j][1]];
             path[i][j][2]=next_table[path[i][j][1]][h_table[j][2]];
             }
         }
         
         
     for (i=0;i<64;i++){
         for (j=0;j<8;j++){
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
             
             if(h_table[j][2]==0){
             path_out[i][j][4]= out_table[path[i][j][1]][0];
             path_out[i][j][5]= out_table[path[i][j][1]][1];
             }
             else{
             path_out[i][j][4]= out_table[path[i][j][1]][2];
             path_out[i][j][5]= out_table[path[i][j][1]][3];
             }
             }
         }
    
     }
