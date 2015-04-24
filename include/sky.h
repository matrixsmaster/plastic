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

#ifndef SKY_H_
#define SKY_H_

#include "CurseGUI.h"
#include "vecmath.h"
#include "misconsts.h"


struct AtmoTime {
	//TODO
};

class AtmoSky {
private:
	uli square;			//linear size of skies buffer
	SGUIPixel* sky;		//skies buf
	AtmoTime time;		//atmospheric time
	vector3d scrot;		//scene rotation
	float windsp;		//wind speed
	vector3d wind;		//wind direction

public:
	AtmoSky(uli sidelen)	{}
	virtual ~AtmoSky()		{}

	void SetTime(const AtmoTime nwtime);
	void SetEulerAngles(const vector3d nwang);

	void RenderTo(SGUIPixel* buf, const uli len);
};

#endif /* SKY_H_ */
