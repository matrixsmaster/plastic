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
#include <pthread.h>
#include "voxel.h"
#include "misconsts.h"
#include "vecmath.h"
#include "wrldgen.h"
#include "vmodel.h"
#include "plastic.h"


#define DEFRAMMAX (2ULL * 1024*1024*1024)

#define MAXINISTRLEN 256
#define FMTINISTRING "%s = %255[^\n]"

#define VOXTABFILENAME "voxtab.dat"
#define ATMOININAME "atmosphere"
#define KEYBINDNAME "controls"
#define CLASNFONAME "classes"


enum EDPipeStatus {
	DPIPE_NOTREADY,		//pipe is partially initialized
	DPIPE_ERROR,		//error state, pipe cannot be used
	DPIPE_IDLE,			//normal state, use Lock()/Unlock() to modify the contents
	DPIPE_BUSY			//I/O operations in progress
};

struct SDataPlacement {
	unsigned filenum;
	vector3dulli pos;
	long offset;
};

//Some shortcuts for long-named types
typedef std::map<std::string,std::string> IniData;
typedef std::map<std::string,IniData> IniMap;
typedef std::vector<VModel*> VModVec;
typedef std::map<vector3dulli,SDataPlacement> PlaceMap;


/* ********************************** DATA PIPE MAIN ********************************** */

class DataPipe {
protected:
	PChunk chunks[HOLDCHUNKS];		//world chunk buffers
	EDPipeStatus status;
	pthread_mutex_t vmutex;
	ulli allocated;					//amount of allocated RAM
	vector3dulli GP;				//global position of central chunk
	char root[MAXPATHLEN];			//root path
	PlaceMap placetab;				//chunk displacement map
	WorldGen* wgen;					//world generator instance
	SVoxelInf* voxeltab;			//voxel types table
	int voxtablen;					//...its length
	IniMap ini;						//map of known (and loaded) ini files
	VModVec objs;					//objects in scene
	ulli rammax;					//max amount of memory allowed to be allocated

	bool Allocator(const SGameSettings* sets);
	bool ScanFiles();
	bool FindChunk(vector3dulli pos, SDataPlacement* res);
	bool LoadVoxTab();
	bool LoadIni(const std::string name);

public:
	DataPipe(const SGameSettings* sets, bool allocate = true);
	virtual ~DataPipe();

	///Returns a status of the pipe.
	virtual EDPipeStatus GetStatus()	{ return status; }

	///Synchronization.
	int Lock()							{ return pthread_mutex_lock(&vmutex); }
	int TryLock()						{ return pthread_mutex_trylock(&vmutex); }
	int Unlock()						{ return pthread_mutex_unlock(&vmutex); }

	///Returns amount of RAM allocated by buffers.
	ulli GetAllocatedRAM()				{ return allocated; }

	void SetMaxRAM(ulli m)				{ rammax = m; }
	ulli GetMaxRAM()					{ return rammax; }

	///Returns a pointer to voxel info table.
	SVoxelInf* GetVoxTable()			{ return voxeltab; }

	///Returns voxel info table length.
	int GetVoxTableLen()				{ return voxtablen; }

	///Discards all chunks buffers and release memory.
	virtual void PurgeChunks();

	///Set up the global position of central chunk.
	virtual void SetGP(vector3dulli pos);

	///Move the world to next chunk.
	///Update chunks buffers either by loading or by generating.
	///Returns false if move is invalid.
	virtual bool Move(EGMoveDir dir);

	///Returns a specific voxel (or its data) in a loaded space.
	virtual voxel GetVoxel(const vector3di* p);			//return voxel code
	virtual SVoxelInf* GetVoxelI(const vector3di* p);	//return voxel info

	///Return an information about voxel by type code.
	virtual SVoxelInf* GetVInfo(const voxel v);

	///Supply INI-file based data by INI name and field name.
	void GetIniDataC(const char* ininame, const char* inifield, char* dest, int maxlen);
	std::string GetIniDataS(const std::string ininame, const std::string inifield);

	///Load dynamic object into scene.
	virtual VModel* LoadModel(const char* fname, const vector3di pos);

	///Delete model from scene by pointer.
	virtual bool UnloadModel(const VModel* ptr);

	///Purge all loaded models.
	virtual void PurgeModels();
};

/* ********************************** DATA PIPE DUMMY ********************************** */
/* This class can be used as a lightweight version of DataPipe.
 * Capable of INI reading and model management, this class uses much less RAM.
 */
class DataPipeDummy : public DataPipe
{
public:
	DataPipeDummy(const SGameSettings* sets);

	void SetVoxTab(SVoxelInf* p, int l)	{ voxtablen = l; voxeltab = p; }

	void SetGP(vector3dulli pos)		{}
	bool Move(EGMoveDir dir)			{ return false; }

	voxel GetVoxel(const vector3di* p);
};

#endif /* DATAPIPE_H_ */
