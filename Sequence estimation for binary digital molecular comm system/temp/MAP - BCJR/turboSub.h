inline int isNumOfBit1Odd(int value, int bitCount)
{
   int num = 0;
   int iShift = 0;
   while (iShift <= bitCount-1)
   {
       num ^= (value>>iShift);
       iShift++;
   }
   return num&1;
}

void table(void);
void lookup(void);

inline void initialize()
{
   table(); 
   lookup();
}

void lookup(void)
{

}


void table(void)
{
     for (int j=0;j<st_num;j++)
     {
         next_table[j][0] = j>>1;
         next_table[j][1] = next_table[j][0] +(1<<(m-1));
         
         from_table[j][0] = (j<<1) & ((1<<m)-1);
         from_table[j][1] = from_table[j][0]+1;
     }
}

inline float logdiff(float a,float b)
{
//       cout << exp(a) << endl << exp(b) << endl << log(exp(a)-exp(b)) << endl;getchar();
    return log(exp(a)-exp(b));
}

float logsum(float a,float b)
{
    float max = (a>b) ? a : b;
    
    float delta = (a>b) ? (a-b) : (b-a);
    
    if (delta>100)delta=100;
    
    delta=log(1+exp(-delta));
    
    return max+delta;
}


/*
// Aided by table look up
inline float logsum(float a,float b)
{
    float max = (a>b) ? a : b;
    
    float delta = (a>b) ? (a-b) : (b-a);
    
    if (delta>100) { delta = 99.999; }

    delta = log(1+lookup_table[int(delta*1000)]);
    
    return max+delta;
}
*/
