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
#define NUMBODPART 17
#define NUMCLASSES 10

//Basic types of robotic bodies
enum EPABodyType {
	PBOD_INVALID = 0,
	PBOD_ELMECH = 1,
	PBOD_PNEUMO = 2,
	PBOD_HYDROL = 4
};

//Basic types of body parts
enum EPABodyPartType {
	PBP_INVALID = -1,
	PBP_HEAD = 0,
	PBP_NECK,
	PBP_CHEST,
	PBP_WAIST,
	PBP_PELVIS,
	PBP_LUPARM,
	PBP_LLWARM,
	PBP_LHAND,
	PBP_RUPARM,
	PBP_RLWARM,
	PBP_RHAND,
	PBP_LUPLEG,
	PBP_LLWLEG,
	PBP_LFOOT,
	PBP_RUPLEG,
	PBP_RLWLEG,
	PBP_RFOOT
};

#define PAMAXLIMBDAM 50

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
	PCLS_NONE			//Not a class actually; terminator
};

//Actor's memory-related attributes
struct SPAPsyBase {
	EPAClass Oppos;		//Opposing class (those who should be hated)
	int Eng;			//Engineering
	int Spch;			//Speechcraft
	int Brv;			//Braveness
	int Trd;			//Trade
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
	int Chr;			//Charisma (or beauty)

	/* Brain and psychology state */
	SPAPsyBase MR;		//Memory-related attribs

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

//Actor's portrait size
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

/* ********************* CONVERSION DATA ********************* */

//Body type to string conversion data
struct SPABRecord {
	EPABodyType b;
	const char* s;
};

static const SPABRecord pabody_to_str[NUMBODTYPE] = {
		{ PBOD_ELMECH,	"Electromech" },
		{ PBOD_PNEUMO,	"Pneumatic" },
		{ PBOD_HYDROL,	"Hydraulic" },
};

//Body part to string conversion data
#define PABPNUMALIASES 3

struct SPABPRecord {
	EPABodyPartType bt;
	const char* aka[PABPNUMALIASES];
};

static const SPABPRecord pabtype_to_str[NUMBODPART] = {
		{ PBP_HEAD,		{ "head","hair","face" }, },
		{ PBP_NECK,		{ "neck",NULL,NULL }, },
		{ PBP_CHEST,	{ "chest","boobs",NULL }, },
		{ PBP_WAIST,	{ "waist",NULL,NULL }, },
		{ PBP_PELVIS,	{ "pelvis",NULL,NULL }, },
		{ PBP_LUPARM,	{ "LUA",NULL,NULL }, },
		{ PBP_LLWARM,	{ "LLA",NULL,NULL }, },
		{ PBP_LHAND,	{ "Lhand",NULL,NULL }, },
		{ PBP_RUPARM,	{ "RUA",NULL,NULL }, },
		{ PBP_RLWARM,	{ "RLA",NULL,NULL }, },
		{ PBP_RHAND,	{ "Rhand",NULL,NULL }, },
		{ PBP_LUPLEG,	{ "LUL",NULL,NULL }, },
		{ PBP_LLWLEG,	{ "LLL",NULL,NULL }, },
		{ PBP_LFOOT,	{ "Lfoot",NULL,NULL }, },
		{ PBP_RUPLEG,	{ "RUL",NULL,NULL }, },
		{ PBP_RLWLEG,	{ "RLL",NULL,NULL }, },
		{ PBP_RFOOT,	{ "Rfoot",NULL,NULL }, },
};

//Class to string conversion data
struct SPACRecord {
	EPAClass c;
	const char* s;
};

static const SPACRecord paclass_to_str[NUMCLASSES+1] = {
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

/* ********************* OTHER DATA ********************* */

//Body parts hierarchy node record
#define PABPHRNUMCHILDS 3

struct SPABPHierarchy {
	EPABodyPartType t;
	EPABodyPartType n[PABPHRNUMCHILDS];
};

static const SPABPHierarchy pabp_tree[14] = {
		{ PBP_HEAD,		{ PBP_NECK, PBP_INVALID } },
		{ PBP_NECK,		{ PBP_CHEST, PBP_INVALID } },
		{ PBP_CHEST,	{ PBP_WAIST, PBP_LUPARM, PBP_RUPARM } },
		{ PBP_WAIST,	{ PBP_PELVIS, PBP_INVALID } },
		{ PBP_PELVIS,	{ PBP_LUPLEG, PBP_RUPLEG, PBP_INVALID } },
		{ PBP_LUPLEG,	{ PBP_LLWLEG, PBP_INVALID } },
		{ PBP_LLWLEG,	{ PBP_LFOOT, PBP_INVALID } },
		{ PBP_RUPLEG,	{ PBP_RLWLEG, PBP_INVALID } },
		{ PBP_RLWLEG,	{ PBP_RFOOT, PBP_INVALID } },
		{ PBP_LUPARM,	{ PBP_LLWARM, PBP_INVALID } },
		{ PBP_LLWARM,	{ PBP_LHAND, PBP_INVALID } },
		{ PBP_RUPARM,	{ PBP_RLWARM, PBP_INVALID } },
		{ PBP_RLWARM,	{ PBP_RHAND, PBP_INVALID } },
		{ PBP_INVALID,	{ PBP_INVALID } }
};

#endif /* ACTORTYPES_H_ */
