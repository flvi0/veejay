/* 
 * Linux VeeJay
 *
 * Copyright(C)2002 Niels Elburg <elburg@hio.hen.nl>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License , or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307 , USA.
 */
#include <config.h>
#include "raster.h"
#include <stdlib.h>
#include "common.h"
#include <math.h>
vj_effect *raster_init(int w, int h)
{
    vj_effect *ve = (vj_effect *) malloc(sizeof(vj_effect));
    ve->num_params = 1;

    ve->defaults = (int *) malloc(sizeof(int) * ve->num_params);	/* default values */
    ve->limits[0] = (int *) malloc(sizeof(int) * ve->num_params);	/* min */
    ve->limits[1] = (int *) malloc(sizeof(int) * ve->num_params);	/* max */
    ve->limits[0][0] = 4;
    ve->limits[1][0] = h/4;
    ve->defaults[0] = 4;
    ve->description = "Grid";
    ve->sub_format = 0;
    ve->extra_frame = 0;
    ve->has_internal_data= 0;

    return ve;
}

void raster_apply(uint8_t *yuv[3], int w, int h, int v )
{
	int x,y;
	for(y=0; y < h; y++)
	{
		for(x=0; x < w; x++)
		{
			yuv[0][y*w+x] = ((x%v>1)? ((y%v>1) ? yuv[0][y*w+x]: 235):235);
		}
	}
	w= w/2;
	h= h/2;
	for(y=0; y < h; y++)
	{
		for(x=0; x < w; x++)
		{
			yuv[1][y*w+x] = ((x%v>1)? ((y%v>1) ? yuv[1][y*w+x]:128):128);
			yuv[2][y*w+x] = ((x%v>1)? ((y%v>1) ? yuv[2][y*w+x]:128):128);
		}
	}
/*
	int x,y;
	int px,py;
	int i,j;
	double r,a;
	unsigned int R = h/2;
	double	curve; //curve
	double  coeef;
	int w2 = w/2;
	int h2 = h/2;
	int n1=0,n2=0,n3=0;
	int k,l,m,o1=0,o2=0;

	double (*pf)(double a, double b, double c);
	
	if( v==0) v =1;
	if( v < 0 ) {
		pf = &__fisheye_i;
		v = v * -1;
	}
	else  {
		pf = &__fisheye;
	}

	curve = 0.001 * v; //curve
	coeef = R / log(curve * R + 1);

	if(!buf)
	{
		buf = (uint8_t*)malloc(sizeof(uint8_t) * w * h );
		if(!buf)return;
	}
	memcpy(buf, yuv[0],(w*h));

	for(y= (-1*h2); y < (h-h2); y++)
//	for(y=0; y < h; y++)
	//for(y=0; y < h; y++)
	{
		for(x = (-1*w2); x < (w-w2); x++)
	//	for(x=0 ;x < w; x++)
		{
			//if(x > 0 && y > 0)
			//
			if(x==0 && y==0) r = 0; 
			else r = sqrt( y*y+x*x);
			if(x==0 && y==0)
			a= 1;
			else
			a = atan2( (float)y,x);

			//if(x > 0 && y < 0) a += 240;
			
			//if(x < 0 && y > 0) a+= 180;
		
			//if(x < 0 && y < 0) a+= 180; 
	
			i = (y+h2)*w+(w2+x);
			if( r <= R)
			{
				//r = coeef * log(1 + curve * r);
				r = pf( r, coeef, curve);
				//px en py ook zonder +
				px = (int) ( r * cos(a) );
				py = (int) ( r * sin(a) );
				px += w2;
				py += h2;
				if(px < 0) px =0;
				if(px > w) px = w;
				if(py < 0) py = 0;
				if(py > (h-1)) py = h-1;
				j = px + py * w;
				//k = py * w + (w - px);
				yuv[0][i] = buf[j];	
			}
			else
			{
				yuv[0][i] = 16;
			}			

			

		}

	}


	printf(" n1 = %d n2 = %d n3 = %d \n",n1,n2,n3);


	memset(yuv[1],128,(w*h)/4);
	memset(yuv[2],128,(w*h)/4);
	*/

}
