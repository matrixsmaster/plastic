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

#include <string>
#include "vecmath.h"
#include "vecmisc.h"
#include "mtx3d.h"
#include "misconsts.h"
#include "CGUIEvents.h"


#define MAXACTNAMELEN 25


//Basic classes of actors (used to determine initial basic values and actor's traits)
enum EPAClass {
	PCLS_INQUISITOR,
	PCLS_GUARD,
	PCLS_ROGUE,
	PCLS_SEXBOT
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
	int Intl;			//Intelligence
	int Brv;			//Braveness
	int Chr;			//Charisma (or beauty)

	/* Battle-relevant values */
	int AP;				//Armor points
	int DT;				//Damage threshold
	float DM;			//Damage multiplier
};

//Actor's stats
struct SPAStats {
	char name[MAXACTNAMELEN];	//Actor's Name
	bool female;				//True for Female characters
	SPABase base;				//Basic values
};

//Basic Actor Class
class PlasticActor {
protected:
	vector3di pos,rot;		//Position and orientation
	SPAStats stats;			//Basic stats
	SPABase curr;			//Current values
	SMatrix3d rotmat;		//Rotation in matrix form

public:
	PlasticActor(SPAStats s);
	PlasticActor(EPAClass c);
	virtual ~PlasticActor() {}

	virtual void AutoInitStats();

	void SetPos(const vector3di p)		{ pos = p; }
	void SetRot(const vector3di r);
	vector3di GetPos()					{ return pos; }
	vector3di GetRot()					{ return rot; }

	void Move(ELMoveDir d, float step);
};

//Special case for a player character
class Player : public PlasticActor {
public:
	Player(SPAStats s) : PlasticActor(s) {}
	Player(EPAClass c) : PlasticActor(c) {}
	virtual ~Player() {}

	void ProcessEvent(const CGUIEvent* e);
};

#endif /* ACTOR_H_ */
