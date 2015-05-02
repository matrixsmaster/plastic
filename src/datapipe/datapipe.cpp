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

	/* Copy root path */
	i = strlen(root_dir);
	if (i > 0) memcpy(root,root_dir,i);
	else return;

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

	/* Load external files */
	if (!LoadVoxTab()) return;	//Voxel table
	ScanFiles();				//map known chunks

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
	if (voxeltab) free(voxeltab);
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
	for (it = placetab.begin(); it != placetab.end(); ++it) {
//		if ((*it)->)
	}

	return false; //FIXME
}

bool DataPipe::LoadVoxTab()
{
	FILE* vtf;
	char pth[MAXPATHLEN];
	char fx;
	SVoxelInf cvf;
	int r,n = 0;

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

		if (r < 9) continue;
		if (fx == 'V') voxeltab[n++] = cvf;
		if (n >= voxtablen) break;
	}

	fclose(vtf);
	return true;
}

void DataPipe::PurgeChunks()
{
	int i;
	for (i = 0; i < HOLDCHUNKS; i++)
		if (chunks[i]) {
			free(chunks[i]);
			chunks[i] = NULL;
			allocated -= sizeof(VChunk);
		}
//	allocated = 0;
}

void DataPipe::SetGP(vector3dulli pos)
{
	SDataPlacement plc;
	gp = pos;

#if HOLDCHUNKS == 1
	/* One chunk right there, simplest scenario */
	if (!FindChunk(pos,&plc))
		wgen->GenerateChunk(chunks[0]);

#elif HOLDCHUNKS == 9
	/* One 3x3 plane of chunks, most widely used scenario */
	int i,j,l;
	vector3dulli cur(pos);
	for (i = -1, l = 0; i < 2; i++) {
		cur.Y = pos.Y + i;
		for (j = -1; j < 2; j++, l++) {
			cur.X = pos.X + j;
			if (!FindChunk(cur,&plc))
				wgen->GenerateChunk(chunks[l]);
		}
	}

	//FIXME: all directions
#elif HOLDCHUNKS == 18
	/* Two 3x3 planes (one right there, and one upper) */

#elif HOLDCHUNKS == 27
	/* Full set of 3x3x3 (the most memory hungry scenario) */

#else
#error "Invalid value of HOLDCHUNKS!"
#endif

	status = DPIPE_IDLE;
}

bool DataPipe::Move(EGMoveDir dir)
{
	//TODO
	return false;
}

voxel DataPipe::GetVoxel(const vector3di* p)
{
	PChunk ch;
	if (status != DPIPE_IDLE) return 0;

#if HOLDCHUNKS == 1
	/* Simplest case */

	if ((p->X < 0) || (p->Y < 0) || (p->Z < 0)) return 0;
	if ((p->X >= CHUNKBOX) || (p->Y >= CHUNKBOX) || (p->Z >= CHUNKBOX)) return 0;
	ch = chunks[0];
	return ((*ch)[p->Z][p->Y][p->X]);

#else

	/* Other cases */
	int x,y,z,px,py,pz;
	x = p->X / CHUNKBOX - ((p->X < 0)? 1:0);
	y = p->Y / CHUNKBOX - ((p->Y < 0)? 1:0);
	z = p->Z / CHUNKBOX - ((p->Z < 0)? 1:0);
	px = abs(p->X) % CHUNKBOX;
	py = abs(p->Y) % CHUNKBOX;
	pz = abs(p->Z) % CHUNKBOX;

#if HOLDCHUNKS == 9
	if ((x < -1) || (y < -1) || (z < 0)) return 0;
	if ((x > 1) || (y > 1) || (z > 0)) return 0;
	++y; ++x; //centerize
	ch = chunks[y*3+x];

	//FIXME: use other chunks
#elif HOLDCHUNKS == 18
#else
#endif

	return ((*ch)[pz][py][px]);
#endif
}

SVoxelInf* DataPipe::GetVoxelI(const vector3di* p)
{
	voxel v = GetVoxel(p);
	if (v < voxtablen) return &voxeltab[v];
	else return NULL;
}

SVoxelInf* DataPipe::GetVInfo(const voxel v)
{
	if (v < voxtablen) return &voxeltab[v];
	else return NULL;
}

bool DataPipe::LoadIni(const std::string name)
{
	FILE* f;
	int r;
	char nm[MAXPATHLEN];
	char key[MAXINISTRLEN],fld[MAXINISTRLEN];
	IniData cur;

	//make filepath and try to open file
	snprintf(nm,MAXPATHLEN,"%s/%s.ini",root,name.c_str());
	f = fopen(nm,"r");
	if (!f) {
		errout("Unable to open INI file '%s'\n",nm);
		return false;
	}

	//loading
	while (!feof(f)) {
		r = fscanf(f,"%s = %s\n",key,fld);
		if (r == 2)
			cur.insert(make_pair(std::string(key),std::string(fld)));
	}
	fclose(f);

	//append to ini map
	ini.insert(make_pair(name,cur));

	return true;
}

void DataPipe::GetIniDataC(const char* ininame, const char* inifield, char* dest, int maxlen)
{
	/* for use in old-styled code */
	std::string nm(ininame), fl(inifield);
	if ((!dest) || (maxlen < 2)) return;
	strncpy(dest,GetIniDataS(nm,fl).c_str(),maxlen);
}

std::string DataPipe::GetIniDataS(const std::string ininame, const std::string inifield)
{
	/* for use in new-styled code */
	std::string res;
	IniData* fnd;

	//search for file in known files map
	if (!ini.count(ininame)) {
		//not found, try to load up
		if (!LoadIni(ininame)) return res;
	}

	//search for field
	fnd = &(ini[ininame]);
	if (!fnd->count(inifield)) return res;
	res = (*fnd)[inifield];

	return res;
}
