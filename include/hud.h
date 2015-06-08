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
#include "actor.h"

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
	std::string pcname;

	///Spawn an overlay window (for internal use only).
	void Spawn(int x, int y, int w, int h, bool logging);
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
	void SetFPS(uli fps);

	///Put string to LOG overlay
	void PutStrToLog(const char* str);

	///Set transparency
	void SetAlpha(HUDOverlayType t, float a);

	///Set radar data
	void SetMap(SGUIPixel *p, int l);

	///Returns radar map width.
	int GetMapWidth();

	///Returns radar map width.
	int GetMapHeight();

	///Update State overlay
	void SetState(std::string str);
	void SetState(const char* str);

	///Update clock overlay
	void UpdateClock();

	///Set time
	void SetPTime(PlasticTime* t);

	///Set global position
	void SetGPos(vector3di gp);

	///Set local position
	void SetLPos(vector3di lp);

	///Set the level of charge (V out of M)
	void SetCharge(int v, int m);

	///Set the health points meter (V out of M)
	void SetHP(int v, int m);

	///Hide or show miscellaneous parts of the HUD.
	void ShowMisc(bool s);

	///Toggle visibility of miscellaneous parts of the HUD.
	void ToggleMisc();

	///Sets the name of the player character
	void SetPCName(const char* n);

};

#endif /* HUD_H_ */
