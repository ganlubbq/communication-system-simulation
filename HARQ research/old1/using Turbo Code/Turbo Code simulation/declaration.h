struct complex{
       float re;
       float im;
       };

int g11[m+1],g12[m+1];                       //generator sequence

int next_table[st_num][2];                   //next states correspond to input 0, 1
int out_table[st_num][4];                    //output bits correspond to input 0, 1
int from_table[st_num][2];                   //previous states correspond to incoming 0, 1
int from_out_table[st_num][4];

float ray[k+m];                             //record or the rayleigh r.v.
float x_metric[k+m];
float LLR[k+m];
float z[k+m]={};
float lookup_table[100000]={};              //the real delta = index/1000

int record[k];                               //record of interleaving order

void table(void);
void lookup(void);

void initialize(){
     
     table(); 
     lookup();
     }

void lookup(void){
     int i;
     for (i=0;i<100000;i++){
         
         lookup_table[i]=exp(-(float)i/1000);
         
         }
     }

void table(void){
     int x,i,j,h,s[m],t0,t1;
     
     for (h=0;h<m+1;h++){
         g11[h]=G11>>(m-h)&1;
         g12[h]=G12>>(m-h)&1;
     }
     
     for (j=0;j<st_num;j++){

         for (h=0;h<m;h++){//s[] stores the content of shift registers
            s[h]=j>>h&1;
         }
        
        
         t0=0;
         t1=0;
         for (h=0;h<m;h++){
             t0=t0^s[h]*g11[h+1];
             t1=t1^s[h]*g12[h+1];
         }
         next_table[j][0]=(j<<1& st_num-1) + (t0^0);
         next_table[j][1]=(j<<1& st_num-1) + (t0^1);  
         
         //printf("%d ",next_table[j][0]);
         //printf("%d \n",next_table[j][1]);
         
         out_table[j][0]=0;
         out_table[j][1]=(t0^0)^t1;
         out_table[j][2]=1;
         out_table[j][3]=(t0^1)^t1;
        
     }
     
    //construct "from" tables
    for (i=0;i<st_num;i++){
        x=0;
        for (j=0;j<st_num;j++){
        
            for (h=0;h<2;h++){
        
                if (next_table[j][h]==i){
                                     
                   from_table[i][x]=j;
                        
                   from_out_table[i][0+2*x]=out_table[j][0+2*h];
                   from_out_table[i][1+2*x]=out_table[j][1+2*h];
                   
                   x++;  
                }
            }
        }
    }
}

/*float logsum(float a,float b){
       float max,delta;
       max=a;
       if (b>max) max=b;
       delta=a-b;
       if (delta<0) delta=0-delta;
       if (delta>100)delta=100;
       delta=log(1+exp(-delta));
       return max+delta;
       }*/
       

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

