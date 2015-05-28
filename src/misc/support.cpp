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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "support.h"
#include "vecmath.h"


void errout(char const* fmt, ...)
{
	va_list vl;
	va_start(vl,fmt);
	vfprintf(stderr,fmt,vl);
	va_end(vl);
}

vector3d tripletovecf(const SCTriple s)
{
	return vector3d(s.r,s.g,s.b);
}

SCTriple vecftotriple(const vector3d s)
{
	SCTriple r;
	r.r = s.X;
	r.g = s.Y;
	r.b = s.Z;
	return r;
}

mmask_t mmask_by_str(const char* s)
{
	if (s) {
		if (!strcmp(s,"CTRL")) return BUTTON_CTRL;
		if (!strcmp(s,"SHIFT")) return BUTTON_SHIFT;
		if (!strcmp(s,"ALT")) return BUTTON_ALT;
	}
	return 0;
}
