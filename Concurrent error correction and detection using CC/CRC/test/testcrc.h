void crc(int *input,int *fcs){
     int i;
     int temp[14],temp1;
     
     for(i=0;i<14;i++){
     if(i<10) temp[i]=input[i];
     else temp[i]=fcs[i-10];
     }

     for(i=0;i<4;i++){
     fcs[i]=0;
     }

     for(i=0;i<14;i++){
     temp1=fcs[0];
     fcs[0]=fcs[1];
     fcs[1]=fcs[2];
     fcs[2]=fcs[3]^temp1;
     fcs[3]=temp[i]^temp1;
     
     }
     
     
     
     }
