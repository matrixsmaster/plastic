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

#ifndef SUPPORT_H_
#define SUPPORT_H_

#include <stdarg.h>
#include <ncurses.h>
#include "vecmath.h"
#include "visual.h"

#define BOOLSTR(X) ((X)? "ON":"OFF")
#define MGETCHAR(X) while (!isprint(X = getchar())) /*trash non-printable chars (NL,LF,EOF etc)*/
#define SIGN(X) ((X < 0)? -1:1)

/// Prints a simple formatted message to stderr.
void errout(char const* fmt, ...);

///Convert SCTriple to CPoint3D.
vector3d tripletovecf(const SCTriple s);

///Convert CPoint3D to SCTriple.
SCTriple vecftotriple(const vector3d s);

///Get mouse event mask by string representation.
mmask_t mmask_by_str(const char* s);

///Local implementation of safe strcpy() with auto-allocating feature.
char* mstrnacpy(char* dest, const char* src, size_t n);

#endif /* SUPPORT_H_ */
