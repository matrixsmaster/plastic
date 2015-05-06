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

CurseGUIControl::CurseGUIControl(CurseGUICtrlHolder* p, int x, int y)
{
	holder = p;
	g_x = x;
	g_y = y;
	back.r = 0;
	back.g = 0;
	back.b = 0;
	wnd = p->GetWindow();
	p->Append(this);
}

void CurseGUIControl::Delete()
{
	holder->Delete(this);
}

CurseGUIPicture::CurseGUIPicture(CurseGUICtrlHolder* p, int x, int y, int w, int h) :
		CurseGUIControl(p,x,y)
{
	pict = NULL;
	autoalloc = false;
	length = w * h * sizeof(SGUIPixel);
}

CurseGUIPicture::~CurseGUIPicture()
{
	if (autoalloc && pict) free(pict);
}

bool CurseGUIPicture::SetAutoAlloc(bool a)
{
	//free memory if auto-allocated
	if (autoalloc && pict) {
		free(pict);
		pict = NULL;
	}
	//set new flag
	autoalloc = a;
	//allocate memory
	if (a) {
		pict = (SGUIPixel*)malloc(length);
		if (!pict) {
			autoalloc = false;
			return false;
		}
	}
	//All operations are completed successfully
	return true;
}

void CurseGUIPicture::SetPicture(SGUIPixel* p)
{
	if (autoalloc) {
		if (!pict) return; //fail silently
		memcpy(pict,p,length);
	} else
		pict = p;

}

void CurseGUIPicture::Update()
{
	if (!pict) return;
	//TODO
}
