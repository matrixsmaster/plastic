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
	//init window
	type = GUIWT_DEBUGUI;
	name = "DebugUI";
	hidden = true;
	edit_line = ">";
	key = 0;
	edit = false;
	scrlk = false;
	locked = 0;

	//create window mutex to make DebugUI I/O thread-safe
	pthread_mutex_init(&wmutex,NULL);

	//place window
	ResizeWnd();
}

CurseGUIDebugWnd::~CurseGUIDebugWnd()
{
	pthread_mutex_destroy(&wmutex);
}

void CurseGUIDebugWnd::ToggleShow()
{
	hidden ^= true;
	focused = !hidden;
	stayontop = focused;
	scrlk = false;
	edit_line = ">";
}

void CurseGUIDebugWnd::Update(bool refr)
{
	vector<string>::iterator it,en;
	int size, w, h, nl, numstr;

	if (hidden) return;

	pthread_mutex_lock(&wmutex);

	wcolor_set(wnd,0,NULL);
	werase(wnd);

	h = g_h - ((boxed)? 3:1);
	if (h <= 0) return;
	w = g_w - ((boxed)? 2:0);

	numstr = h;

	/* Add edit line */
	if (edit) {
		edit_line += key;
		edit = false;
	}
	mvwaddnstr(wnd, h+1, 1, edit_line.c_str(), w);

	size = log.size();
	if (size > 0) {
		if (numstr > size)
			numstr = size;

		it = (scrlk)? (log.begin() + locked) : (log.end() - 1);
		en = it - numstr;
		if (en < log.begin()) en = log.begin();

		for (; it >= en; --it) {
			if (w < (int)it->size()) {
				// partitioning into multiple lines
				nl = (it->size() / w);
				for (int i = nl; i >= 0 ; i--) {
					mvwaddnstr(wnd, h--, 1, it->c_str()+(w*i), w);
				}
			} else mvwaddnstr(wnd, h--, 1, it->c_str(), w);
		}
	}

	pthread_mutex_unlock(&wmutex);

	DrawDecoration();
	if (refr) wrefresh(wnd);
}

bool CurseGUIDebugWnd::PutEvent(SGUIEvent* e)
{
	//DebugUI events processing
	switch (e->t) {
	case GUIEV_RESIZE:
		ResizeWnd();
		break;
	case GUIEV_KEYPRESS:
		switch (e->k) {
		case KEY_ENTER:
		case 10: /* in case enter isn't KEY_ENTER */
			//TODO parse command
			if(edit_line.size() > 1) {
				PutString(edit_line.c_str()+1);
				edit_line = ">";
			}
			break;

		case KEY_BACKSPACE:
		case 127: /* in case backspace isn't KEY_BACKSPACE */
			if(edit_line.size() > 1)
				edit_line.erase(edit_line.end() - 1);
			break;

		case KEY_F(12): /* Scroll Lock */
			scrlk ^= true;
			if (scrlk) locked = (log.size())? (log.size()-1) : 0;
			break;

		case KEY_NPAGE: /* Scroll down */
			if (scrlk) locked++;
			if (locked >= log.size()) locked = log.size()-1;
			break;

		case KEY_PPAGE: /* Scroll up */
			if (scrlk && locked) locked--;
			break;

		default:
			//FIXME: are we need this?
			if((e->k >= 'A' && e->k <= 'Z') ||
					(e->k >= 'a' && e->k <= 'z') ||
					(e->k == '-') ||
					(e->k == ' ')) {
				key = e->k;
				edit = true;
				return true;
			}
			break;
		}
		break;
	default: break;
	}
	return false;
}

void CurseGUIDebugWnd::PutString(char* str)
{
	PutString(string(str));
}

void CurseGUIDebugWnd::PutString(string str)
{
	pthread_mutex_lock(&wmutex);
	log.push_back(str);
	pthread_mutex_unlock(&wmutex);
}

void CurseGUIDebugWnd::ResizeWnd()
{
	int w, h;
	h = parent->GetHeight() / 4;
	w = parent->GetWidth();
	Resize(w, h);
}
