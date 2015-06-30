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
#include "datapipe.h"

VSprite* DataPipe::LoadSprite(const char* fname)
{
	char fn[MAXPATHLEN];
	VSprite* spr = new VSprite();

	snprintf(fn,MAXPATHLEN,"%s/%s",root,fname);
	if (!spr->LoadFromFile(fn)) {
		delete spr;
		return NULL;
	}

	WriteLock();
	allocated += spr->GetAllocatedRAM();
	sprs.push_back(spr);
	WriteUnlock();

	return spr;
}

bool DataPipe::UnloadSprite(VSprite* ptr)
{
	VSprVec::iterator si;
	if (!ptr) return false;

	ReadLock();
	for (si = sprs.begin(); si != sprs.end(); ++si)
		if ((*si) == ptr) {
			ReadUnlock();
			WriteLock();
			allocated -= (*si)->GetAllocatedRAM();
			delete ((*si));
			sprs.erase(si);
			WriteUnlock();
			return true;
		}
	ReadUnlock();

	return false;
}

void DataPipe::PurgeSprites()
{
	VSprVec::iterator mi;

	WriteLock();
	for (mi = sprs.begin(); mi != sprs.end(); ++mi) {
		allocated -= (*mi)->GetAllocatedRAM();
		delete (*mi);
	}

	sprs.clear();
	WriteUnlock();
}

void DataPipe::AddSprite(VSprite* spr)
{
	if (!spr) return;

	WriteLock();
	sprs.push_back(spr);
	allocated += spr->GetAllocatedRAM();
	WriteUnlock();
}
