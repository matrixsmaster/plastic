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
#include "vecmath.h"
#include "plastic.h"
#include "visual.h"

#define BOOLSTR(X) ((X)? "ON":"OFF")

/// Prints out SGameSettings data.
void printsettings(SGameSettings* s);

/// Prints a simple formatted message to stderr.
void errout(char const* fmt, ...);

/// Argument parser. Supposed to be used for analyze startup environment.
bool argparser(int argc, char* argv[], SGameSettings* sets);

///Convert SCTriple to CPoint3D.
vector3d tripletovecf(const SCTriple s);

///Convert CPoint3D to SCTriple.
SCTriple vecftotriple(const vector3d s);

#endif /* SUPPORT_H_ */
