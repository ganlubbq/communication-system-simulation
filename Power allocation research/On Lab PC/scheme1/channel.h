//#define k1 1.1988
#define k2 1.0//0.36
#define k3 1.0//1.064

//float k3=5.0/2.0 -k1 -k2/2.0;
float k1=5.0/2.0 -k3 -k2/2.0;


float _k1=sqrt(k1);
float _k2=sqrt(k2);
float _k3=sqrt(k3);
     

void awgn(int *v,float snr,float *r){
     int j=0;
     float N0,sd;
     
//     printf("%f\n",k1);
//     system("pause");
     
     N0=(float)(k+h)/k/snr;                            //SNR = Eb*2/N0 , Eb=1
     sd=sqrt(N0/2);                                //standard deviation of gaussian r.v.
   
     while (j<(k+h)){
    
           if (j==2 || j==3)r[j]=2.0*_k2*v[j]-_k2;   //bpsk modulation
           else if (j > 5) r[j]=2.0*_k3*v[j]-_k3;
           else r[j]=2.0*_k1*v[j]-_k1;
           
           //r[j]=2.0*v[j]-1;
           
           r[j]+=gaurand()*sd;                     //add noise
              
           j++;
     }
   
}
