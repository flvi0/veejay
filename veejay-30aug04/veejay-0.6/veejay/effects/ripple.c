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


/* This effect recalculates a pretty large table if 'waves' or 'amplitude'
   is changed. Results will be placed in ripple_table, a copy of the 
   frame is kept in ripple_data. So is the calculation of the first frame slow,
   the following frames will use the cached coordinates until the user changes
   the number of waves or the amplitude. 


*/

#include "ripple.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "vj-common.h"
extern void *(* veejay_memcpy)(void *to, const void *from, size_t len);

#define RIPPLE_DEGREES 360.0
#define RIPPLE_VAL 180.0

static double *ripple_table;
static uint8_t *ripple_data[3];
static double *ripple_sin;
static double *ripple_cos;

static int ripple_waves = 0;
static int ripple_ampli = 0;
static int ripple_attn = 0;

vj_effect *ripple_init(int width, int height)
{
    vj_effect *ve = (vj_effect *) malloc(sizeof(vj_effect));
    ve->num_params = 3;
    ve->defaults = (int *) malloc(sizeof(int) * ve->num_params);	/* default values */
    ve->limits[0] = (int *) malloc(sizeof(int) * ve->num_params);	/* min */
    ve->limits[1] = (int *) malloc(sizeof(int) * ve->num_params);	/* max */
    ve->limits[0][0] = 1;
    ve->limits[1][0] = 3600;
    ve->limits[0][1] = 1;
    ve->limits[1][1] = 80;
    ve->limits[0][2] = 1;
    ve->limits[1][2] = 360;
    ve->defaults[0] = 132;
    ve->defaults[1] = 47;
    ve->defaults[2] = 7;
    ve->description = "Ripple";
    ve->sub_format = 1;
    ve->extra_frame = 0;
    ve->has_internal_data= 1;

    return ve;
}

int	ripple_malloc(int width, int height)
{
   int i;
    ripple_table = (double*) vj_malloc(sizeof(double) * width * height + 16);
    if(!ripple_table) return 0;
    ripple_data[0] = (uint8_t*)vj_malloc(sizeof(uint8_t) * width * height + 16);
    if(!ripple_data[0]) return 0; 
    ripple_data[1] = (uint8_t*)vj_malloc(sizeof(uint8_t) * width * height + 16);
    if(!ripple_data[1]) return 0;
    ripple_data[2] = (uint8_t*)vj_malloc(sizeof(uint8_t) * width * height + 16);
    if(!ripple_data[2]) return 0; 
    ripple_sin = (double*) vj_malloc(sizeof(double) * RIPPLE_DEGREES);
    if(!ripple_sin) return 0;
    ripple_cos = (double*) vj_malloc(sizeof(double) * RIPPLE_DEGREES);
    if(!ripple_cos) return 0;
    for(i=0; i < RIPPLE_DEGREES; i++) {
 	ripple_sin[i] = sin ((M_PI * i) / RIPPLE_VAL);
	ripple_cos[i] = sin ((M_PI * i) / RIPPLE_VAL);
    }

    return 1;

}

void ripple_free() {
	if(ripple_table) free(ripple_table);
	if(ripple_sin) free(ripple_sin);
	if(ripple_cos) free(ripple_cos);
	if(ripple_data[0]) free(ripple_data[0]);
	if(ripple_data[1]) free(ripple_data[1]);
	if(ripple_data[2]) free(ripple_data[2]);
}


void ripple_apply(uint8_t *yuv1[3], int width, int height, int _w, int _a , int _att ) {

	double wp2 = width * 0.5;
	double hp2 = height * 0.5;
	int x,y,dx,dy,a=0,sx=0,sy=0,angle=0;
	double r,z;
	double maxradius,frequency,amplitude;
	double waves = (_w/10.0);
	double ampli = (double) (_a/10.0);
	double attenuation = (_att/10.0);
	int have_calc_data = 1;
	int i;
	int len=(width*height);
	maxradius = sqrt(wp2 * wp2 + hp2 * hp2);
	frequency = 360.0 * waves / maxradius;
	amplitude = maxradius / ampli;

	if(ripple_waves != _w) {
		ripple_waves = _w;	
		have_calc_data=0;
	}
	if(ripple_ampli != _a) {
		ripple_ampli = _a;
		have_calc_data=0;
	}
	if(ripple_attn != _att) {
		ripple_attn = _att;
		have_calc_data = 0;
	}
	veejay_memcpy( ripple_data[0], yuv1[0], (width*height));
	veejay_memcpy( ripple_data[1], yuv1[1], (width*height));
	veejay_memcpy( ripple_data[2], yuv1[2], (width*height));

	if (have_calc_data==0) {
  	   for(y=0; y < height-1;y++) {
		for (x=0; x < width; x++) {
		  dx = x - wp2;
		  dy = y - hp2;
		  
		  angle = 180.0 * (atan2(dx,dy)/M_PI);

		  if (angle < 0) angle+=360.0;

		  r = sqrt( dx * dx + dy * dy);

		  z = amplitude/ pow(r,attenuation) * ripple_sin[ ((int)(frequency * r)) % 360 ];

		  a = ((int) (angle)) % 360;
		  sx = (int) (x+z * ripple_cos[a]);
		  sy = (int) (y+z * ripple_sin[a]);

		  if(sy > (height-1)) sy = height-1;
		  if(sx > width) sx = width;
		  if(sx < 0) sx =0;
		  if(sy < 0) sy =0;
	 		
		  ripple_table[(y*width)+x] = (sx + (sy * width));

		  yuv1[0][((y * width) +x)] = ripple_data[0][(sx +( sy * width)) ];		
		  yuv1[1][((y * width) +x)] = ripple_data[1][(sx +( sy * width)) ];
		  yuv1[2][((y * width) +x)] = ripple_data[2][(sx +( sy * width)) ];
		}
	    }
	}
	else {
	   for(y=0; y < height-1;y++) {
		for (x=0; x < width; x++) {
		  sx = ripple_table[(y*width)+x];	
		  yuv1[0][(y * width) +x] = ripple_data[0][sx];
		  yuv1[1][(y * width) +x] = ripple_data[1][sx];
		  yuv1[2][(y * width) +x] = ripple_data[2][sx];
		}
  	   }
	}
}
