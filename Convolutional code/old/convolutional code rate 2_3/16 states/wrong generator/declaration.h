struct state{
       struct state *from;
       int in[2];
       };
       
struct state node[(k+m)/2+1][st_num];

int next_table[st_num][4];
float out_table[st_num][12];

int *u,*u1,*u2,*v,*v1,*v2,*v3,*y;

void declaration(void);
void table(void);

void declaration(){

     table();
   
     u=(int *) malloc((k+m)*sizeof(int));
    u1=(int *) malloc((k+m)/2*sizeof(int));
    u2=(int *) malloc((k+m)/2*sizeof(int));
     v=(int *) malloc((k+m)/2*3*sizeof(int));
    v1=(int *) malloc(((k+m))/2*sizeof(int));
    v2=(int *) malloc(((k+m))/2*sizeof(int));
    v3=(int *) malloc(((k+m))/2*sizeof(int));
     y=(int *) malloc((k+m)*sizeof(int));
     
     }


void table(void){
     int x,i,j,h,temp,s[m],t[12];
     
     for (j=0;j<st_num;j++){
         
         temp=j;
         for(h=m-1;h>=0;h--){ //derive current state in binary form
             s[h]=temp/(int)pow(2,h);//leftmost = 2^0
             if(h!=0) temp=temp%(int)pow(2,h);
             }
         
          
          next_table[j][0]=2*s[0]+8*s[2];      //table for the next states
          next_table[j][1]=2*s[0]+4+8*s[2];
          next_table[j][2]=1+2*s[0]+8*s[2];
          next_table[j][3]=1+2*s[0]+4+8*s[2];
          
         /* for(i=0;i<4;i++){
              printf("%d ",next_table[j][i]);
              }
           printf("\n");
           system("pause");*/
          
          t[0]=s[0]^s[1]^s[2];   //table for the path outputs
          t[1]=s[3];
          t[2]=s[2]^s[3];
                 
          t[3]=s[0]^s[1]^s[2]; 
          t[4]=1^s[3];
          t[5]=1^s[2]^s[3];
          
          t[6]=1^s[0]^s[1]^s[2]; 
          t[7]=1^s[3];
          t[8]=1^s[2]^s[3];
         
          t[9]=1^s[0]^s[1]^s[2];
         t[10]=s[3];
         t[11]=s[2]^s[3];
         
         
          for(i=0;i<12;i++){
              if(t[i] == 1) out_table[j][i] = 1;
              else out_table[j][i] = -1;
              }        
         }
     }
