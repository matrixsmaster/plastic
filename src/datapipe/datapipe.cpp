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


/* Voxel access interlocking macros variations for multithreaded access */
#ifdef DPLOCKEACHVOX
#define DP_GETVOX_LOCK ReadLock()
#define DP_GETVOX_UNLOCK ReadUnlock()
#else
#define DP_GETVOX_LOCK
#define DP_GETVOX_UNLOCK
#endif


DataPipe::DataPipe(SGameSettings* sets, bool allocate)
{
	int i;

	/* Init variables */
	status = DPIPE_ERROR;
	settings = *sets;

	memset(root,0,MAXPATHLEN);
	memset(chunks,0,sizeof(chunks));
	memset(chstat,0,sizeof(chstat));
	memset(&voxeltab,0,sizeof(voxeltab));
	allocated = 0;
	chsavelast = 0;

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

	/* Create PRNG */
	rngen = new PRNGen(true);
	if (sets->seed) rngen->SetSeed(sets->seed);
	mapseed = rngen->NextNumber(); //store map generation seed value

	/* Allocate memory */
	if (allocate) {
		if (!Allocator(sets)) return;
		/* Map known chunks or delete old files */
		ScanFiles();
	}

	/* OK */
	status = DPIPE_NOTREADY;
}

DataPipe::~DataPipe()
{
	status = DPIPE_NOTREADY;

	if (rngen) delete rngen;

	FreeVoxTab();
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

	/* Allocate voxel info table */
	sz = DEFVOXTYPES * sizeof(SVoxelInf);
	voxeltab.len = DEFVOXTYPES;
	voxeltab.rlen = 0;
	voxeltab.tab = (SVoxelInf*)malloc(sz);
	if (!voxeltab.tab) {
		errout("Unable to allocate RAM for voxel table.\n");
		return false;
	}
	memset(voxeltab.tab,0,sz);
	allocated += sz;

	/* Load voxel table */
	if (!LoadVoxTab()) {
		errout("Invalid root path or voxel table file is corrupted.\n");
		return false;
	}

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
	char mark[MAXVOXMARKLEN];
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
		//reset
		memset(&cvf,0,sizeof(cvf));
		memset(mark,0,sizeof(mark));

		//read and parse the string
		r = fscanf(vtf,VOXTABSTRING,&fx,
				(int*)&(cvf.type),
				&(cvf.pix.fg.r),&(cvf.pix.fg.g),&(cvf.pix.fg.b),
				&(cvf.pix.bg.r),&(cvf.pix.bg.g),&(cvf.pix.bg.b),
				cvf.sides,mark);

		//check the number of successfully parsed params and string prefix
		if ((r < VOXTABSTRPARAMS) || (fx != 'V')) continue;

		//check voxel type
		if ((cvf.type >= 0) && (cvf.type < NUMVOXTYPES))
			voxeltab.stat[cvf.type]++;
		else {
			errout("Unknown voxel type %d\n",(int)cvf.type);
			fclose(vtf);
			return false;
		}

		//apply a mark
		if ((strlen(mark) > 1) && (mark[0] != '!')) {
			cvf.mark = (char*)malloc(strlen(mark)+1);
			if (cvf.mark) strcpy(cvf.mark,mark);
		}

		//append new voxel
		voxeltab.tab[n++] = cvf;
		if (n >= voxeltab.len) break;
	}

	//set the number of voxels defined in table file
	voxeltab.rlen = n;

	//we're good to go
	fclose(vtf);
	return true;
}

void DataPipe::FreeVoxTab()
{
	unsigned i;

	if (voxeltab.tab) {
		for (i = 0; i < voxeltab.len; i++) {
			if (voxeltab.tab[i].mark) free(voxeltab.tab[i].mark);
		}
		free(voxeltab.tab);
	}

	memset(&voxeltab,0,sizeof(voxeltab));
}

inline bool DataPipe::ConvertSceneCoord(const vector3di* p, int* px, int* py, int* pz, unsigned* l)
{
#if HOLDCHUNKS == 1
	/* Simplest case */

	*px = p->X;
	*py = p->Y;
	*pz = p->Z;

	if (	(*px < 0) || (*py < 0) || (*pz < 0) ||
			(*px >= CHUNKBOX) || (*py >= CHUNKBOX) || (*pz >= CHUNKBOX) ) {
		return false;
	}
	*l = 0;

#else
	int x,y,z;

	/* Other cases */
	x = p->X / CHUNKBOX - ((p->X < 0)? 1:0);
	y = p->Y / CHUNKBOX - ((p->Y < 0)? 1:0);
	z = p->Z / CHUNKBOX - ((p->Z < 0)? 1:0);
	*px = p->X % CHUNKBOX;
	*py = p->Y % CHUNKBOX;
	*pz = p->Z % CHUNKBOX;
	if (*px < 0) *px = CHUNKBOX + *px;
	if (*py < 0) *py = CHUNKBOX + *py;
	if (*pz < 0) *pz = CHUNKBOX + *pz;

#if HOLDCHUNKS == 9
	if (	(x < -1) || (y < -1) || (z < 0) ||
			(x >  1) || (y >  1) || (z > 0) ) {
		return false;
	}
	++y; ++x; //centerize
	*l = y * 3 + x;

#elif HOLDCHUNKS == 18
	if (	(x < -1) || (y < -1) || (z < -1) ||
			(x >  1) || (y >  1) || (z > 0) ) {
		return false;
	}
	++z; ++y; ++x; //centerize
	*l = z * 9 + y * 3 + x;

#elif HOLDCHUNKS == 27
	if (	(x < -1) || (y < -1) || (z < -1) ||
			(x >  1) || (y >  1) || (z >  1) ) {
		return false;
	}
	++z; ++y; ++x; //centerize
	*l = z * 9 + y * 3 + x;

#else /* Just a compile-time check */
#error "Invalid value of HOLDCHUNKS!"

#endif
#endif

	return true;
}

voxel DataPipe::GetVoxel(const vector3di* p, bool dynskip)
{
	PChunk ch;
	int px,py,pz;
	unsigned l;
	voxel tmp = 0;

	if (status != DPIPE_IDLE) return 0;
	DP_GETVOX_LOCK;

	/* Check for dynamic objects */
	if (!dynskip) {
		tmp = IntersectModel(p,NULL,NULL,false);
		if (tmp) {
			DP_GETVOX_UNLOCK;
			return tmp;
		}
	}

	/* Converse scene co-ords */
	if (!ConvertSceneCoord(p,&px,&py,&pz,&l)) {
		DP_GETVOX_UNLOCK;
		return 0;
	}

	/* Check the chunk status and return voxel id */
	if (chstat[l].s == DPCHK_READY) {
		ch = chunks[l];
		tmp = (*ch)[pz][py][px];
	}

	DP_GETVOX_UNLOCK;
	return tmp;
}

void DataPipe::ChangeVoxelAt(const vector3di* p, const voxel nv)
{
	PChunk ch;
	int px,py,pz;
	unsigned l;

	DP_GETVOX_LOCK;

	/* Converse co-ords */
	if (!ConvertSceneCoord(p,&px,&py,&pz,&l)) {
		DP_GETVOX_UNLOCK;
		return;
	}

	/* Check the chunk status and return voxel id */
	if (chstat[l].s == DPCHK_READY) {
		ch = chunks[l];
		(*ch)[pz][py][px] = nv;
		chstat[l].changed = true;
	}

	DP_GETVOX_UNLOCK;
}

const SVoxelInf* DataPipe::GetVoxelI(const vector3di* p, bool dynskip)
{
	voxel v = GetVoxel(p,dynskip);
	if (v < voxeltab.len) return &(voxeltab.tab[v]);
	else return NULL;
}

const SVoxelInf* DataPipe::GetVInfo(const voxel v)
{
	if (v < voxeltab.len) return &(voxeltab.tab[v]);
	else return NULL;
}

const SVoxelInf* DataPipe::GetVInfo(const char* mrk)
{
	unsigned i;

	ReadLock(); //soft-lock pipe while we searching
	for (i = 0; i < voxeltab.rlen; i++)
		if (	(voxeltab.tab[i].mark) &&
				(!strcmp(voxeltab.tab[i].mark,mrk)) ) {
			ReadUnlock();
			return &(voxeltab.tab[i]);
		}
	ReadUnlock();
	return NULL;
}

voxel DataPipe::AppendVoxel(const SVoxelInf* nvox)
{
	unsigned l;
	voxel v = 0;

	if (!nvox) return 0;

	//Hard lock the Pipe till this important operation is done.
	WriteLock();

	//find some empty space in already allocated table
	for (l = 0; l < voxeltab.rlen; l++)
		if (voxeltab.tab[l].not_used) {
			v = l;
#ifdef DPDEBUG
			dbg_logstr("Empty voxel place was found.");
#endif
			break;
		}

	//if all places is occupied, try to enlarge the table
	l = (v)? v:(voxeltab.rlen + 1);

	//check for some room
	if (l >= voxeltab.len) {
		if (l >= MAXVOXTYPES) {
			//no space left, abnormal termination
#ifdef DPDEBUG
			dbg_logstr("No logical space left for a new voxel.");
#endif
			status = DPIPE_ERROR;
			WriteUnlock();
			return 0;
		}

		//allocate more space for a voxel table
		l *= sizeof(SVoxelInf); //new table size
		voxeltab.len = l;
		voxeltab.tab = (SVoxelInf*)realloc(voxeltab.tab,l);

		if (!voxeltab.tab) {
			//out of memory
#ifdef DPDEBUG
			dbg_logstr("No memory space left for a new voxel.");
#endif
			status = DPIPE_ERROR;
			WriteUnlock();
			return 0;
		}
		allocated += sizeof(SVoxelInf); //update allocated memory counter
	}

	if (!v) {
		//append
		v = voxeltab.rlen;
		voxeltab.rlen++;
	} //else : overwrite

	//add new voxel info
	voxeltab.tab[v] = *nvox;

#ifdef DPDEBUG
	dbg_print("Registered new voxel id=%hu.",v);
#endif

	WriteUnlock();
	return v;
}

void DataPipe::RemoveVoxel(voxel v)
{
	if ((!v) || (v >= voxeltab.rlen)) return;

	WriteLock();

	voxeltab.tab[v].not_used = true;
	if (voxeltab.tab[v].mark) {
		free(voxeltab.tab[v].mark);
		voxeltab.tab[v].mark = NULL;
	}
	voxeltab.tab[v].texture = NULL;
	voxeltab.tab[v].type = VOXT_EMPTY;

	WriteUnlock();

#ifdef DPDEBUG
	dbg_print("Voxel unregistered. id=%hu.",v);
#endif
}

void DataPipe::ConnectWorldGen(WorldGen* ptr)
{
	if (ptr) {
		wgen = ptr;
		allocated += wgen->GetAllocatedRAM();
	} else {
		allocated -= wgen->GetAllocatedRAM();
		wgen = NULL;
	}
}
