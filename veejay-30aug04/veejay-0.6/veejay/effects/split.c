/* 
 * Linux VeeJay
 *
 * Copyright(C)2002 Niels Elburg <elburg@hio.hen.nl>
 cat*
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
#include "split.h"
#include <stdlib.h>
#include <stdio.h>
#include "vj-common.h"

static uint8_t *split_fixme[3];
extern void *(* veejay_memcpy)(void *to, const void *from, size_t len);
vj_effect *split_init(int width,int height)
{
    vj_effect *ve = (vj_effect *) malloc(sizeof(vj_effect));
    ve->num_params = 2;
    ve->defaults = (int *) malloc(sizeof(int) * ve->num_params);	/* default values */
    ve->limits[0] = (int *) malloc(sizeof(int) * ve->num_params);	/* min */
    ve->limits[1] = (int *) malloc(sizeof(int) * ve->num_params);	/* max */
    ve->defaults[0] = 8;
    ve->defaults[1] = 1;

    ve->limits[0][0] = 0;
    ve->limits[1][0] = 13;

    ve->limits[0][1] = 0;
    ve->limits[1][1] = 1;

    ve->description = "Splitted Screens";
    ve->sub_format = 0;
    ve->extra_frame = 1;
    ve->has_internal_data = 1;
    return ve;
}
int	split_malloc(int width, int height)
{
  split_fixme[0] = (uint8_t *) vj_malloc(sizeof(uint8_t) * width * height + 1);
  if(!split_fixme[0]) return 0;
    split_fixme[1] = (uint8_t *) vj_malloc(sizeof(uint8_t) * width * height );
  if(!split_fixme[1]) return 0; 
   split_fixme[2] = (uint8_t *) vj_malloc(sizeof(uint8_t) * width * height);
  if(!split_fixme[2]) return 0;
   return 1;

}

void split_free() {
  if(split_fixme[0]) free(split_fixme[0]);
  if(split_fixme[1]) free(split_fixme[1]);
  if(split_fixme[2]) free(split_fixme[2]);
}

void split_fib_downscale(uint8_t * yuv[3], int width, int height)
{
    unsigned int i, len = (width * height) / 2;
    unsigned int f;
    unsigned int x, y;
    const unsigned int ilen = width * height;
    i = 0;
    for (y = 0; y < len; y += width) {
	for (x = 0; x < width; x++) {
	    i++;
	    f = (i + 1) + (i - 1);
	    if( f >= ilen ) break;
	    yuv[0][y + x] = yuv[0][f];
//	    yuv[1][y + x] = yuv[1][f];
//	    yuv[2][y + x] = yuv[2][f];
	}
    }

    int uv_len = len / 2;
i = 0;
    int uv_width = width / 2;
    for (y = 0; y < len; y += uv_width) {
	for (x = 0; x < uv_width; x++) {
	    i++;
	    f = (i + 1) + (i - 1);
	    if( f >= uv_len ) break;
	    yuv[1][y + x] = yuv[1][f];
	    yuv[2][y + x] = yuv[2][f];
	}
    }

}
void split_fib_downscaleb(uint8_t * yuv[3], int width, int height)
{
    unsigned int len = (width * height) / 2;
    unsigned int uv_len = len / 4;
    uint8_t *y = yuv[0];
    uint8_t *u = yuv[1];
    uint8_t *v = yuv[2];
    int x;
    split_fib_downscale(yuv, width, height);

	veejay_memcpy( yuv[0]+len, yuv[0] , len );
	veejay_memcpy( yuv[1]+uv_len, yuv[1], uv_len);
	veejay_memcpy( yuv[2]+uv_len, yuv[2], uv_len);
/*
    for(x=0; x < len; x++ ) {
	yuv[0][x+len] = yuv[0][x];
//	yuv[1][x+len] = yuv[1][x];
//	yuv[2][x+len] = yuv[2][x];
	}
    len = len / 2;
	 for(x=0; x < len; x++ ) {
	yuv[1][x+len] = yuv[1][x];
	yuv[2][x+len] = yuv[2][x];
	}
*/
}

void dosquarefib(uint8_t * yuv[3], int width, int height)
{
    unsigned int i, len = (width * height) / 2;
    unsigned int f;
    unsigned int x, y, y1, y2;
    unsigned int uv_len = len / 4;
    unsigned int uv_width = width / 2;
    const unsigned int uv_height = height / 2;
    unsigned int w3 = width >> 2;
    const unsigned int u_w3 = w3 / 2;
    const unsigned int muv_len = (width * height)/4;
    i = 0;
    for (y = 0; y < len; y += width) {
	for (x = 0; x < width; x++) {
	    i++;
	    f = (i + 1) + (i - 1);
	    split_fixme[0][y + x] = yuv[0][f];
	//    split_fixme[1][y + x] = yuv[1][f];
	//    split_fixme[2][y + x] = yuv[2][f];
	}
    }
    i = 0;
    for (y = 0; y < uv_len; y += uv_width) {
	for (x = 0; x < uv_width; x++) {
	    i++;
	    f = (i + 1) + (i - 1);
            if(f > muv_len) break;		 
	 //   split_fixme[0][y + x] = yuv[0][f];
	    split_fixme[1][y + x] = yuv[1][f];
	    split_fixme[2][y + x] = yuv[2][f];
	}
    }

    len = len >> 1;
    width = width >> 1;
    for (y = 0; y < len; y += width) {
	for (x = 0; x < width; x++) {
	    i++;
	    f = (i + 1) + (i - 1);
	    split_fixme[0][y + x] = split_fixme[0][f];
	  //  split_fixme[0][y + x] = split_fixme[1][f];
	  //  split_fixme[0][y + x] = split_fixme[2][f];
	}
    }
    uv_len = uv_len >> 1;
    uv_width = uv_width >> 1;
     for (y = 0; y < uv_len; y += uv_width) {
	for (x = 0; x < uv_width; x++) {
	    i++;
	    f = (i + 1) + (i - 1);
	    if(f > muv_len) break;
	    split_fixme[1][y + x] = split_fixme[1][f];
	    split_fixme[2][y + x] = split_fixme[2][f];
	}
    }


    for (y = 0; y < height; y++) {
	y1 = (y * width) >> 1;
	y2 = y * width;
	for (i = 0; i < 4; i++)
	    for (x = 0; x < w3; x++) {
		yuv[0][y2 + x + (i * w3)] = split_fixme[0][y1 + x];
	    }
    }
    for (y = 0; y < uv_height; y++) {
	y1 = (y * uv_width) >> 1;
	y2 = y * uv_width;
	for (i = 0; i < 4; i++)
	    for (x = 0; x < u_w3; x++) {
		yuv[0][y2 + x + (i * u_w3)] = split_fixme[0][y1 + x];
	    }
    }

}

void split_push_downscale_uh(uint8_t * yuv[3], int width, int height)
{

   unsigned int x, y, y1, y2,j=0;
   unsigned int len = (width*height)/2;
  /* for(y=0; y < (width*height/2); y+=width) {
	for(x=0; x < width; x++) {
		split_fixme[0][y+x] = yuv[0][y+x];
		split_fixme[1][y+x] = yuv[1][y+x];
		split_fixme[2][y+x] = yuv[2][y+x];
	}
    } */
   veejay_memcpy( split_fixme[0], yuv[0], len);
   veejay_memcpy( split_fixme[1], yuv[1], len);
   veejay_memcpy( split_fixme[2], yuv[2], len);

}
void split_push_downscale_lh(uint8_t * yuv[3], int width, int height)
{

    unsigned int x, y, y1, y2, j = 0;

    const unsigned int vheight = height / 2;
    unsigned int hlen = height / 2;
    const unsigned int uv_len = (width * height)/4;
    const unsigned int uv_width = width  / 2;
    const unsigned int uv_vheight = height  / 4;
    const unsigned int uv_height = height / 2;
    const unsigned int uv_hlen = uv_len / 2;

    for (y = 0; y < height; y+=2) {
	j++;
	y2 = j * width;
	y1 = y * width;
	for (x = 0; x < width; x++) {
	    split_fixme[0][y2 + x] = yuv[0][y1 + x];
	}
    }

    j = 0;
    for (y = 0; y < uv_height; y+=2) {
	j++;
	y2 = j * uv_width;
	y1 = y * uv_width;
	for (x = 0; x < uv_width; x++) {
	    split_fixme[1][y2 + x] = yuv[1][y1 + x];
	    split_fixme[2][y2 + x] = yuv[2][y1 + x];
	}
    }

    veejay_memcpy( yuv[0]+hlen, split_fixme[0] , hlen );
    veejay_memcpy( yuv[1]+uv_hlen, split_fixme[1], uv_hlen);
    veejay_memcpy( yuv[2]+uv_hlen, split_fixme[2], uv_hlen);
}

void split_push_vscale_left(uint8_t * yuv[3], int width, int height)
{
    unsigned int x, y, y1;

    unsigned int wlen = width >> 1;
    const unsigned int uv_height = height/2;
    const unsigned int uv_width = width / 2;
    const unsigned int uv_len = uv_height * uv_width;
    const unsigned int uv_wlen = wlen / 2;

    for (y = 0; y < height; y++) {
	y1 = y * width;
	for (x = 0; x < wlen; x++) {
	    split_fixme[0][y1 + x] = yuv[0][y1 + (x * 2)];
	 //   split_fixme[1][y1 + x] = yuv[1][y1 + (x * 2)];
	 //   split_fixme[2][y1 + x] = yuv[2][y1 + (x * 2)];
	}
    }
    for (y = 0; y < uv_height; y++) {
	y1 = y * uv_width;
	for (x = 0; x < uv_wlen; x++) {
	    split_fixme[1][y1 + x] = yuv[1][y1 + (x * 2)];
	    split_fixme[2][y1 + x] = yuv[2][y1 + (x * 2)];
	}
    }


    for (y = 0; y < height; y++) {
	y1 = y * width;
	for (x = 0; x < wlen; x++) {
	    yuv[0][y1 + x] = split_fixme[0][y1 + x];
//	    yuv[1][y1 + x] = split_fixme[1][y1 + x];
//	    yuv[2][y1 + x] = split_fixme[2][y1 + x];
	}
    }

    for (y = 0; y < uv_height; y++) {
	y1 = y * uv_width;
	for (x = 0; x < uv_wlen; x++) {
	    yuv[1][y1 + x] = split_fixme[1][y1 + x];
	    yuv[2][y1 + x] = split_fixme[2][y1 + x];
	}
    }


}
void split_push_vscale_right(uint8_t * yuv[3], int width, int height)
{
    unsigned int x, y, y1;

    unsigned int wlen = width >> 1;
 const unsigned int uv_height = height/2;
    const unsigned int uv_width = width / 2;
    const unsigned int uv_len = uv_height * uv_width;
    const unsigned int uv_wlen = wlen / 2;


    for (y = 0; y < height; y++) {
	y1 = y * width;
	for (x = 0; x < wlen; x++) {
	    split_fixme[0][y1 + x] = yuv[0][y1 + (x * 2)];
//	    split_fixme[1][y1 + x] = yuv[1][y1 + (x * 2)];
//	    split_fixme[2][y1 + x] = yuv[2][y1 + (x * 2)];
	}
    }
    for (y = 0; y < uv_height; y++) {
	y1 = y * uv_width;
	for (x = 0; x < uv_wlen; x++) {
//	    split_fixme[0][y1 + x] = yuv[0][y1 + (x * 2)];
	    split_fixme[1][y1 + x] = yuv[1][y1 + (x * 2)];
	    split_fixme[2][y1 + x] = yuv[2][y1 + (x * 2)];
	}
    }

    for (y = 0; y < height; y++) {
	y1 = y * width;
	for (x = 0; x < wlen; x++) {
	    yuv[0][y1 + x + wlen] = split_fixme[0][y1 + x];
//	    yuv[1][y1 + x + wlen] = split_fixme[1][y1 + x];
//	    yuv[2][y1 + x + wlen] = split_fixme[2][y1 + x];
	}
    }
    for (y = 0; y < uv_height; y++) {
	y1 = y * uv_width;
	for (x = 0; x < uv_wlen; x++) {
//	    yuv[0][y1 + x + wlen] = split_fixme[0][y1 + x];
	    yuv[1][y1 + x + uv_wlen] = split_fixme[1][y1 + x];
	    yuv[2][y1 + x + uv_wlen] = split_fixme[2][y1 + x];
	}
    }

}



void split_corner_yuvdata_ul(uint8_t * yuv[3], uint8_t * yuv2[3],
			     int width, int height)
{
    unsigned int w_len = width / 2;
    unsigned int h_len = height / 2;
    unsigned int x, y;
    unsigned int y1;
    const unsigned int uv_height = height/2;
    const unsigned int uv_width = width / 2;
    const unsigned int uv_len = uv_height * uv_width;
    const unsigned int uv_wlen = w_len / 2;
    const unsigned int uv_hlen = h_len / 2;

    for (y = 0; y < h_len; y++) {
	y1 = width * y;
	for (x = 0; x < w_len; x++) {
	    yuv[0][y1 + x] = yuv2[0][y1 + x];
	//    yuv[1][y1 + x] = yuv2[1][y1 + x];
	 //   yuv[2][y1 + x] = yuv2[2][y1 + x];
	}
    }
    for (y = 0; y < uv_hlen; y++) {
	y1 = uv_width * y;
	for (x = 0; x < uv_wlen; x++) {
	    yuv[1][y1 + x] = yuv2[1][y1 + x];
	    yuv[2][y1 + x] = yuv2[2][y1 + x];
	}
    }


}
void split_corner_yuvdata_ur(uint8_t * yuv[3], uint8_t * yuv2[3],
			     int width, int height)
{
    unsigned int w_len = width / 2;
    unsigned int h_len = height / 2;
    unsigned int x, y;
    unsigned int y1;
    const unsigned int uv_height = height/2;
    const unsigned int uv_width = width / 2;
    const unsigned int uv_len = uv_height * uv_width;
    const unsigned int uv_wlen = w_len / 2;
    const unsigned int uv_hlen = h_len / 2;

    for (y = 0; y < h_len; y++) {
	y1 = width * y;
	for (x = w_len; x < width; x++) {
	    yuv[0][y1 + x] = yuv2[0][y1 + x];
	 //   yuv[1][y1 + x] = yuv2[1][y1 + x];
	   // yuv[2][y1 + x] = yuv2[2][y1 + x];

	}
    }
    for (y = 0; y < uv_hlen; y++) {
	y1 = uv_width * y;
	for (x = uv_wlen; x < uv_width; x++) {
	  //  yuv[0][y1 + x] = yuv2[0][y1 + x];
	    yuv[1][y1 + x] = yuv2[1][y1 + x];
	    yuv[2][y1 + x] = yuv2[2][y1 + x];

	}
    }

}
void split_corner_yuvdata_dl(uint8_t * yuv[3], uint8_t * yuv2[3],
			     int width, int height)
{
    unsigned int w_len = width / 2;
    unsigned int h_len = height / 2;
    unsigned int x, y;
    unsigned int y1;
    const unsigned int uv_height = height/2;
    const unsigned int uv_width = width / 2;
    const unsigned int uv_len = uv_height * uv_width;
    const unsigned int uv_wlen = w_len / 2;
    const unsigned int uv_hlen = h_len / 2;


    for (y = h_len; y < height; y++) {
	y1 = width * y;
	for (x = 0; x < w_len; x++) {
	    yuv[0][y1 + x] = yuv2[0][y1 + x];
	 //   yuv[1][y1 + x] = yuv2[1][y1 + x];
	 //   yuv[2][y1 + x] = yuv2[2][y1 + x];
	}
    }
    for (y = uv_hlen; y < uv_height; y++) {
	y1 = uv_width * y;
	for (x = 0; x < uv_wlen; x++) {
	    yuv[1][y1 + x] = yuv2[1][y1 + x];
	    yuv[2][y1 + x] = yuv2[2][y1 + x];
	}
    }

} 


void split_corner_yuvdata_dr(uint8_t * yuv[3], uint8_t * yuv2[3],
			     int width, int height)
{
    unsigned int w_len = width / 2;
    unsigned int h_len = height / 2;
    unsigned int x, y;
    unsigned int y1;
    const unsigned int uv_height = height/2;
    const unsigned int uv_width = width / 2;
    const unsigned int uv_len = uv_height * uv_width;
    const unsigned int uv_wlen = w_len / 2;
    const unsigned int uv_hlen = h_len / 2;


    for (y = h_len; y < height; y++) {
	y1 = width * y;
	for (x = w_len; x < width; x++) {
	    yuv[0][y1 + x] = yuv2[0][y1 + x];
//	    yuv[1][y1 + x] = yuv2[1][y1 + x];
//	    yuv[2][y1 + x] = yuv2[2][y1 + x];
	}
    }
    for (y = uv_hlen; y < uv_height; y++) {
	y1 = uv_width * y;
	for (x = uv_wlen; x < uv_width; x++) {
	    yuv[1][y1 + x] = yuv2[1][y1 + x];
	    yuv[2][y1 + x] = yuv2[2][y1 + x];
	}
    }

}


void split_v_first_halfs(uint8_t * yuv[3], uint8_t * yuv2[2], int width,
			 int height)
{

    unsigned int r, c;
    const unsigned int uv_height = height/2;
    const unsigned int uv_width = width / 2;
    const unsigned int uv_len = uv_height * uv_width;



    for (r = 0; r < (width * height); r += width) {
	for (c = width / 2; c < width; c++) {
	    yuv[0][c + r] = yuv2[0][(width - c) + r];
	 //   yuv[1][c + r] = yuv2[1][(width - c) + r];
	 //   yuv[2][c + r] = yuv2[2][(width - c) + r];
	}
    }
    for (r = 0; r < uv_len; r += uv_width) {
	for (c = uv_width/2; c < uv_width; c++) {
	    yuv[1][c + r] = yuv2[1][(uv_width - c) + r];
	    yuv[2][c + r] = yuv2[2][(uv_width - c) + r];
	}
    }

}
void split_v_second_half(uint8_t * yuv[3], uint8_t * yuv2[2], int width,
			 int height)
{
    unsigned int r, c;
    const unsigned int uv_height = height/2;
    const unsigned int uv_width = width / 2;
    const unsigned int uv_len = uv_height * uv_width;



    for (r = 0; r < (width * height); r += width) {
	for (c = width / 2; c < width; c++) {
	    yuv[0][c + r] = yuv2[0][c + r];
//	    yuv[1][c + r] = yuv2[1][c + r];
//	    yuv[2][c + r] = yuv2[2][c + r];
	}
    }


    for (r = 0; r < uv_len; r += uv_width) {
	for (c = uv_width / 2; c < uv_width; c++) {
	   // yuv[0][c + r] = yuv2[0][c + r];
	    yuv[1][c + r] = yuv2[1][c + r];
	    yuv[2][c + r] = yuv2[2][c + r];
	}
    }
}

void split_v_first_half(uint8_t * yuv[3], uint8_t * yuv2[2], int width,
			int height)
{
    unsigned int r, c;

    const unsigned int uv_height = height/2;
    const unsigned int uv_width = width / 2;
    const unsigned int uv_len = uv_height * uv_width;


    for (r = 0; r < (width * height); r += width) {
	for (c = 0; c < width / 2; c++) {
	    yuv[0][c + r] = yuv2[0][c + r];
	//    yuv[1][c + r] = yuv2[1][c + r];
	//    yuv[2][c + r] = yuv2[2][c + r];
	}
    }

    for (r = 0; r < uv_len; r += uv_width) {
	for (c = 0; c < uv_width / 2; c++) {
//	    yuv[0][c + r] = yuv2[0][c + r];
	    yuv[1][c + r] = yuv2[1][c + r];
	    yuv[2][c + r] = yuv2[2][c + r];
	}
    }

}

void split_v_second_halfs(uint8_t * yuv[3], uint8_t * yuv2[3], int width,
			  int height)
{
    unsigned int r, c;
    const unsigned int lw = width / 2;
    const unsigned int len = (width * height);
    const unsigned int uv_height = height/2;
    const unsigned int uv_width = width / 2;
    const unsigned int uv_len = uv_height * uv_width;


    for (r = 0; r < len; r += width) {
	for (c = 0; c < lw; c++) {
	    yuv[0][c + r] = yuv2[0][(width - c) + r];
//	    yuv[1][c + r] = yuv2[1][(width - c) + r];
//	    yuv[2][c + r] = yuv2[2][(width - c) + r];
	}
    }

    for (r = 0; r < uv_len; r += uv_width) {
	for (c = 0; c < uv_width/2; c++) {
//	    yuv[0][c + r] = yuv2[0][(width - c) + r];
	    yuv[1][c + r] = yuv2[1][(width - c) + r];
	    yuv[2][c + r] = yuv2[2][(width - c) + r];
	}
    }





}

void split_h_first_half(uint8_t * yuv[3], uint8_t * yuv2[3], int width,
			int height)
{
   const unsigned int len = (width * height) / 2;
   const unsigned int uv_len = len / 4;


    veejay_memcpy(yuv[0], yuv2[0], len);
    veejay_memcpy(yuv[1], yuv2[1], uv_len);
    veejay_memcpy(yuv[2], yuv2[2], uv_len);  

}
void split_h_second_half(uint8_t * yuv[3], uint8_t * yuv2[3], int width,
			 int height)
{
	const unsigned int len = (width * height) / 2;
	const unsigned int uv_len = len / 4;

	veejay_memcpy( yuv[0], yuv2[0]+len, len );
	veejay_memcpy( yuv[1], yuv2[1]+uv_len, uv_len );
	veejay_memcpy( yuv[2], yuv2[2]+uv_len, uv_len );
}
void split_h_first_halfs(uint8_t * yuv[3], uint8_t * yuv2[3], int width,
			 int height)
{
	const unsigned int len = (width * height) / 2;
	const unsigned int uv_len = len / 4;

	veejay_memcpy( yuv[0], yuv2[0], len );
	veejay_memcpy( yuv[1], yuv2[1], uv_len);
	veejay_memcpy( yuv[2], yuv2[2], uv_len);
}
void split_h_second_halfs(uint8_t * yuv[3], uint8_t * yuv2[3], int width,
			  int height)
{
	const unsigned int len = (width * height) / 2;
	const unsigned int uv_len = len / 4;

	veejay_memcpy( yuv[0]+len, yuv2[0], len );
	veejay_memcpy( yuv[1]+uv_len, yuv2[1], uv_len);
	veejay_memcpy( yuv[2]+uv_len, yuv2[2], uv_len);
}

void split_apply(uint8_t * yuv[3], uint8_t * yuv2[3], int width,
		 int height, int n, int swap)
{

    switch (n) {
    case 0:
	if (swap)
	    split_push_downscale_uh(yuv2, width, height);
	split_h_first_half(yuv, yuv2, width, height);
	break;
    case 1:
	//if (swap)
	  //  split_push_downscale_lh(yuv2, width, height);
	split_h_second_half(yuv, yuv2, width, height);
	break;
    case 2:
	//if (swap)
	  //  split_push_downscale_lh(yuv2, width, height);
	 /**/ split_h_first_halfs(yuv, yuv2, width, height);
	break;
    case 3:
	if (swap)
	    split_push_downscale_uh(yuv2, width, height);
	 /**/ split_h_second_halfs(yuv, yuv2, width, height);
	break;
    case 4:
	if (swap)
	    split_push_vscale_left(yuv2, width, height);
	 /**/ split_v_first_half(yuv, yuv2, width, height);
	break;
    case 5:
	if (swap)
	    split_push_vscale_right(yuv2, width, height);
	 /**/ split_v_second_half(yuv, yuv2, width, height);
	break;
    case 6:
	if (swap)
	    split_push_vscale_left(yuv2, width, height);
	 /**/ split_v_first_halfs(yuv, yuv2, width, height);
	break;

    case 7:
	//if (swap)
	    split_push_vscale_right(yuv2, width, height);
	 // split_v_second_halfs(yuv, yuv2, width, height);
	break;
    case 8:
	if (swap)
	    split_fib_downscale(yuv2, width, height);
	split_corner_yuvdata_ul(yuv, yuv2, width, height);
	break;
    case 9:
	if (swap)
	    split_fib_downscale(yuv2, width, height);
	split_corner_yuvdata_ur(yuv, yuv2, width, height);
	break;
    case 10:
	if (swap)
	    split_fib_downscaleb(yuv2, width, height);
	 /**/ split_corner_yuvdata_dr(yuv, yuv2, width, height);
	break;
    case 11:
	if (swap)
	    split_fib_downscaleb(yuv2, width, height);
	 /**/ split_corner_yuvdata_dl(yuv, yuv2, width, height);
	break;
    case 12:
	split_push_vscale_left(yuv2, width, height);
	 /**/ split_push_vscale_right(yuv, width, height);
	split_v_first_half(yuv, yuv2, width, height);
	break;
    case 13:
	split_push_downscale_uh(yuv2, width, height);
	 // split_push_downscale_lh(yuv, width, height);
	split_h_first_half(yuv, yuv2, width, height);
	break;
    }

}
