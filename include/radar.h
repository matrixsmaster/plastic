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

#ifndef RADAR_H_
#define RADAR_H_

#include "voxel.h"
#include "visual.h"
#include "vecmath.h"
#include "datapipe.h"

class PlasticRadar {
private:
	DataPipe* pipe;
	SGUIPixel* rad;
	int g_w,g_h,g_l;
	vector3di center;

	void Resize();

public:
	PlasticRadar(DataPipe* pipeptr);
	virtual ~PlasticRadar();

	void SetWH(int w, int h);
	void SetCenter(const vector3di cpos)	{ center = cpos; }

	SGUIPixel* GetImage()					{ return rad; }
	int GetImageLen()						{ return g_l; }

	void Update();
};

#endif /* RADAR_H_ */
