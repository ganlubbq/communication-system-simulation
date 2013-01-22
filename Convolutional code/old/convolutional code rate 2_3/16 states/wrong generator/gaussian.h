double unirand(void);
double gaurand(void);

double unirand(void)                      //uniform (0,1] distribution
{
return double(rand())/double(RAND_MAX); 
}

double gaurand(void)                      //  N(0,1) distribution
{
double u1,u2,v1,v2;
double s=2;
while (s>=1){
      u1=unirand();
      u2=unirand();
      v1=2*u1-1;
      v2=2*u2-1;
      s=pow(v1,2)+pow(v2,2);
};
double x1=v1*sqrt((-2*log(s))/s);
return x1;
}
