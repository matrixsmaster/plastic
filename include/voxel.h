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

#ifndef VOXEL_H_
#define VOXEL_H_

#include "visual.h"

///Number of voxels at a side of a chunk.
#define CHUNKBOX 256

///Number of voxels to be interpreted as one map cell.
#define VOXGRAIN 8

///Number of chunks in current active buffers.
#define HOLDCHUNKS 1

///Number of intial voxel types.
#define DEFVOXTYPES 1024

///Main voxel data type. Used as index.
typedef unsigned short int voxel; //must be a 16-bit wide

///Voxel physical type.
#define NUMVOXTYPES 10
enum EVoxelType {
	VOXT_EMPTY = 0,
	VOXT_SAND,
	VOXT_STONE,
	VOXT_DIRT,
	VOXT_GRASS,
	VOXT_WATER,
	VOXT_METAL,
	VOXT_GLASS,
	VOXT_PLAST,
	VOXT_WOOD,
	//append here
};

///Voxel index info structure.
struct SVoxelInf {
	EVoxelType type;
	SGUIPixel pix;
	char sides[6];
};

///Unified voxel table structure.
struct SVoxelTab {
	SVoxelInf* tab;
	unsigned len;
	int stat[NUMVOXTYPES];
};

///Chunk data type {[z][y][x]}
typedef voxel VChunk[CHUNKBOX][CHUNKBOX][CHUNKBOX];
typedef VChunk * PChunk;


#endif /* VOXEL_H_ */
