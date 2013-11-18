#define k1 0.6825
#define k2 0.9177
#define k3 1.096
#define k4 1.232
#define k5 0.8447

/*
#define k1 1.0
#define k2 1.0
#define k3 1.0
#define k4 1.0
#define k5 1.0*/

float k6=( 15.0-(3.0*k4+4.0*k3+2.0*k2+1.0*k1 + 2.0*k5) ) / 3.0;

float _k1=sqrt(k1);
float _k2=sqrt(k2);
float _k3=sqrt(k3);
float _k4=sqrt(k4);     
float _k5=sqrt(k5);
float _k6=sqrt(k6);

float ray[k+h]; //record or the rayleigh r.v.

void awgn(int *v,float snr,float *r){
     int j=0;
     float N0,sd;

     //printf("%f\n",k6);
     //system("pause");
     
     N0=(float)(k+h)/k/snr;                        //SNR = Eb*2/N0 , Eb=1
     
     sd=sqrt(N0/2.0);                                //standard deviation of gaussian r.v.
   
     r[0]=2.0*_k4*v[0]-_k4;
     r[1]=2.0*_k3*v[1]-_k3;
     r[2]=2.0*_k2*v[2]-_k2;
     r[3]=2.0*_k3*v[3]-_k3;
     r[4]=2.0*_k4*v[4]-_k4;
     r[5]=2.0*_k4*v[5]-_k4;
     r[6]=2.0*_k3*v[6]-_k3;
     r[7]=2.0*_k1*v[7]-_k1;
     r[8]=2.0*_k3*v[8]-_k3;
     r[9]=2.0*_k2*v[9]-_k2;
     r[10]=2.0*_k6*v[10]-_k6;
     r[11]=2.0*_k6*v[11]-_k6;
     r[12]=2.0*_k5*v[12]-_k5;
     r[13]=2.0*_k6*v[13]-_k6;
     r[14]=2.0*_k5*v[14]-_k5;
   
     while (j<(k+h)){

       //    r[j]=2.0*v[j]-1.0;
           
           ray[j] = rayrand();
           
           r[j] *= ray[j];
           
           r[j] += gaurand()*sd;                     //add noise
              
           j++;
     }
     
     
   
}
