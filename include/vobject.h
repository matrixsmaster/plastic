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

#ifndef VOBJECT_H_
#define VOBJECT_H_

#include "voxel.h"
#include "vecmath.h"
#include "vecmisc.h"
#include "mtx3d.h"


class VSceneObject {
protected:
	vector3di gpos,pos;		//Global and local position
	vector3di spos;			//Scene position
	vector3di scenter;		//Scene central chunk position
	vector3di gmov;			//Global movement vector
	vector3di rot;			//current rotation vector
	SMatrix3d rotm;			//current rotation matrix

	void SetRotI();
	void SetPosI();

public:
	VSceneObject();
	virtual ~VSceneObject()						{}

	virtual void GMove();

	virtual void SetScenePos(const vector3di* p);
	virtual void SetScenePos(const vector3di p);

	virtual void SetPos(const vector3di p);
	virtual vector3di GetPos()					{ return pos; }
	virtual const vector3di* GetPosP()			{ return &pos; }

	virtual void SetRot(const vector3di r);
	virtual vector3di GetRot()					{ return rot; }
	virtual const vector3di* GetRotP()			{ return &rot; }

	virtual void SetGPos(const vector3di p);
	virtual vector3di GetGPos()					{ return gpos; }
	virtual const vector3di* GetGPosP()			{ return &gpos; }

	virtual vector3di GetSPos()					{ return spos; }
	virtual const vector3di* GetSPosP()			{ return &spos; }

	virtual vector3di GetGMov()					{ return gmov; }
	virtual const vector3di* GetGMovP()			{ return &gmov; }
};


#endif /* VOBJECT_H_ */
