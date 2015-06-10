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
}

PlasticSociety::~PlasticSociety()
{
	RemoveAllActors();
}

//FIXME: debug only
bool PlasticSociety::TestActorSpawn(PlasticActor* PC)
{
	PlasticActor* npc = new PlasticActor(PCLS_COMMONER,PBOD_PNEUMO,pipe);
	actors.push_back(npc);
	npc->SetPos(PC->GetPos());
	npc->SetGPos(PC->GetGPos());
	npc->SetRot(PC->GetRot());
	return (npc->Spawn());
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
	//TODO
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
