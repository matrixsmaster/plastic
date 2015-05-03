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

#include "CGUISpecWnd.h"

using namespace std;

CurseGUIDebugWnd::CurseGUIDebugWnd(CurseGUI* scrn) :
	CurseGUIWnd(scrn,0,0,2,2)
{
	type = GUIWT_DEBUGUI;
	hidden = true;
	edit_line = ">";
	key = 0;
	edit = false;

	//TODO: do something
	CGUIEvent e;
	e.t = GUIEV_RESIZE;
	PutEvent(&e);
}

CurseGUIDebugWnd::~CurseGUIDebugWnd()
{
	//TODO
}

void CurseGUIDebugWnd::ToggleShow()
{
	hidden ^= true;
	focused = !hidden;
	edit_line = ">";
}

void CurseGUIDebugWnd::Update(bool refr)
{
	vector<string>::iterator it;
	int size, w, h, nl, numstr;

	if (hidden) return;

	wcolor_set(wnd,0,NULL);
	werase(wnd);

	if(g_h < ((boxed)? 3:2)) return;
	h = g_h - ((boxed)? 3:2);
	numstr = h+1;

	/* Add edit line */
	if (edit) {
		edit_line += key;
		edit = false;
	}
	mvwaddnstr(wnd, h+1, 1, edit_line.c_str(), -1);

	size = log.size();
	if (size > 0) {
		if((h - size >= 0))
			numstr = size + 1;

		for (it = log.end()-1; it != log.end() - numstr; it--) {
			w = g_w - ((boxed)? 2:1);
			if(it->size() % (w) != 0) {
				// partitioning into multiple lines
				nl = (it->size() / (w));
				for(int i = nl; i >= 0 ; i--) {
					mvwaddnstr(wnd, h--, 1, it->c_str()+(w*i), w);
				}
			} else mvwaddnstr(wnd, h--, 1, it->c_str(), -1);
		}
	}

	if (boxed) box(wnd,0,0);
	if (refr) wrefresh(wnd);
}

bool CurseGUIDebugWnd::PutEvent(CGUIEvent* e)
{
	//DebugUI events processing
	switch (e->t) {
	case GUIEV_RESIZE:
		ResizeWnd();
		break;
	case GUIEV_KEYPRESS:
		switch (e->k) {
		case KEY_ENTER:
		case 10: /* in case enter isn't enter */
//			if(!hidden) { //FIXME: there's no need to do such a things :)
				//TODO parse command
				if(edit_line.size() > 1) {
					PutString(edit_line.c_str()+1);
					edit_line = ">";
				}
//			}
			break;
		case KEY_BACKSPACE:
		case 127: /* in case backspace isn't delete */
//			if(!hidden)
				if(edit_line.size() > 1)
					edit_line.erase(edit_line.end() - 1);
			break;
		default:
//			if(!hidden) {
				if((e->k >= 'A' && e->k <= 'Z') ||
						(e->k >= 'a' && e->k <= 'z') ||
						(e->k == '-') ||
						(e->k == ' ')) {
					key = e->k;
					edit = true;
					return true;
				}
//			}
			break;
		}
		break;
	default: break;
	}
	return false;
}

void CurseGUIDebugWnd::PutString(char* str)
{
	string log_str(str);
	log.push_back(log_str);
}

void CurseGUIDebugWnd::PutString(std::string str)
{
	log.push_back(str);
}

void CurseGUIDebugWnd::ResizeWnd()
{
	int w, h;
	h = parent->GetHeight() / 4;
	w = parent->GetWidth();
	Resize(w, h);
}
