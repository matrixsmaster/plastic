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


class VModel {
private:
	int s_x,s_y,s_z;		//extent
	voxel* dat;				//original data
	ulli datlen;
	voxel* buf;				//modified buffer
	ulli buflen;
	int bufside;
	vector3di pos;
	vector3d rot;
	SMatrix3d rotm;
	bool changed;
	vector3di oldrres;
	vector3di dcenter;
	vector3di center;

	bool AllocBuf();

public:
	VModel();
	virtual ~VModel();

	vector3di GetOrgSize()			{ return vector3di(s_x,s_y,s_z); }
	vector3di GetModSize()			{ return vector3di(bufside); }
	int GetBoundSide()				{ return bufside; }
	ulli GetOrgLen()				{ return datlen; }
	ulli GetModLen()				{ return buflen; }

	bool LoadFromFile(const char* fn);

	void SetPos(const vector3di p)	{ pos = p; }
	vector3di GetPos()				{ return pos; }
	vector3di* GetPosP()			{ return &pos; }
	void SetRot(const vector3d r);
	vector3d GetRot()				{ return rot; }

	void ApplyRot();

	voxel GetVoxelAt(const vector3di* p);
};

#endif /* VMODEL_H_ */
