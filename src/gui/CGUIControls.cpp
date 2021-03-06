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

/* Implementation file of Controls Holder and Controls base class */

#include "CGUIControls.h"

using namespace std;


CurseGUICtrlHolder::~CurseGUICtrlHolder()
{
	vector<CurseGUIControl*>::iterator it;
	if (controls.empty()) return;

	for (it = controls.begin(); it != controls.end(); ++it)
		delete (*it);
	controls.clear();
}

void CurseGUICtrlHolder::Append(CurseGUIControl* ctl)
{
	if (ctl) controls.push_back(ctl);
}

void CurseGUICtrlHolder::Delete(CurseGUIControl* ctl)
{
	vector<CurseGUIControl*>::iterator it;

	for (it = controls.begin(); it != controls.end(); ++it)
		if ((*it) == ctl) {
			delete ctl;
			controls.erase(it);
			break;
		}
}

void CurseGUICtrlHolder::Update()
{
	vector<CurseGUIControl*>::iterator it;
	if (controls.empty()) return;

	for (it = controls.begin(); it != controls.end(); ++it)
		(*it)->Update();
}

bool CurseGUICtrlHolder::PutEvent(SGUIEvent* e)
{
	vector<CurseGUIControl*>::iterator it;
	bool r = false;
	if (controls.empty()) return false;

	for (it = controls.begin(); it != controls.end(); ++it)
		if ((*it)->PutEvent(e)) {
			r = true;
			break;
		}
	return r;
}

void CurseGUICtrlHolder::Rotate()
{
	vector<CurseGUIControl*>::iterator it;
	bool flg = false;

	for (it = controls.begin(); it != controls.end(); ++it) {
		if ((!flg) && ((*it)->IsSelected())) {
			flg = true;
			(*it)->Select(false);
		} else if ((flg) && ((*it)->Select(true))) {
			return;
		}
	}

	//second pass
	for (it = controls.begin(); it != controls.end(); ++it)
		if ((*it)->Select(true)) return;
}

void CurseGUICtrlHolder::Select(CurseGUIControl* ctl)
{
	vector<CurseGUIControl*>::iterator it;

	for (it = controls.begin(); it != controls.end(); ++it)
		(*it)->Select(((*it) == ctl));
}

int CurseGUICtrlHolder::GetNumControls(ECGUIControlType t)
{
	int c;
	vector<CurseGUIControl*>::iterator it;

	for (c = 0, it = controls.begin(); it != controls.end(); ++it)
		if ((*it)->GetType() == t) c++;

	return c;
}

CurseGUIControl* CurseGUICtrlHolder::GetControl(int num)
{
	if ((num < 0) || (num >= ((int)controls.size())))
		return NULL;

	return controls.at(num);
}

CurseGUIControl* CurseGUICtrlHolder::GetControl(int num, ECGUIControlType t)
{
	int c;
	vector<CurseGUIControl*>::iterator it;

	for (c = 0, it = controls.begin(); it != controls.end(); ++it)
		if ((*it)->GetType() == t) {
			if (c++ == num) return (*it);
		}

	return NULL;
}

/* ******************************************************************** */

CurseGUIControl::CurseGUIControl(CurseGUICtrlHolder* p, int x, int y)
{
	holder = p;
	typ = GUICL_EMPTY;

	g_x = x;
	g_y = y;
	g_w = 0;
	g_h = 0;

	//default white on black color format
	fmt.bg.r = 0; fmt.bg.g = 0; fmt.bg.b = 0;
	fmt.fg.r = 1000; fmt.fg.g = 1000; fmt.fg.b = 1000;
	fmt.sym = ' ';

	selected = false;

	wnd = p->GetWindow();
	p->Append(this);
}

void CurseGUIControl::Delete()
{
	holder->Delete(this);
}
