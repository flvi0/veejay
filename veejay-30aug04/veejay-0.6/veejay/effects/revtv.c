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

#include "revtv.h"
#include "common.h"

#include <stdlib.h>
vj_effect *revtv_init(int max_width, int max_height)
{
    vj_effect *ve = (vj_effect *) malloc(sizeof(vj_effect));
    ve->num_params = 4;
    ve->defaults = (int *) malloc(sizeof(int) * ve->num_params);	/* default values */
    ve->limits[0] = (int *) malloc(sizeof(int) * ve->num_params);	/* min */
    ve->limits[1] = (int *) malloc(sizeof(int) * ve->num_params);	/* max */
    ve->defaults[0] = 2;
    ve->defaults[1] = 42;
    ve->defaults[2] = 201;
    ve->defaults[3] = 6;
    ve->limits[0][0] = 1;	/* line spacing */
    ve->limits[1][0] = max_height;
    ve->limits[0][1] = 1;	/* vscale */
    ve->limits[1][1] = max_width;
    ve->limits[0][2] = 0;	/* luminance intensity */
    ve->limits[1][2] = 255;
    ve->limits[0][3] = 0;	/* color range */
    ve->limits[1][3] = 7;
    ve->description = "RevTV";
    ve->sub_format = 0;
    ve->extra_frame = 0;
    ve->has_internal_data = 0;
    return ve;
}

/**********************************************************************************************
 *
 * revTV: taken from effectv-0.3.5. linespace = 1 gives a nice result.
 * added the variable 'color' , so the user is free to choose replacement result. Default was 0xff.
 *
 **********************************************************************************************/
void revtv_apply(uint8_t * yuv[3], int width, int height, int linespace,
		 int vscale, int color, int color_num)
{
    int x, y;
    uint8_t *nsrc;
    int X1, X2, X3;
    int yval;
    int uv_width = width >> 1;
    int uv_height = height >> 1;

    int colorCb = bl_pix_get_color_cb(color);
    int colorCr = bl_pix_get_color_cr(color);

    for (y = 0; y < height; y += linespace) {
	for (x = 0; x <= width; x++) {
	    nsrc = yuv[0] + (y * width) + x;
	    X1 = ((*nsrc) & 0xff0000) >> (16 - 1);
	    X2 = ((*nsrc) & 0xff00) >> (8 - 2);
	    X3 = (*nsrc) & 0xff;
	    yval = y - ((short) (X1 + X2 + X3) / vscale);
	    if (yval > 0)
		yuv[0][x + (yval * width)] = color;
	}
    }
    if (color_num > 0) {
	for (y = 0; y < uv_height; y += linespace) {
	    for (x = 0; x <= uv_width; x++) {
		nsrc = yuv[1] + (y * uv_width) + x;
		X1 = ((*nsrc) & 0xff0000) >> (8 - 1);
		X2 = ((*nsrc) & 0xff00) >> (4 - 2);
		X3 = ((*nsrc) & 0xff) >> 1;
		yval = y - ((short) (X1 + X2 + X3) / vscale);
		if (yval > 0)
		    yuv[1][x + (yval * uv_width)] = colorCr;
		nsrc = yuv[2] + (y * uv_width) + x;
		X1 = ((*nsrc) & 0xff0000) >> (8 - 1);
		X2 = ((*nsrc) & 0xff00) >> (4 - 2);
		X3 = ((*nsrc) & 0xff) >> 1;
		yval = y - ((short) (X1 + X2 + X3) / vscale);
		if (yval > 0)
		    yuv[2][x + (yval * uv_width)] = colorCb;
	    }
	}
    }
}
void revtv_free(){}
