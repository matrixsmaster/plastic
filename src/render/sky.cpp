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

#include "sky.h"
#include "support.h"
#include "debug.h"


AtmoSky::AtmoSky(DataPipe* pipeptr)
{
	//init variables
	pipe = pipeptr;
	windsp = 0;
	ctime = NULL;
	state = ASKY_NIGHT;

	//load settings
	day_sky.r = ((short)atoi(pipe->GetIniDataS(ATMOININAME,"DaySkyColorR").c_str()));
	day_sky.g = ((short)atoi(pipe->GetIniDataS(ATMOININAME,"DaySkyColorG").c_str()));
	day_sky.b = ((short)atoi(pipe->GetIniDataS(ATMOININAME,"DaySkyColorB").c_str()));
	day_cld.r = ((short)atoi(pipe->GetIniDataS(ATMOININAME,"DayCloudColorR").c_str()));
	day_cld.g = ((short)atoi(pipe->GetIniDataS(ATMOININAME,"DayCloudColorG").c_str()));
	day_cld.b = ((short)atoi(pipe->GetIniDataS(ATMOININAME,"DayCloudColorB").c_str()));
	ngh_sky.r = ((short)atoi(pipe->GetIniDataS(ATMOININAME,"NightSkyColorR").c_str()));
	ngh_sky.g = ((short)atoi(pipe->GetIniDataS(ATMOININAME,"NightSkyColorG").c_str()));
	ngh_sky.b = ((short)atoi(pipe->GetIniDataS(ATMOININAME,"NightSkyColorB").c_str()));
	ngh_cld.r = ((short)atoi(pipe->GetIniDataS(ATMOININAME,"NightCloudColorR").c_str()));
	ngh_cld.g = ((short)atoi(pipe->GetIniDataS(ATMOININAME,"NightCloudColorG").c_str()));
	ngh_cld.b = ((short)atoi(pipe->GetIniDataS(ATMOININAME,"NightCloudColorB").c_str()));

	dawnst = ((int)atoi(pipe->GetIniDataS(ATMOININAME,"DawnTime").c_str()));
	duskst = ((int)atoi(pipe->GetIniDataS(ATMOININAME,"DuskTime").c_str()));
	tranln = ((int)atoi(pipe->GetIniDataS(ATMOININAME,"Transition").c_str()));

	//calculate transitions
	trn_sky.r = (day_sky.r - ngh_sky.r) / (60 * tranln);
	trn_sky.g = (day_sky.g - ngh_sky.g) / (60 * tranln);
	trn_sky.b = (day_sky.b - ngh_sky.b) / (60 * tranln);
	trn_cld.r = (day_cld.r - ngh_cld.r) / (60 * tranln);
	trn_cld.g = (day_cld.g - ngh_cld.g) / (60 * tranln);
	trn_cld.b = (day_cld.b - ngh_cld.b) / (60 * tranln);

	//Initial update
	once = false;
	Quantum();
}

AtmoSky::~AtmoSky()
{
	//FIXME
}

void AtmoSky::SetEulerAngles(const vector3d nwang)
{
	scrot = nwang;
	scrot.Y *= -1; //inverse azimuth

	//correct rotation vector XY to conform our planar mapping
	if (scrot.X >=  SKYHEMI) scrot.X = scrot.X - SKYANGLE;
	if (scrot.X <= -SKYHEMI) scrot.X = SKYANGLE + scrot.X;
	if (scrot.Y >=  SKYHEMI) scrot.Y = scrot.Y - SKYANGLE;
	if (scrot.Y <= -SKYHEMI) scrot.Y = SKYANGLE + scrot.Y;

//	dbg_print("Sky rot. vector = [%.1f, %.1f]",scrot.X,scrot.Y);
}

void AtmoSky::Quantum()
{
	uli i;
	SCTriple cur_cld,cur_sky;
	int dt;
	bool uclouds = false;
//	vector3d tmpa,tmpb;

	if (!ctime) return;

	dt = (ctime->hr * 60 + ctime->mn) - evtime;

	switch (state) {
	case ASKY_NIGHT:
		if ((ctime->hr >= dawnst) && (ctime->hr < duskst)) {
			state = ASKY_DAWN;
			evtime = ctime->hr * 60 + ctime->mn;
			dt = 0;
		}
		cur_sky = ngh_sky;
		cur_cld = ngh_cld;
		break;

	case ASKY_DAWN:
		if (ctime->hr >= (dawnst+tranln)) {
			state = ASKY_DAY;
			uclouds = true;
		}
		cur_sky = vecftotriple(tripletovecf(ngh_sky) + (tripletovecf(trn_sky) * dt));
		cur_cld = vecftotriple(tripletovecf(ngh_cld) + (tripletovecf(trn_cld) * dt));
		break;

	case ASKY_DAY:
		if (ctime->hr >= duskst) {
			state = ASKY_DUSK;
			evtime = ctime->hr * 60 + ctime->mn;
			dt = 0;
		}
		cur_sky = day_sky;
		cur_cld = day_cld;
		break;

	case ASKY_DUSK:
		if (ctime->hr >= (duskst+tranln)) {
			state = ASKY_NIGHT;
			uclouds = true;
		}
		cur_sky = vecftotriple(tripletovecf(day_sky) - (tripletovecf(trn_sky) * dt));
		cur_cld = vecftotriple(tripletovecf(day_cld) - (tripletovecf(trn_cld) * dt));
		break;
	}

	//FIXME: temporary implementation
	for (i = 0; i < SKYSIZE; i++) {
		if (uclouds || (!once)) {
			switch (state) {
			case ASKY_NIGHT:
				sky[i].sym = (rand() < RAND_MAX/6)? '*':' ';
				break;

			case ASKY_DAY:
				sky[i].sym = (rand() < RAND_MAX/6)? '~':' ';
				break;

			default: break;
			}
		}

		sky[i].bg = cur_sky;
		sky[i].fg = cur_cld;
	}

	once = true;
}

void AtmoSky::RenderTo(SGUIPixel* buf, const uli w, const uli h)
{
	vector2di sp,ep;
	int x,y,i,j,l;

	if (!buf) return;

	//generate points
	sp.X = SKYHEMI + scrot.Y;
	sp.Y = SKYHEMI + scrot.X;
	ep.X = sp.X + w;
	ep.Y = sp.Y + h;
//	dbg_print("Sky [%d %d] [%d %d]",sp.X,sp.Y,ep.X,ep.Y);

	//that's a very dumb implementation, so should be replaced later with
	//a true spherical mapping
	for (i = sp.Y, l = 0; i < ep.Y; i++) {
		y = i;
		//wrap
		if (y >= SKYANGLE) y = SKYANGLE - y;
		if (y < 0) y = SKYANGLE + y;

		for (j = sp.X; j < ep.X; j++, l++) {
			x = j;
			//wrap
			if (x >= SKYANGLE) x = SKYANGLE - x;
			if (x < 0) x = SKYANGLE + x;

			buf[l] = sky[y*SKYANGLE+x];
		}
	}
}
