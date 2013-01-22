struct state{
       struct state *from;
       int in;
       };

struct state node[k+m+1][st_num];

int retransmit=0;

int g11[m+1],g12[m+1];                       //generator sequence

int nextstate[st_num][2];                    //table for the next 2 states corresponding to input 0 and 1
int nextoutput[st_num][4];                   //table for the path outputs corresponding to input 0 and 1

int inputbit[st_num];                        //table for the inputs which result in the current state
int prestate[st_num][2];                     //table for the previous 2 states, from small to large
int preoutput[st_num][4];                    //table for the previous path outputs

int buffer[2*(k+m)];

float ray[2*(k+m)]; //record or the rayleigh r.v.

void table(void){
     int x,i,j,h,temp,temp1,temp2,s[m],in;
     
     for (i=0;i<st_num;i++){
         x=i>>1;
         nextstate[i][0] = x;
         nextstate[i][1] = x + (int)pow(2,m-1);
         
         temp=i;
         
         //derive current state in binary form
         for(h=m-1;h>=0;h--){
             s[m-1-h]=temp/(int)pow(2,h);
             if(h!=0) temp=temp%(int)pow(2,h);
             }
            
         nextoutput[i][0]=0*g11[0];
         nextoutput[i][1]=0*g12[0];
         nextoutput[i][2]=1*g11[0];
         nextoutput[i][3]=1*g12[0];
         for (j=0;j<m;j++){
            nextoutput[i][0]=nextoutput[i][0]^s[j]*g11[j+1];
            nextoutput[i][1]=nextoutput[i][1]^s[j]*g12[j+1];
            nextoutput[i][2]=nextoutput[i][2]^s[j]*g11[j+1];
            nextoutput[i][3]=nextoutput[i][3]^s[j]*g12[j+1];
            }
         }
     
     //construct input table
     temp=(int)pow(2,m-1);
     for (i=0;i<st_num;i++){
         if (i>=temp) inputbit[i]=1;
         else inputbit[i]=0;
         //printf("%d\n",i); 
         }
         
     
     for (i=0;i<st_num;i++){
         x=i;
         temp=(int)pow(2,m-1);
         if (x>=temp) x-=temp;
         
         x=x<<1;
         prestate[i][0]=x; 
         prestate[i][1]=x+1; 
         
         in=inputbit[i];
         
         temp=x;
         
         //derive previous state(small) in binary form
         for(h=m-1;h>=0;h--){
             s[m-1-h]=temp/(int)pow(2,h);
             if(h!=0) temp=temp%(int)pow(2,h);
             }
         
         
         preoutput[i][0]=in*g11[0];
         preoutput[i][1]=in*g12[0];
         preoutput[i][2]=in*g11[0];
         preoutput[i][3]=in*g12[0];
         for (j=0;j<m-1;j++){
            preoutput[i][0]=preoutput[i][0]^s[j]*g11[j+1];
            preoutput[i][1]=preoutput[i][1]^s[j]*g12[j+1];
            preoutput[i][2]=preoutput[i][2]^s[j]*g11[j+1];
            preoutput[i][3]=preoutput[i][3]^s[j]*g12[j+1];
            }
         preoutput[i][0]=preoutput[i][0]^0*g11[m];
         preoutput[i][1]=preoutput[i][1]^0*g12[m];
         preoutput[i][2]=preoutput[i][2]^1*g11[m];
         preoutput[i][3]=preoutput[i][3]^1*g12[m];
         }
     
    }

void generator_enumerate(){
     int i,temp1,temp2,check;
     
     temp1=G11;temp2=G12;
     check=(int)pow(2,m+1);
     
     //generator check
     if ( temp1>=check ) temp1-=check;
     if ( temp2>=check ) temp2-=check;
     
     for (i=0;i<m+1;i++){
         g11[i]=temp1/(int)pow(2,m-i);
         temp1=temp1%(int)pow(2,m-i);
         
         g12[i]=temp2/(int)pow(2,m-i);
         temp2=temp2%(int)pow(2,m-i);
         }
     }

void declaration(){
     generator_enumerate();
     
     table();
     }
