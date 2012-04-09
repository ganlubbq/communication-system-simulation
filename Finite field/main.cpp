/* Channel coding HW3, Finite field arithmetics
 * All input arguments and returns are the index of F{0, 1, a, a^2, a*3...},
 * not powers of Alpha! 
 * Date : 2007/4/20
 */

#define m 4//GF(2^m)

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

#include"GF_generator.h"
#include"GF_arithmetic.h"
#include"poly_arithmetic.h"

int main(){
    int i,j;;
    int result;
    int s,min,hour;
    long temptime1,temptime2;

    time(&temptime1);                                       
 
    generateGF();
 
    //--------------------------------------------------------------------------
    //Test GF arithmetics

    result = addGF(3,4);
    printf("a^%d\n",result-1);
  
    result = mulGF(3,4);
    printf("a^%d\n",result-1);
 
    result = inverseGF(3);
    printf("a^%d\n",result-1);
  
    //--------------------------------------------------------------------------
    //Test poly arithmetics
    
    int polya[Max]={1,6,0,3,2};
    int polyb[Max]={1,0,2,0,8};
    int polyc[Max];
    int polyd[Max];
    
    addPoly(polya,polyb,polyc);
    mulPoly(polya,polyb,polyd);
    
    printf("Add : ");
    for (i=0;i<Max;i++){
        printf("%2d ",polyc[i]);
    }
    printf("\nMul : ");
    for (i=0;i<Max;i++){
        printf("%2d ",polyd[i]);
    }
    //--------------------------------------------------------------------------
    //Generate RS generator poly
    int init[Max]={2,1};
    int RS_generator[Max]={1};
    
    for (i=0;i<6;i++){
        mulPoly(RS_generator,init,RS_generator);
        
        init[0]++;
    }
    
    printf("\nRS generator : \n");
    for (i=0;i<Max;i++){
        if (RS_generator[i] != 0) printf("a^%d*X^%d ",RS_generator[i]-1,i);
    }
    //--------------------------------------------------------------------------
    
    time(&temptime2);//calculate execution time
    s=temptime2-temptime1;
    hour=s/3600;
    s=s%3600;
    min=s/60;
    s=s%60;

    printf("\n\n你用了%2d小時 %2d分 %2d秒\n",hour,min,s);
    system("pause");
}
