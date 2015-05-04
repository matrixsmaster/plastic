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
#include "debug.h" // tmp

using namespace std;

CurseGUIOverlay::CurseGUIOverlay(CurseGUI* scrn, int x, int y, int w, int h) :
		CurseGUIWnd(scrn,x,y,0,0)
{
	type = GUIWT_OVERLAY;
	cnt = 0;
	m_x = x; //FIXME: what's this?
	m_y = y;
	m_w = w;
	m_h = h;

	name = "Overlay";

	CGUIEvent e;
	e.t = GUIEV_RESIZE;
	PutEvent(&e);

	transparent = true;
}

CurseGUIOverlay::~CurseGUIOverlay()
{
	//TODO
}

void CurseGUIOverlay::Update(bool refr)
{
	//TODO

	wcolor_set(wnd,0,NULL);
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
	default: break;
	}
	return false;
}

void CurseGUIOverlay::PutString(char* str)
{
	string log_str(str);
	log.push_back(log_str);
}

void CurseGUIOverlay::PutString(string str)
{
	log.push_back(str);
}

void CurseGUIOverlay::ResizeWnd()
{
	Resize(m_w, m_h);
}

void CurseGUIOverlay::PutLog()
{
	vector<string>::iterator it;
	int h;
	h = g_h - 1;
	chtype ch = 0, chclr;
	int nl = h + 1;
	int y = m_y+h;

	if(!log.empty()) {
		if(nl - log.size() >= 0)
			nl = log.size() + 1;

		for(it = log.end() - 1; it != log.end() - nl; it--) {
			for(size_t i = 0; i < it->size(); ++i) {
				ch = mvinch(y, m_x+i);

				if(transparent) {
					chclr = ch & A_COLOR;
				} else {
					//TODO ?
					chclr = 0;
				}

				ch = it->at(i);
				ch ^= chclr;
				mvwaddch(wnd, h, i, ch);
			}
			h--; y--;
		}
	}
}

