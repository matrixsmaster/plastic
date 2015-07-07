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

/* Part of DataPipe class. Voxel chunks management facilities. */

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


void DataPipe::ScanFiles()
{
	FILE* f;
	char fn[MAXPATHLEN];
	SDataPlacement cp;
	SVector3di cv;

	//TODO: scan world files to map known chunks
	for (chsavelast = 0; ; chsavelast++) {
		//get the save file name and open it
		snprintf(fn,sizeof(fn),CHUNKSAVEFILE,root,chsavelast);
		f = fopen(fn,"rb");
		if (!f) break;

		//scan chunks contained in file
		while (!feof(f)) {
			cp.offset = (ulli)ftell(f);
			cp.filenum = chsavelast;

			//read chunk position
			if (fread(&cv,sizeof(cv),1,f) < 1) {
				if (feof(f)) break; //EOF will be set only after the next read beyond the EOF :)
				errout("Unable to read chunk position vector at %llu in file '%s'\n",cp.offset,fn);
				break;
			}
			cp.pos = vector3di(cv);

			//read chunk data length
			if (fread(&(cp.length),sizeof(cp.length),1,f) < 1) {
				errout("Unable to read chunk data length at %llu in file '%s'\n",cp.offset,fn);
				break;
			}

			//append position information and advance through file
			placetab.insert(std::make_pair(GetChunkLinearOffset(cp.pos),cp));
			fseek(f,cp.length,SEEK_CUR);

#ifdef DPDEBUG
			dbg_print("[DP] Detected data for chunk at [%d %d %d]; file #%llu",cv.X,cv.Y,cv.Z,chsavelast);
#endif
		}

		//close the file
		fclose(f);
	}

	//TODO: if new game, delete these files
	return;
}

bool DataPipe::FindChunk(const vector3di pos, SDataPlacement* res)
{
	ulli lin;
	if (!res) return false;

#ifdef DPDEBUG
	dbg_print("[DP] Searching for chunk [%d %d %d]...",pos.X,pos.Y,pos.Z);
#endif

	lin = GetChunkLinearOffset(pos);

	if (placetab.count(lin)) {
		*res = placetab[lin];
#ifdef DPDEBUG
		dbg_print("[DP] Found! [%d %d %d]",res->pos.X,res->pos.Y,res->pos.Z);
#endif
		return true;
	}

	return false;
}

bool DataPipe::LoadChunk(SDataPlacement* res, PChunk buf)
{
	//TODO
	return false;
}

void DataPipe::SaveChunk(const unsigned l)
{
	FILE* sav;
	SDataPlacement plc;
	char fn[MAXPATHLEN];
	vector3di pos;
	SVector3di sp;
	bool fnd = false;

	//check if this chunk should be saved
	if (!chstat[l].changed) return;

	pos = vector3di(chstat[l].gx,chstat[l].gy,chstat[l].gz);

#ifdef DPDEBUG
	dbg_print("[DP] Saving chunk %u at [%d %d %d]",l,pos.X,pos.Y,pos.Z);
#endif

	//if there's a file containing our chunk, use its number
	if (!FindChunk(pos,&plc))
		plc.filenum = chsavelast;
	else
		fnd = true;

	//get the save file name and open it
	snprintf(fn,sizeof(fn),CHUNKSAVEFILE,root,plc.filenum);
	sav = fopen(fn,"a+b");
	if (!sav) {
		errout("[DP] SaveChunk(): unable to open the file '%s'\n",fn);
		return;
	}

	//if our file isn't new, seek to the position needed
	if (fnd) {
		fseek(sav,plc.offset,SEEK_SET);
	} else {
		plc.offset = (ulli)ftell(sav);
		//check the size of the resulting file
		if ((plc.offset + (sizeof(VChunk) + sizeof(SVector3di)) * 2) >= settings.maxchfile) {
			//open the new file (next time)
			chsavelast++;
		}
	}

	//fill in placement info
	plc.length = sizeof(SVector3di) + sizeof(VChunk);

	//write to file
	sp = pos.ToSimVecInt();
	fwrite(&sp,sizeof(sp),1,sav);
	fwrite(&(plc.length),sizeof(plc.length),1,sav);
	fwrite(chunks[l],sizeof(VChunk),1,sav);

	//append data placement information
	if (!fnd) {
		plc.pos = pos;
		placetab.insert(std::make_pair(GetChunkLinearOffset(pos),plc));
	}

	//and we're done
	fclose(sav);
	chstat[l].changed = false; //remove 'changed' flag
}

ulli DataPipe::GetChunkLinearOffset(const vector3di p)
{
	uli r;
	vector3di sz;

	if (!wgen) return 0;

	sz = wgen->GetSizeVector();
	r = p.Z * sz.X * sz.Y;
	r += p.Y * sz.X;
	r += p.X;

	return r;
}

void DataPipe::PurgeChunks()
{
	unsigned i;

	WriteLock();
	for (i = 0; i < HOLDCHUNKS; i++)
		if (chunks[i]) {
			SaveChunk(i);
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

	for (l = 0; l < HOLDCHUNKS; l++) {
		SaveChunk(l);
		chstat[l].s = DPCHK_QUEUE;
	}

#if HOLDCHUNKS == 1
	l = 0;

#elif HOLDCHUNKS == 9
	l = 1 * 3 + 1;

#else /*18 or 27*/
	l = 1 * 9 + 1 * 3 + 1;

#endif

#ifdef DPDEBUG
	dbg_print("[DP] SetGP(): [%d %d %d]",GP.X,GP.Y,GP.Z);
#endif

	MakeChunk(l,GP);
	UpdateModelsSceneRoot();

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
	dbg_print("[DP] Move(): shift = [%d %d %d]",shf.X,shf.Y,shf.Z);
#endif

	rgp = GP + shf;
	if (rgp.Z == 0) {
		//TODO: Z-thru
	}
	if (wgen)
		wgen->WrapCoords(&rgp);

#if HOLDCHUNKS == 1
	SetGP(rgp);
	return true;

#else /* 9, 18, 27 */
	int l,nl,x,y,z,nx,ny;
	PChunk swa;
	SChunkState swb;

#if HOLDCHUNKS == 9
	if (shf.Z) {
		SetGP(GP+shf);
		return true;
	}

#else /* 18, 27 */
	int nz;

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

	//Swap remaining chunks and mark new ones
	for (x = 0; x < 3; x++) {
		for (y = 0; y < 3; y++) {
#if HOLDCHUNKS == 18
			for (z = 0; z < 2; z++) {
#else
			for (z = 0; z < 3; z++) {
#endif
				nx = (shf.X < 0)? 2-x:x;
				ny = (shf.Y < 0)? 2-y:y;
#if HOLDCHUNKS > 9
#if HOLDCHUNKS == 27
				nz = (shf.Z < 0)? 2-z:z;
#else
				nz = (shf.Z < 0)? 1-z:z;
#endif
				l = nz * 9 + ny * 3 + nx;
				nl = (nz + shf.Z) * 9 + (ny + shf.Y) * 3 + nx + shf.X;
#else
				l = ny * 3 + nx;
				nl = (ny + shf.Y) * 3 + nx + shf.X;
#endif /* 9 chunks */
				if (	((shf.X > 0) && (nx > 1)) ||
						((shf.X < 0) && (nx < 1)) ||
						((shf.Y > 0) && (ny > 1)) ||
						((shf.Y < 0) && (ny < 1)) ||
#if HOLDCHUNKS > 9
						((shf.Z > 0) && (nz > 1)) ||
						((shf.Z < 0) && (nz < 1)) ||
#endif
						(nl < 0) || (nl >= HOLDCHUNKS)) {
					//new chunk
					chstat[l].s = DPCHK_QUEUE;
#ifdef DPDEBUG
					dbg_print("[DP] Marking chunk %d",l);
#endif
					continue;
				}

#ifdef DPDEBUG
				dbg_print("[DP] Swapping chunks %d <-> %d",l,nl);
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
	}

	//Update models root
	UpdateModelsSceneRoot();

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
			if (chstat[l].s == DPCHK_QUEUE) {
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
				if (chstat[l].s == DPCHK_QUEUE) {
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
				if (chstat[l].s == DPCHK_QUEUE) {
					fnd = true;
					goto chunk_found;
				}
			}
		}
	}
#endif

chunk_found:
	if (fnd) {
		chstat[l].s = DPCHK_LOADING;
		MakeChunk(l,cur);
	}

	ReadUnlock();

#endif
}

void DataPipe::MakeChunk(const unsigned l, const vector3di pos)
{
	SDataPlacement plc;

	if (chstat[l].changed) SaveChunk(l);

	if (!FindChunk(pos,&plc)) {
		if (wgen) {
			wgen->GenerateChunk(chunks[l],pos);
			chstat[l].s = DPCHK_READY;
			chstat[l].gx = pos.X;
			chstat[l].gy = pos.Y;
			chstat[l].gz = pos.Z;
#ifdef DPDEBUG
			dbg_print("[DP] Chunk %u generated at [%d %d %d]",l,pos.X,pos.Y,pos.Z);
#endif
		} else {
			chstat[l].s = DPCHK_ERROR;
			chstat[l].changed = false;
#ifdef DPDEBUG
			dbg_print("[DP] No WorldGen instance to generate chunk at [%d %d %d]",l,pos.X,pos.Y,pos.Z);
#endif
		}

	} else if (!LoadChunk(&plc,chunks[l])) {
		chstat[l].s = DPCHK_ERROR;
		chstat[l].changed = false;
#ifdef DPDEBUG
		dbg_print("[DP] Error loading chunk %u at [%d %d %d]",l,pos.X,pos.Y,pos.Z);
#endif

	} else {
		chstat[l].s = DPCHK_READY;
		chstat[l].gx = pos.X;
		chstat[l].gy = pos.Y;
		chstat[l].gz = pos.Z;
#ifdef DPDEBUG
		dbg_print("[DP] Chunk %u loaded at [%d %d %d]",l,pos.X,pos.Y,pos.Z);
#endif

	}
}
