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

/* Data types used for all kinds of actors. */

#ifndef ACTORTYPES_H_
#define ACTORTYPES_H_

#include "misconsts.h"

#define NUMCLASSES 10

//Basic classes of actors (used to determine initial basic values and actor's traits)
enum EPAClass {
	PCLS_INQUISITOR,
	PCLS_ROGUE,
	PCLS_GUARD,
	PCLS_SEXBOT,
	PCLS_COMMONER,
	PCLS_MAID,
	PCLS_PSYCHO,
	PCLS_MECHANIC,
	PCLS_SMUGGLER,
	PCLS_TRADER
};

//Actor's basic value used in game mechanics
struct SPABase {
	/* Main fields */
	EPAClass Cls;		//Actor's class (mean of purpose)
	bool autoinit;		//If true, all data below will be automatically filled

	/* Status and condition */
	int HP;				//Health points (means integral system condition)
	float Qual;			//Overall actor's quality

	/* Physical state */
	int CC;				//Charge capacity or current charge
	float Spd;			//Speed multiplier (rounded result used)
	int Str;			//Basic strength on fully charged actor

	/* Brain and psychology state */
	int Eng;			//Engineering
	int Spch;			//Speechcraft
	int Brv;			//Braveness
	int Chr;			//Charisma (or beauty)
	int Trd;			//Trade

	/* Battle-relevant values */
	int AP;				//Armor points
	int DT;				//Damage threshold
	float DM;			//Damage multiplier
};

//Actor's stats
struct SPAStats {
	char name[MAXACTNAMELEN];	//Actor's name
	bool female;				//True for Female characters
	char model[MAXPATHLEN];		//Actor's model
	SPABase base;				//Basic values
};

//Class to string conversion data
struct SEPACRecord {
	EPAClass c;
	const char* s;
};

static const SEPACRecord paclass_to_str[NUMCLASSES] = {
		{ PCLS_INQUISITOR,	"Inquisitor" },
		{ PCLS_ROGUE,		"Rogue" },
		{ PCLS_GUARD,		"Guard" },
		{ PCLS_SEXBOT,		"Sexbot" },
		{ PCLS_COMMONER,	"Commoner" },
		{ PCLS_MAID,		"Maid" },
		{ PCLS_PSYCHO,		"Psycho" },
		{ PCLS_MECHANIC,	"Mechanic" },
		{ PCLS_SMUGGLER,	"Smuggler" },
		{ PCLS_TRADER,		"Merchant" },
};

#endif /* ACTORTYPES_H_ */
