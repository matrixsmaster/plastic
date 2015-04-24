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

#include <ncurses.h>
#include "CGUIColorManager.h"

using namespace std;

CGUIColorManager::CGUIColorManager()
{
	changed = true;
}

CGUIColorManager::~CGUIColorManager()
{
	Flush();
}

bool CGUIColorManager::isEqual(const SCTriple* a, const SCTriple* b, const short tol)
{
	if ( ((a->r - tol) > b->r) || ((b->r) > (a->r + tol)) ) return false;
	if ( ((a->g - tol) > b->g) || ((b->g) > (a->g + tol)) ) return false;
	if ( ((a->b - tol) > b->b) || ((b->b) > (a->b + tol)) ) return false;
	return true;
}

short CGUIColorManager::FindNearest(const SCTriple* cl)
{
	//TODO
	return 0;
}

void CGUIColorManager::Flush()
{
	colors.clear();
	pairs.clear();
	changed = true;
}

short CGUIColorManager::CheckColor(const SCTriple* cl)
{
	short i;

	//check known colors with some tolerance
	for (i = 0; i < (short)colors.size(); i++) {
		if (isEqual(cl,&(colors.at(i)),COLTOLERANCE))
			return i;
	}

	//register new color
	if (((short)colors.size()) < (COLORS)) {
		changed = true;
		colors.push_back(*cl);
		return ((short)(colors.size()-1));
	} else
		return -1; //unable to append
}

short CGUIColorManager::CheckPair(const SGUIPixel* px)
{
	short i;
	SGUIExtPairs npr;

	//check for registered pair
	for (i = 0; i < (short)pairs.size(); i++) {
		if (	isEqual(&(px->bg),&(pairs.at(i).tb),COLTOLERANCE) &&
				isEqual(&(px->fg),&(pairs.at(i).tf),COLTOLERANCE) )
			return i;
	}

	//add new pair
	if (((short)pairs.size()) < (COLOR_PAIRS-1)) {
		changed = true;
		npr.tf = px->fg;
		npr.tb = px->bg;
		npr.cf = CheckColor(&(px->fg));
		npr.cb = CheckColor(&(px->bg));
		pairs.push_back(npr);
		return ((short)(pairs.size()-1));
	}

	return 0; //default
}

void CGUIColorManager::Apply()
{
	vector<SCTriple>::iterator ic;
	vector<SGUIExtPairs>::iterator ip;
	short i;

	if (!changed) return;

	for (i = 0, ic = colors.begin(); ic != colors.end(); ++ic,++i) {
		init_color(i,ic->r,ic->g,ic->b);
	}

	for (i = 0, ip = pairs.begin(); ip != pairs.end(); ++ip,++i) {
		init_pair(i,ip->cf,ip->cb);
	}

	changed = false;
}
