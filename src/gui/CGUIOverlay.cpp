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

CurseGUIOverlay::CurseGUIOverlay(CurseGUI* scrn, int x, int y, int w, int h, bool logg) :
		CurseGUIWnd(scrn,x,y,0,0),
		m_x(x),
		m_y(y),
		m_w(w),
		m_h(h),
		logging(logg) //FIXME: mixed-style initialization, use either one or another!
{
	type = GUIWT_OVERLAY;

	name = "Overlay";

	SGUIEvent e;
	e.t = GUIEV_RESIZE;
	PutEvent(&e);

	alpha = CGUIOVERLAYDEFALPHA;

	pixl.bg.r = 0; pixl.bg.g = 0; pixl.bg.b = 0;
	pixl.fg.r = 1000; pixl.fg.g = 500; pixl.fg.b = 1000;
}

CurseGUIOverlay::~CurseGUIOverlay()
{
	//TODO
}

void CurseGUIOverlay::Update(bool refr)
{
	PutLog();
	wcolor_set(wnd,0,NULL);
	if (refr) wrefresh(wnd);
}

bool CurseGUIOverlay::PutEvent(SGUIEvent* e)
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

void CurseGUIOverlay::PutString(const char* str)
{
	//Put string to OverlayUI log
	string log_str(str);
	log.push_back(log_str);
}

void CurseGUIOverlay::PutString(string str)
{
	//Put string to OverlayUI log
	log.push_back(str);
}

void CurseGUIOverlay::SetBckgrMask(SGUIPixel* pxl)
{
	//TODO
	pixl.bg.r = pxl->bg.r;
	pixl.bg.g = pxl->bg.g;
	pixl.bg.b = pxl->bg.b;
	pixl.fg.r = pxl->fg.r;
	pixl.fg.g = pxl->fg.g;
	pixl.fg.b = pxl->fg.b;
}

void CurseGUIOverlay::ClearLog()
{
	log.clear();
}

void CurseGUIOverlay::ResizeWnd()
{
	//FIXME: are you really need a method to wrap one function call o_O ?
	Resize(m_w, m_h);
}

void CurseGUIOverlay::PutLog()
{
	vector<string>::iterator it;
	int h = g_h - 1;
	chtype ch = 0;
	int nl = g_h;
	int y = m_y+h;
	SGUIPixel pxl;
	short lc = -1;
	short pair;
	int i;

	if (log.empty()) return;

	if (alpha <= 0)
		wcolor_set(wnd, cmanager->CheckPair(&pixl), NULL);

	if(nl - log.size() >= 0)
		nl = log.size() + 1;

	for(it = log.end() - 1; it != log.end() - nl; --it, --h, --y) {


		if (alpha > 0) {
			//deal with each character separately in transparent mode
			for (i = 0; i < (int)it->size(); ++i) {
				//get underlying symbol information
				ch = mvinch(y, m_x+i);
				pair = (ch & A_COLOR) >> NCURSES_ATTR_SHIFT;

				pxl.sym = it->at(i);

				//get the color information by pair code
				if(!cmanager->GetPairColors(&pxl, pair)) continue;

				//Set transparency
				pxl.bg.r = float(pxl.bg.r)*alpha;
				pxl.bg.g = float(pxl.bg.g)*alpha;
				pxl.bg.b = float(pxl.bg.b)*alpha;

				pxl.fg.r = 1000;
				pxl.fg.g = 1000;
				pxl.fg.b = 1000;

				//Apply new symbol
				lc = cmanager->CheckPair(&pxl);
				wcolor_set(wnd, lc, NULL);
				mvwaddch(wnd, h, i, pxl.sym);
			}

		} else {
			//just print out the string
			mvwaddnstr(wnd,h,m_x,it->c_str(),g_w);
			//and fill the possible gap
			i = it->size();
			while (i < g_w)
				mvwaddch(wnd,h,m_x + i++,' ');
		}
	}
}
