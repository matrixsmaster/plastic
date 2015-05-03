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

#ifndef HUD_H_
#define HUD_H_

#include <vector>
#include "misconsts.h"
#include "CurseGUI.h"
#include "CGUIOverlay.h"

class HUD {
private:
	CurseGUI* gui;
	std::vector<CurseGUIOverlay*> overlays;

	///Spawn an overlay window (for internal use only).
	void Spawn(int x, int y, int w, int h, const char* txt);

public:
	HUD(CurseGUI* guiptr);
	virtual ~HUD();

	///Updates FPS counter.
	void UpdateFPS(uli fps);

	void PutStrBottom(const char* str);
	bool GetTransparent();
	void SetTransparent(bool t);
};

#endif /* HUD_H_ */