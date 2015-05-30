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

#ifndef CGUICOLORMANAGER_H_
#define CGUICOLORMANAGER_H_

#include <vector>
#include "visual.h"


///Basic component tolerance to distinguish different colors.
#define COLTOLERANCE 10

///Define this to make frame transitions smooth (for fast output).
///Undef for slow output to make frames totally independent.
#define USEPAIRGARBAGE 1

///Define this if your scene uses a lot of colors. (Slows down frame rendering).
///Undef for scenes with a well-defined set of colors to increase performance.
#define USECOLRGARBAGE 1

///Every pair, which usage drops lower than this value, will be discarded.
#define PAIRGARBLEVEL -3

///Skip this amount of frames between garbage collections.
#define PAIRGARBSKIP 20


struct SGUIExtPairs {
	SCTriple tf,tb;
	short cf,cb;
	int use;
	bool inited;
};

class CGUIColorManager {
protected:
	bool changed;
	int frameskip;

	std::vector<SCTriple> colors;
	std::vector<SGUIExtPairs> pairs;

	bool isEqual(const SCTriple* a, const SCTriple* b, const short tol);
	short FindNearest(const SCTriple* cl);
	short CheckColor(const SCTriple* cl);
	void CollectGarbagePairs();
	bool CollectGarbageColor(const short cc);

public:
	CGUIColorManager();
	virtual ~CGUIColorManager();

	short CheckPair(const SGUIPixel* px);
	bool GetPairColors(SGUIPixel* dest, short pair);

	void Flush();
	void Apply();

	void StartFrame();
	void EndFrame();
};

#endif /* CGUICOLORMANAGER_H_ */
