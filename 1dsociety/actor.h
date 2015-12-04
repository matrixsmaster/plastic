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

#ifndef ACTOR_H_
#define ACTOR_H_

#include <vector>
#include "objs.h"

//Basic classes of actors (used to determine initial basic values and actor's traits)
#define NUMCLASSES 10
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

//Basic motives
#define NUMMOTIVES 9
enum EActMotive {
	AMOT_CHARGE = 0,
	AMOT_SOCIAL,
	AMOT_SEX,
	AMOT_CONDITION,
	AMOT_FREEWILL,
	AMOT_WEALTH,
	AMOT_BEAUTY,
	AMOT_OUTFIT,
	AMOT_AGRO
};

//Actor's memory-related attributes
struct SPAPsyBase {
	EPAClass Oppos;		//Opposing class (those who should be hated)
	int Eng;			//Engineering
	int Spch;			//Speechcraft
	int Brv;			//Braveness
	int Trd;			//Trade
	int Mtv[NUMMOTIVES];//Motives vector
	int Sor;			//Sexual orientation
};

//Actor's basic value used in game mechanics
struct SPABase {
	/* Status and condition */
//	int Body;			//Body type mask or current body type
	int HP;				//Health points (means integral system condition)
	int Qual;			//Overall actor's quality (%)

	/* Physical state */
	int CC;				//Charge capacity or current charge
	int Spd;			//Speed (%)
//	int Str;			//Basic strength on fully charged actor
//	int Eff;			//Efficiency (%)
//	int RS;				//Reaction speed
//	int Acc;			//Accuracy (%)
	int Chr;			//Charisma (or beauty)

	/* Brain and psychology state */
	SPAPsyBase MR;		//Memory-related attribs

	/* Battle-relevant values */
	int AP;				//Armor points
	int DT;				//Damage threshold
//	int DM;				//Damage multiplier (%)
};

struct SPACRecord {
	EPAClass c;
	const char* s;
	float pop;
	SPABase atr;
};

static const SPACRecord paclass_tab[NUMCLASSES+1] = {
		{ PCLS_INQUISITOR,	"Inquisitor",	0.1f,	{ 100,  90, 1000, 100,  20,
							{ PCLS_ROGUE,	  0,  20,  100,  0, { 50,  5, 10, 30, 50,  5,  5,  5, 80 }, 0 }, 10,  10 }, },

		{ PCLS_ROGUE,		"Rogue",		0.1f,	{ 100,  60,  700, 100,  20,
							{ PCLS_INQUISITOR,0,  10,  100,  0, { 50,  5, 10, 30, 50,  5,  5,  5,100 }, 0 }, 10,   9 }, },

		{ PCLS_GUARD,		"Guard",		0.1f,	{ 100,  70,  750,  90,  25,
							{ PCLS_SMUGGLER,  0,  10,   80,  0, { 75, 10, 20, 50, 30, 10,  5,  5,100 }, 0 }, 10,   8 }, },

		{ PCLS_SEXBOT,		"Sexbot",		0.1f,	{ 100,  50,  100,  20,  80,
							{ PCLS_NONE,	  0,  25,    5, 10, { 10,  5,100, 10,  5,  1,100, 75,  1 }, 0 },  0,   0 }, },

		{ PCLS_COMMONER,	"Commoner",		0.1f,	{ 100,  60,  400,  50,  45,
							{ PCLS_NONE,	  0,  40,   10, 10, {100, 80, 50, 30,  5,  5,  5, 10, 30 }, 0 },  1,   1 }, },

		{ PCLS_MAID,		"Maid",			0.1f,	{ 100,  60,  300,  30,  20,
							{ PCLS_NONE,	  0,  20,   10,  0, { 30, 20, 30, 30, 10,  5, 70, 40,  5 }, 0 },  1,   1 }, },

		{ PCLS_NOBLE,		"Noble",		0.1f,	{ 100, 100, 1000, 100,  80,
							{ PCLS_NONE,	  0,  90,   10, 70, { 90, 70, 40,100,100, 90, 70, 60, 40 }, 0 },  5,   5 }, },

		{ PCLS_MECHANIC,	"Mechanic",		0.1f,	{ 100,  70,  700,  90,  30,
							{ PCLS_NONE,	 80,  20,   40, 20, { 30,  1,  5, 70, 60, 10,  5,  1, 10 }, 0 },  2,   2 }, },

		{ PCLS_SMUGGLER,	"Smuggler",		0.1f,	{ 100,  60,  700,  70,  20,
							{ PCLS_GUARD,	  0,  50,   40, 60, { 50,  5, 10, 40, 50,100,  5,  1,  5 }, 0 },  1,   1 }, },

		{ PCLS_TRADER,		"Merchant",		0.1f,	{ 100,  70,  500,  70,  50,
							{ PCLS_NONE,	  0,  70,   30, 70, { 70, 10,  5, 40, 40,100, 10,  5,  5 }, 0 },  0,   1 }, },

		{ PCLS_NONE,		"Nobody" },
};

struct SActor {
	unsigned ID;
	EPAClass cls;
	SPABase base,curr;
	bool female;
	std::vector<SDynObj>* inv;
};

#endif /* ACTOR_H_ */
