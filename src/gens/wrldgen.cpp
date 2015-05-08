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


WorldGen::WorldGen(uli r)
{
	ulli l;

	radius = r;
	rng = new PRNGen(true);
	allocated = 0;
	org_seed = 0;

	wrldsz.X = (int)ceil(2.f * M_PI * radius);
	wrldsz.Y = wrldsz.X;
	wrldsz.Z = (int)radius;
	plane = wrldsz.X * wrldsz.Y;

	//allocate map memory
	l = plane * sizeof(SWGCell);
	map = (SWGCell*)malloc(l);
	memset(map,0,l);
	if (map) allocated = l;
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
}

bool WorldGen::LoadMap(const char* fname)
{
	FILE* mf;

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
	int i,j,k;
	SWGCell* ptr;

	if ((!fname) || (!map)) return;

	snprintf(txtname,MAXPATHLEN,"%s.txt",fname);
	mt = fopen(txtname,"w");
	if (!mt) return;

	fprintf(mt,"World radius = %lu\n",radius);
	fprintf(mt,"World size = [%d, %d, %d]\n",wrldsz.X,wrldsz.Y,wrldsz.Z);
	fprintf(mt,"World map seed = %ld\n",org_seed);

	fputc('\n',mt);
	for (i = 0, ptr = map; i < wrldsz.Y; i++) {
		for (j = 0; j < wrldsz.X; j++, ptr++) {
			for (k = 0; k < WGNUMKINDS; k++)
				if (wrld_tab[k].t == ptr->t)
					fputc(wrld_tab[k].sym,mt);
		}
		fputc('\n',mt);
	}

	fclose(mt);
/*
	mf = fopen(fname,"wb");
	if (!mf) return;

	//TODO

	fclose(mf);
	*/
}

void WorldGen::NewMap(long seed)
{
	int i,j,k;
	SWGMapCount mapcnt[WGNUMKINDS];
	SWGCell* ptr;
	float p,d,fd,maxd;
	vector2di cur,cnt;

	if (!map) return;

	org_seed = seed;
	rng->SetSeed(seed);

	cnt.X = wrldsz.X / 2;
	cnt.Y = wrldsz.Y / 2;
	maxd = cnt.Module();

	for (k = 0; k < WGNUMKINDS; k++) {
		mapcnt[k].cur = 0;
		mapcnt[k].max = plane / 100 * wrld_tab[k].prc;
	}

	for (i = 0, ptr = map; i < wrldsz.Y; i++) {
		for (j = 0; j < wrldsz.X; j++, ptr++) {
			d = (vector2di(j,i) - cnt).Module();
			fd = d / maxd * 100.f;
			for (k = 0; k < WGNUMKINDS; k++) {
				if ((mapcnt[k].max > 0) && (mapcnt[k].cur >= mapcnt[k].max))
					continue;
				p = rng->FloatNum() * fd;
				if (wrld_tab[k].cprob > p) {
					mapcnt[k].cur++;
					ptr->t = wrld_tab[k].t;
					rng->NextNumber();
					ptr->seed = rng->GetSeed();
					ptr->elev = '0';
					break;
				}
			}
		}
	}
}
