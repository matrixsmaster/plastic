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
	OVRL_FPS = 0,		//Fps
	OVRL_LOG,			//Log
	OVRL_STATE,			//Top status overlay
	OVRL_MAP,			//Map
	OVRL_STBTM,			//Status Bar
	OVRL_CLOCK,			//Date and time
	OVRL_CHRG_HP		//Charge and HP
};


class HUD {
private:
	CurseGUI* gui;
	std::vector<CurseGUIOverlay*> overlays;
	SOVRLStats stats;
	PlasticTime* plt;

	///Spawn an overlay window (for internal use only).
	void Spawn(int x, int y, int w, int h, bool logging, const char* txt);
	std::string intToString(int n);

	///Controls initialization
	void InitControls();

	void ClearLog(HUDOverlayType t);
	void PutString(HUDOverlayType t, std::string str);

	void SetLabelCaption(char *buf, int s, int l, int x, int y, int z);

public:
	HUD(CurseGUI* guiptr);
	virtual ~HUD();

	///Updates FPS counter.
	void UpdateFPS(uli fps);

	///Put string to LOG overlay
	void PutStrToLog(const char* str);

	///Set transparency
	void SetAlpha(HUDOverlayType t, float a);

	///Set background mask
	void SetBckgrMask(SGUIPixel* pxl);

	///Draw local world map
	void DrawMap();

	///Update State overlay
	void UpdateState(std::string str);

	///Update clock overlay
	void UpdateClock();

	///Update charge and HP overlay
	void UpdateChargeHP();

	///Set time
	void SetPTime(PlasticTime* t);

	///Set global position
	void SetGPos(vector3di gp); //{ st_gp = gp; }

	///Set local position
	void SetLPos(vector3di lp); //{ st_lp = lp; }

	///Set the level of charge
	void SetCharge(int v);

	///Set the level health points
	void SetHP(int v);

	///Change the visibility
	void SetHidden();
};

#endif /* HUD_H_ */
