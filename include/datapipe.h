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

#include <vector>
#include <map>
#include <string>
#include "voxel.h"
#include "misconsts.h"
#include "vecmath.h"
#include "wrldgen.h"


#define MAXINISTRLEN 256
#define VOXTABFILENAME "voxtab.dat"
#define ATMOININAME "atmosphere"
#define KEYBINDNAME "controls"


enum EDPipeStatus {
	DPIPE_NOTREADY,
	DPIPE_ERROR,
	DPIPE_IDLE,
	DPIPE_BUSY
};

struct SDataPlacement {
	unsigned filenum;
	vector3dulli pos;
	long offset;
};

typedef std::map<std::string,std::string> IniData;


class DataPipe {
private:
	PChunk chunks[HOLDCHUNKS];				//world chunk buffers
	EDPipeStatus status;
	ulli allocated;							//amount of allocated RAM
	vector3dulli gp;						//global position of central chunk
	char root[MAXPATHLEN];					//root path
	std::vector<SDataPlacement> placetab;	//chunk displacement table
	WorldGen* wgen;							//world generator instance
	SVoxelInf* voxeltab;					//voxel types table
	int voxtablen;							//...its length
	std::map<std::string,IniData> ini;		//map of known (and loaded) ini files

	bool ScanFiles();
	bool FindChunk(vector3dulli pos, SDataPlacement* res);
	bool LoadVoxTab();
	bool LoadIni(const std::string name);

public:
	DataPipe(char*);
	virtual ~DataPipe();

	///Returns a status of the pipe.
	EDPipeStatus GetStatus()	{ return status; }

	///Returns amount of RAM allocated by buffers.
	ulli GetAllocatedRAM()		{ return allocated; }

	///Returns a pointer to voxel info table.
	SVoxelInf* GetVoxTable()	{ return voxeltab; }

	///Returns voxel info table length.
	int GetVoxTableLen()		{ return voxtablen; }

	///Discards all chunks buffers and release memory.
	void PurgeChunks();

	///Set up the global position of central chunk.
	void SetGP(vector3dulli pos);

	///Move the world to next chunk.
	///Update chunks buffers either by loading or by generating.
	///Returns false if move is invalid.
	bool Move(EGMoveDir dir);

	///Returns a specific voxel (or its data) in a loaded space.
	voxel GetVoxel(const vector3di* p);			//return voxel code
	SVoxelInf* GetVoxelI(const vector3di* p);	//return voxel info

	///Return an information about voxel by type code.
	SVoxelInf* GetVInfo(const voxel v);

	///Supply INI-file based data by INI name and field name.
	void GetIniDataC(const char* ininame, const char* inifield, char* dest, int maxlen);
	std::string GetIniDataS(const std::string ininame, const std::string inifield);
};

#endif /* DATAPIPE_H_ */
