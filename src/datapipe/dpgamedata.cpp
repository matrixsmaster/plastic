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

/* Part of DataPipe class. Game data management facilities. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datapipe.h"
#include "support.h"
#include "vecmisc.h"

#ifdef DPDEBUG
#include "debug.h"
#endif


void DataPipe::SetNewGame()
{
	memset(&svhead,0,sizeof(svhead));

	//TODO

	svhead.gtime.year = PLTIMEINITYEAR;
}

bool DataPipe::LoadLastGame()
{
	//TODO
	return false;
}

vector3di DataPipe::GetInitialPCGPos()
{
	if (svhead.pgz == 0)
		return wgen->GetPCInitPos();
	return (vector3di(svhead.pgx,svhead.pgy,svhead.pgz));
}

vector3di DataPipe::GetInitialPCLPos()
{
	vector3di r;
	r.X = CHUNKBOX / 2;
	r.Y = r.X;
	r.Z = GetElevationUnder(&r) + 1;

#ifdef DPDEBUG
	dbg_print("Player position calculated as [%d %d %d]",r.X,r.Y,r.Z);
#endif

	if (r.Z >= CHUNKBOX) {
		//FIXME: do something in this situation
	}

	return r;
}
