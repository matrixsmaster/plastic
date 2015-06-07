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

/* DataPipe is the main dynamic storage container and processor with lots of side features. */

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
#include "vsprite.h"
#include "pltime.h"
#include "plastic.h"


///Default amount of available RAM.
#define DEFRAMMAX (2ULL * 1024*1024*1024)

///Define this to enable DataPipe debugging.
#define DPDEBUG 1

///Soft lock DataPipe on each voxel access operation.
//#define DPLOCKEACHVOX 1

///Amount of time to wait between locking write operations.
#define DPWRLOCKTIME 80

///Amount of time to wait between locking write operations.
#define DPRDLOCKTIME 50

///Maximum length of argument string in INI file.
#define MAXINISTRLEN 256
///INI file string format.
#define FMTINISTRING "%s = %255[^\n]"

///Voxel table file name.
#define VOXTABFILENAME "voxtab.dat"

///Atmospherics settings file name.
#define ATMOININAME "atmosphere"

///Control bindings file name.
#define KEYBINDNAME "controls"

///Actors classes data file name.
#define CLASNFONAME "classes"


/* States of DataPipe */
enum EDPipeStatus {
	DPIPE_NOTREADY,		//pipe is partially initialized
	DPIPE_ERROR,		//error state, pipe cannot be used
	DPIPE_IDLE,			//normal state, use Lock()/Unlock() to modify the contents
	DPIPE_BUSY			//central chunk is loading
};

/* States of voxel chunk inside DataPipe */
enum EDChunkStatus {
	DPCHK_EMPTY,
	DPCHK_READY,
	DPCHK_QUEUE,
	DPCHK_LOADING,
	DPCHK_ERROR
};

/* File data placement map record */
struct SDataPlacement {
	unsigned filenum;
	vector3di pos;
	long offset;
};

/* Save file header structure */
struct SSavedGameHeader {
	PlasticTime gtime;
	int plx,pgx,ply,pgy,plz,pgz;
};

//Some shortcuts for long-named types
typedef std::map<std::string,std::string> IniData;
typedef std::map<std::string,IniData> IniMap;
typedef std::vector<VModel*> VModVec;
typedef std::vector<VSprite*> VSprVec;
typedef std::map<vector3dulli,SDataPlacement> PlaceMap;


/* ********************************** DATA PIPE MAIN ********************************** */

class DataPipe {
protected:
	EDPipeStatus status;
	SGameSettings settings;
	char root[MAXPATHLEN];				//root path
	ulli allocated;						//amount of allocated RAM
	ulli rammax;						//max amount of memory allowed to be allocated

	PChunk chunks[HOLDCHUNKS];			//world chunk buffers
	EDChunkStatus chstat[HOLDCHUNKS];	//chunks status
	PlaceMap placetab;					//chunk displacement map

	SVoxelTab voxeltab;					//voxel types table

	pthread_mutex_t vmutex;				//main voxel mutex
	pthread_mutex_t cndmtx;				//condition mutex
	int readcnt;						//read operations counter
	int writecnt;						//write operations counter

	vector3di GP;						//global position of central chunk

	WorldGen* wgen;						//world generator instance
	IniMap ini;							//map of known (and loaded) ini files
	SSavedGameHeader svhead;			//save file header data

	VModVec objs;						//objects in scene
	VSprVec sprs;						//sprites in scene

	bool Allocator(SGameSettings* sets);
	bool ScanFiles();
	bool FindChunk(vector3di pos, SDataPlacement* res);
	bool LoadVoxTab();
	bool LoadIni(const std::string name);
	void MakeChunk(unsigned l, vector3di pos);
	bool LoadChunk(SDataPlacement* res, PChunk buf);

public:
	DataPipe(SGameSettings* sets, bool allocate = true);
	virtual ~DataPipe();

	///Returns a status of the pipe.
	virtual EDPipeStatus GetStatus()	{ return status; }

	///Synchronization.
	int ReadLock();
	int ReadUnlock();
	int WriteLock();
	int WriteUnlock();

	///Returns amount of RAM allocated by buffers.
	ulli GetAllocatedRAM()				{ return allocated; }

	void SetMaxRAM(ulli m)				{ rammax = m; }
	ulli GetMaxRAM()					{ return rammax; }

	///Returns a pointer to voxel info table.
	SVoxelTab* GetVoxTable()			{ return &voxeltab; }

	///Discards all chunks buffers and release memory.
	virtual void PurgeChunks();

	///Connects (or disconnects) world generator instance.
	virtual void ConnectWorldGen(WorldGen* ptr);

	///Returns currently active world generator instance.
	virtual WorldGen* GetWorldGen()		{ return wgen; }

	///Set up the global position of central chunk.
	virtual void SetGP(vector3di pos);

	///Returns the global position of central chunk.
	virtual vector3di GetGP()			{ return GP; }

	///Move the world to next chunk.
	///Update chunks buffers either by loading or by generating.
	///Returns false if move is invalid.
	virtual bool Move(const vector3di shf);

	///Do NOT call this method in the outside code, unless you're know what you're doing!
	virtual void ChunkQueue();

	///Returns a specific voxel (or its data) in a loaded space.
	virtual voxel GetVoxel(const vector3di* p);				//return voxel code
	virtual const SVoxelInf* GetVoxelI(const vector3di* p);	//return voxel info

	///Return an information about voxel by type code.
	virtual const SVoxelInf* GetVInfo(const voxel v);

	///Return an elevation (max Z occupied by voxel) under point on XY-plane.
	virtual int GetElevationUnder(const vector3di* p);

	///Supply INI-file based data by INI name and field name.
	void GetIniDataC(const char* ininame, const char* inifield, char* dest, int maxlen);
	std::string GetIniDataS(const std::string ininame, const std::string inifield);

	///Returns a pointer to models vector.
	virtual VModVec* GetModels()					{ return &objs; }

	///Load dynamic object into scene.
	virtual VModel* LoadModel(const char* fname, const vector3di pos, const vector3di gpos);

	///Delete model from scene by pointer.
	virtual bool UnloadModel(const VModel* ptr);

	///Purge all loaded models.
	virtual void PurgeModels();

	///Find which dynamic object have a non-empty voxel at given co-ords.
	///Return voxel code and fills object pointer if supplied.
	virtual voxel IntersectModel(const vector3di* p, VModel** obj, const bool autolock);

	///Updates all scene root positions for all loaded models.
	virtual void UpdateModelsSceneRoot();

	//FIXME: comment
	virtual VSprVec* GetSprites()					{ return &sprs; }
	virtual VSprite* LoadSprite(const char* fname);
	virtual void PurgeSprites();

	//FIXME: comment
	SSavedGameHeader* LoadGameHeader();
	bool SaveGameHeader(SSavedGameHeader* hdr);
	bool LoadStaticWorld();
	bool SaveStaticWorld();
};


/* ********************************** DATA PIPE DUMMY ********************************** */
/* This class can be used as a lightweight version of DataPipe.
 * Capable of INI reading and model management, this class uses much less RAM.
 */
class DataPipeDummy : public DataPipe
{
public:
	DataPipeDummy(SGameSettings* sets);

	void SetVoxTab(SVoxelTab* p)		{ voxeltab = *p; }

	void SetGP(vector3di pos)			{}
	bool Move(const vector3di shf)		{ return false; }

	voxel GetVoxel(const vector3di* p);

	const SWGCell* GetGlobalSurfaceMap() { return NULL; }
	vector2di GetGlobalSurfaceSize()	{ return vector2di(); }
};

#endif /* DATAPIPE_H_ */
