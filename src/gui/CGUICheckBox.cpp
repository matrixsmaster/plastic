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

/* Implementation file of CheckBox control class */

#include "CGUIControls.h"

using namespace std;


CurseGUICheckBox::CurseGUICheckBox(CurseGUICtrlHolder* p, int x, int y, int w, std::string capt) :
		CurseGUIControl(p,x,y)
{
	g_w = (w < 3)? 3:w;
	checked = false;
	disabled = false;
	text = capt;
}

bool CurseGUICheckBox::Select(bool s)
{
	if (disabled) {
		selected = false;
		return false;
	}
	selected = s;
	return true;
}

void CurseGUICheckBox::Check()
{
	SGUIEvent ne;

	checked ^= true;

	ne.t = GUIEV_CTLBACK;
	ne.b.t = (checked)? GUIFB_CHECKON:GUIFB_CHECKOFF;
	ne.b.ctl = this;
	ne.b.wnd = holder->GetWindow();
	ne.b.wnd->GetParent()->AddEvent(&ne);
}

void CurseGUICheckBox::Update()
{
	WINDOW* wd = wnd->GetWindow();

	wcolor_set(wd,wnd->GetColorManager()->CheckPair(&fmt),NULL);
	if (selected) wattrset(wd,A_BOLD);

	if (disabled) mvwaddnstr(wd,g_y,g_x,"(O) ",g_w);
	else if (checked) mvwaddnstr(wd,g_y,g_x,"(X) ",g_w);
	else mvwaddnstr(wd,g_y,g_x,"( ) ",g_w);

	mvwaddnstr(wd,g_y,g_x+4,text.c_str(),g_w-4);

	if (selected) wattrset(wd,A_NORMAL);
}

bool CurseGUICheckBox::PutEvent(SGUIEvent* e)
{
	int x,y;

	if (disabled) return false;

	switch (e->t) {
	case GUIEV_KEYPRESS:
		if (!selected) return false;

		//Process key on selected button
		switch (e->k) {
		case ' ': /*toggle check by space*/
			Check();
			return true;
		default: break;
		}
		break;

	case GUIEV_MOUSE:
		//Check co-ords is in range
		x = e->m.x - wnd->GetPosX() - g_x;
		y = e->m.y - wnd->GetPosY() - g_y;
		//use only the check mark as a hot spot (3x1)
		if ((x < 0) || (x >= 3)) return false;
		if ((y < 0) || (y >= 1)) return false;

		//Do some action with button
		if (e->m.bstate & CGMOUSE_LEFT) {
			Check();
			return true;
		}
		break;

	default: break;
	}
	return false;
}
