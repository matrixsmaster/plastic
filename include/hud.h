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
#include "vecmath.h"
#include "pltime.h"

#define STAT_OVRL_HEIGHT 3

enum HUDOverlayType {
	OVRL_FPS = 0,
	OVRL_LOG,
	OVRL_ST,
	OVRL_MAP,
	OVRL_STBTM,
	OVRL_CLOCK,
	OVRL_CHARGE,
	OVRL_HP
};


class HUD {
private:
	CurseGUI* gui;
	std::vector<CurseGUIOverlay*> overlays;
	SOVRLStats stats;
	vector3di st_gp;
	vector3di st_lp;
	PlasticTime* plt;

	///Spawn an overlay window (for internal use only).
	void Spawn(int x, int y, int w, int h, bool logging, const char* txt);
	std::string intToString(int n);

	void ClearLog(HUDOverlayType t);
	void PutString(HUDOverlayType t, std::string str);

public:
	HUD(CurseGUI* guiptr);
	virtual ~HUD();

	///Updates FPS counter.
	void UpdateFPS(uli fps);

	//Update status message (gpos, lpos, etc)
	void UpdateStatusOvrl();

	//Put string to LOG overlay
	void PutStrToLog(const char* str);

	//Set transparency
	void SetAlpha(HUDOverlayType t, float a);

	//Set background mask
	void SetBckgrMask(SGUIPixel* pxl);

	//Draw local world map
	void DrawMap();

	//Update State overlay
	void UpdateState(std::string str);

	//Update clock overlay
	void UpdateClock();

	//Update charge overlay
	void UpdateCharge();

	//Update HP overlay
	void UpdateHP();

	void SetPTime(PlasticTime* t);

	//Set global position
	void SetGPos(vector3di gp) { st_gp = gp; }

	//Set local position
	void SetLPos(vector3di lp) { st_lp = lp; }
};

#endif /* HUD_H_ */
