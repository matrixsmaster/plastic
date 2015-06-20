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

/* Part of DataPipe class. VModel management facilities. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datapipe.h"
#include "support.h"
#include "vecmisc.h"


VModel* DataPipe::LoadModel(const char* fname, const vector3di pos, const vector3di gpos)
{
	VModel* m;
	char fn[MAXPATHLEN];
	if ((status != DPIPE_IDLE) || (allocated >= rammax)) return NULL;

	m = new VModel(&voxeltab);

	snprintf(fn,MAXPATHLEN,"%s/%s",root,fname);
	if (!m->LoadFromFile(fn)) {
		delete m;
		return NULL;
	}

	allocated += m->GetAllocatedRAM();
	m->SetPos(pos);
	m->SetGPos(gpos);
	m->SetScenePos(GP);

	WriteLock();
	objs.push_back(m);
	WriteUnlock();

	return m;
}

bool DataPipe::UnloadModel(const VModel* ptr)
{
	VModVec::iterator it;
	if (!ptr) return false;

	for (it = objs.begin(); it != objs.end(); ++it)
		if ((*it) == ptr) {
			WriteLock();
			allocated -= (*it)->GetAllocatedRAM();
			delete ((*it));
			objs.erase(it);
			WriteUnlock();
			return true;
		}

	return false;
}

void DataPipe::PurgeModels()
{
	VModVec::iterator mi;

	WriteLock();
	for (mi = objs.begin(); mi != objs.end(); ++mi) {
		allocated -= (*mi)->GetAllocatedRAM();
		delete (*mi);
	}

	objs.clear();
	WriteUnlock();
}

voxel DataPipe::IntersectModel(const vector3di* p, VModel** obj, const bool autolock)
{
	voxel r;
	VModVec::iterator mi;

	if (autolock) ReadLock();

	if (!objs.empty()) {
		for (mi = objs.begin(); mi != objs.end(); ++mi) {
			if (IsPntInsideCubeI(p,(*mi)->GetSPosP(),(*mi)->GetBoundSide())) {
				r = (*mi)->GetVoxelAt(p);
				if (r) {
					if (autolock) ReadUnlock();
					if (obj) *obj = *mi;
					return r;
				}
			}
		}
	}

	if (autolock) ReadUnlock();
	if (obj) *obj = NULL;
	return 0;
}

void DataPipe::AddModel(VModel* obj)
{
	WriteLock();
	objs.push_back(obj);
	WriteUnlock();
}

bool DataPipe::RemoveModel(VModel* obj)
{
	VModVec::iterator mi;

	if (objs.empty()) return false;

	for (mi = objs.begin(); mi != objs.end(); ++mi)
		if ((*mi) == obj) {
			WriteLock();
			objs.erase(mi);
			WriteUnlock();
			return true;
		}

	return false;
}

void DataPipe::UpdateModelsSceneRoot()
{
	VModVec::iterator mi;

	if (objs.empty()) return;
	for (mi = objs.begin(); mi != objs.end(); ++mi)
		(*mi)->SetScenePos(GP);
}
