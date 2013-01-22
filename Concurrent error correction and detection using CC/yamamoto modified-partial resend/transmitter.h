void input(int *u,int *v){
    int i,j,temp1,temp2;
    
    /*if (retransmit==1){
       for (i=0;i<2*(k+m);i++){
           v[i]=buffer[i];
           }
       }*/
       
   // else{
    
    for (i=-m;i<k+m;i++){
        if (-1<i && i<k) u[i]=rand()%2;
        else u[i]=0;
       }
  
    for (i=0;i<k+m;i++){                                   //encoding
        
        temp1=u[i]*g11[0];
        temp2=u[i]*g12[0];
        
        for (j=1;j<=m;j++){
        
            temp1=temp1^u[i-j]*g11[j];
            temp2=temp2^u[i-j]*g12[j];
            }
        
        v[2*i]=temp1;
        v[(2*i)+1]=temp2;
        buffer[2*i]=temp1;
        buffer[(2*i)+1]=temp2;
        
        }
    
    //}
    }


void resend(int idx,float sd,float *r){
     int i,begin;
     
     begin=idx-S;
     //printf("%d\n",idx);
     //system("pause");
     
          
              
     for (i=begin*2;i<idx*2;i++){
         //r[i]=(2*buffer[i]-1)+gaurand()*sd;
               
         ray[i]=rayrand();
         r[i]=(2*buffer[i]-1)*ray[i]+gaurand()*sd;
         }
     retransmit=0;
     }
     
