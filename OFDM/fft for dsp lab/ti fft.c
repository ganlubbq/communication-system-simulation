





void DSP_radix2 (short x[ ],short nx,short w[ ])
{
	short n1,n2,ie,ia,i,j,k,l;
	short xt,yt,c,s;
	
	n2 = nx;
	ie = 1;
	for (k=nx; k > 1; k = (k >> 1) ) {
		n1 = n2;
		n2 = n2>>1;
		ia = 0;
		for (j=0; j < n2; j++) {
			c = w[2*ia];
			s = w[2*ia+1];
			ia = ia + ie;
			for (i=j; i < nx; i += n1) {
				l = i + n2;
				xt			= x[2*l] - x[2*i];
				x[2*i]		= x[2*i] + x[2*l];
				yt			= x[2*l+1] - x[2*i+1];
				x[2*i+1]	= x[2*i+1] + x[2*l+1];

                x[2*l]		= (c*xt + s*yt)>>15;
				x[2*l+1]	= (c*yt - s*xt)>>15;
			}
		}
		ie = ie<<1;
	}
}
