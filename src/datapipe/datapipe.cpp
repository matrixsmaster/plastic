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


DataPipe::DataPipe(SGameSettings* sets, bool allocate)
{
	int i;

	/* Init variables */
	status = DPIPE_ERROR;
	pthread_mutex_init(&vmutex,NULL);
	memset(chunks,0,sizeof(chunks));
	memset(chstat,0,sizeof(chstat));
	allocated = 0;
	memset(root,0,MAXPATHLEN);
	wgen = NULL;
	memset(&voxeltab,0,sizeof(voxeltab));
	rammax = sets->rammax;

	/* Copy root path */
	i = strlen(sets->root);
	if (i > 0) memcpy(root,sets->root,i);
	else return;

	/* Allocate everything */
	if (allocate) {
		if (!Allocator(sets)) return;
	}

	/* OK */
	status = DPIPE_NOTREADY;
}

DataPipe::~DataPipe()
{
	status = DPIPE_NOTREADY;

	if (wgen) delete wgen;
	if (voxeltab.tab) free(voxeltab.tab);

	PurgeSprites();
	PurgeModels();
	PurgeChunks();

	pthread_mutex_destroy(&vmutex);
}

bool DataPipe::Allocator(SGameSettings* sets)
{
	int i;
	size_t sz;
	char tmp[MAXPATHLEN];

	/* Allocate voxel info table */
	sz = DEFVOXTYPES * sizeof(SVoxelInf);
	voxeltab.len = DEFVOXTYPES;
	voxeltab.tab = (SVoxelInf*)malloc(sz);
	if (!voxeltab.tab) {
		errout("Unable to allocate RAM for voxel table.\n");
		return false;
	}
	memset(voxeltab.tab,0,sz);
	allocated += sz;

	/* Create and init the world generator */
	if (sets->world_r < WGMINRADIUS) {
		errout("Impossibly small world radius.\n");
		return false;
	}
	wgen = new WorldGen(sets->world_r,&voxeltab);
	snprintf(tmp,MAXPATHLEN,"%s/usr/worldmap",root);

	if ((sets->new_game) || (!wgen->LoadMap(tmp))) {
		wgen->NewMap((sets->wg_seed)? sets->wg_seed:rand());
		wgen->SaveMap(tmp);
	} else
		sets->world_r = wgen->GetRadius(); //override current radius with a saved one
	allocated += wgen->GetAllocatedRAM();

	/* Load external files */
	if (!LoadVoxTab()) {		//Voxel table
		errout("Invalid root path or voxel table file is corrupted.\n");
		return false;
	}
	ScanFiles();				//map known chunks

	/* Allocate chunks buffers memory */
	sz = sizeof(VChunk);
	for (i = 0; i < HOLDCHUNKS; i++) {
		chunks[i] = (PChunk)malloc(sz);
		if (!chunks[i]) {
			errout("Unable to allocate RAM for voxel chunk %d.\n",i);
			return false; //destructor will purge all the chunks
		}
		allocated += sz;
		if (allocated >= rammax) {
			errout("Maximum amount of memory allowed to be used is reached.\n");
			return false;
		}
	}

	//TODO: load Player data
	if (!sets->new_game) {
		//...
	}

	/* All clear. */
	return true;
}

bool DataPipe::ScanFiles()
{
	//TODO: scan world files to map known chunks
	//TODO: if new game, delete these files
	return true;
}

bool DataPipe::FindChunk(vector3di pos, SDataPlacement* res)
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
	int r;
	unsigned n = 0;

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

		if (r < 9) continue; //check number of successfully parsed params

		if (fx == 'V') {
			if ((cvf.type >= 0) && (cvf.type < NUMVOXTYPES))
				voxeltab.stat[cvf.type]++;
			else {
				errout("Unknown voxel type %d\n",(int)cvf.type);
				fclose(vtf);
				return false;
			}
			voxeltab.tab[n++] = cvf;
		}
		if (n >= voxeltab.len) break;
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

	status = DPIPE_NOTREADY;
	Unlock();
}

void DataPipe::SetGP(vector3di pos)
{
	unsigned l;

	GP = pos;

	Lock();
	status = DPIPE_BUSY;

	for (l = 0; l < HOLDCHUNKS; l++)
		chstat[l] = DPCHK_QUEUE;

#if HOLDCHUNKS == 1
	l = 0;

#elif HOLDCHUNKS == 9
	l = 1 * 3 + 1;

#else
	l = 1 * 9 + 1 * 3 + 1;

#endif

	MakeChunk(l,GP);

	status = DPIPE_IDLE;
	Unlock();
}

bool DataPipe::Move(EGMoveDir dir)
{
	Lock();
	status = DPIPE_BUSY;
	//TODO
	status = DPIPE_IDLE;
	Unlock();
	return false;
}

void DataPipe::ChunkQueue()
{
	if (status != DPIPE_IDLE) return;

#if HOLDCHUNKS == 1
	/* One chunk right there, simplest scenario. Do nothing. */
	return;
#else

	vector3di cur;
	bool fnd = false;
	unsigned l;

//	Lock();
//	status = DPIPE_BUSY;

#if HOLDCHUNKS == 9
	/* One 3x3 plane of chunks, most widely used scenario */
	int i,j;
	cur.Z = GP.Z:
	for (i = -1, l = 0; i < 2; i++) {
		cur.Y = GP.Y + i;
		for (j = -1; j < 2; j++, l++) {
			cur.X = GP.X + j;
			if (chstat[l] == DPCHK_QUEUE) {
				fnd = true;
				goto chunk_found;
			}
		}
	}

#elif HOLDCHUNKS == 18
	/* Two 3x3 planes (one right there, and one underneath) */
	int i,j,k;
	for (i = -1, l = 0; i <= 0; i++) {
		cur.Z = GP.Z + i;
		for (j = -1; j < 2; j++) {
			cur.Y = GP.Y + j;
			for (k = -1; k < 2; k++, l++) {
				cur.X = GP.X + k;
				if (chstat[l] == DPCHK_QUEUE) {
					fnd = true;
					goto chunk_found;
				}
			}
		}
	}

#elif HOLDCHUNKS == 27
	/* Full set of 3x3x3 (the most memory hungry scenario) */
	int i,j,k;
	for (i = -1, l = 0; i < 2; i++) {
		cur.Z = GP.Z + i;
		for (j = -1; j < 2; j++) {
			cur.Y = GP.Y + j;
			for (k = -1; k < 2; k++, l++) {
				cur.X = GP.X + k;
				if (chstat[l] == DPCHK_QUEUE) {
					fnd = true;
					goto chunk_found;
				}
			}
		}
	}
#endif

chunk_found:
	if (fnd) {
		chstat[l] = DPCHK_LOADING;
		MakeChunk(l,cur);
	}

//	status = DPIPE_IDLE;
//	Unlock();
#endif
}

void DataPipe::MakeChunk(unsigned l, vector3di pos)
{
	SDataPlacement plc;

	if (!FindChunk(pos,&plc)) {
		wgen->GenerateChunk(chunks[l],pos);
		chstat[l] = DPCHK_READY;
	} else if (!LoadChunk(&plc,chunks[l]))
		chstat[l] = DPCHK_ERROR;
	else
		chstat[l] = DPCHK_READY;
}

bool DataPipe::LoadChunk(SDataPlacement* res, PChunk buf)
{
	//TODO
	return false;
}

/* GetVoxel() Interlocking macro for multithreaded access */
#ifdef DPLOCKEACHVOX
#define DP_GETVOX_LOCK Lock()
#define DP_GETVOX_UNLOCK Unlock()
#else
#define DP_GETVOX_LOCK
#define DP_GETVOX_UNLOCK
#endif

voxel DataPipe::GetVoxel(const vector3di* p)
{
	PChunk ch;
	int px,py,pz;
	unsigned l;
	VModVec::iterator mi;
	voxel tmp = 0;

#if HOLDCHUNKS > 1
	int x,y,z;
#endif

	if (status != DPIPE_IDLE) return 0;
	DP_GETVOX_LOCK;

	/* Check for dynamic objects */
	if (!objs.empty()) {
		for (mi = objs.begin(); mi != objs.end(); ++mi) {
			if (IsPntInsideCubeI(p,(*mi)->GetPosP(),(*mi)->GetBoundSide())) {
				tmp = (*mi)->GetVoxelAt(p);
				if (tmp) {
					DP_GETVOX_UNLOCK;
					return tmp;
				}
			}
		}
	}

#if HOLDCHUNKS == 1
	/* Simplest case */

	px = p->X;
	py = p->Y;
	pz = p->Z;

	if (	(px < 0) || (py < 0) || (pz < 0) ||
			(px >= CHUNKBOX) || (py >= CHUNKBOX) || (pz >= CHUNKBOX) ) {
		DP_GETVOX_UNLOCK;
		return 0;
	}
	l = 0;

#else

	/* Other cases */
	x = p->X / CHUNKBOX - ((p->X < 0)? 1:0);
	y = p->Y / CHUNKBOX - ((p->Y < 0)? 1:0);
	z = p->Z / CHUNKBOX - ((p->Z < 0)? 1:0);
	px = p->X % CHUNKBOX;
	py = p->Y % CHUNKBOX;
	pz = p->Z % CHUNKBOX;
	if (px < 0) px = CHUNKBOX + px;
	if (py < 0) py = CHUNKBOX + py;
	if (pz < 0) pz = CHUNKBOX + pz;

#if HOLDCHUNKS == 9
	if (	(x < -1) || (y < -1) || (z < 0) ||
			(x >  1) || (y >  1) || (z > 0) ) {
		DP_GETVOX_UNLOCK;
		return 0;
	}
	++y; ++x; //centerize
	l = y * 3 + x;

#elif HOLDCHUNKS == 18
	if (	(x < -1) || (y < -1) || (z < -1) ||
			(x >  1) || (y >  1) || (z > 0) ) {
		DP_GETVOX_UNLOCK;
		return 0;
	}
	++z; ++y; ++x; //centerize
	l = z * 9 + y * 3 + x;

#elif HOLDCHUNKS == 27
	if (	(x < -1) || (y < -1) || (z < -1) ||
			(x >  1) || (y >  1) || (z >  1) ) {
		DP_GETVOX_UNLOCK;
		return 0;
	}
	++z; ++y; ++x; //centerize
	l = z * 9 + y * 3 + x;

#else
#error "Invalid value of HOLDCHUNKS!"

#endif
#endif

	if (chstat[l] == DPCHK_READY) {
		ch = chunks[l];
		tmp = (*ch)[pz][py][px];
	}

	DP_GETVOX_UNLOCK;
	return tmp;
}

SVoxelInf* DataPipe::GetVoxelI(const vector3di* p)
{
	voxel v = GetVoxel(p);
	if (v < voxeltab.len) return &(voxeltab.tab[v]);
	else return NULL;
}

SVoxelInf* DataPipe::GetVInfo(const voxel v)
{
	if (v < voxeltab.len) return &(voxeltab.tab[v]);
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

VSprite* DataPipe::LoadSprite(const char* fname)
{
	char fn[MAXPATHLEN];
	VSprite* spr = new VSprite();

	snprintf(fn,MAXPATHLEN,"%s/%s",root,fname);
	if (!spr->LoadFromFile(fn)) {
		delete spr;
		return NULL;
	}

	allocated += spr->GetAllocatedRAM();

//	Lock();
	sprs.push_back(spr);
//	Unlock();

	return spr;
}

void DataPipe::PurgeSprites()
{
	VSprVec::iterator mi;

//	Lock();
	for (mi = sprs.begin(); mi != sprs.end(); ++mi) {
		allocated -= (*mi)->GetAllocatedRAM();
		delete (*mi);
	}

	sprs.clear();
//	Unlock();
}
