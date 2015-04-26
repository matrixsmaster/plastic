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

#include <sstream>
#include "CGUIOverlay.h"

using namespace std;

CurseGUIOverlay::CurseGUIOverlay(CurseGUI* scrn, int x, int y) :
		CurseGUIWnd(scrn,x,y,0,0)
{
	type = GUIWT_OVERLAY;
	cnt = 0;

	//TODO do something
	CGUIEvent e;
	e.t = GUIEV_RESIZE;
	PutEvent(&e);
}

CurseGUIOverlay::~CurseGUIOverlay()
{
	//TODO
}

void CurseGUIOverlay::Update(bool refr)
{
	//TODO

	PutLog();

	if (refr) wrefresh(wnd);
}

bool CurseGUIOverlay::PutEvent(CGUIEvent* e)
{
	//OverlayUI events processing
	switch (e->t) {
	case GUIEV_RESIZE:
		ResizeWnd();
		break;
	case GUIEV_KEYPRESS:
		break;
	default: break;
	}
	return false;
}

void CurseGUIOverlay::PutString(char* str)
{
	//TODO
	string log_str(str);
	log.push_back(log_str);
}

void CurseGUIOverlay::PutString(string str)
{
	log.push_back(str);
}

void CurseGUIOverlay::ResizeWnd()
{
	int w, h;
	h = parent->GetHeight()/4;
	w = parent->GetWidth()/4;
	Resize(w, h);
}

void CurseGUIOverlay::PutLog()
{
	vector<string>::iterator it;
	int h;
	h = g_h - 1;
	for(it = log.begin(); it != log.end(); ++it)
		mvwaddnstr(wnd, h--, 1, it->c_str(), -1);
}

