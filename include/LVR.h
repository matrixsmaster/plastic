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


//#define LVRDEBUG 1
#define LVRDOUBLEBUFFERED 1

#define DEFSCALE 1.f
#define DEFFOVX 115
#define DEFFOVY 62
#define DEFFARPLANE 61
#define DEFFOGPLANE 21
#define DEFFOGGRAY 40


class LVR {
protected:
	int g_w,g_h;					//Graphics width and height
	uli rendsize;					//Linear size of the frame
	int far,fog;					//Far plane distance and Fog start plane distance
	double dfog;					//Fog differential
	vector2di mid;					//Center point of the screen
	vector3d fov;					//Field Of View
	SMatrix3d rot;					//Camera rotation matrix
	vector3d offset,eulerot,scale;	//Camera transformations
	vector3di fogcol;				//Fog color
	DataPipe* pipeptr;				//DataPipe instance
	SGUIPixel* render;				//Rendered frame data pointer
	int activebuf;					//Active (drawing) buffer number
	int* zbuf;						//Depth buffer
	vector3di* pbuf;				//Point (voxel) buffer
	char* mask;						//Renderer mask (which places will not be rendered)

	void ReallocBuffers();

public:
	LVR(DataPipe* pipe);
	virtual ~LVR();

	//TODO: comment it
	virtual SGUIPixel* GetRender();
	uli GetRenderLen()						{ return rendsize; }

	virtual bool Resize(int w, int h);
	virtual void SetMask(char* m, int w, int h);
	virtual void SetMid(const vector2di m)	{ mid = m; }

	virtual void SetEulerRotation(const vector3d r);
	virtual void SetPosition(const vector3d pos);

	virtual void SetScale(const double s);
	double GetScale()						{ return scale.Z; }
	virtual void SetFOV(const vector3d f);
	vector3d GetFOV()						{ return fov; }
	virtual void SetFarDist(const int d);
	int GetFarDist()						{ return far; }
	virtual void SetFogStart(const int d);
	int GetFogStart()						{ return fog; }
	virtual void SetFogColor(const vector3di nfc);
	vector3di GetFogColor()					{ return fogcol; }

	virtual const int* GetZBuf()			{ return zbuf; }
	virtual const vector3di* GetPBuf()		{ return pbuf; }
	virtual vector3di GetProjection(const vector2di pnt);

	virtual void Frame();
	virtual void Postprocess();
	virtual void SwapBuffers();
};

#endif /* LVR_H_ */
