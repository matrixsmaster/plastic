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
#include "debug.h"


bool PlasticWorld::NewGame()
{
	vector3di v;

	/* Reset savegame data */
	memset(&gamesave,0,sizeof(gamesave));
	gamesave.verA = VERMAJOR;
	gamesave.verB = VERMINOR;
	gamesave.verC = VERSUBVR;

	/* Init world generator */
	wgen->NewMap(data->GetMapSeed());
	data->SetGP(wgen->GetPCInitPos()); //init central chunk

	/* Delete old files */
	data->ScanFiles();

	/* Init Player */
	PC->SetGPos(wgen->GetPCInitPos());
	PC->SetScenePos(data->GetGP());
	v.X = CHUNKBOX / 2;
	v.Y = v.X;
	v.Z = data->GetElevationUnder(&v) + 1;
	PC->SetPos(v);

	/* Init game time */
	memset(&gtime,0,sizeof(gtime));
	gtime.year = PLTIMEINITYEAR;

	/* Place actors */
	society->CreatePopulation();
	dbg_print("Population: %u",society->GetNumActors());
	//FIXME: debug statistics
	for (int i = 0; i < NUMCLASSES; i++) {
		dbg_print("Class %s pop. %d, %d women",paclass_to_str[i].s,
				society->GetStatistic()[i].prc,
				society->GetStatistic()[i].female);
	}

	return true;
}

bool PlasticWorld::LoadGame()
{
	GDVec plr;
	SSavedGameHeader* hptr;

	plr.push_back(PC);

	/* Load game header and check its version */
	if (!((hptr = data->LoadGameHeader()))) return false;
	if (	(hptr->verA != VERMAJOR) ||
			(hptr->verB != VERMINOR) ||
			(hptr->verC != VERSUBVR) )
		return false;

	/* Load static world data */
	if (!data->LoadStaticWorld()) return false;

	/* Init Player */
	if (!data->DeserializeThem<PlasticActor>(&plr,"player",false)) return false;
	data->SetGP(PC->GetGPos());

	/* Load all actors */
	if (!society->Load()) return false;

	/* Restore global data */
	gamesave = *hptr;
	gtime = gamesave.gtime;
	fixtimegap = true;

	return true;
}

bool PlasticWorld::SaveGame()
{
	GDVec plr;

	/* Store global data */
	plr.push_back(PC);
	gamesave.gtime = gtime;

	/* Begin saving process */
	if (!data->SaveGameHeader(&gamesave)) return false;
	if (!data->SaveStaticWorld()) return false;
	if (!data->SerializeThem(&plr,"player")) return false;
	if (!society->Save()) return false;

	return true;
}

void PlasticWorld::PushRenderConf()
{
	gamesave.rend_fovx = render->GetFOV().X;
	gamesave.rend_fovy = render->GetFOV().Y;
	gamesave.rend_fovz = render->GetFOV().Z;
	gamesave.rend_scale = render->GetScale();
	gamesave.rend_pp = render->GetPostprocess();
	gamesave.rend_used = true;
}

void PlasticWorld::PullRenderConf()
{
	vector3d rfov;

	if (!gamesave.rend_used) return;
	rfov = vector3d(gamesave.rend_fovx,gamesave.rend_fovy,gamesave.rend_fovz);
	render->SetFOV(rfov);
	render->SetPostprocess(gamesave.rend_pp);
}

bool PlasticWorld::LoadBasicInventory()
{
	//TODO: use BASEINVININAME as INI name
	return true;
}
