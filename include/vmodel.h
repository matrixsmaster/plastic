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

#ifndef VMODEL_H_
#define VMODEL_H_

#include "voxel.h"
#include "vecmath.h"
#include "mtx3d.h"
#include "misconsts.h"
#include "vobject.h"


class VModel : public VSceneObject {
private:
	int s_x,s_y,s_z;		//extent
	voxel** dat;			//original data
	ulli datlen;			//linear length of single state buffer
	int nstates;			//number of states available
	voxel* buf;				//modified (working) buffer
	ulli buflen;			//linear length of working buf
	int bufside;			//side length of working buf
	bool changed;			//optimization flag of rotations
	vector3di oldrres;		//old rotational result
	vector3di dcenter;		//original data center point
	vector3di center;		//working buf center point
	int state;				//current state
	SVoxelTab* vtab;		//voxel table

	bool AllocBuf();

public:
	VModel(SVoxelTab* tabptr);
	virtual ~VModel();

	vector3di GetOrgSize()			{ return vector3di(s_x,s_y,s_z); }
	vector3di GetModSize()			{ return vector3di(bufside); }

	int GetBoundSide()				{ return bufside; }
	ulli GetOrgLen()				{ return datlen; }
	ulli GetModLen()				{ return buflen; }

	int GetNumStates()				{ return nstates; }
	int GetState()					{ return state; }
	void SetState(int s);

	bool LoadFromFile(const char* fn);
	ulli GetAllocatedRAM();

	void SetRot(const vector3di r);

	void ApplyRot();

	voxel GetVoxelAt(const vector3di* p);
};

struct SVoxCharPair {
	char c;
	voxel v;
};

#endif /* VMODEL_H_ */
