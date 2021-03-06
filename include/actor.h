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
#include <ncurses.h>
#include "actortypes.h"
#include "vecmath.h"
#include "vecmisc.h"
#include "mtx3d.h"
#include "misconsts.h"
#include "CGUIEvents.h"
#include "datapipe.h"
#include "keybinder.h"
#include "vmodel.h"
#include "vsprite.h"
#include "inventory.h"
#include "namegen.h"
#include "animator.h"
#include "IGData.h"
#include "aicontroller.h"


class PlasticWorld;

//Basic Actor Class
class PlasticActor : public VSceneObject, public IGData {
protected:
	bool isnpc;				//NPC flag
	SPAAttrib attrib;		//Basic attribs
	SPABase base,curr;		//Base and current stats
	DataPipe* pipe;			//DataPipe instance
	PlasticWorld* world;	//World instance
	VModel* model;			//Actor's model
	Inventory invent;		//Actor's inventory
	VSprite* portrait;		//Actor's portrait
	bool portrait_reg;		//Actor's portrait registration flag
	DAnimator* anim;		//Actor's animations
	voxel headtxd;			//Textured head voxel id
	int limbs[NUMBODPART];	//Limbs damage array
	AIController ai;		//A.I.

	void InitVars();
	void RmPortrait();
	void RemoveLimb(EPABodyPartType x);
	void UpdateLimbs();

public:
	PlasticActor(SPAAttrib a, DataPipe* pptr);
	PlasticActor(EPAClass c, bool fem, NameGen* names, DataPipe* pptr);
	PlasticActor();
	virtual ~PlasticActor();

	///Game data interface serialization implementation.
	virtual bool SerializeToFile(FILE* f);		//keep it right after destructor
	virtual bool DeserializeFromFile(FILE* f);

	//FIXME: comment!
	void SetDataPipe(DataPipe* p)		{ pipe = p; }
	virtual void AutoInitStats();

	bool IsNPC()						{ return isnpc; }

	virtual void UpdateModelPos();
	virtual void ReadModelPos();

	virtual void SetPos(const vector3di p);
	virtual void SetRot(const vector3di r);

	Inventory* GetInventory()			{ return &invent; }
	bool UseObject(InventoryObject* obj);
	bool WearObject(InventoryObject* obj);

	SGUIPixel* GetPortrait();
	VModel* GetModel()					{ return model; }

	SPAAttrib GetAttributes()			{ return attrib; }
	SPABase GetStats(bool current);
	void SetCurStats(const SPABase ns)	{ curr = ns; }
	void ModCurStats(const SPABase ns);

	void Move(ELMoveDir d, float step);

	bool Spawn(PlasticWorld* wrld);
	void Delete();
	void Animate();

	void Damage(const vector3di* pnt);
};

//Special case for a player character
class Player : public PlasticActor {
private:
	KeyBinder* binder;
	int maxrspd,rspd;
	mmask_t rot_hor,rot_ver;
	EPCState state;

public:
	Player(SPAAttrib s, DataPipe* pptr);
	virtual ~Player();

	const char* GetStateStr();

	bool ProcessEvent(const SGUIEvent* e);
};

#endif /* ACTOR_H_ */
