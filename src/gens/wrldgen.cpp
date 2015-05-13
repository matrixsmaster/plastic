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


WorldGen::WorldGen(uli r, SVoxelInf* tab, int tablen)
{
	ulli l;

	radius = r;
	vtab = tab;
	vtablen = tablen;
	rng = new PRNGen(false);
	map = NULL;
	allocated = 0;
	org_seed = 0;

	//calculate dimensions of the world
	wrldsz.X = 2 * WG_PI * radius;
	wrldsz.Y = wrldsz.X;
	wrldsz.Z = (int)radius;
	planeside = 2 * WG_PI * (radius - WGAIRCHUNKS);
	plane = planeside * planeside;
	planeX = planeY = planeside;  //spherical world in this case

	//integrate world volume
	volume = 0;
	for (l = 1; l <= radius; l++)
		volume += (2ULL * WG_PI * l) * (2ULL * WG_PI * l);

	//allocate map memory (only if radius is correct)
	if (r >= WGMINRADIUS) {
		l = plane * sizeof(SWGCell);
		map = (SWGCell*)malloc(l);
		memset(map,0,l);
		if (map) allocated = l;
	}

	//reset covering stats
	memset(cover,0,sizeof(cover));
	cities = 0;
	factories = 0;
}

WorldGen::~WorldGen()
{
	delete rng;
	if (map) free(map);
}

void WorldGen::GenerateChunk(PChunk buf)
{
	int x,y,z,t;
	voxel v;
	if (!buf) return;
	//FIXME: for debug only
#if 1
	for (z = 0; z < CHUNKBOX; z++) {
		for (y = 0; y < CHUNKBOX; y++) {
			for (x = 0; x < CHUNKBOX; x++) {
				if (z < 128) {
//					t = (rand() & 3) + 1;
					t = (((y % 2)? x:(x+1)) % 2) + 1 + (z % 2);
					v = (voxel)t;
					if (rand() < RAND_MAX / 10) v = 0;
				} else v = 0;
				if ((x > 108) && (x < 148) && (y > 108) && (y < 148) && (z > 126) && (z < 135)) v = 4;
				(*buf)[z][y][x] = v;
			}
		}
	}

#else

	vector3d crn[4];
	SWGCell* ptr = map + (planeY / 2 * planeX + planeX / 2);
//	z = (CHUNKBOX / WGELEVATIONS) * ptr->elev;

	crn[1].X = 0;
	crn[1].Y = CHUNKBOX;
	crn[1].Z = (CHUNKBOX / WGELEVATIONS) * 0;//ptr->elev;

	ptr++;
	crn[2].X = CHUNKBOX;
	crn[2].Y = CHUNKBOX;
	crn[2].Z = (CHUNKBOX / WGELEVATIONS) * 1;//ptr->elev;

	ptr += planeX;
	crn[3].X = CHUNKBOX;
	crn[3].Y = 0;
	crn[3].Z = (CHUNKBOX / WGELEVATIONS) * 1;//ptr->elev;

	ptr--;
	crn[0].X = 0;
	crn[0].Y = 0;
	crn[0].Z = (CHUNKBOX / WGELEVATIONS) * 1;//ptr->elev;

//	int zi;

	for (y = 0; y < CHUNKBOX; y++) {
		for (x = 0; x < CHUNKBOX; x++) {
			t = InterpolateZQ(crn,x,y);
//			for (zi = 0; zi < CHUNKBOX; zi++) {
//				v = (z+zi < t)? 1:0;
//				(*buf)[zi][y][x] = v;
//			}
			for (z = 0; z < CHUNKBOX; z++) {
				v = (z < t)? 1:0;
				(*buf)[z][y][x] = v;
			}
		}
	}
#endif
}

bool WorldGen::LoadMap(const char* fname)
{
	FILE* mf;
	SWGMapHeader hdr;

	if ((!fname) || (!map)) return false;

	mf = fopen(fname,"rb");
	if (!mf) return false;

	//TODO

	fclose(mf);
	return false;
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
	fprintf(mt,"World surface dimensions = [%d, %d]\n",planeX,planeY);
	fprintf(mt,"World surface area = %llu\n",plane);
	fprintf(mt,"World map seed = %ld\n",org_seed);

	/* Print out the map itself */
	fputc('\n',mt);
	for (i = 0; i < planeY; i++) {
		ptr = map + ((planeY-i-1)*planeX); //invert Y axis
		for (j = 0; j < planeX; j++, ptr++)
			fputc(wrld_code[ptr->t],mt);
		fputc('\n',mt);
	}

	/* Print out some topography information */
	fprintf(mt,"\nTopography map:\n");
	for (i = 0; i < planeY; i++) {
		ptr = map + ((planeY-i-1)*planeX); //invert Y axis
		for (j = 0; j < planeX; j++, ptr++) {
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
	hdr.planeside = planeside;
	hdr.sx = wrldsz.X;
	hdr.sy = wrldsz.Y;
	hdr.sz = wrldsz.Z;
	hdr.seed = org_seed;

/*
	mf = fopen(fname,"wb");
	if (!mf) return;

	//TODO

	fclose(mf);
	*/
}

void WorldGen::NewMap(long seed)
{
	int i,j,k,u,v,w,q,t,elv;
	SWGCell* ptr,*tmp;
	bool flg;
	float df,pr,mm;
	vector2di cnt;

	if (!map) return;

	org_seed = seed;
	rng->SetSeed(seed);

	/* Generate the basic landscape. Rocks and sand and nothing more */
	elv = 1;
	for (i = 0, ptr = map; i < planeY; i++) {
		for (j = 0; j < planeX; j++, ptr++) {
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
		}
	}

	/* Generate rivers and banks */
	u = (int)floor(rng->FloatNum() * (WGRIVERQ * (float)plane / 100.f) + 1);
	for (k = 0; k < u; k++) {
		i = rng->RangedNumber(planeY);
		j = rng->RangedNumber(planeX);

		flg = false;
		do {
			/* Move to a random direction */
			switch (rng->RangedNumber(4)) {
			default:	/* north */	i++; break;
			case 1:		/* east */	j++; break;
			case 2:		/* south */	i--; break;
			case 3:		/* west */	j--; break;
			}

			flg = ((i < 0) || (i >= planeY));
			flg = (flg || (j < 0) || (j >= planeX));

			if (!flg) {
				ptr = map + (i * planeX + j);
				ptr->t = WGCC_WATERONLY;
				ptr->elev = 1; //to flatten the river :)

				/* Create realistic waterside (full eight directions) */
				for (v = 0; v < 8; v++) {
					switch (v) {
					case 0: tmp = ptr - planeX; break;
					case 1: tmp = ptr - planeX + 1; break;
					case 2: tmp = ptr + 1; break;
					case 3: tmp = ptr + planeX + 1; break;
					case 4: tmp = ptr + planeX; break;
					case 5: tmp = ptr + planeX - 1; break;
					case 6: tmp = ptr - 1; break;
					case 7: tmp = ptr - planeX - 1; break;
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
		i = rng->RangedNumber(planeY);
		j = rng->RangedNumber(planeX);
		v = (int)floor(rng->FloatNum() * (WGPLANTSZ * (float)plane / 100.f) + 2);
		w = (int)floor(rng->FloatNum() * (WGPLANTSZ * (float)plane / 100.f) + 2);

		/* Create the bridge from the center point to the west or east shore */
		ptr = map + ((i + v/2) * planeX + (j + w/2));
		while (	(ptr >= map) && (ptr < map + plane) &&
				((ptr->t == WGCC_WATERONLY) || ((ptr->t == WGCC_WATERSIDE))) ) {
			ptr->t = WGCC_CONCRETEB;
			(j > planeX / 2)? ptr--:ptr++;
		}

		/* Fill plant area */
		for (q = i; q < i + v; q++) {
			if (q >= planeY) continue; //check position

			ptr = map + (q * planeX + j);
			for (t = j; t < j + w; t++, ptr++) {
				if (t >= planeX) break; //check position

				ptr->t = (rng->FloatNum() < 0.5)? WGCC_CONCRETEB:WGCC_SPECBUILD;
				ptr->elev = 1;
			}
		}
	}

	/* Generate cities */
	u = (int)floor(rng->FloatNum() * (WGCITYQ * (float)plane / 100.f) + 1);
	for (k = 0; k < u; k++) {
		i = rng->RangedNumber(planeY);
		j = rng->RangedNumber(planeX);
		v = (int)floor(rng->FloatNum() * (WGCITYSZ * (float)plane / 100.f) + 2);
		w = (int)floor(rng->FloatNum() * (WGCITYSZ * (float)plane / 200.f) + v);
		if ((i+v >= planeY) || (j+w >= planeX)) continue; //check position

		cities++;
		cnt = vector2di(w/2,v/2);
		mm = cnt.Module();
		cnt += vector2di(j,i);
		elv = rng->RangedNumber(3);

		for (q = i; q < i + v; q++) {
			ptr = map + (q * planeX + j);
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
	for (i = 0, ptr = map; i < planeY; i++)
		for (j = 0; j < planeX; j++, ptr++) {
			u = rng->RangedNumber(100);
			for (k = 0; k < u; k++)
				rng->NextNumber(); //trash out one more number
			ptr->seed = rng->GetSeed();
			cover[ptr->t]++;
		}
}
