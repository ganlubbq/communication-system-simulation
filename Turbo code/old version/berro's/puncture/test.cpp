#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

void test(float *x){
     x[2]=6.4;
     }

int main(){
    
    int j,k=30000;
    
    float x[k];
    
    for (j=0;j<k;j++){
         x[j]=j;
         }
    
    test(x);
    
    
    printf("%f\n",x[555]);
    
    system("pause");
    
    
    
    
    }
