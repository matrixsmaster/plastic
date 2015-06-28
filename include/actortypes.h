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


#define NUMBODTYPE 3
#define NUMCLASSES 10

//Basic types of robotic bodies
enum EPABodyType {
	PBOD_INVALID = 0,
	PBOD_ELMECH = 1,
	PBOD_PNEUMO = 2,
	PBOD_HYDROL = 4
};

//Basic classes of actors (used to determine initial basic values and actor's traits)
enum EPAClass {
	PCLS_INQUISITOR,
	PCLS_ROGUE,
	PCLS_GUARD,
	PCLS_SEXBOT,
	PCLS_COMMONER,
	PCLS_MAID,
	PCLS_NOBLE,
	PCLS_MECHANIC,
	PCLS_SMUGGLER,
	PCLS_TRADER,
	PCLS_NONE			//Not a class actually; endpoint
};

//Actor's basic value used in game mechanics
struct SPABase {
	/* Status and condition */
	int Body;			//Body type mask or current body type
	int HP;				//Health points (means integral system condition)
	int Qual;			//Overall actor's quality (%)

	/* Physical state */
	int CC;				//Charge capacity or current charge
	int Spd;			//Speed (%)
	int Str;			//Basic strength on fully charged actor
	int Eff;			//Efficiency (%)
	int RS;				//Reaction speed
	int Acc;			//Accuracy (%)

	/* Brain and psychology state */
	EPAClass Oppos;		//Opposing class (those who should be hated)
	int Eng;			//Engineering
	int Spch;			//Speechcraft
	int Brv;			//Braveness
	int Chr;			//Charisma (or beauty)
	int Trd;			//Trade

	/* Battle-relevant values */
	int AP;				//Armor points
	int DT;				//Damage threshold
	int DM;				//Damage multiplier (%)
};

#define ACTORATTRIBMAX 5000

//Actor's attributes
struct SPAAttrib {
	char name[MAXACTNAMELEN];	//Actor's name
	bool female;				//True for Female characters
	EPAClass cls;				//Actor's class (mean of purpose)
	EPABodyType body;			//Actor's body type
	char model[MAXPATHLEN];		//Actor's model
};

//Class to string conversion data
struct SEPACRecord {
	EPAClass c;
	const char* s;
};

static const SEPACRecord paclass_to_str[NUMCLASSES+1] = {
		{ PCLS_INQUISITOR,	"Inquisitor" },
		{ PCLS_ROGUE,		"Rogue" },
		{ PCLS_GUARD,		"Guard" },
		{ PCLS_SEXBOT,		"Sexbot" },
		{ PCLS_COMMONER,	"Commoner" },
		{ PCLS_MAID,		"Maid" },
		{ PCLS_NOBLE,		"Noble" },
		{ PCLS_MECHANIC,	"Mechanic" },
		{ PCLS_SMUGGLER,	"Smuggler" },
		{ PCLS_TRADER,		"Merchant" },
		{ PCLS_NONE,		"Nobody" },
};

//Body type to string conversion data
struct SEPABRecord {
	EPABodyType b;
	const char* s;
};

static const SEPABRecord pabody_to_str[NUMBODTYPE] = {
		{ PBOD_ELMECH,	"Electromech" },
		{ PBOD_PNEUMO,	"Pneumatic" },
		{ PBOD_HYDROL,	"Hydraulic" },
};

#define ACTPORTRAITW 30
#define ACTPORTRAITH 20

//Player character state
#define PCHARNUMSTATES 4

enum EPCState {
	PCS_EXPLORING,
	PCS_INTERACTING,
	PCS_COMBAT,
	PCS_VEHICLE
};

//Save file header structure
struct SPAFileHeader {
	int gpx,gpy,gpz;
	int lpx,lpy,lpz;
	bool have_portrait;
	int port_w,port_h;
};

#endif /* ACTORTYPES_H_ */
