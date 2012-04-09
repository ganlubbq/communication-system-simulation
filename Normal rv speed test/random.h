
float urand(int *idum);
float randn(int *idum);


float randn(int *idum)
{

	static int iset=0;

	static float gset;

	float fac,r,v1,v2;



	if	(iset == 0)

	{

		do

		{

			v1=2.0*urand(idum)-1.0;		

			v2=2.0*urand(idum)-1.0;		

			r=v1*v1+v2*v2;			

		}

		while	(r>=1.0 || r==0.0);		

		fac=sqrt(-2.0*log(r)/r);



		

		gset=v1*fac;

		iset=1;				

		return v2*fac;

	}

	else						

	{

		iset=0;					

		return gset;			

	}

}



float urand(int *idum)

{

  int j;

  int k;

  static int idum2=123456789;

  static int iy=0;

  static int iv[32];

  float temp;



  if (*idum <=0)

  {

    if (-(*idum)<1)*idum=1;

    else *idum=-(*idum);

    idum2=(*idum);

    for (j=32+7;j>=0;j--)

    {

      k=(*idum)/53668;

      *idum=40014*(*idum-k*53668)-k*12211;

      if (*idum < 0) *idum+=2147483563;

      if (j<32) iv[j]=*idum;

    }

    iy=iv[0];

  }

  

  k=(*idum)/53668;

  *idum=40014*(*idum-k*53668)-k*12211;

  

  if (*idum < 0) *idum += 2147483563;

  

  k=idum2/52774;

  

  idum2=40692*(idum2-k*52774)-k*3791;

  

  if (idum2 < 0 ) idum2 += 2147483399;

  

  j=iy/67108862;

  iy=iv[j]-idum2;

  iv[j]=*idum;

  

  if (iy<1) iy += 2147483562;

  

  if ((temp=iy/2147483563.0) > 0.99999988) return 0.99999988;

  else return temp;

}

