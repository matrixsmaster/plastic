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

#include "datapipe.h"
#include "CurseGUI.h"
#include "vecmath.h"
#include "misconsts.h"
#include "pltime.h"


#define SKYANGLE 360
#define SKYSIZE (SKYANGLE * SKYANGLE)
#define SKYHEMI (SKYANGLE / 2)


enum ESkyState {
	ASKY_NIGHT,
	ASKY_DAWN,
	ASKY_DAY,
	ASKY_DUSK
};

class AtmoSky {
private:
	DataPipe* pipe;				//DataPipe access pointer
	SGUIPixel sky[SKYSIZE];		//skies buf
	const PlasticTime* ctime;	//current time
	bool once;					//first update flag
	vector3d scrot;				//scene rotation
	float windsp;				//wind speed
	vector3d wind;				//wind direction
	SCTriple day_cld;			//clouds color (day)
	SCTriple day_sky;			//sky main color (day)
	SCTriple ngh_cld;			//clouds color (night)
	SCTriple ngh_sky;			//sky main color (night)
	SCTriple trn_cld;			//clouds color (transition delta)
	SCTriple trn_sky;			//sky main color (transition delta)
	int dawnst,duskst;			//dawn and dusk start time (game hour)
	int tranln;					//transition state length (hours)
	int evtime;					//current event time
	ESkyState state;			//current state of skies

public:
	AtmoSky(DataPipe* pipeptr);
	virtual ~AtmoSky();

	void SetTime(const PlasticTime* nwtime)			{ ctime = nwtime; }
	void SetEulerAngles(const vector3d nwang);

	void Quantum();
	void RenderTo(SGUIPixel* buf, const uli w, const uli h);
};

#endif /* SKY_H_ */
