float unirand(void)                      //uniform (0,1] distribution
{
       return float(rand())/float(RAND_MAX); 
}

float gaurand(void)                      //  N(0,1) distribution
{
       float u1,u2,v1,v2,x1,s=2;
       
       while (s>=1){
             u1=unirand();
             u2=unirand();
             v1=2*u1-1;
             v2=2*u2-1;
             s=v1*v1 + v2*v2;
       };
       x1=v1*sqrt((-2*log(s))/s);
       return x1;
}

float rayrand(void){                      //rayleigh
       float u;

       u=sqrt(pow(gaurand(),2)+pow(gaurand(),2))/sqrt(2);

       return u;
       }


void rand_permute(int len,int *a){
     int i,j,x;
     int temp[len];
     
     for (i=0;i<len;i++){
         temp[i]=i;
     }
     
     for (i=0;i<len;i++){
         x=(rand()%(len-i));
         
         a[i]=temp[x];
         
         for (j=x;j<len-1-i;j++){
             temp[j]=temp[j+1];
             
         }
     }
}
