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


AtmoSky::AtmoSky(uli sidelen, DataPipe* pipeptr)
{
	//init variables and allocate memory
	square = sidelen * sidelen;
	sky = (SGUIPixel*)malloc(square*sizeof(SGUIPixel));
	pipe = pipeptr;
	windsp = 0;

	//load settings
	day_sky.r = ((short)atoi(pipe->GetIniDataS(ATMOININAME,"DaySkyColorR").c_str()));
	day_sky.g = ((short)atoi(pipe->GetIniDataS(ATMOININAME,"DaySkyColorG").c_str()));
	day_sky.b = ((short)atoi(pipe->GetIniDataS(ATMOININAME,"DaySkyColorB").c_str()));
	day_cld.r = ((short)atoi(pipe->GetIniDataS(ATMOININAME,"DayCloudColorR").c_str()));
	day_cld.g = ((short)atoi(pipe->GetIniDataS(ATMOININAME,"DayCloudColorG").c_str()));
	day_cld.b = ((short)atoi(pipe->GetIniDataS(ATMOININAME,"DayCloudColorB").c_str()));
}

AtmoSky::~AtmoSky()
{
	if (sky) free(sky);
}

void AtmoSky::Quantum()
{
	//TODO: updater
}

void AtmoSky::RenderTo(SGUIPixel* buf, const uli len)
{
	uli i;
	if ((!buf) || (len < 1)) return;

	//FIXME
	for (i = 0; i < len; i++) {
		buf[i].bg = day_sky;
		buf[i].fg = day_cld;
		buf[i].sym = ' ';
	}
}
