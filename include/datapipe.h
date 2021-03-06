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
#include "prngen.h"
#include "plastic.h"
#include "IGData.h"
#include "LVRconsts.h"


///Default amount of available RAM.
#define DEFRAMMAX (2ULL * 1024*1024*1024)

///Define this to enable DataPipe debugging.
//#define DPDEBUG 1

///Soft lock DataPipe on each voxel access operation.
///Don't use this feature with LVR.
//#define DPLOCKEACHVOX 1

///Amount of time to wait between locking write operations.
#define DPWRLOCKTIME 80

///Amount of time to wait between locking write operations.
#define DPRDLOCKTIME 50

///Maximum length of argument string in INI file.
#define MAXINISTRLEN 256
///INI file string format.
#define FMTINISTRING "%s = %255[^\n]"

///Maximum length of a word in a dictionary.
#define MAXDICTSTRLEN 512

///Maximum length of a voxel mark.
#define MAXVOXMARKLEN 48
///Voxel definition string format,
#define VOXTABSTRING "%c%d %hd %hd %hd %hd %hd %hd %6c %47[^\n]"
///Number of parameters in voxel definition string.
#define VOXTABSTRPARAMS 10

///Voxel table file name.
#define VOXTABFILENAME "voxtab.dat"

///Chunks save file name pattern.
#define CHUNKSAVEFILE "%s/usr/C%032llX"

///INI file name pattern.
#define INIFILEPATTERN "%s/%s.ini"

///Atmospherics settings file name.
#define ATMOININAME "atmosphere"

///Control bindings file name.
#define KEYBINDNAME "controls"

///Actors classes data file name.
#define CLASNFONAME "classes"

///Game text messages db file name.
#define MSGSDBFNAME "messages"

///Dictionary file name pattern.
#define DICTFILEPATTERN "%s/dct/%s.dat"

///Game save data 'header' file.
#define GAMEHDRFNPAT "%s/usr/savegame"

///World map file name pattern.
#define WORLDMAPFNPAT "%s/usr/worldmap"

///Package file name pattern.
#define PACKAGEFNPAT "%s/usr/%s.pck"

///Global inventory base file name.
#define BASEINVININAME "iobjects"


/* **************************** DATA PIPE SUPPORT TYPES **************************** */

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

/* Chunk's state data structure */
struct SChunkState {
	EDChunkStatus s;
	bool changed;
	SVector3di pos;
};

/* Chunk data file sub-header structure */
struct SChunkFileRec {
	SVector3di pos;
	ulli length;
};

/* File data placement map record */
struct SDataPlacement {
	ulli filenum;
	vector3di pos;
	ulli offset;
	ulli length;
};

/* Save file header structure */
struct SSavedGameHeader {
	int verA,verB,verC;
	PlasticTime gtime;
	long gseed;
	bool rend_used;
	float rend_scale;
	float rend_fovx,rend_fovy,rend_fovz;
	SLVRPostProcess rend_pp;
};

//Some shortcuts for long-named types
typedef std::map<std::string,std::string> IniData;
typedef std::map<std::string,IniData> IniMap;
typedef std::vector<VModel*> VModVec;
typedef std::vector<VSprite*> VSprVec;
typedef std::map<ulli,SDataPlacement> PlaceMap;
typedef std::vector<std::string> DPDict;
typedef std::map<std::string,DPDict> DPDictMap;
typedef std::vector<IGData*> GDVec;


/* ********************************** DATA PIPE MAIN ********************************** */

class DataPipe {
protected:
	EDPipeStatus status;
	SGameSettings settings;
	char root[MAXPATHLEN];				//root path
	ulli allocated;						//amount of allocated RAM
	ulli rammax;						//max amount of memory allowed to be allocated

	PChunk chunks[HOLDCHUNKS];			//world chunk buffers
	SChunkState chstat[HOLDCHUNKS];		//chunks state buffer
	PlaceMap placetab;					//chunk displacement map
	ulli chsavelast;					//number of last chunks save file

	SVoxelTab voxeltab;					//voxel types table

	pthread_mutex_t vmutex;				//main voxel mutex
	pthread_mutex_t cndmtx;				//condition mutex
	int readcnt;						//read operations counter
	int writecnt;						//write operations counter

	vector3di GP;						//global position of central chunk

	WorldGen* wgen;						//world generator instance
	long mapseed;						//world map seed value

	PRNGen* rngen;						//main random numbers generator instance
	IniMap ini;							//map of known (and loaded) ini files
	DPDictMap dicts;					//all loaded dictionaries

	SSavedGameHeader svhead;			//save file header data

	VModVec objs;						//objects in scene
	VSprVec sprs;						//sprites in scene

	bool Allocator(SGameSettings* sets);

	bool FindChunk(const vector3di pos, SDataPlacement* res);
	void MakeChunk(const unsigned l, const vector3di pos);
	bool LoadChunk(SDataPlacement* res, PChunk buf);
	void SaveChunk(const unsigned l);

	bool LoadVoxTab();
	void FreeVoxTab();

	inline bool ConvertSceneCoord(const vector3di* p, int* px, int* py, int* pz, unsigned* l);
	ulli GetChunkLinearOffset(const vector3di p);

	bool LoadIni(const std::string name);
	bool LoadDict(const std::string name);

	FILE* DeserialOpen(const char* fn);

public:
	DataPipe(SGameSettings* sets, bool allocate = true);
	virtual ~DataPipe();

	///Returns a status of the pipe.
	virtual EDPipeStatus GetStatus()	{ return status; }

	///Returns a pointer to a game settings structure.
	virtual const SGameSettings* GetGameSettings()	{ return &settings; }

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

	///Scans user data files and maps known chunks or purges them if needed.
	///Note: this method should be called only after legitimate WorldGen was set.
	virtual void ScanFiles();

	///Connects (or disconnects) world generator instance.
	virtual void ConnectWorldGen(WorldGen* ptr);

	///Returns currently active world generator instance.
	virtual WorldGen* GetWorldGen()		{ return wgen; }

	///Returns currently active RNG.
	virtual PRNGen* GetRNG()			{ return rngen; }

	///Returns generated and stored map seed.
	virtual long GetMapSeed()			{ return mapseed; }

	///Set up the global position of central chunk.
	virtual void SetGP(vector3di pos);

	///Returns the global position of central chunk.
	virtual vector3di GetGP()			{ return GP; }

	///Move the world to next chunk.
	///Update chunks buffers either by loading or by generating.
	///Returns false if move is invalid.
	virtual bool Move(const vector3di shf);

	///Do NOT call this method in the outside code, unless you know what you're doing!
	virtual void ChunkQueue();

	///Returns a specific voxel (or its data) in a loaded space.
	///Set dynskip to skip testing for dynamic objects.
	virtual voxel GetVoxel(const vector3di* p, bool dynskip = false);				//return voxel code
	virtual const SVoxelInf* GetVoxelI(const vector3di* p, bool dynskip = false);	//return voxel info

	///Returns an information about the voxel by id.
	virtual const SVoxelInf* GetVInfo(const voxel v);

	///Returns an information about the voxel by mark.
	virtual const SVoxelInf* GetVInfo(const char* mrk);

	///Appends a new voxel into voxel table and returns its index.
	///That index then can be used as a new voxel, until restart of DataPipe.
	virtual voxel AppendVoxel(const SVoxelInf* nvox);

	///Tries to remove a voxel from the universe.
	///This voxel will only be marked as removed.
	virtual void RemoveVoxel(voxel v);

	///Return an elevation (max Z occupied by voxel) under some point on XY-plane.
	virtual int GetElevationUnder(const vector3di* p);

	///Changes a voxel at a given scene co-ords.
	virtual void ChangeVoxelAt(const vector3di* p, const voxel nv);

	///Supply INI-file based data by INI name and field name.
	void GetIniDataC(const char* ininame, const char* inifield, char* dest, int maxlen);
	char* GetIniDataC(const char* ininame, const char* inifield);
	std::string GetIniDataS(const std::string ininame, const std::string inifield);

	///Returns a pointer to models vector.
	virtual VModVec* GetModels()					{ return &objs; }

	///Load dynamic object into scene.
	virtual VModel* LoadModel(const char* fname, const vector3di pos, const vector3di gpos);

	///Delete model from scene by pointer.
	virtual bool UnloadModel(VModel* ptr);

	///Purge all loaded models.
	virtual void PurgeModels();

	///Find which dynamic object have a non-empty voxel at given co-ords.
	///Return voxel code and fills object pointer if supplied.
	///Excludes model 'excl' from search if specified.
	virtual voxel IntersectModel(const vector3di* p, VModel** obj, const VModel* excl, const bool autolock);

	///Adds an existing model to working set.
	virtual void AddModel(VModel* obj);

	///Explicitly removes a particular model from a working set (doesn't destroy a model).
	virtual bool RemoveModel(VModel* obj);

	///Updates all scene root positions for all loaded models.
	virtual void UpdateModelsSceneRoot();

	///Returns true if the given point is out of current scene borders (Local coords).
	virtual bool IsOutOfSceneLC(const vector3di pnt);

	///Returns true if the given point is out of current scene borders (Global coords).
	virtual bool IsOutOfSceneGC(const vector3di pnt);

	///Returns a pointer to sprites vector.
	virtual VSprVec* GetSprites()					{ return &sprs; }

	///Loads a sprite into memory.
	virtual VSprite* LoadSprite(const char* fname);

	///Removes a sprite from memory.
	virtual bool UnloadSprite(VSprite* ptr);

	///Removes all the sprites from memory.
	virtual void PurgeSprites();

	///Adds an existing sprite to working set.
	virtual void AddSprite(VSprite* spr);

	///Returns a dictionary (vector of strings) by its name.
	DPDict* GetDictionary(const char* dct_name);		//old style
	///Returns a dictionary (vector of strings) by its name.
	DPDict* GetDictionary(const std::string dct_name);	//new style

	//FIXME: comment
	SSavedGameHeader* LoadGameHeader();
	bool SaveGameHeader(SSavedGameHeader* hdr);
	bool LoadStaticWorld();
	bool SaveStaticWorld();

	//FIXME: comment
	template <class T> bool DeserializeThem(GDVec* arr, const char* name, bool alloc = true);
	bool SerializeThem(GDVec* arr, const char* name);
};

/* Specialization of the deserializing template should be here, in header file. */
template <class T> bool DataPipe::DeserializeThem(GDVec* arr, const char* name, bool alloc)
{
	FILE* f;
	ulli i,n = 1;
	T* p;

	/* Check input */
	if ((!arr) || ((!alloc) && (arr->empty()))) return false;
	if (!((f = DeserialOpen(name)))) return false;

	/* Load number of elements */
	if ((!fread(&n,sizeof(n),1,f)) || (!n)) {
		fclose(f);
		return (n == 0); //if a file contains zero objects, it's fine
	}

	/* Load objects */
	for (i = 0; (i < n) && (!feof(f)); i++) {
		/* Check allocation */
		if ((!alloc) && (i >= arr->size())) break;
		/* Create new or use an existing object */
		if (alloc) {
			p = new T();
			arr->push_back(p);
		} else
			p = static_cast<T*> (arr->at(i));
		/* Load it up */
		if (!p->DeserializeFromFile(f)) break;
	}

	/* We're done */
	fclose(f);
	return (i == n); //we should load exactly N objects
}


/* ********************************** DATA PIPE DUMMY ********************************** */
/* This class can be used as a lightweight version of DataPipe.
 * Capable of INI reading and model management, this class uses much less RAM.
 */
class DataPipeDummy : public DataPipe
{
public:
	DataPipeDummy(SGameSettings* sets);
	virtual ~DataPipeDummy();

	void SetVoxTab(SVoxelTab* p)				{ voxeltab = *p; }

	void SetGP(vector3di pos)					{}
	bool Move(const vector3di shf)				{ return false; }

	voxel GetVoxel(const vector3di* p);
	voxel GetVoxel(const vector3di* p, bool)	{ return GetVoxel(p); }
};

#endif /* DATAPIPE_H_ */
