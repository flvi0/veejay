
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

#include <stdlib.h>
#include "frameborder.h"
#include "common.h"

vj_effect *frameborder_init(int width, int height)
{

    vj_effect *ve = (vj_effect *) malloc(sizeof(vj_effect));
    ve->num_params = 1;
    ve->defaults = (int *) malloc(sizeof(int) * ve->num_params);	/* default values */
    ve->limits[0] = (int *) malloc(sizeof(int) * ve->num_params);	/* min */
    ve->limits[1] = (int *) malloc(sizeof(int) * ve->num_params);	/* max */
    ve->has_internal_data = 0; 
    ve->defaults[0] = 1;
    ve->limits[0][0] = 1;
    ve->limits[1][0] = height / 2;

    ve->description = "Frame Border Translation";
    ve->sub_format = 0;
    ve->extra_frame = 1;
    return ve;
}


void frameborder_apply(uint8_t * yuv1[3], uint8_t * yuv2[3], int width,
		       int height, int size)
{
    frameborder_yuvdata(yuv1[0], yuv1[1], yuv1[2], yuv2[0], yuv2[1],
			yuv2[2], width, height, (size), (size), (size),
			(size));

}
void frameborder_free(){}
