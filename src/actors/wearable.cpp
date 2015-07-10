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

#include "inventory.h"

using namespace std;

WearableObject::WearableObject() : InventoryObject(INVK_WEARABLE)
{
	oldmod = NULL;
}

WearableObject::~WearableObject()
{
	if (oldmod) RemoveFromModel(oldmod);
}

void WearableObject::PushPrivateData(FILE* f)
{
	//TODO
}

bool WearableObject::PullPrivateData(FILE* f)
{
	//TODO
	return true;
}

void WearableObject::AddReplacement(const voxel old_id, const voxel new_id)
{
	replacement.insert(make_pair(old_id,new_id));
}

void WearableObject::AddReplacement(const SVoxelTab* vtab, const char* oldmkr, const char* newmrk)
{
	//TODO
}

void WearableObject::RemoveReplacement(const voxel original_id)
{
	if (replacement.count(original_id))
		replacement.erase(original_id);
}

void WearableObject::ApplyToModel(VModel* mod)
{
	map<voxel,voxel>::iterator im;

	//check model
	if (!mod) {
		oldmod = NULL;
		return;
	}

	//replace voxels
	for (im = replacement.begin(); im != replacement.end(); ++im)
		mod->ReplaceVoxel(im->first,im->second);

	oldmod = mod;
}

void WearableObject::RemoveFromModel(VModel* mod)
{
	map<voxel,voxel>::iterator im;

	//check model
	if (!mod) {
		oldmod = NULL;
		return;
	}

	//replace known voxels backwards (using new id as original id)
	for (im = replacement.begin(); im != replacement.end(); ++im)
		mod->ReplaceVoxel(im->second,im->first);

	oldmod = NULL;
}
