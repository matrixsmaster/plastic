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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "wrldgen.h"
#include "vecmath.h"
#include "vecmisc.h"
//#include "debug.h" //FIXME: debug


WorldGen::WorldGen(uli r, SVoxelTab* tab)
{
	//init variables
	radius = r;
	vtab = tab;
	rng = new PRNGen(false);
	map = NULL;
	allocated = 0;
	org_seed = 0;
	plane = 0;
	planeside = 0;
	volume = 0;
	factories = 0;
	cities = 0;

	//calculate dimensions of the world
	wrldsz.X = 2UL * WG_PI * radius;
	wrldsz.Y = wrldsz.X;
	wrldsz.Z = (int)radius;

	ResetVolume();
}

WorldGen::~WorldGen()
{
	delete rng;
	if (map) free(map);
}

void WorldGen::ResetVolume()
{
	/* calculate areas and volume
	 * this set of variables is capable of working
	 * with the other geometry forms of the worlds
	 */
	planeside = wrldsz.X;
	plane = planeside * planeside;
	planev = vector2di(planeside);  //rectangular area
	volume = wrldsz.X * wrldsz.Y * wrldsz.Z;
}

SWGCell WorldGen::GetCell(vector3di crd)
{
	SWGCell cl;
	int z;
	uli l;

	memset(&cl,0,sizeof(cl));
	WrapCoords(&crd);

	if ((crd.Z == 1) || (crd.Z >= (int)(radius - WGAIRCHUNKS))) {
		//core air pocket or air above the surface
		cl.chunkt = WGCT_AIR;
	} else {
		l = crd.Y * planev.X + crd.X;
		//create seed linked with surface cell seed
		//TODO

		//get surface level at point
		z = map[l].elev;
		z = radius - WGAIRCHUNKS - WGELEVATIONS + z;

		if (crd.Z < z) {
			//water-only chunks under water-only surface cells
			if (map[l].t == WGCC_WATERONLY)
				cl.chunkt = WGCT_WATER;
			else
				//regular underground otherwise
				cl.chunkt = WGCT_UNDERGR;
		} else if (crd.Z == z)
			//surface level, surface cell
			cl = map[l];
		else
			//above the surface
			cl.chunkt = WGCT_AIR;
	}

	return cl;
}

SWGCell WorldGen::GetSurfaceCell(vector3di crd)
{
	SWGCell cl;

	WrapCoords(&crd);
	cl = map[crd.Y * planev.X + crd.X];

	return cl;
}

void WorldGen::WrapCoords(vector3di* pnt)
{
	pnt->X = pnt->X % plane;
	pnt->Y = pnt->Y % plane;
	pnt->Z = (pnt->Z == 0)? 1:abs(pnt->Z);

	if (pnt->X < 0) pnt->X += plane;
	if (pnt->Y < 0) pnt->Y += plane;
}

voxel WorldGen::GetVoxelOfType(EVoxelType t)
{
	voxel i;
	int j,n;

	if (!vtab) return 0;

	n = (int)floor((float)(vtab->stat[t]) * rng->FloatNum());

	for (i = 0, j = 0; i < vtab->len; i++)
		if (vtab->tab[i].type == t) {
			if (j++ == n) return i;
		}

	return 0;
}

void WorldGen::CalcChunkElevRect(vector3d* arr, const vector3di chpos, int qx, int qy)
{
	int i,t;
	vector2di tmp;
	vector3d shf(qx*CHUNKBOX/2,qy*CHUNKBOX/2,0); //quarter shift off center

	//shift quarter shift itself, to make easy to use it as a simple vector offset
	if (qx > 0) qx = 0;
	if (qy > 0) qy = 0;

	//fill rectangle shifted to quarter provided
	for (i = 0; i < 4; i++) {
		tmp = RectangleCornerK(i);
		t = GetSurfaceCell(chpos+vector3di(qx+tmp.X,qy+tmp.Y,0)).elev;
//		dbg_print("i=%d qx=%d qy=%d tx=%d ty=%d",i,qx,qy,tmp.X+qx,tmp.Y+qy);
		arr[i].X = tmp.X * CHUNKBOX;
		arr[i].Y = tmp.Y * CHUNKBOX;
		arr[i].Z = t * CHUNKBOX + CHUNKBOX / WGELEVHFACTOR;
		arr[i] += shf;
	}
}

void WorldGen::GenerateChunk(PChunk buf, vector3di pos)
{
	int x,y,z,t;
	voxel v,vgr;
	SWGCell cell;
	vector3d crn[4],pnt,tmp;
	voxel grains[CHUNKBOX/VOXGRAIN][CHUNKBOX/VOXGRAIN][CHUNKBOX/VOXGRAIN];

	if (!buf) return;

	//set memory
	memset(grains,0,sizeof(grains));
	cell = GetCell(pos);

	//prepare large 'grains' of voxels
	for (z = 0; z < (CHUNKBOX/VOXGRAIN); z++)
		for (y = 0; y < (CHUNKBOX/VOXGRAIN); y++)
			for (x = 0; x < (CHUNKBOX/VOXGRAIN); x++)
				grains[z][y][x] = GetVoxelOfType(VOXT_SAND); //FIXME: use cell type

	switch (cell.chunkt) {
	case WGCT_SURFACE:

		//set RNG seed
		rng->SetSeed(cell.seed);

		//generate chunk data
		for (y = 0; y < CHUNKBOX; y++) {
			pnt.Y = y;
			for (x = 0; x < CHUNKBOX; x++) {
				//calculate landscape height in current point
				pnt.X = x;
				if ((x == 0) || (x == CHUNKBOX/2))
					//if moved to another quarter, recalc corners rectangle
					CalcChunkElevRect(crn,pos,((x < CHUNKBOX/2)? -1:1),((y < CHUNKBOX/2)? -1:1));
				//get bilinear interpolation of landscape
				tmp = BilinearInterpolation(crn,&pnt);
				t = (int)(floor(tmp.Z)) - (cell.elev * CHUNKBOX);

				//go through voxel column
				for (z = 0; z < CHUNKBOX; z++) {
					vgr = grains[z/VOXGRAIN][y/VOXGRAIN][x/VOXGRAIN];
					//apply some noise
					if (rng->RangedNumber(100) < 5) vgr = GetVoxelOfType(VOXT_SAND); //FIXME: use cell type

					//set the actual data
					v = (z < t)? vgr:0;
					(*buf)[z][y][x] = v;
				}
			}
		}
		break;

	case WGCT_UNDERGR:
		for (z = 0; z < CHUNKBOX; z++)
			for (y = 0; y < CHUNKBOX; y++)
				for (x = 0; x < CHUNKBOX; x++)
					(*buf)[z][y][x] = grains[z/VOXGRAIN][y/VOXGRAIN][x/VOXGRAIN];
		break;

	case WGCT_WATER:
		v = GetVoxelOfType(VOXT_WATER);
		for (z = 0; z < CHUNKBOX; z++)
			for (y = 0; y < CHUNKBOX; y++)
				for (x = 0; x < CHUNKBOX; x++)
					(*buf)[z][y][x] = v;
		break;

	default: break;
	}

	rng->TimeSeed(); //reset seed
}

bool WorldGen::LoadMap(const char* fname)
{
	FILE* mf;
	SWGMapHeader hdr;

	if (!fname) return false;

	mf = fopen(fname,"rb");
	if (!mf) return false;

	fread(&hdr,sizeof(hdr),1,mf);

	fclose(mf);

	//apply loaded data
	radius = hdr.radius;
	wrldsz = vector3di(hdr.sx,hdr.sy,hdr.sz);

	ResetVolume();
	NewMap(hdr.seed);

	return true;
}

void WorldGen::SaveMap(const char* fname)
{
	char txtname[MAXPATHLEN];
	FILE* mf,*mt;
	int i,j;
	SWGCell* ptr;
	SWGMapHeader hdr;

	if ((!fname) || (!map)) return;

	/* Store the map information in human-readable form (for reference) */
	snprintf(txtname,MAXPATHLEN,"%s.txt",fname);
	mt = fopen(txtname,"w");
	if (!mt) return;

	/* Print out basic information about the map */
	fprintf(mt,"World radius = %lu\n",radius);
	fprintf(mt,"World bounding box size = [%d, %d, %d]\n",wrldsz.X,wrldsz.Y,wrldsz.Z);
	fprintf(mt,"World surface area = %llu\n",plane);
	fprintf(mt,"World map seed = %ld\n",org_seed);

	/* Print out the map itself */
	fputc('\n',mt);
	for (i = 0; i < planev.Y; i++) {
		ptr = map + ((planev.Y-i-1)*planev.X); //invert Y axis
		for (j = 0; j < planev.X; j++, ptr++)
			fputc(wrld_code[ptr->t],mt);
		fputc('\n',mt);
	}

	/* Print out some topography information */
	fprintf(mt,"\nTopography map:\n");
	for (i = 0; i < planev.Y; i++) {
		ptr = map + ((planev.Y-i-1)*planev.X); //invert Y axis
		for (j = 0; j < planev.X; j++, ptr++) {
			switch (ptr->elev) {
			case 0: fputc('-',mt); break;
			case 2: fputc('+',mt); break;
			default: fputc('0',mt);
			}
		}
		fputc('\n',mt);
	}

	fclose(mt);

	/* Store the actual usable binary map information */
	hdr.radius = radius;
	hdr.sx = wrldsz.X;
	hdr.sy = wrldsz.Y;
	hdr.sz = wrldsz.Z;
	hdr.seed = org_seed;

	mf = fopen(fname,"wb");
	if (!mf) return;

	fwrite(&hdr,sizeof(hdr),1,mf);

	fclose(mf);
}

void WorldGen::NewMap(long seed)
{
	int i,j,k,u,v,w,q,t,elv;
	SWGCell* ptr,*tmp;
	bool flg;
	float df,pr,mm;
	vector2di cnt;
	ulli l;

	/* Allocate map memory */
	if (radius < WGMINRADIUS) return;
	l = plane * sizeof(SWGCell);
	map = (SWGCell*)realloc(map,l);
	if (!map) return;

	allocated = l;
	memset(map,0,l);

	/* Reset covering stats */
	memset(cover,0,sizeof(cover));
	cities = 0;
	factories = 0;

	/* Reset initial Player position */
	pcpos = vector3di(0);

	/* Prepare RNG */
	org_seed = seed;
	rng->SetSeed(seed);

	/* Generate the basic landscape. Rocks and sand and nothing more */
	elv = 1;
	for (i = 0, ptr = map; i < planev.Y; i++) {
		for (j = 0; j < planev.X; j++, ptr++) {
			switch (rng->RangedNumber(3)) {
			case 1: ptr->t = WGCC_ROCKSTONE; break;
			case 2: ptr->t = WGCC_DIRTNSAND; break;
			default: ptr->t = WGCC_WASTELAND;
			}
			//height mapping:
			if (rng->FloatNum() > 0.5) {
				if (--elv < 0) elv = 0;
			} else if (rng->FloatNum() > 0.5) {
				if (++elv > 2) elv = 2;
			}
			ptr->elev = elv;
			//type of chunk here
			ptr->chunkt = WGCT_SURFACE;
		}
	}

	/* Generate rivers and banks */
	u = (int)floor(rng->FloatNum() * (WGRIVERQ * (float)plane / 100.f) + 1);
	for (k = 0; k < u; k++) {
		i = rng->RangedNumber(planev.Y);
		j = rng->RangedNumber(planev.X);

		flg = false;
		l = 0;
		do {
			/* Move to a random direction */
			switch (rng->RangedNumber(4)) {
			default:	/* north */	i++; break;
			case 1:		/* east */	j++; break;
			case 2:		/* south */	i--; break;
			case 3:		/* west */	j--; break;
			}

			flg = ((i < 0) || (i >= planev.Y));
			flg = (flg || (j < 0) || (j >= planev.X));
			flg = (flg || (++l > WGRIVERMAX));

			if (!flg) {
				ptr = map + (i * planev.X + j);
				ptr->t = WGCC_WATERONLY;
				ptr->elev = 0; //to flatten the river :)

				/* Create realistic waterside (full eight directions) */
				for (v = 0; v < 8; v++) {
					switch (v) {
					case 0: tmp = ptr - planev.X; break;
					case 1: tmp = ptr - planev.X + 1; break;
					case 2: tmp = ptr + 1; break;
					case 3: tmp = ptr + planev.X + 1; break;
					case 4: tmp = ptr + planev.X; break;
					case 5: tmp = ptr + planev.X - 1; break;
					case 6: tmp = ptr - 1; break;
					case 7: tmp = ptr - planev.X - 1; break;
					}
					if ((tmp < map) || (tmp >= map + plane))
						continue;
					if (tmp->t != WGCC_WATERONLY)
						tmp->t = WGCC_WATERSIDE;
				}
			}

		} while (!flg);
	}

	/* Generate plants and industrial areas */
	u = (int)floor(rng->FloatNum() * (WGPLANTQ * (float)plane / 100.f) + 1);
	factories += u;
	for (k = 0; k < u; k++) {
		i = rng->RangedNumber(planev.Y);
		j = rng->RangedNumber(planev.X);
		v = (int)floor(rng->FloatNum() * (WGPLANTSZ * (float)plane / 100.f) + 2);
		w = (int)floor(rng->FloatNum() * (WGPLANTSZ * (float)plane / 100.f) + 2);

		/* Create the bridge from the center point to the west or east shore */
		ptr = map + ((i + v/2) * planev.X + (j + w/2));
		while (	(ptr >= map) && (ptr < map + plane) &&
				((ptr->t == WGCC_WATERONLY) || ((ptr->t == WGCC_WATERSIDE))) ) {
			ptr->t = WGCC_CONCRETEB;
			(j > planev.X / 2)? ptr--:ptr++;
		}

		/* Fill plant area */
		for (q = i; q < i + v; q++) {
			if (q >= planev.Y) continue; //check position

			ptr = map + (q * planev.X + j);
			for (t = j; t < j + w; t++, ptr++) {
				if (t >= planev.X) break; //check position

				ptr->t = (rng->FloatNum() < 0.5)? WGCC_CONCRETEB:WGCC_SPECBUILD;
				ptr->elev = 1;

				//store initial player position helper
				if (pcpos == vector3di(0)) {
					pcpos.X = t;
					pcpos.Y = q;
					pcpos.Z = radius - WGAIRCHUNKS - WGELEVATIONS + ptr->elev;
				}
			}
		}
	}

	/* Generate cities */
	u = (int)floor(rng->FloatNum() * (WGCITYQ * (float)plane / 100.f) + 1);
	for (k = 0; k < u; k++) {
		i = rng->RangedNumber(planev.Y);
		j = rng->RangedNumber(planev.X);
		v = (int)floor(rng->FloatNum() * (WGCITYSZ * (float)plane / 100.f) + 2);
		w = (int)floor(rng->FloatNum() * (WGCITYSZ * (float)plane / 200.f) + v);
		if ((i+v >= planev.Y) || (j+w >= planev.X)) continue; //check position

		cities++;
		cnt = vector2di(w/2,v/2);
		mm = cnt.Module();
		cnt += vector2di(j,i);
		elv = rng->RangedNumber(3);

		for (q = i; q < i + v; q++) {
			ptr = map + (q * planev.X + j);
			for (t = j; t < j + w; t++, ptr++) {
				df = ((vector2di(t,q) - cnt).Module()) / mm;
				pr = (rng->FloatNum() - 0.5) * WGCITYNOISE;
				df += pr;
				if (df < WGCITYCNTS) {
					//center
					ptr->t = WGCC_HUGEBUILD;
				} else if (df < WGCITYENDS) {
					//ends
					ptr->t = WGCC_MIDDLBLDS;
				} else if (df < WGCITYSUBS) {
					//suburbs
					ptr->t = WGCC_SMALLBLDS;
				} else if (rng->FloatNum() > 0.5) {
					//parks; don't touch industrial sites
					if ((ptr->t != WGCC_CONCRETEB) && (ptr->t != WGCC_SPECBUILD))
						ptr->t = WGCC_TREEGRASS;
				} else
					continue; //or just doesn't touch anything
				ptr->elev = elv;
			}
		}
	}

	/* Store RNG data and calculate covering */
	for (i = 0, ptr = map; i < planev.Y; i++)
		for (j = 0; j < planev.X; j++, ptr++) {
			u = rng->RangedNumber(100);
			for (k = 0; k < u; k++)
				rng->NextNumber(); //trash out one more number
			ptr->seed = rng->GetSeed();
			cover[ptr->t]++;
		}
}
