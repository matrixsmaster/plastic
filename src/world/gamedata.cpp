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

#include "world.h"
#include "datapipe.h"


bool PlasticWorld::NewGame()
{
	vector3di v;

	/* Init world generator */
	wgen->NewMap(data->GetMapSeed());
	PC->SetGPos(wgen->GetPCInitPos());
	data->SetGP(PC->GetGPos()); //init central chunk
	v.X = CHUNKBOX / 2;
	v.Y = v.X;
	v.Z = data->GetElevationUnder(&v) + 1;
	PC->SetPos(v);

	/* Init game time */
	memset(&gtime,0,sizeof(gtime));
	gtime.year = PLTIMEINITYEAR;

	/* Place actors */
	//TODO

	return true;
}

bool PlasticWorld::LoadGame()
{
	//TODO
	return false;
}

bool PlasticWorld::SaveGame()
{
	if (!data->SaveGameHeader(&gamesave)) return false;
	if (!data->SaveStaticWorld()) return false;

	return true;
}
