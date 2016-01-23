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
     for (int l=0;l<st_num;l++)
     {
         next_table[l][0] = l>>1;
         next_table[l][1] = next_table[l][0] +(1<<(m-1));
         
         from_table[l][0] = (l<<1) & ((1<<m)-1);
         from_table[l][1] = from_table[l][0]+1;
     }
}


/*
// Aided by table look up
inline double logsum(double a,double b)
{
    double max = (a>b) ? a : b;
    
    double delta = (a>b) ? (a-b) : (b-a);
    
    if (delta>100) { delta = 99.999; }

    delta = log(1+lookup_table[int(delta*1000)]);
    
    return max+delta;
}
*/
