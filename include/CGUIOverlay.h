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

/* Overlay window is defined here */

#ifndef INCLUDE_CGUIOVERLAY_H_
#define INCLUDE_CGUIOVERLAY_H_

#include <vector>
#include <string>
#include "CurseGUI.h"


struct SPOS {
	int x, y, z;
};

// structure to show in status overlay
struct SOVRLStats {
	int fps;
	SPOS gp;
	SPOS lp;
};


class CurseGUIOverlay : public CurseGUIWnd {
private:
	std::vector<std::string> log;

	int m_x, m_y;		//window position
	int m_w, m_h;		//width and height
	float transparent;	//window transparency
	bool logging;		//window logging


	SGUIPixel pixl;


	void ResizeWnd();
	void PutLog();
	void SetBckgr();

public:
	CurseGUIOverlay(CurseGUI* scrn, int x, int y, int w, int h, bool logg);
	virtual ~CurseGUIOverlay();

	//Overlay willn't gain or loose the focus.
	bool GainFocus()					{ return false; }
	bool LooseFocus()					{ return true; }

	void SetTransparent(bool t)			{ transparent = t; }
	void SetTransparent(float t);

	bool GetTransparent() 				{ return transparent; }

	void SetBckgrMask(SGUIPixel* pxl);

	void Update(bool refr);
	bool PutEvent(SGUIEvent* e);

	void PutString(char* str);
	void PutString(std::string str);

	void ClearLog();


	void SetTransparentUp();
	void SetTransparentDown();

	//TODO add transparent, use ColorManager
};



#endif /* INCLUDE_CGUIOVERLAY_H_ */
