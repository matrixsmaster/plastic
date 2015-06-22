/**
 *  Plastic Inquisitor
 *  Copyright (C) 2015 The Plastic Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Lightweight Voxel Renderer constants and types */

#ifndef LVRCONSTS_H_
#define LVRCONSTS_H_

#include "voxel.h"
#include "visual.h"


///Define this to use debugging output.
//#define LVRDEBUG 1

///Define this to use double-buffering in LVR.
///It's required to be enabled if using RenderPool.
#define LVRDOUBLEBUFFERED 1

///Set of main settings defaults.
#define DEFSCALE 1.f
#define DEFFOVX 115
#define DEFFOVY 62
#define DEFFARPLANE 80

///LVR Post-processing settings.
struct SLVRPostProcess {
	int fog_dist;
	SCTriple fog_col;
	int noise;
	int txd_fplane;
	int txd_nplane;
	int txd_minw,txd_minh;
};

///Default post-processing.
#define DEFPOSTPROC { \
	40, \
	{40, 40, 40}, \
	0, \
	20, 5, 3, 3, \
}

#endif /* LVRCONSTS_H_ */
