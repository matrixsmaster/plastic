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

char* mstrnacpy(char* dest, const char* src, size_t n)
{
	char* buf;
	size_t l;

	//Check source and get its length
	if (!src) return NULL;
	l = strlen(src);

	if (!dest) {
		//Auto-allocation
		if (!n) n = l + 1;
		buf = (char*)malloc(n);
		if (!buf) return NULL;
	} else
		buf = dest;

	//Check length and copy the string
	if (!n) return buf;
	strncpy(buf,src,n);

	//Terminate result
	if ((l >= n) && (n > 0))
		buf[n-1] = 0;

	return buf;
}
