/* EffecTV - Realtime Digital Video Effektor
 * Copyright (C) 2001-2003 FUKUCHI Kentaro
 *
 * RippleTV - Water ripple effect
 * Copyright (C) 2001 - 2002 FUKUCHI Kentaro
 * 
 * ported to Linux VeeJay by:
 * Copyright(C)2002 Niels Elburg <nelburg@looze.net>
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
#include "rippletv.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include "vj-common.h"

static uint8_t *ripple_data[3];

static int stat;
static signed char *vtable;
static int *map;
static int *map1, *map2, *map3;
static int map_h, map_w;
static int sqrtable[256];
static const int point = 16;
static const int impact = 2;
static const int decay = 8;
static const int loopnum = 2;
static int bgIsSet = 0;
extern void *(* veejay_memcpy)(void *to, const void *from, size_t len);


/* from EffecTV:
 * fastrand - fast fake random number generator
 * Warning: The low-order bits of numbers generated by fastrand()
 *          are bad as random numbers. For example, fastrand()%4
 *          generates 1,2,3,0,1,2,3,0...
 *          You should use high-order bits.
 */
unsigned int wfastrand_val;

unsigned int wfastrand()
{
	return (wfastrand_val=wfastrand_val*1103515245+12345);
}

static void setTable()
{
	int i;

	for(i=0; i<128; i++) {
		sqrtable[i] = i*i;
	}
	for(i=1; i<=128; i++) {
		sqrtable[256-i] = -i*i;
	}
}



vj_effect *water_init(int width, int height)
{
    int i;
    vj_effect *ve = (vj_effect *) malloc(sizeof(vj_effect));
    ve->num_params = 1;
    ve->defaults = (int *) malloc(sizeof(int) * ve->num_params);	/* default values */
    ve->limits[0] = (int *) malloc(sizeof(int) * ve->num_params);	/* min */
    ve->limits[1] = (int *) malloc(sizeof(int) * ve->num_params);	/* max */
    ve->limits[0][0] = 1;
    ve->limits[1][0] = 3600;
    ve->defaults[0] = 10;
    ve->description = "RippleTV - Raindrops";
    ve->sub_format = 0;
    ve->extra_frame = 0;
    ve->has_internal_data= 1;
    return ve;
}

int water_malloc(int width, int height)
{
	ripple_data[0] = (uint8_t*)vj_malloc(sizeof(uint8_t) * width * height + 16);
	if(!ripple_data[0]) return 0;
	memset( ripple_data[0], 16, (width*height+16));


	map_h = height / 2 + 1;
	map_w = width / 2 + 1;
	map = (int*) vj_malloc (sizeof(int) * map_h * map_w * 3);
	if(!map) return 0;
	vtable = (signed char*) vj_malloc( sizeof(signed char) * map_w * map_h * 2);
	if(!vtable) return 0;
	map3 = map + map_w * map_h * 2;
	setTable();
	memset(map, 0, map_h*map_w*3*sizeof(int));
	memset(vtable, 0, map_h*map_w*2*sizeof(signed char));
	map1 = map;
	map2 = map + map_h*map_w;

	stat = 1;

	return 1;
}

void water_free() {
	if(ripple_data[0]) free(ripple_data[0]);
	if(map) free(map);
	if(vtable) free(vtable);

}


static inline void drop(int power)
{
	int x, y;
	int *p, *q;

	x = wfastrand()%(map_w-4)+2;
	y = wfastrand()%(map_h-4)+2;
	p = map1 + y*map_w + x;
	q = map2 + y*map_w + x;
	*p = power;
	*q = power;
	*(p-map_w) = *(p-1) = *(p+1) = *(p+map_w) = power/2;
	*(p-map_w-1) = *(p-map_w+1) = *(p+map_w-1) = *(p+map_w+1) = power/4;
	*(q-map_w) = *(q-1) = *(q+1) = *(q+map_w) = power/2;
	*(q-map_w-1) = *(q-map_w+1) = *(q+map_w-1) = *(p+map_w+1) = power/4;
}

static void raindrop()
{
	static int period = 0;
	static int rain_stat = 0;
	static unsigned int drop_prob = 0;
	static int drop_prob_increment = 0;
	static int drops_per_frame_max = 0;
	static int drops_per_frame = 0;
	static int drop_power = 0;

	int i;

	if(period == 0) {
		switch(rain_stat) {
		case 0:
			period = (wfastrand()>>23)+100;
			drop_prob = 0;
			drop_prob_increment = 0x00ffffff/period;
			drop_power = (-(wfastrand()>>28)-2)<<point;
			drops_per_frame_max = 2<<(wfastrand()>>30); // 2,4,8 or 16
			rain_stat = 1;
			break;
		case 1:
			drop_prob = 0x00ffffff;
			drops_per_frame = 1;
			drop_prob_increment = 1;
			period = (drops_per_frame_max - 1) * 16;
			rain_stat = 2;
			break;
		case 2:
			period = (wfastrand()>>22)+1000;
			drop_prob_increment = 0;
			rain_stat = 3;
			break;
		case 3:
			period = (drops_per_frame_max - 1) * 16;
			drop_prob_increment = -1;
			rain_stat = 4;
			break;
		case 4:
			period = (wfastrand()>>24)+60;
			drop_prob_increment = -(drop_prob/period);
			rain_stat = 5;
			break;
		case 5:
		default:
			period = (wfastrand()>>23)+500;
			drop_prob = 0;
			rain_stat = 0;
			break;
		}
	}
	switch(rain_stat) {
	default:
	case 0:
		break;
	case 1:
	case 5:
		if((wfastrand()>>8)<drop_prob) {
			drop(drop_power);
		}
		drop_prob += drop_prob_increment;
		break;
	case 2:
	case 3:
	case 4:
		for(i=drops_per_frame/16; i>0; i--) {
			drop(drop_power);
		}
		drops_per_frame += drop_prob_increment;
		break;
	}
	period--;
}

static int last_fresh_rate = 0;
static int new_fresh_rate = 0;
void	water_apply(uint8_t *yuv1[3], int width, int height, int fresh_rate)
{
	int x, y, i;
	int dx, dy;
	int h, v;
	int wi, hi;
	int *p, *q, *r;
	signed char *vp;
	uint8_t *src,*dest,*dest2;
	

	if(last_fresh_rate != fresh_rate)
	{
		last_fresh_rate = fresh_rate;
		memset( map, 0, (map_h*map_w*2*sizeof(int)));
	}


	veejay_memcpy ( ripple_data[0], yuv1[0], width*height);
	dest = yuv1[0];
	src = ripple_data[0];

	/* impact from the motion or rain drop */
	raindrop();

	/* simulate surface wave */
	wi = map_w;
	hi = map_h;
	
	/* This function is called only 30 times per second. To increase a speed
	 * of wave, iterates this loop several times. */
	for(i=loopnum; i>0; i--) {
		/* wave simulation */
		p = map1 + wi + 1;
		q = map2 + wi + 1;
		r = map3 + wi + 1;
		for(y=hi-2; y>0; y--) {
			for(x=wi-2; x>0; x--) {
				h = *(p-wi-1) + *(p-wi+1) + *(p+wi-1) + *(p+wi+1)
				  + *(p-wi) + *(p-1) + *(p+1) + *(p+wi) - (*p)*9;
				h = h >> 3;
				v = *p - *q;
				v += h - (v >> decay);
				*r = v + *p;
				p++;
				q++;
				r++;
			}
			p += 2;
			q += 2;
			r += 2;
		}

		/* low pass filter */
		p = map3 + wi + 1;
		q = map2 + wi + 1;
		for(y=hi-2; y>0; y--) {
			for(x=wi-2; x>0; x--) {
				h = *(p-wi) + *(p-1) + *(p+1) + *(p+wi) + (*p)*60;
				*q = h >> 6;
				p++;
				q++;
			}
			p+=2;
			q+=2;
		}

		p = map1;
		map1 = map2;
		map2 = p;
	}

	vp = vtable;
	p = map1;
	for(y=hi-1; y>0; y--) {
		for(x=wi-1; x>0; x--) {
			/* difference of the height between two voxel. They are twiced to
			 * emphasise the wave. */
			vp[0] = sqrtable[((p[0] - p[1])>>(point-1))&0xff]; 
			vp[1] = sqrtable[((p[0] - p[wi])>>(point-1))&0xff]; 
			p++;
			vp+=2;
		}
		p++;
		vp+=2;
	}

	hi = height;
	wi = width;
	vp = vtable;

/*	dest2 = dest;
        p = map1;
        for(y=0; y<hi; y+=2) {
                for(x=0; x<wi; x+=2) {
                        h = (p[0]>>(point-5))+128;
                        if(h < 0) h = 0;
                        if(h > 255) h = 255;
                        dest[0] = h;
                        dest[1] = h;
                        dest[wi] = h;
                        dest[wi+1] = h;
                        p++;
                        dest+=2;
                        vp+=2;
                }
                dest += width;
                vp += 2;
                p++;
        }

*/
	 
	
	for(y=0; y<hi; y+=2) {
		for(x=0; x<wi; x+=2) {
			h = (int)vp[0];
			v = (int)vp[1];
			dx = x + h;
			dy = y + v;
			if(dx<0) dx=0;
			if(dy<0) dy=0;
			if(dx>=wi) dx=wi-1;
			if(dy>=hi) dy=hi-1;
			dest[0] = src[dy*wi+dx];

			i = dx;

			dx = x + 1 + (h+(int)vp[2])/2;
			if(dx<0) dx=0;
			if(dx>=wi) dx=wi-1;
			dest[1] = src[dy*wi+dx];

			dy = y + 1 + (v+(int)vp[map_w*2+1])/2;
			if(dy<0) dy=0;
			if(dy>=hi) dy=h-1;
			dest[wi] = src[dy*wi+i];

			dest[wi+1] = src[dy*wi+dx];
			dest+=2;
			vp+=2;
		}
		dest += wi;
		vp += 2;
	}
}
