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

#include "vecmisc.h"
#include "datapipe.h"


DataPipeDummy::DataPipeDummy(const SGameSettings* sets) :
		DataPipe(sets,false)
{
	if (status == DPIPE_NOTREADY) status = DPIPE_IDLE;
}

voxel DataPipeDummy::GetVoxel(const vector3di* p)
{
	VModVec::iterator mi;
	voxel tmp = 0;

	Lock();
	if (!objs.empty()) {
		for (mi = objs.begin(); mi != objs.end(); ++mi) {
			if (IsPntInsideCubeI(p,(*mi)->GetPosP(),(*mi)->GetBoundSide())) {
				tmp = (*mi)->GetVoxelAt(p);
				if (tmp) break;
			}
		}
	}
	Unlock();

	return tmp;
}