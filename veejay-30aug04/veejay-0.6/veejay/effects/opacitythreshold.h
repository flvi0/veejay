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

#ifndef OPACITYTHRESHOLD_H
#define OPACITYTHRESHOLD_H
#include "../vj-effect.h"
#include <sys/types.h>
#include <stdint.h>

vj_effect *opacitythreshold_init();
void opacitythreshold_apply(uint8_t * yuv1[3], uint8_t * yuv2[3],
			    int width, int height, int opacity,
			    int threshold, int t2);
void opacitythreshold_free();
#endif
