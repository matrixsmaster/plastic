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

/* Implementation file of EditBox control class */

#include "CGUIControls.h"

using namespace std;


CurseGUIEditBox::CurseGUIEditBox(CurseGUICtrlHolder* p, int x, int y, int w, std::string txt) :
		CurseGUIControl(p,x,y)
{
	g_w = w;
	text = txt;
}

void CurseGUIEditBox::Enter()
{
	SGUIEvent ne;

	ne.t = GUIEV_CTLBACK;
	ne.b.t = GUIFB_EDITOK;
	ne.b.ctl = this;
	ne.b.wnd = holder->GetWindow();
	ne.b.wnd->GetParent()->AddEvent(&ne);
}

void CurseGUIEditBox::Update()
{
	int r;
	string capt;
	WINDOW* wd = wnd->GetWindow();

	capt.reserve(g_w+2);

	r = g_w - 2 - (int)text.size();

	capt = "(" + text.substr(0,g_w-2);
	while (r-- > 0) capt += '_';
	capt += ')';

	wcolor_set(wd,wnd->GetColorManager()->CheckPair(&fmt),NULL);
	if (selected) wattrset(wd,A_BOLD);
	mvwaddnstr(wd,g_y,g_x,capt.c_str(),g_w);
	if (selected) wattrset(wd,A_NORMAL);
}

bool CurseGUIEditBox::PutEvent(SGUIEvent* e)
{
	int x,y;

	switch (e->t) {
	case GUIEV_KEYPRESS:
		if (!selected) return false;

		//Process key on selected button
		switch (e->k) {
		case 10:
		case 13:
		case KEY_ENTER: /*input confirmation*/
			Enter();
			return true;

		case 127:
		case KEY_BACKSPACE:
			if (!text.empty()) text.erase(text.end()-1);
			return true;

		default:
			if (!isprint(e->k)) return false;
			text += e->k;
			return true;
		}
		break;

	case GUIEV_MOUSE:
		//Check co-ords is in range
		x = e->m.x - wnd->GetPosX() - g_x;
		y = e->m.y - wnd->GetPosY() - g_y;
		if ((x < 0) || (x >= g_w)) return false;
		if ((y < 0) || (y >= 1)) return false;

		//Do some action with button
		if (e->m.bstate & BUTTON1_CLICKED) {
			holder->Select(this);
			return true;
		}
		break;

	default: break;
	}
	return false;
}
