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

/* DataPipe is the main dynamic storage container */

#ifndef DATAPIPE_H_
#define DATAPIPE_H_

#include "voxel.h"
#include "misconsts.h"

enum EDPipeStatus {
	DPIPE_NOTREADY,
	DPIPE_ERROR,
	DPIPE_IDLE,
	DPIPE_BUSY
};

enum EMoveDir {
	PMOVE_UP,
	PMOVE_DOWN,
	PMOVE_NORTH,
	PMOVE_SOUTH,
	PMOVE_WEST,
	PMOVE_EAST
};

class DataPipe {
private:
	PChunk chunks[HOLDCHUNKS];		//world chunk buffers
	EDPipeStatus status;
	ulli allocated;					//amount of allocated RAM
	ulli gp_X, gp_Y, gp_Z;			//global position of central chunk

	bool ScanFiles();

public:
	DataPipe(char*);
	virtual ~DataPipe();

	///Returns a status of the pipe.
	EDPipeStatus GetStatus()	{ return status; }

	///Returns amount of RAM allocated by buffers.
	ulli GetAllocatedRAM()		{ return allocated; }

	///Discards all chunks buffers and release memory.
	void PurgeChunks();

	///Set up the global position of central chunk.
	void SetGP(ulli x, ulli y, ulli z);

	///Move the world to next chunk.
	///Update chunks buffers either by loading or by generating.
	///Returns false if move is invalid.
	bool Move(EMoveDir dir);
};

#endif /* DATAPIPE_H_ */
