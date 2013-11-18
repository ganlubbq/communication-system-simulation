/* Modify the normal r.v. generator
 * Date : 2007/4/14
 */

//Returns an uniform [0,1] r.v.
float unirand(void)
{
      return (float)rand()/RAND_MAX; 
}

//Returns a Normal r.v. with mean = 0, standard deviation = 1
float gaurand(void)
{
      float v1,v2;
      float w;
      float y1;
      static float y2;
      static int use_last=0;
      
      if (use_last == 0){
         do {
             v1=2*unirand()-1;
             v2=2*unirand()-1;
             w=v1*v1+v2*v2;
         }while (w>=1);
         
         w=sqrt((-2*log(w))/w);
         
         y1=v1*w;
         y2=v2*w;
         
         use_last=1;
      }
      
      else {
           y1=y2;
           
           use_last=0;
      }
      
      return y1;
}

//Returns a rayleigh r.v.
float rayrand(void){
      float u;

      u=sqrt(pow(gaurand(),2)+pow(gaurand(),2))/sqrt(2);

      return u;
}
