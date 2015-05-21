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

/* Main Game Actor Definition file. Used for all kinds of actors. */

#ifndef ACTOR_H_
#define ACTOR_H_

#include <string>
#include "actortypes.h"
#include "vecmath.h"
#include "vecmisc.h"
#include "mtx3d.h"
#include "misconsts.h"
#include "CGUIEvents.h"
#include "datapipe.h"
#include "keybinder.h"
#include "vmodel.h"
#include "inventory.h"


//Basic Actor Class
class PlasticActor {
protected:
	vector3di gpos,pos;		//Global and local position
	SPAAttrib attrib;		//Basic attribs
	SPABase base,curr;		//Base and current stats
	vector3di rot;			//Orientation
	SMatrix3d rotmat;		//Rotation in matrix form
	DataPipe* pipe;			//DataPipe instance
	VModel* model;			//Actor's model
	Inventory invent;		//Actor's inventory
	SGUIPixel* portrait;	//Actor's portrait

	void InitVars();

public:
	PlasticActor(SPAAttrib a, DataPipe* pptr);
	PlasticActor(EPAClass c, EPABodyType b, DataPipe* pptr);
	virtual ~PlasticActor();

	virtual void AutoInitStats();

	void SetPos(const vector3di p)		{ pos = p; }
	void SetRot(const vector3di r);
	vector3di GetPos()					{ return pos; }
	vector3di GetRot()					{ return rot; }
	void SetGPos(const vector3di p)		{ gpos = p; }
	vector3di GetGPos()					{ return gpos; }

	Inventory* GetInventory()			{ return &invent; }

	SGUIPixel* GetPortrait()			{ return portrait; }

	void Move(ELMoveDir d, float step);

	bool Spawn();
	void Delete();
};

//Special case for a player character
class Player : public PlasticActor {
private:
	KeyBinder* binder;

public:
	Player(SPAAttrib s, DataPipe* pptr);
	virtual ~Player();

	void ProcessEvent(const SGUIEvent* e);
};

#endif /* ACTOR_H_ */
