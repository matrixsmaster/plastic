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
#include "debug.h"


AtmoSky::AtmoSky(DataPipe* pipeptr)
{
	//init variables
	pipe = pipeptr;
	windsp = 0;

	//load settings
	day_sky.r = ((short)atoi(pipe->GetIniDataS(ATMOININAME,"DaySkyColorR").c_str()));
	day_sky.g = ((short)atoi(pipe->GetIniDataS(ATMOININAME,"DaySkyColorG").c_str()));
	day_sky.b = ((short)atoi(pipe->GetIniDataS(ATMOININAME,"DaySkyColorB").c_str()));
	day_cld.r = ((short)atoi(pipe->GetIniDataS(ATMOININAME,"DayCloudColorR").c_str()));
	day_cld.g = ((short)atoi(pipe->GetIniDataS(ATMOININAME,"DayCloudColorG").c_str()));
	day_cld.b = ((short)atoi(pipe->GetIniDataS(ATMOININAME,"DayCloudColorB").c_str()));

	//Initial update
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

	//correct rotation vector XY
	if (scrot.X >=  SKYHEMI) scrot.X = scrot.X - SKYANGLE;
	if (scrot.X <= -SKYHEMI) scrot.X = SKYANGLE + scrot.X;
	if (scrot.Y >=  SKYHEMI) scrot.Y = scrot.Y - SKYANGLE;
	if (scrot.Y <= -SKYHEMI) scrot.Y = SKYANGLE + scrot.Y;

	dbg_print("Sky rot. vector = [%.1f, %.1f]",scrot.X,scrot.Y);
}

void AtmoSky::Quantum()
{
	uli i;
	//TODO: updater
	for (i = 0; i < SKYSIZE; i++) {
		sky[i].bg = day_sky;
		sky[i].fg = day_cld;
		sky[i].sym = (rand() < RAND_MAX/6)? '~':' ';
	}
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
