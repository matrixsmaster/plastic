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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datapipe.h"
#include "support.h"
#include "vecmisc.h"


DataPipe::DataPipe(const SGameSettings* sets)
{
	int i;
	size_t sz;
	char tmp[MAXPATHLEN];

	/* Init variables */
	status = DPIPE_ERROR;
	pthread_mutex_init(&vmutex,NULL);
	memset(chunks,0,sizeof(chunks));
	allocated = 0;
	memset(root,0,MAXPATHLEN);
	rammax = sets->rammax;

	/* Copy root path */
	i = strlen(sets->root);
	if (i > 0) memcpy(root,sets->root,i);
	else return;

	/* Create and init the world generator */
	wgen = new WorldGen(sets->world_r);
	snprintf(tmp,MAXPATHLEN,"%s/usr/worldmap",root);
	if (!wgen->LoadMap(tmp)) {
		wgen->NewMap((sets->wg_seed)? sets->wg_seed:rand());
		wgen->SaveMap(tmp);
	}

	/* Allocate voxel info table */
	sz = DEFVOXTYPES * sizeof(SVoxelInf);
	voxtablen = DEFVOXTYPES;
	voxeltab = (SVoxelInf*)malloc(sz);
	if (!voxeltab) {
		errout("Unable to allocate RAM for voxel table.\n");
		return;
	}
	memset(voxeltab,0,sz);
	allocated += sz;

	/* Load external files */
	if (!LoadVoxTab()) return;	//Voxel table
	ScanFiles();				//map known chunks

	/* Allocate chunks buffers memory */
	sz = sizeof(VChunk);
	for (i = 0; i < HOLDCHUNKS; i++) {
		chunks[i] = (PChunk)malloc(sz);
		if (!chunks[i]) {
			errout("Unable to allocate RAM for voxel chunk %d.\n",i);
			return; //destructor will purge all the chunks
		}
		allocated += sz;
		if (allocated >= rammax) {
			errout("Maximum amount of memory allowed to be used is reached.\n");
			return;
		}
	}

	/* All clear. */
	status = DPIPE_NOTREADY;
}

DataPipe::~DataPipe()
{
	status = DPIPE_NOTREADY;

	delete wgen;
	voxtablen = 0;
	if (voxeltab) free(voxeltab);

	PurgeModels();
	PurgeChunks();

	pthread_mutex_destroy(&vmutex);
}

bool DataPipe::ScanFiles()
{
	//TODO: scan world files to map known chunks
	return true;
}

bool DataPipe::FindChunk(vector3dulli pos, SDataPlacement* res)
{
	if (!res) return false;

	//TODO

	return false; //FIXME
}

bool DataPipe::LoadVoxTab()
{
	FILE* vtf;
	char pth[MAXPATHLEN];
	char fx;
	SVoxelInf cvf;
	int r,n = 0;

	//combine a file path
	snprintf(pth,MAXPATHLEN,"%s/%s",root,VOXTABFILENAME);
	vtf = fopen(pth,"r");
	if (!vtf) return false;

	//read table
	while (!feof(vtf)) {
		r = fscanf(vtf,"%c%d %hd %hd %hd %hd %hd %hd %6c\n",&fx,
				(int*)&(cvf.type),
				&(cvf.pix.fg.r),&(cvf.pix.fg.g),&(cvf.pix.fg.b),
				&(cvf.pix.bg.r),&(cvf.pix.bg.g),&(cvf.pix.bg.b),
				cvf.sides);

		if (r < 9) continue;
		if (fx == 'V') voxeltab[n++] = cvf;
		if (n >= voxtablen) break;
	}

	fclose(vtf);
	return true;
}

void DataPipe::PurgeChunks()
{
	int i;

	Lock();
	for (i = 0; i < HOLDCHUNKS; i++)
		if (chunks[i]) {
			free(chunks[i]);
			chunks[i] = NULL;
			allocated -= sizeof(VChunk);
		}
	Unlock();
}

void DataPipe::SetGP(vector3dulli pos)
{
	SDataPlacement plc;
	gp = pos;
	Lock();

#if HOLDCHUNKS == 1
	/* One chunk right there, simplest scenario */
	if (!FindChunk(pos,&plc))
		wgen->GenerateChunk(chunks[0]);

#elif HOLDCHUNKS == 9
	/* One 3x3 plane of chunks, most widely used scenario */
	int i,j,l;
	vector3dulli cur(pos);
	for (i = -1, l = 0; i < 2; i++) {
		cur.Y = pos.Y + i;
		for (j = -1; j < 2; j++, l++) {
			cur.X = pos.X + j;
			if (!FindChunk(cur,&plc))
				wgen->GenerateChunk(chunks[l]);
		}
	}

	//FIXME: all directions
#elif HOLDCHUNKS == 18
	/* Two 3x3 planes (one right there, and one upper) */

#elif HOLDCHUNKS == 27
	/* Full set of 3x3x3 (the most memory hungry scenario) */

#else
#error "Invalid value of HOLDCHUNKS!"
#endif

	Unlock();
	status = DPIPE_IDLE;
}

bool DataPipe::Move(EGMoveDir dir)
{
	//TODO
	return false;
}

voxel DataPipe::GetVoxel(const vector3di* p)
{
	PChunk ch;
	VModVec::iterator mi;
	voxel tmp;

	if (status != DPIPE_IDLE) return 0;
	Lock();

	/* Check for dynamic objects */
	if (!objs.empty()) {
		for (mi = objs.begin(); mi != objs.end(); ++mi) {
			if (IsPntInsideCubeI(p,(*mi)->GetPosP(),(*mi)->GetBoundSide())) {
				tmp = (*mi)->GetVoxelAt(p);
				if (tmp) {
					Unlock();
					return tmp;
				}
			}
		}
	}

#if HOLDCHUNKS == 1
	/* Simplest case */

	if (	(p->X < 0) || (p->Y < 0) || (p->Z < 0) ||
			(p->X >= CHUNKBOX) || (p->Y >= CHUNKBOX) || (p->Z >= CHUNKBOX) ) {
		Unlock();
		return 0;
	}
	ch = chunks[0];
	tmp = (*ch)[p->Z][p->Y][p->X];
	Unlock();
	return tmp;

#else

	/* Other cases */
	int x,y,z,px,py,pz;
	x = p->X / CHUNKBOX - ((p->X < 0)? 1:0);
	y = p->Y / CHUNKBOX - ((p->Y < 0)? 1:0);
	z = p->Z / CHUNKBOX - ((p->Z < 0)? 1:0);
	px = abs(p->X) % CHUNKBOX;
	py = abs(p->Y) % CHUNKBOX;
	pz = abs(p->Z) % CHUNKBOX;

#if HOLDCHUNKS == 9
	if (	(x < -1) || (y < -1) || (z < 0) ||
			(x >  1) || (y >  1) || (z > 0) ) {
		Unlock();
		return 0;
	}
	++y; ++x; //centerize
	ch = chunks[y*3+x];

	//FIXME: use other chunks
#elif HOLDCHUNKS == 18
#else
#endif

	tmp = (*ch)[pz][py][px];
	Unlock();
	return tmp;
#endif
}

SVoxelInf* DataPipe::GetVoxelI(const vector3di* p)
{
	voxel v = GetVoxel(p);
	if (v < voxtablen) return &voxeltab[v];
	else return NULL;
}

SVoxelInf* DataPipe::GetVInfo(const voxel v)
{
	if (v < voxtablen) return &voxeltab[v];
	else return NULL;
}

VModel* DataPipe::LoadModel(const char* fname, const vector3di pos)
{
	VModel* m;
	char fn[MAXPATHLEN];
	if ((status != DPIPE_IDLE) || (allocated >= rammax)) return NULL;

	m = new VModel();

	snprintf(fn,MAXPATHLEN,"%s/%s",root,fname);
	if (!m->LoadFromFile(fn)) {
		delete m;
		return NULL;
	}

	allocated += m->GetAllocatedRAM();
	m->SetPos(pos);

	Lock();
	objs.push_back(m);
	Unlock();

	return m;
}

bool DataPipe::UnloadModel(const VModel* ptr)
{
	VModVec::iterator it;
	if (!ptr) return false;

	for (it = objs.begin(); it != objs.end(); ++it)
		if ((*it) == ptr) {
			Lock();
			allocated -= (*it)->GetAllocatedRAM();
			delete ((*it));
			objs.erase(it);
			Unlock();
			return true;
		}

	return false;
}

void DataPipe::PurgeModels()
{
	VModVec::iterator mi;

	Lock();
	for (mi = objs.begin(); mi != objs.end(); ++mi) {
		allocated -= (*mi)->GetAllocatedRAM();
		delete (*mi);
	}

	objs.clear();
	Unlock();
}
