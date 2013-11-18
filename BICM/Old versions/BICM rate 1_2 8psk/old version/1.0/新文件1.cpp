void bit_metric_comput(struct complex *r){     //compute the bit based metrics
     int i,j;
     float temp[8];
     
     for (i=0;i<(k+m)*2/3;i++){
         
         for (j=0;j<8;j++){//the usual 8 symbol metric
             temp[j]=mapping[j].re*r[i].re + mapping[j].im*r[i].im;
             }
         
         bit_metric1[0][i]=max(temp[0],temp[2],temp[6],temp[4]);
         bit_metric1[1][i]=max(temp[1],temp[3],temp[5],temp[7]);
         bit_metric2[0][i]=max(temp[0],temp[1],temp[5],temp[4]);
         bit_metric2[1][i]=max(temp[2],temp[3],temp[6],temp[7]);
         bit_metric3[0][i]=max(temp[0],temp[1],temp[2],temp[3]);
         bit_metric3[1][i]=max(temp[4],temp[5],temp[6],temp[7]);
         
         }
     }
