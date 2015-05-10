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


#define COLTOLERANCE 10
#define PAIRGARBLEVEL -16
#define PAIRGARBSKIP 3


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
	void CollectGarbage();

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
