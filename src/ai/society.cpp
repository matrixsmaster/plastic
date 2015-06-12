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

#include "society.h"
#include "debug.h"


PlasticSociety::PlasticSociety(DataPipe* data)
{
	pipe = data;
	names = new NameGen(pipe);
}

PlasticSociety::~PlasticSociety()
{
	RemoveAllActors();
	delete names;
}

void PlasticSociety::RemoveAllActors()
{
	std::vector<PlasticActor*>::iterator it;
	for (it = actors.begin(); it != actors.end(); ++it)
		delete ((*it));
	actors.clear();
}

void PlasticSociety::UpdateActorsPresence()
{
	std::vector<PlasticActor*>::iterator it;

	for (it = actors.begin(); it != actors.end(); ++it) {
		(*it)->SetScenePos(pipe->GetGP());
		if ((*it)->GetModel()) {
			if (pipe->IsOutOfSceneGC((*it)->GetGPos())) {
				(*it)->Delete();
				dbg_logstr("Actor model removed");
			}
		} else {
			if (!pipe->IsOutOfSceneGC((*it)->GetGPos())) {
				(*it)->Spawn();
				dbg_logstr("Actor model spawned");
			}
		}
	}
}

void PlasticSociety::CreatePopulation()
{
	int i,j;
	SWGCell cell;
	vector3di cgp,clp;
	PlasticActor* npc;
	WorldGen* wgen = pipe->GetWorldGen();
	PRNGen* rng = pipe->GetRNG();

	for (i = 0; i < wgen->GetPlaneSide(); i++) {
		cgp.Y = i;
		for (j = 0; j < wgen->GetPlaneSide(); j++) {
			cgp.X = j;
			cell = wgen->GetSurfaceCell(&cgp);
			//FIXME: debug only
			if (cell.t == WGCC_SPECBUILD) {
				clp.X = CHUNKBOX / 2;
				clp.Y = clp.X;
				clp.Z = pipe->GetElevationUnder(&clp);
				npc = new PlasticActor(PCLS_COMMONER,names,pipe);
				actors.push_back(npc);
				npc->SetGPos(cgp);
				npc->SetPos(clp);
			}
		}
	}
}

PlasticActor* PlasticSociety::GetActor(VModel* mod)
{
	std::vector<PlasticActor*>::iterator oi;

	for (oi = actors.begin(); oi != actors.end(); ++oi) {
		if ((*oi)->GetModel() == mod)
			return (*oi);
	}

	return NULL;
}

PlasticActor* PlasticSociety::GetActor(uli n)
{
	if (n >= actors.size()) return NULL;
	return actors.at(n);
}
