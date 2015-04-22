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
#include <string.h>
#include "datapipe.h"
#include "support.h"


DataPipe::DataPipe(char* root_dir)
{
	int i;
	size_t sz;

	/* Init variables */
	status = DPIPE_ERROR;
	memset(chunks,0,sizeof(chunks));
	allocated = 0;
	memset(root,0,MAXPATHLEN);

	/* Create the world generator */
	wgen = new WorldGen();

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
	LoadVoxTab(); //and load'em up!

	/* Scan world data files */
	i = strlen(root_dir);
	if (i > 0) {
		memcpy(root,root_dir,i);
		if (!ScanFiles()) return;
	} else
		return;

	/* Allocate chunks buffers memory */
	sz = sizeof(VChunk);
	for (i = 0; i < HOLDCHUNKS; i++) {
		chunks[i] = (PChunk)malloc(sz);
		if (!chunks[i]) {
			errout("Unable to allocate RAM for voxel chunk %d.\n",i);
			PurgeChunks();
			return;
		}
		allocated += sz;
	}

	/* All clear. */
	status = DPIPE_NOTREADY;
}

DataPipe::~DataPipe()
{
	PurgeChunks();
	delete wgen;
}

bool DataPipe::ScanFiles()
{
	//TODO: scan world files to map known chunks
	return true;
}

bool DataPipe::FindChunk(vector3dulli pos, SDataPlacement* res)
{
	std::vector<SDataPlacement>::iterator it;
	if (!res) return false;

	//TODO
	for (it = placetab.begin(); it != placetab.end(); it++) {
//		if ((*it)->)
	}

	return false; //FIXME
}

void DataPipe::LoadVoxTab()
{
	//
}

void DataPipe::PurgeChunks()
{
	int i;
	for (i = 0; i < HOLDCHUNKS; i++)
		if (chunks[i]) {
			free(chunks[i]);
			chunks[i] = NULL;
		}
	allocated = 0;
}

void DataPipe::SetGP(vector3dulli pos)
{
	SDataPlacement plc;
	gp = pos;

	//FIXME: search in all directions
	if (!FindChunk(pos,&plc))
		wgen->GenerateChunk(chunks[0]);

	status = DPIPE_IDLE;
}

bool DataPipe::Move(EMoveDir dir)
{
	//TODO
	return false;
}

voxel DataPipe::GetVoxel(const vector3di* p)
{
	PChunk ch;
	//FIXME: use other chunks
	if ((p->X < 0) || (p->Y < 0) || (p->Z < 0)) return 0;
	if ((p->X >= CHUNKBOX) || (p->Y >= CHUNKBOX) || (p->Z >= CHUNKBOX)) return 0;
	ch = chunks[0];
	return ((*ch)[p->Z][p->Y][p->X]);
}

SVoxelInf* DataPipe::GetVoxelI(const vector3di* p)
{
	voxel v = GetVoxel(p);
	if (v < voxtablen) return &voxeltab[v];
	else return NULL;
}
