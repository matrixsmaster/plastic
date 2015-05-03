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

class CurseGUIOverlay : public CurseGUIWnd {
private:
	std::vector<std::string> log;

	int cnt;
	int m_x, m_y;
	int m_w, m_h;
	bool transparent;
	bool stayontop;

	void ResizeWnd();
	void PutLog();

public:
	CurseGUIOverlay(CurseGUI* scrn, int x, int y, int w, int h);
	virtual ~CurseGUIOverlay();

	//Overlay willn't gain or loose the focus.
	bool GainFocus()					{ return false; }
	bool LooseFocus()					{ return true; }

	void SetTransparent(bool t)			{ transparent = t; }
	bool GetTransparent() 				{ return transparent; }
	void StayOnTop(bool s)				{ stayontop = s; }
	bool IsStayOnTop()					{ return stayontop; }

	void Update(bool refr);
	bool PutEvent(CGUIEvent* e);

	void PutString(char* str);
	void PutString(std::string str);
};



#endif /* INCLUDE_CGUIOVERLAY_H_ */
