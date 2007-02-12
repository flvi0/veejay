/* 
 * Linux VeeJay
 *
 * Copyright(C)2007 Niels Elburg <elburg@hio.hen.nl>
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
#include "colorhis.h"
#include <stdlib.h>
#include <ffmpeg/avutil.h>
#include <libyuv/yuvconv.h>
#include <veejay/vj-global.h>
#include "common.h"
vj_effect *colorhis_init(int w, int h)
{
    vj_effect *ve = (vj_effect *) vj_calloc(sizeof(vj_effect));
    ve->num_params = 4;

    ve->defaults = (int *) vj_calloc(sizeof(int) * ve->num_params);	/* default values */
    ve->limits[0] = (int *) vj_calloc(sizeof(int) * ve->num_params);	/* min */
    ve->limits[1] = (int *) vj_calloc(sizeof(int) * ve->num_params);	/* max */
    ve->limits[0][0] = 0;
    ve->limits[1][0] = 3;
    ve->limits[0][1] = 0;
    ve->limits[1][1] = 1;
    ve->limits[0][2] = 0;
    ve->limits[1][2] = 255;
    ve->limits[0][3] = 0;
    ve->limits[1][3] = 255;

    ve->defaults[0] = 0; // r only, g only, b only, rgb
    ve->defaults[1] = 0; // draw
    ve->defaults[2] = 200; // intensity
    ve->defaults[3] = 132; // strength

    ve->description = "Color Histogram";
    ve->sub_format = 1;
    ve->extra_frame = 0;
    ve->has_user = 0;
    return ve;
}

static void	*histogram_ = NULL;
static	VJFrame *rgb_frame_ = NULL;
static uint8_t  *rgb_ = NULL;

int	colorhis_malloc(int w, int h)
{
	if( histogram_ )
		veejay_histogram_del(histogram_);
	if( rgb_ )
		free(rgb_);
	histogram_ = veejay_histogram_new();
	rgb_ = vj_malloc(sizeof(uint8_t) * w * h * 3 );
	rgb_frame_ = yuv_rgb_template( rgb_, w, h, PIX_FMT_RGB24 );
	return 1;
}

void	colorhis_free()
{
	if( histogram_ )
		veejay_histogram_del(histogram_);
	if( rgb_ )
		free(rgb_);
	if( rgb_frame_)
		free(rgb_frame_);
	rgb_ = NULL;
	rgb_frame_ = NULL;
	histogram_ = NULL;
}


void colorhis_apply( VJFrame *frame, int width, int height,int mode, int val, int intensity, int strength)
{

	yuv_convert_any( frame, rgb_frame_, PIX_FMT_YUV444P, PIX_FMT_RGB24 );

	if( val == 0 )
	{
		veejay_histogram_draw_rgb( histogram_, frame, rgb_, intensity, strength, mode );
	}
	else
	{
		veejay_histogram_analyze_rgb( histogram_,rgb_, frame );
		veejay_histogram_equalize_rgb( histogram_, frame, rgb_, intensity, strength, mode );
	
	
		yuv_convert_any( rgb_frame_, frame, PIX_FMT_RGB24, PIX_FMT_YUV444P );
	}	
}

