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

///Number of voxels to be interpreted as one map subcell.
#define VOXGRAIN 8

/* Number of chunks in current active buffers.
 *  1: One chunk right there, simplest scenario.
 *  9: One 3x3 plane of chunks, most widely used scenario
 * 18: Two 3x3 planes (one right there, and one underneath)
 * 27: Full set of 3x3x3 (the most memory hungry scenario)
 */
#define HOLDCHUNKS 27

///Number of intial voxel types.
#define DEFVOXTYPES 1024

///Number of maximum voxel types allowed.
#define MAXVOXTYPES 32000

///Main voxel data type. Used as index.
typedef unsigned short int voxel; //must be a 16-bit wide

///Voxel physical type.
#define NUMVOXTYPES 10
enum EVoxelType {
	VOXT_EMPTY = 0,
	VOXT_WATER = 1,
	VOXT_SAND = 2,
	VOXT_STONE = 3,
	VOXT_DIRT = 4,
	VOXT_GRASS = 5,
	VOXT_METAL = 6,
	VOXT_GLASS = 7,
	VOXT_PLAST = 8,
	VOXT_WOOD = 9,
	//append here
};

///Voxel index info structure.
struct SVoxelInf {
	EVoxelType type;		//Physical type of the voxel.
	SGUIPixel pix;			//Color format used to display a voxel.
	char sides[6];			//Symbols for each of six sides of a voxel cube.
	char* mark;				//Mark. A voxel's 'name'.
	void* texture;			//A 'texture'. May be anything.
	bool not_used;			//A flag used to implement dynamic memory management.
};

///Unified voxel table structure.
struct SVoxelTab {
	SVoxelInf* tab;			//The table (array of SVoxelInf) itself.
	unsigned len;			//Length of allocated table.
	unsigned rlen;			//Real table length (number of voxels defined).
	int stat[NUMVOXTYPES];	//Statistic. Number of voxels defined for each voxel type.
};

///Chunk data type {[z][y][x]}
typedef voxel VChunk[CHUNKBOX][CHUNKBOX][CHUNKBOX];
typedef VChunk * PChunk;


#endif /* VOXEL_H_ */
