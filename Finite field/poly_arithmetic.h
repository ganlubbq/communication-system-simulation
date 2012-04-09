/* Representation of polynomial : 
 * 1 + 0*X + a^n*X^2 + 0X^3 + a^m*X^4 + ... => 1 0 (n+1) 0 (m+1) ... 
 */

#define Max 10//Max order of polynomial

void addPoly(int *a, int *b, int *result){
     int i;
     
     for (i=0;i<Max;i++){
         result[i]=addGF(a[i], b[i]);
     }
}

void mulPoly(int *a, int *b, int *result){
     int i,j;
     int temp[2*Max]={0};
     
     for (i=0;i<Max;i++){
         for (j=0;j<Max;j++){
             temp[j+i] = addGF( temp[j+i], mulGF(a[j],b[i]) );
         }
     }
     
     for (i=0;i<Max;i++){
         result[i]=temp[i];
     }
}



