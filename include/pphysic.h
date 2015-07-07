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

#ifndef PPHYSIC_H_
#define PPHYSIC_H_

#include <vector>
#include "datapipe.h"
#include "vecmath.h"
#include "vmodel.h"

#define CONTACT 1
//#define PHYDEBUG


struct SPPModelRec {
	VModel* modptr;
	vector3di oldspos;
	vector3di newpos;
	bool moved;
	bool changed;
	bool contact;
};

typedef std::vector<SPPModelRec> PPModVec;

struct SPPCollision {
	vector3di start;
	int depth;
	bool no_collision;
	SPPModelRec* next_obj;
};

class PlasticPhysics {
private:
	DataPipe* pipe;
	volatile bool locked;
	PPModVec mods;

	bool CheckSurroundingVox(const VModel* ptr, const vector3di p);
	vector3di ResolveCollision(const SPPCollision ccol, const vector3di v);
	bool Contact(const SPPModelRec* mod);

public:
	PlasticPhysics(DataPipe* pipeptr);
	virtual ~PlasticPhysics();

	void SetPause(bool on)			{ locked = on; }
	bool GetPause()					{ return locked; }

	const SPPCollision Collision(const SPPModelRec* mod);

	void Quantum();
};

#endif /* PPHYSIC_H_ */
