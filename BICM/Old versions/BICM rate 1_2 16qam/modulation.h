void modulation(int *,struct complex *);

void modulation(int *v,struct complex *r){
     int i,j,temp;
     

      mapping[0].re= -3;            //Gray
      mapping[0].im= 3;
     
      mapping[1].re= -3;
      mapping[1].im= 1;
     
      mapping[2].re= -3;
      mapping[2].im= -3;

      mapping[3].re= -3;
      mapping[3].im= -1;
      
      mapping[4].re= -1;
      mapping[4].im= 3;
      
      mapping[5].re= -1;
      mapping[5].im= 1;
      
      mapping[6].re= -1;
      mapping[6].im= -3;
      
      mapping[7].re= -1;
      mapping[7].im= -1;
     
      mapping[8].re= 3;
      mapping[8].im= 3;
     
      mapping[9].re= 3;
      mapping[9].im= 1;
     
      mapping[10].re= 3;
      mapping[10].im= -3;

      mapping[11].re= 3;
      mapping[11].im= -1;
      
      mapping[12].re= 1;
      mapping[12].im= 3;
      
      mapping[13].re= 1;
      mapping[13].im= 1;
      
      mapping[14].re= 1;
      mapping[14].im= -3;
      
      mapping[15].re= 1;
      mapping[15].im= -1;
     
     for(i=0;i<(k+m)/2;i++){
           temp=8*v[z*i] + 4*v[z*i+1] + 2*v[z*i+2] + v[z*i+3];
           /*printf("%d\n",temp);
           system("pause");*/
           r[i] = mapping[temp]; 
           }     
      
   
     }
