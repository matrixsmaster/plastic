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
#include <unistd.h>
#include "datapipe.h"
#include "support.h"
#include "vecmisc.h"

#ifdef DPDEBUG
#include "debug.h"
#endif


DataPipe::DataPipe(SGameSettings* sets, bool allocate)
{
	int i;

	/* Init variables */
	status = DPIPE_ERROR;

	memset(root,0,MAXPATHLEN);
	memset(chunks,0,sizeof(chunks));
	memset(chstat,0,sizeof(chstat));
	memset(&voxeltab,0,sizeof(voxeltab));
	allocated = 0;

	wgen = NULL;
	rammax = sets->rammax;

	pthread_mutex_init(&vmutex,NULL);
	pthread_mutex_init(&cndmtx,NULL);
	readcnt = 0;
	writecnt = 0;

	/* Copy root path */
	i = strlen(sets->root);
	if (i > 0) memcpy(root,sets->root,i);
	else return;

	/* Allocate (and load) everything */
	if (allocate) {
		if (!Allocator(sets)) return;
		if (sets->new_game) SetNewGame();
		else if (!LoadLastGame()) return;
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

	pthread_mutex_destroy(&cndmtx);
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

	/* All clear. */
	return true;
}

int DataPipe::ReadLock()
{
	int r;

	/* Lock conditions */
	r = pthread_mutex_lock(&cndmtx);

	/* If write operation attempted or in progress, wait till its finished */
	while (writecnt > 0) {
		pthread_mutex_unlock(&cndmtx);
		usleep(DPWRLOCKTIME);
		pthread_mutex_lock(&cndmtx);
	}

	/* Increment readers counter and move on */
	readcnt++;
	pthread_mutex_unlock(&cndmtx);
	return r;
}

int DataPipe::ReadUnlock()
{
	int r;

	/* Lock conditions */
	r = pthread_mutex_lock(&cndmtx);

	/* Decrement readers counter */
	if (--readcnt < 0) readcnt = 0;

	/* Release conditions */
	pthread_mutex_unlock(&cndmtx);
	return r;
}

int DataPipe::WriteLock()
{
	int r;

	/* Lock conditions */
	r = pthread_mutex_lock(&cndmtx);

	/* Set write attempt */
	writecnt++;

	/* Wait for readers to gone */
	while (readcnt > 0) {
		pthread_mutex_unlock(&cndmtx);
		usleep(DPRDLOCKTIME);
		pthread_mutex_lock(&cndmtx);
	}

	/* Try to lock main voxel mutex (write op. mutex) */
	while (pthread_mutex_trylock(&vmutex)) {
		/* Release conditions to allow many writers to step in queue */
		pthread_mutex_unlock(&cndmtx);
		usleep(DPWRLOCKTIME);
		pthread_mutex_lock(&cndmtx);
	}

	/* Release conditions and move on */
	pthread_mutex_unlock(&cndmtx);
	return r;
}

int DataPipe::WriteUnlock()
{
	/* Lock conditions */
	pthread_mutex_lock(&cndmtx);

	/* Decrement readers counter */
	if (--writecnt < 0) writecnt = 0;

	/* Release conditions */
	pthread_mutex_unlock(&cndmtx);

	/* And just unlock main voxel mutex */
	return (pthread_mutex_unlock(&vmutex));
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

/* GetVoxel() Interlocking macros variations for multithreaded access */
#ifdef DPLOCKEACHVOX
#define DP_GETVOX_LOCK ReadLock()
#define DP_GETVOX_UNLOCK ReadUnlock()
#else
#define DP_GETVOX_LOCK
#define DP_GETVOX_UNLOCK
#endif

voxel DataPipe::GetVoxel(const vector3di* p)
{
	PChunk ch;
	int px,py,pz;
	unsigned l;
	voxel tmp = 0;

#if HOLDCHUNKS > 1
	int x,y,z;
#endif

	if (status != DPIPE_IDLE) return 0;
	DP_GETVOX_LOCK;

	/* Check for dynamic objects */
	tmp = IntersectModel(p,NULL,false);
	if (tmp) {
		DP_GETVOX_UNLOCK;
		return tmp;
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

#else /* Just a compile-time check */
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

const SVoxelInf* DataPipe::GetVoxelI(const vector3di* p)
{
	voxel v = GetVoxel(p);
	if (v < voxeltab.len) return &(voxeltab.tab[v]);
	else return NULL;
}

const SVoxelInf* DataPipe::GetVInfo(const voxel v)
{
	if (v < voxeltab.len) return &(voxeltab.tab[v]);
	else return NULL;
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
