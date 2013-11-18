void modulation(int *,struct complex *);

void modulation(int *c,struct complex *r){
     int i,j,temp;
     
     /*for(i=0;i<8;i++){
           mapping[i].re= cos(i*(pi/4));
           mapping[i].im= sin(i*(pi/4));
           }*/
     
     
    /*  mapping[0].re= cos(0*(pi/4));            //Gray
      mapping[0].im= sin(0*(pi/4));
     
      mapping[1].re= cos(1*(pi/4));
      mapping[1].im= sin(1*(pi/4));
     
      mapping[2].re= cos(3*(pi/4));
      mapping[2].im= sin(3*(pi/4));

      mapping[3].re= cos(2*(pi/4));
      mapping[3].im= sin(2*(pi/4));
      
      mapping[4].re= cos(7*(pi/4));
      mapping[4].im= sin(7*(pi/4));
      
      mapping[5].re= cos(6*(pi/4));
      mapping[5].im= sin(6*(pi/4));
      
      mapping[6].re= cos(4*(pi/4));
      mapping[6].im= sin(4*(pi/4));
      
      mapping[7].re= cos(5*(pi/4));
      mapping[7].im= sin(5*(pi/4));*/
      
      mapping[0].re= cos(0*(pi/4));            //SP
      mapping[0].im= sin(0*(pi/4));
     
      mapping[1].re= cos(4*(pi/4));
      mapping[1].im= sin(4*(pi/4));
     
      mapping[2].re= cos(2*(pi/4));
      mapping[2].im= sin(2*(pi/4));

      mapping[3].re= cos(6*(pi/4));
      mapping[3].im= sin(6*(pi/4));
      
      mapping[4].re= cos(1*(pi/4));
      mapping[4].im= sin(1*(pi/4));
      
      mapping[5].re= cos(5*(pi/4));
      mapping[5].im= sin(5*(pi/4));
      
      mapping[6].re= cos(3*(pi/4));
      mapping[6].im= sin(3*(pi/4));
      
      mapping[7].re= cos(7*(pi/4));
      mapping[7].im= sin(7*(pi/4));
      
      /*mapping[0].re= cos(0*(pi/4));            //Mixed
      mapping[0].im= sin(0*(pi/4));
     
      mapping[1].re= cos(1*(pi/4));
      mapping[1].im= sin(1*(pi/4));
     
      mapping[2].re= cos(2*(pi/4));
      mapping[2].im= sin(2*(pi/4));

      mapping[3].re= cos(3*(pi/4));
      mapping[3].im= sin(3*(pi/4));
      
      mapping[4].re= cos(6*(pi/4));
      mapping[4].im= sin(6*(pi/4));
      
      mapping[5].re= cos(7*(pi/4));
      mapping[5].im= sin(7*(pi/4));
      
      mapping[6].re= cos(4*(pi/4));
      mapping[6].im= sin(4*(pi/4));
      
      mapping[7].re= cos(5*(pi/4));
      mapping[7].im= sin(5*(pi/4));*/
     
     for(i=0;i<(k+m)/2;i++){
           temp=4*c[3*i] + 2*c[3*i+1] + c[3*i+2];
           r[i] = mapping[temp]; 
           }     
      
   
     }
