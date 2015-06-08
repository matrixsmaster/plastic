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

/* Part of DataPipe class. Utilities. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datapipe.h"
#include "support.h"
#include "vecmisc.h"


int DataPipe::GetElevationUnder(const vector3di* p)
{
	vector3di t = *p;

	//If not locking on each access, lock now.
#ifndef DPLOCKEACHVOX
	ReadLock();
#endif

	//Just go down by Z.
	for (t.Z = CHUNKBOX-1; t.Z >= 0; t.Z--) {
		if (GetVoxel(&t)) {
#ifndef DPLOCKEACHVOX
			//Don't forget to unlock.
			ReadUnlock();
#endif
			return t.Z;
		}
	}

	//Unlock if needed.
#ifndef DPLOCKEACHVOX
	ReadUnlock();
#endif

	//Nothing is found. Empty column.
	return -1;
}

bool DataPipe::IsOutOfScene(const vector3di pnt)
{
	int cx,cy,cz;

	cx = pnt.X / CHUNKBOX - ((pnt.X < 0)? 1:0);
	cy = pnt.Y / CHUNKBOX - ((pnt.Y < 0)? 1:0);
	cz = pnt.Z / CHUNKBOX - ((pnt.Z < 0)? 1:0);

#if HOLDCHUNKS == 1
	return ( cx + cy + cz != 0);

#elif HOLDCHUNKS == 9
	return (	(cx < -1) || (cy < -1) || (cz < 0) ||
				(cx >  1) || (cy >  1) || (cz > 0) );

#elif HOLDCHUNKS == 18
	return (	(cx < -1) || (cy < -1) || (cz < -1) ||
				(cx >  1) || (cy >  1) || (cz > 0) );

#elif HOLDCHUNKS == 27
	return (	(cx < -1) || (cy < -1) || (cz < -1) ||
				(cx >  1) || (cy >  1) || (cz >  1) );
#endif
}
