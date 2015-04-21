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

#ifndef LVR_H_
#define LVR_H_

#include "voxel.h"
#include "datapipe.h"
#include "misconsts.h"
#include "CurseGUI.h"
#include "vecmath.h"
#include "mtx3d.h"


class LVR {
private:
	DataPipe* pipeptr;
	SGUIPixel* render;
	float* zbuf;
	uli rendsize;
	SMatrix3d rot[3];
	vector3d offset;
	SVoxelInf* table;

public:
	LVR(DataPipe* pipe);
	virtual ~LVR();

	SGUIPixel* GetRender()		{ return render; }
	uli GetRenderLen()			{ return rendsize; }
	bool Resize(int w, int h);
	void SetEulerRotation(vector3d r);
	void SetPosition(vector3d pos);
	void Frame();
};

#endif /* LVR_H_ */
