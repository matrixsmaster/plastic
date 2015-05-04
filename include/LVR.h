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

/* Lightweight Voxel Renderer */

#ifndef LVR_H_
#define LVR_H_

#include "voxel.h"
#include "datapipe.h"
#include "misconsts.h"
#include "CurseGUI.h"
#include "vecmath.h"
#include "vecmisc.h"
#include "mtx3d.h"
#include "sky.h"


#define LVRDEBUG 1
#define DEFFARPLANE 32
#define DEFFOGPLANE 21
#define DEFFOVX 28
#define DEFFOVY 14


//TODO: comment it
class LVR {
private:
	int g_w,g_h;
	uli rendsize;
	int far,fog;
	double dfog;
	vector2di mid;
	vector3d fov;
	SMatrix3d rot;
	vector3d offset,eulerot,scale;
	vector3di fogcol;
	DataPipe* pipeptr;
	SGUIPixel* render;
	float* zbuf;
	vector3di* pbuf;
	AtmoSky* skies;
	char* mask;

public:
	LVR(DataPipe* pipe);
	virtual ~LVR();

	SGUIPixel* GetRender()			{ return render; }
	uli GetRenderLen()				{ return rendsize; }

	bool Resize(int w, int h);
	void SetMask(char* m, int w, int h);
	void RemoveSkies();

	void SetEulerRotation(const vector3d r);
	void SetPosition(const vector3d pos);

	void SetScale(const double s);
	double GetScale()				{ return scale.Z; }
	void SetFOV(const vector3d f);
	vector3d GetFOV()				{ return fov; }
	void SetFarDist(const int d);
	int GetFarDist()				{ return far; }
	void SetFogStart(const int d);
	int GetFogStart()				{ return fog; }
	void SetFogColor(const vector3di nfc);
	vector3di GetFogColor()			{ return fogcol; }

	vector3di GetProjection(const vector2di pnt);

	void Frame();
	void Postprocess();
};

#endif /* LVR_H_ */
