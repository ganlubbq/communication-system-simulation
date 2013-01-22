struct state{
       struct state *from;
       int in[2];
       };
       
struct state node[(k+m)/2+1][st_num];

int next_table[64][4];
float out_table[64][12];
float ray[(k+m)/2*3];

int *u,*u1,*u2,*v,*v1,*v2,*v3,*y;

void declaration(void);

void declaration(){

     u=(int *) malloc((k+m)*sizeof(int));
    u1=(int *) malloc((k+m)/2*sizeof(int));
    u2=(int *) malloc((k+m)/2*sizeof(int));
     v=(int *) malloc((k+m)/2*3*sizeof(int));
    v1=(int *) malloc(((k+m))/2*sizeof(int));
    v2=(int *) malloc(((k+m))/2*sizeof(int));
    v3=(int *) malloc(((k+m))/2*sizeof(int));
     y=(int *) malloc((k+m)*sizeof(int));
     
     }
