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

#include <string.h>
#include "society.h"

#ifdef SCDEBUG
#include "debug.h"
#endif

using namespace std;

PlasticSociety::PlasticSociety(DataPipe* data, PlasticWorld* wrld)
{
	pipe = data;
	world = wrld;
	names = new NameGen(pipe);
	maxpopulation = 0;
}

PlasticSociety::~PlasticSociety()
{
	RemoveAllActors();
	delete names;
}

void PlasticSociety::RemoveAllActors()
{
	vector<PlasticActor*>::iterator it;
	for (it = actors.begin(); it != actors.end(); ++it)
		delete ((*it));
	actors.clear();
}

void PlasticSociety::UpdateActorsPresence()
{
	vector<PlasticActor*>::iterator it;

	for (it = actors.begin(); it != actors.end(); ++it) {
		(*it)->SetScenePos(pipe->GetGP());
		if ((*it)->GetModel()) {
			if (pipe->IsOutOfSceneGC((*it)->GetGPos())) {
				(*it)->Delete();
#ifdef SCDEBUG
				dbg_logstr("Actor model removed");
#endif
			}
		} else {
			if (!pipe->IsOutOfSceneGC((*it)->GetGPos())) {
				(*it)->Spawn(world);
#ifdef SCDEBUG
				dbg_logstr("Actor model spawned");
#endif
			}
		}
	}
}

void PlasticSociety::CreatePopulation()
{
	int i,j,k,n,q,c,w;
	bool f;
	SWGCell cell;
	vector3di cgp,clp;
	PlasticActor* npc;
	WorldGen* wgen = pipe->GetWorldGen();
	PRNGen* rng = pipe->GetRNG();

	maxpopulation = wgen->GetPlaneArea() * SCMAXPOPPERCHUNK;
#ifdef SCDEBUG
	dbg_print("MAX population = %llu",maxpopulation);
#endif

	for (i = 0; i < wgen->GetPlaneSide(); i++) {
		cgp.Y = i;
		for (j = 0; j < wgen->GetPlaneSide(); j++) {
			cgp.X = j;
			cell = wgen->GetSurfaceCell(&cgp);

			for (k = 0; k < WGNUMKINDS; k++) {
				if (cellcap_tab[k].ct != cell.t) continue;

				//calc population of the cell
				n = rng->RangedNumber(cellcap_tab[k].max-cellcap_tab[k].min+1);
				n += cellcap_tab[k].min;

				for (q = 0; q < n; q++) {
					//generate class and gender
					do {
						c = rng->RangedNumber(NUMCLASSES);
						for (w = 0; w < NUMCLASSES; w++) {
							if (clvolume_tab[w].cls == (EPAClass)c) {
								if (rng->RangedNumber(100) <= clvolume_tab[w].prc) {
									f = (rng->RangedNumber(100) <= clvolume_tab[w].female);
									break;
								} else {
									c = 0;
									break;
								}
							}
						}
					} while (!c);

					//generate spawn spot
					clp.X = rng->RangedNumber(CHUNKBOX);
					clp.Y = rng->RangedNumber(CHUNKBOX);
					clp.Z = pipe->GetElevationUnder(&clp) + SCACTORELEV;

					//create it
					npc = new PlasticActor(clvolume_tab[w].cls,f,names,pipe);
					npc->SetGPos(cgp);
					npc->SetPos(clp);
					actors.push_back(npc);
				}

#ifdef SCDEBUG
				if (n) dbg_print("Generated %d actors on [%d %d]",n,j,i);
#endif
				break;
			}
		}
	}

	GatherStatistic();
}

bool PlasticSociety::Load()
{
	GDVec* vec;

	vec = reinterpret_cast<GDVec*> (&actors);
	//FIXME
	pipe->DeserializeThem(vec,"actors");

	GatherStatistic();
	return true;
}

PlasticActor* PlasticSociety::GetActor(VModel* mod)
{
	vector<PlasticActor*>::iterator oi;

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

void PlasticSociety::GatherStatistic()
{
	vector<PlasticActor*>::iterator oi;
	SPAAttrib ca;

	memset(&stat,0,sizeof(stat));

	for (oi = actors.begin(); oi != actors.end(); ++oi) {
		ca = (*oi)->GetAttributes();
		stat[ca.cls].prc++;
		if (ca.female) stat[ca.cls].female++;
	}
}

void PlasticSociety::RollAnimations()
{
	vector<PlasticActor*>::iterator oi;

	pipe->ReadLock();
	for (oi = actors.begin(); oi != actors.end(); ++oi)
		(*oi)->Animate();
	pipe->ReadUnlock();
}
