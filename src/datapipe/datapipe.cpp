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
	pthread_cond_init(&cntcnd,NULL);
	readcnt = 0;
	writeatt = false;

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

	pthread_cond_destroy(&cntcnd);
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

	/* If write operation attempted, wait till its finished */
	while (writeatt) {
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
	if (readcnt == 0)
		/* Send a signal to writers (if any) */
		pthread_cond_signal(&cntcnd);

	/* Release conditions */
	pthread_mutex_unlock(&cndmtx);
	return r;
}

int DataPipe::WriteLock()
{
	int r;

	/* Lock conditions */
	r = pthread_mutex_lock(&cndmtx);

	/* Set write attempt to true */
	writeatt = true;

	/* Wait for readers to gone */
	while (readcnt)
		pthread_cond_wait(&cntcnd,&cndmtx);

	/* Try to lock main voxel mutex (write op. mutex) */
	while (pthread_mutex_trylock(&vmutex)) {
		/* Release conditions to allow many writers to step in queue */
		pthread_mutex_unlock(&cndmtx);
		usleep(DPWRLOCKTIME);
		pthread_mutex_lock(&cndmtx);
	}

	/* Set write attempt flag again to deal with possible WRUnlock */
	writeatt = true;

	/* Release conditions and move on */
	pthread_mutex_unlock(&cndmtx);
	return r;
}

int DataPipe::WriteUnlock()
{
	/* Set write attempt to false */
	pthread_mutex_lock(&cndmtx);
	writeatt = false;
	pthread_mutex_unlock(&cndmtx);

	/* And just unlock main voxel mutex */
	return (pthread_mutex_unlock(&vmutex));
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

	WriteLock();
	for (i = 0; i < HOLDCHUNKS; i++)
		if (chunks[i]) {
			free(chunks[i]);
			chunks[i] = NULL;
			allocated -= sizeof(VChunk);
		}

	status = DPIPE_NOTREADY;
	WriteUnlock();
}

void DataPipe::SetGP(vector3di pos)
{
	unsigned l;

	GP = pos;

	WriteLock();
	status = DPIPE_BUSY;

	for (l = 0; l < HOLDCHUNKS; l++)
		chstat[l] = DPCHK_QUEUE;

#if HOLDCHUNKS == 1
	l = 0;

#elif HOLDCHUNKS == 9
	l = 1 * 3 + 1;

#else /*18 or 27*/
	l = 1 * 9 + 1 * 3 + 1;

#endif

#ifdef DPDEBUG
	dbg_print("SetGP(): [%d %d %d]",GP.X,GP.Y,GP.Z);
#endif

	MakeChunk(l,GP);

	status = DPIPE_IDLE;
	WriteUnlock();
}

bool DataPipe::Move(const vector3di shf)
{
	vector3di rgp;

	//Check shift vector first
	if ( (abs(shf.X) > 1) || (abs(shf.Y) > 1) || (abs(shf.Z) > 1) )
		return false;

#ifdef DPDEBUG
	dbg_print("Move(): shift = [%d %d %d]",shf.X,shf.Y,shf.Z);
#endif

	rgp = GP + shf;
	if (rgp.Z == 0) {
		//TODO: Z-thru
	}
	wgen->WrapCoords(&rgp);

#if HOLDCHUNKS == 1
	SetGP(rgp);
	return true;

#else /* 9, 18, 27 */
	int l,nl,x,y,nx,ny;
	PChunk swa;
	EDChunkStatus swb;

#if HOLDCHUNKS == 9
	if (shf.Z) {
		SetGP(GP+shf);
		return true;
	}

#else /* 18, 27 */
	int z,nz;

#if HOLDCHUNKS == 18
	if (shf.Z > 0) {
		SetGP(GP+shf);
		return true;
	}

#endif
#endif

	//Lock everything
	WriteLock();
	status = DPIPE_BUSY;
	GP = rgp;

#if HOLDCHUNKS == 9

	//Swap remaining chunks and mark new ones
	for (x = 0; x < 3; x++) {
		for (y = 0; y < 3; y++) {
			nx = (shf.X < 0)? 2-x:x;
			ny = (shf.Y < 0)? 2-y:y;
			l = ny * 3 + nx;
			nl = (ny + shf.Y) * 3 + nx + shf.X;
			if (	((shf.X > 0) && (nx > 1)) ||
					((shf.X < 0) && (nx < 1)) ||
					((shf.Y > 0) && (ny > 1)) ||
					((shf.Y < 0) && (ny < 1)) ||
					(nl < 0) || (nl >= HOLDCHUNKS)) {
				//new chunk
				chstat[l] = DPCHK_QUEUE;
#ifdef DPDEBUG
				dbg_print("Marking chunk %d",l);
#endif
				continue;
			}

#ifdef DPDEBUG
			dbg_print("Swapping chunks %d <-> %d",l,nl);
#endif
			//swap chunks
			swa = chunks[l];
			chunks[l] = chunks[nl];
			chunks[nl] = swa;

			swb = chstat[l];
			chstat[l] = chstat[nl];
			chstat[nl] = swb;
		}
	}

#elif HOLDCHUNKS == 18
#elif HOLDCHUNKS == 27
#endif

	//Release everything
	status = DPIPE_IDLE;
	WriteUnlock();
	return true;
#endif
}

void DataPipe::ChunkQueue()
{
	if (status != DPIPE_IDLE) return;

#if HOLDCHUNKS == 1
	/* Do nothing */
	return;
#else

	vector3di cur;
	bool fnd = false;
	unsigned l;

	/* Apply soft-lock to not interfere with rendering
	 * while time-consuming loading or generation is
	 * processing.
	 */
	ReadLock();

#if HOLDCHUNKS == 9
	int i,j;
	cur.Z = GP.Z;
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

	ReadUnlock();

#endif
}

void DataPipe::MakeChunk(unsigned l, vector3di pos)
{
	SDataPlacement plc;

	if (!FindChunk(pos,&plc)) {
		wgen->GenerateChunk(chunks[l],pos);
		chstat[l] = DPCHK_READY;

#ifdef DPDEBUG
		dbg_print("Chunk %u generated at [%d %d %d]",l,pos.X,pos.Y,pos.Z);
#endif

	} else if (!LoadChunk(&plc,chunks[l])) {
		chstat[l] = DPCHK_ERROR;

#ifdef DPDEBUG
		dbg_print("Error loading chunk at [%d %d %d]",l,pos.X,pos.Y,pos.Z);
#endif

	} else {
		chstat[l] = DPCHK_READY;

#ifdef DPDEBUG
		dbg_print("Chunk %u loaded at [%d %d %d]",l,pos.X,pos.Y,pos.Z);
#endif

	}
}

bool DataPipe::LoadChunk(SDataPlacement* res, PChunk buf)
{
	//TODO
	return false;
}

void DataPipe::SetNewGame()
{
	memset(&svhead,0,sizeof(svhead));

	//TODO

	svhead.gtime.year = PLTIMEINITYEAR;
}

bool DataPipe::LoadLastGame()
{
	//TODO
	return false;
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

	WriteLock();
	objs.push_back(m);
	WriteUnlock();

	return m;
}

bool DataPipe::UnloadModel(const VModel* ptr)
{
	VModVec::iterator it;
	if (!ptr) return false;

	for (it = objs.begin(); it != objs.end(); ++it)
		if ((*it) == ptr) {
			WriteLock();
			allocated -= (*it)->GetAllocatedRAM();
			delete ((*it));
			objs.erase(it);
			WriteUnlock();
			return true;
		}

	return false;
}

void DataPipe::PurgeModels()
{
	VModVec::iterator mi;

	WriteLock();
	for (mi = objs.begin(); mi != objs.end(); ++mi) {
		allocated -= (*mi)->GetAllocatedRAM();
		delete (*mi);
	}

	objs.clear();
	WriteUnlock();
}

voxel DataPipe::IntersectModel(const vector3di* p, VModel** obj, const bool autolock)
{
	voxel r;
	VModVec::iterator mi;

	if (autolock) ReadLock();

	if (!objs.empty()) {
		for (mi = objs.begin(); mi != objs.end(); ++mi) {
			if (IsPntInsideCubeI(p,(*mi)->GetPosP(),(*mi)->GetBoundSide())) {
				r = (*mi)->GetVoxelAt(p);
				if (r) {
					if (autolock) ReadUnlock();
					if (obj) *obj = *mi;
					return r;
				}
			}
		}
	}

	if (autolock) ReadUnlock();
	if (obj) *obj = NULL;
	return 0;
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
