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
	Flush();
}

CGUIColorManager::~CGUIColorManager()
{
	colors.clear();
	pairs.clear();
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
	SCTriple tmp;

	colors.clear();
	pairs.clear();

	//Add default black and white pair
	tmp.r = 0; tmp.g = 0; tmp.b = 0;
	colors.push_back(tmp);
	tmp.r = 1000; tmp.g = 1000; tmp.b = 1000;
	colors.push_back(tmp);

	changed = true;
	frameskip = 0;
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
		return FindNearest(cl); //unable to append, find color as close as possible
}

short CGUIColorManager::CheckPair(const SGUIPixel* px)
{
	short i;
	SGUIExtPairs npr;

	//check for registered pair
	for (i = 0; i < (short)pairs.size(); i++) {
		if (	isEqual(&(px->bg),&(pairs.at(i).tb),COLTOLERANCE) &&
				isEqual(&(px->fg),&(pairs.at(i).tf),COLTOLERANCE) ) {
			//increment (or restore) usage counter
			if (pairs[i].use < 0) pairs[i].use = 1;
			else pairs[i].use++;
			//return pair number
			return ++i;
		}
	}

	//add new pair
	if (((short)pairs.size()) < (COLOR_PAIRS-1)) {
		changed = true;
		//make new pair data
		npr.tf = px->fg;
		npr.tb = px->bg;
		npr.cf = CheckColor(&(px->fg));
		npr.cb = CheckColor(&(px->bg));
		npr.use = 1;
		npr.inited = false;
		//add and return new pair index
		pairs.push_back(npr);
		return ((short)(pairs.size()));
	}

	return 0; //default
}

bool CGUIColorManager::GetPairColors(SGUIPixel* dest, short pair)
{
	if ((!dest) || (pair < 1) || (pair > (short)pairs.size()))
		return false;
	pair--;
	dest->bg = pairs.at(pair).tb;
	dest->fg = pairs.at(pair).tf;
	return true;
}

void CGUIColorManager::Apply()
{
	vector<SCTriple>::iterator ic;
	vector<SGUIExtPairs>::iterator ip;
	short i;

	if (!changed) return;

	//init color table (max 256 colors, so doesn't need to hold 'inited' flags)
	for (i = 0, ic = colors.begin(); ic != colors.end(); ++ic,++i) {
		init_color(i,ic->r,ic->g,ic->b);
	}

	//init pairs table (max 32767, so should rely on 'inited' flag data)
	for (i = 1, ip = pairs.begin(); ip != pairs.end(); ++ip,++i) {
		if (!ip->inited) {
			init_pair(i,ip->cf,ip->cb);
			ip->inited = true;
		}
	}

	changed = false;
}

void CGUIColorManager::CollectGarbage()
{
	vector<SGUIExtPairs>::iterator ip,jp;
	bool flg = false;

	for (ip = pairs.begin(); ip != pairs.end(); ++ip) {
		if (ip->use < PAIRGARBLEVEL) {
			//table is now changed
			changed = true;

			//current element is subject to remove
			ip = pairs.erase(ip);

			//now we should re-init all pairs down (if it wasn't done earlier)
			if (!flg) {
				for (jp = ip; jp != pairs.end(); ++jp)
					jp->inited = false;
				flg = true; //this process will be committed just once
			}

			--ip;
		}
	}
}

void CGUIColorManager::StartFrame()
{
	vector<SGUIExtPairs>::iterator ip;

	//decrement usage counters
	for (ip = pairs.begin(); ip != pairs.end(); ++ip) {
		ip->use--;
	}

	//collect garbage if enough frames passed
	if (++frameskip >= PAIRGARBSKIP) {
		frameskip = 0;
		CollectGarbage();
	}
}

void CGUIColorManager::EndFrame()
{
	//TODO: should we do something else here?
	Apply();
}
