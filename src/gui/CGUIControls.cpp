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

#include "CGUIControls.h"

using namespace std;

CurseGUIControl::CurseGUIControl(CurseGUIWnd* p, int x, int y)
{
	owner = p;
	g_x = x;
	g_y = y;
	back.r = 0;
	back.g = 0;
	back.b = 0;
	//TODO: init something here :)
}

CurseGUICtrlHolder::~CurseGUICtrlHolder()
{
	vector<CurseGUIControl*>::iterator it;
	if (controls.empty()) return;

	for (it = controls.begin(); it != controls.end(); ++it)
		delete (*it);
	controls.clear();
}

void CurseGUICtrlHolder::Update()
{
	vector<CurseGUIControl*>::iterator it;
	if (controls.empty()) return;

	for (it = controls.begin(); it != controls.end(); ++it)
		(*it)->Update();
}
