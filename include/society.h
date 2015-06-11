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

#ifndef SOCIETY_H_
#define SOCIETY_H_

#include <vector>
#include "vecmath.h"
#include "datapipe.h"
#include "actor.h"
#include "misconsts.h"
#include "namegen.h"


class PlasticSociety {
private:
	DataPipe* pipe;
	NameGen* names;
	std::vector<PlasticActor*> actors;

	void RemoveAllActors();

public:
	PlasticSociety(DataPipe* data);
	virtual ~PlasticSociety();

	//FIXME: comments
	void UpdateActorsPresence();
	bool TestActorSpawn(PlasticActor* PC); //FIXME: debug only
	void CreatePopulation();
	uli GetNumActors()						{ return actors.size(); }
	PlasticActor* GetActor(VModel* mod);
	PlasticActor* GetActor(uli n);
};

#endif /* SOCIETY_H_ */
