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

DataPipe::DataPipe(char* root)
{
	int i;

	status = DPIPE_ERROR;
	memset(chunks,0,sizeof(chunks));
	allocated = 0;
	gp_X = gp_Y = gp_Z = 0;

	if (!ScanFiles()) return;

	//allocate chunks buffers memory
	for (i = 0; i < HOLDCHUNKS; i++) {
		chunks[i] = (PChunk)malloc(sizeof(VChunk));
		if (!chunks[i]) {
			PurgeChunks();
			return;
		}
		allocated += sizeof(VChunk);
	}

	//All clear.
	status = DPIPE_NOTREADY;
}

DataPipe::~DataPipe()
{
	PurgeChunks();
}

bool DataPipe::ScanFiles()
{
	//TODO: scan world files to map known chunks
	return true;
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

void DataPipe::SetGP(ulli x, ulli y, ulli z)
{
	gp_X = x; gp_Y = y; gp_Z = z;
	//TODO: load buffers to new position
	status = DPIPE_IDLE;
}

bool DataPipe::Move(EMoveDir dir)
{
	//TODO
	return false;
}
