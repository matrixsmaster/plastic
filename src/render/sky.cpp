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


AtmoSky::AtmoSky(uli sidelen)
{
	//TODO
}

AtmoSky::~AtmoSky()
{
	//TODO
}

void AtmoSky::SetTime(const AtmoTime nwtime)
{
	//TODO
}

void AtmoSky::SetEulerAngles(const vector3d nwang)
{
	//TODO
}

void AtmoSky::RenderTo(SGUIPixel* buf, const uli len)
{
	uli i;
	if ((!buf) || (len < 1)) return;

	//FIXME
	for (i = 0; i < len; i++) {
		buf[i].bg.r = 0;
		buf[i].bg.g = 500;
		buf[i].bg.b = 00;
		buf[i].fg.r = 500;
		buf[i].fg.g = 100;
		buf[i].fg.b = 100;

		buf[i].sym = '9';
	}
}
