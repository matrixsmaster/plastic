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
#include "datapipe.h"
#include "support.h"
#include "vecmisc.h"

#ifdef DPDEBUG
#include "debug.h"
#endif


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

bool DataPipe::LoadChunk(SDataPlacement* res, PChunk buf)
{
	//TODO
	return false;
}

void DataPipe::SaveChunk(const unsigned l)
{
	//check if this chunk should be saved
	if (chstat[l] != DPCHK_CHANGED) return;

	//TODO

	chstat[l] = DPCHK_READY; //remove 'changed' flag
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
	dbg_print("Move(): shift = [%d %d %d]",shf.X,shf.Y,shf.Z);
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
	EDChunkStatus swb;

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

void DataPipe::MakeChunk(const unsigned l, const vector3di pos)
{
	SDataPlacement plc;

	if (chstat[l] == DPCHK_CHANGED) {
#ifdef DPDEBUG
		dbg_print("Saving chunk at [%d %d %d]",pos.X,pos.Y,pos.Z);
#endif
		SaveChunk(l);
	}

	if (!FindChunk(pos,&plc)) {
		if (wgen) {
			wgen->GenerateChunk(chunks[l],pos);
			chstat[l] = DPCHK_READY;
#ifdef DPDEBUG
			dbg_print("Chunk %u generated at [%d %d %d]",l,pos.X,pos.Y,pos.Z);
#endif
		} else {
			chstat[l] = DPCHK_ERROR;
#ifdef DPDEBUG
			dbg_print("No WorldGen instance to generate chunk at [%d %d %d]",l,pos.X,pos.Y,pos.Z);
#endif
		}

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
