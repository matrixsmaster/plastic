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

#ifndef VISUAL_H_
#define VISUAL_H_

#include "CurseGUI.h"

#define COLPAIRS 7

static const struct SGUIWCol gui_coltable[COLPAIRS] = {
		{COLOR_WHITE,COLOR_BLACK}, //the first one is default
		{COLOR_WHITE,COLOR_BLUE},
		{COLOR_YELLOW,COLOR_GREEN},
		{COLOR_WHITE,COLOR_YELLOW},
		{COLOR_BLACK,COLOR_WHITE},
		{COLOR_WHITE,COLOR_RED},
		{COLOR_WHITE,COLOR_MAGENTA}
};

#endif /* VISUAL_H_ */
