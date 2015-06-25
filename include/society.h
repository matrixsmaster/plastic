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
#include "wrldgen.h"


//FIXME: comment
//#define SCDEBUG 1
#define SCACTORELEV 6
#define SCMAXPOPPERCHUNK 10

//FIXME: comment
struct SSCCellCapacity {
	EWGCellContent ct;
	int min,max;
};

//FIXME: comment
struct SSCGenderClassification {
	EPAClass cls;
	int prc;		//percent of class volume in society
	int female;		//% of women in particular class
};

static const SSCCellCapacity cellcap_tab[WGNUMKINDS] = {
		{ WGCC_WASTELAND, 0, 0, },
		{ WGCC_DIRTNSAND, 0, 0, },
		{ WGCC_ROCKSTONE, 0, 0, },
		{ WGCC_WATERSIDE, 0, 1, },
		{ WGCC_WATERONLY, 0, 0, },
		{ WGCC_TREEGRASS, 0, 2, },
		{ WGCC_CONCRETEB, 0, 0, },
		{ WGCC_SMALLBLDS, 3, 8, },
		{ WGCC_MIDDLBLDS, 4, 16 },
		{ WGCC_HUGEBUILD,10, 24 },
		{ WGCC_SPECBUILD, 6, 10 },
		{ WGCC_CITYCENTR, 0, 0, },
};

static const SSCGenderClassification clvolume_tab[NUMCLASSES] = {
		{ PCLS_INQUISITOR,	2,	10},
		{ PCLS_ROGUE,		2,	10},
		{ PCLS_GUARD,		10,	20},
		{ PCLS_SEXBOT,		10,	90},
		{ PCLS_COMMONER,	30, 60},
		{ PCLS_MAID,		15,	80},
		{ PCLS_NOBLE,		2,	65},
		{ PCLS_MECHANIC,	5,	60},
		{ PCLS_SMUGGLER,	9,	20},
		{ PCLS_TRADER,		15,	50},
};

class PlasticSociety {
private:
	DataPipe* pipe;
	PlasticWorld* world;
	NameGen* names;
	std::vector<PlasticActor*> actors;
	ulli maxpopulation;
	SSCGenderClassification stat[NUMCLASSES];

	void RemoveAllActors();

public:
	PlasticSociety(DataPipe* data, PlasticWorld* wrld);
	virtual ~PlasticSociety();

	//FIXME: comments
	void CreatePopulation();
	bool Load();
	bool Save();

	uli GetNumActors()								{ return actors.size(); }
	PlasticActor* GetActor(VModel* mod);
	PlasticActor* GetActor(uli n);
	void GatherStatistic();
	const SSCGenderClassification* GetStatistic()	{ return stat; }

	//FIXME: comments
	void UpdateActorsPresence();
	void RollAnimations();
};

#endif /* SOCIETY_H_ */
