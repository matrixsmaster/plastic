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

#ifndef WRLDGEN_H_
#define WRLDGEN_H_

#include "voxel.h"
#include "misconsts.h"
#include "vecmath.h"
#include "prngen.h"

#define WGCHUNKSZ (CHUNKBOX / VOXGRAIN)

/* Map cell content type (surface cells) */
#define WGNUMKINDS 10

enum EWGCellContent {
	WGCC_WASTELAND,	//wasteland - nothing but sand
	WGCC_DIRTNSAND, //dirt and sand
	WGCC_ROCKSTONE,	//rocks and stones
	WGCC_WATERSIDE, //waterside areas
	WGCC_WATERONLY,	//nothing but water
	WGCC_TREEGRASS, //just a grass plus some trees
	WGCC_SMALLBLDS, //3 or 4 small buildings
	WGCC_MIDDLBLDS, //2 middle-size buildings
	WGCC_HUGEBUILD,	//one huge building right in the center
	WGCC_SPECBUILD	//special (factory, plant) building
};

/* Map parameters for each content type */
struct SWGMapParam {
	EWGCellContent t;
	char sym;
	float prc;
	float cprob;		//probability of presence v. distance
};

/* Counting helper for map generation */
struct SWGMapCount {
	int cur,max;
};

/* Distribution table (max percent, sorted by) */
static const SWGMapParam wrld_tab[WGNUMKINDS] = {
		{ WGCC_HUGEBUILD, 'L', 2,   1 },
		{ WGCC_MIDDLBLDS, 'M', 5,   3 },
		{ WGCC_SMALLBLDS, 'S', 10,  5 },
		{ WGCC_SPECBUILD, 'P', 4,   7 },
		{ WGCC_TREEGRASS, '$', 10, 20 },
		{ WGCC_WATERONLY, '~', 10, 25 },
		{ WGCC_WATERSIDE, '%', 9,  30 },
		{ WGCC_DIRTNSAND, '#', 4,  41 },
		{ WGCC_ROCKSTONE, '^', 5,  50 },
		{ WGCC_WASTELAND, '.', 0, 100 },
};

/* Map cell data */
struct SWGCell {
	EWGCellContent t;
	char elev;
	long seed;
};

//enum EWGChunkType {
//	WGCT_AIR,
//	WGCT_SURFACE,
//	WGCT_UNDERGR,
//	WGCT_WATER
//};
//
///* Map chunk data */
//struct SWGChunk {
//	EWGChunkType t;
//	EWGCellContent c;
//	long seed;
//	voxel lead;
//};

class WorldGen {
private:
	uli radius;
	PRNGen* rng;
	ulli allocated;
	long org_seed;
	vector3di wrldsz;
	ulli plane;
	SWGCell* map;

public:
	WorldGen(uli r);
	virtual ~WorldGen();

	bool LoadMap(const char* fname);
	void SaveMap(const char* fname);
	void NewMap(long seed);

	ulli GetAllocatedRAM()			{ return allocated; }

	void GenerateChunk(PChunk buf);
};

#endif /* WRLDGEN_H_ */
