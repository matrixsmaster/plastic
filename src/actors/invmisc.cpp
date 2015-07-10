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

/* Implementation module for miscellaneous inventory object types */

#include "inventory.h"

using namespace std;

/* ******************************************************************** */

VoxelObject::VoxelObject() : InventoryObject(INVK_VOXEL)
{
	stored = 0;
	name = "voxel"; //FIXME: testing
}

VoxelObject::VoxelObject(voxel s) : InventoryObject(INVK_VOXEL)
{
	stored = s;
	name = "voxel"; //FIXME: testing
}

void VoxelObject::PushPrivateData(FILE* f)
{
	fwrite(&stored,sizeof(stored),1,f);
}

bool VoxelObject::PullPrivateData(FILE* f)
{
	return (fread(&stored,sizeof(stored),1,f) == 1);
}

/* ******************************************************************** */

AIPartObject::AIPartObject() : InventoryObject(INVK_AIPART)
{
	chip = NULL;
	controller = false;
}

AIPartObject::~AIPartObject()
{
	if (chip) delete chip;
}

void AIPartObject::PushPrivateData(FILE* f)
{
	fwrite(&controller,sizeof(controller),1,f);
	if ((!controller) && (chip)) chip->PushData(f);
}

bool AIPartObject::PullPrivateData(FILE* f)
{
	if (!fread(&controller,sizeof(controller),1,f))
		return false;

	if (controller) {
		chip = new AIChip();
		return (chip->PullData(f));
	}

	return true; //just a controller
}
