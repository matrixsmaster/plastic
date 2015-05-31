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

/* Implementation file of Button control class */

#include "CGUIControls.h"

using namespace std;


CurseGUIButton::CurseGUIButton(CurseGUICtrlHolder* p, int x, int y, int w, std::string capt) :
		CurseGUIControl(p,x,y)
{
	typ = GUICL_BUTTON;
	text = capt;
	g_w = w;
}

void CurseGUIButton::Update()
{
	int l,r;
	string butt;
	WINDOW* wd = wnd->GetWindow();

	butt.reserve(g_w+2);

	if (g_w > ((int)text.size() + 2)) {
		//make it centered
		l = (g_w - 2) / 2 - text.size() / 2;
		r = (g_w - 2) - l - text.size();
	} else
		l = r = 0;

	butt = "[";
	while (l-- > 0) butt += ' ';
	butt += text.substr(0,g_w-2);
	while (r-- > 0) butt += ' ';
	butt += ']';

	wcolor_set(wd,wnd->GetColorManager()->CheckPair(&fmt),NULL);
	if (selected) wattrset(wd,A_BOLD);
	mvwaddnstr(wd,g_y,g_x,butt.c_str(),g_w);
	if (selected) wattrset(wd,A_NORMAL);
}

void CurseGUIButton::Click()
{
	SGUIEvent ne;

	ne.t = GUIEV_CTLBACK;
	ne.b.t = GUIFB_SWITCHED;
	ne.b.ctl = this;
	ne.b.wnd = holder->GetWindow();
	ne.b.wnd->GetParent()->AddEvent(&ne);
}

bool CurseGUIButton::PutEvent(SGUIEvent* e)
{
	int x,y;

	switch (e->t) {
	case GUIEV_KEYPRESS:
		if (!selected) return false;

		//Process key on selected button
		switch (e->k) {
		case ' ': /*push button by space*/
			Click();
			return true;
		default: break;
		}
		break;

	case GUIEV_MOUSE:
		//Check co-ords is in range
		x = e->m.x - wnd->GetPosX() - g_x;
		y = e->m.y - wnd->GetPosY() - g_y;
		if ((x < 0) || (x >= g_w)) return false;
		if ((y < 0) || (y >= 1)) return false;

		//Do some action with button
		if (e->m.bstate & GUIMOUS_LEFT) {
			holder->Select(this);
			Click();
			return true;
		}
		break;

	default: break;
	}
	return false;
}
