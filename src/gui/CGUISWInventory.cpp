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
#include "CGUIControls.h"
#include "inventory.h"


CurseGUIInventoryWnd::CurseGUIInventoryWnd(CurseGUI* scrn, Inventory* iptr) :
	CurseGUIWnd(scrn,0,0,2,2)
{
	type = GUIWT_OTHER;
	name = "Inventory";
	showname = true;

	invent = iptr;

	ResizeWnd();
}

void CurseGUIInventoryWnd::ResizeWnd()
{
	int w,h,x,y;

	int x1,y1,wt,ht,wwt,nr,nc;
	wwt = 6; nr = 7; nc = 5;
	x1 = 1; y1 = 1;
	ht = 15; wt = wwt * nc + nc + 1;

	w = INVENTSIZEX * parent->GetWidth() / 100;
	h = INVENTSIZEY * parent->GetHeight() / 100;

	//lower-right corner
	x = parent->GetWidth() - w;
	y = parent->GetHeight() - h;

	Move(x,y);
	Resize(w,h);

	//reset holder
	if (ctrls) delete ctrls;
	ctrls = new CurseGUICtrlHolder(this);

	//create controls
	table = new CurseGUITable(ctrls,x1,y1,nr,nc,wwt,ht,wt);
	table->SetData("  N", 0, 0);
	table->SetData(" Name", 0, 1);
	table->SetData("Weight", 0, 2);
	table->SetData(" Cond.", 0, 3);
	table->SetData(" Cost", 0, 4);

	new CurseGUILabel(ctrls, x1, y1+ht, wt, 1, "Description:");
	description_lbl = new CurseGUILabel(ctrls, x1, y1+ht+1, wt, 5, "");

	destroy_btn = new CurseGUIButton(ctrls, wt+2, y1, 11, "Destroy");
	drop_btn = new CurseGUIButton(ctrls, wt+2, y1+1, 11, "Drop");
	wear_btn = new CurseGUIButton(ctrls, wt+2, y1+2, 11, "Wear");
	use_btn = new CurseGUIButton(ctrls, wt+2, y1+3, 11, "Use");
	repair_btn = new CurseGUIButton(ctrls, wt+2, y1+4, 11, "Repair");

	new CurseGUILabel(ctrls, wt+3, y1+6, 7, 1, "Search");
	search_edit = new CurseGUIEditBox(ctrls, wt+2, y1+7, 12, "_____________");
}

bool CurseGUIInventoryWnd::PutEvent(SGUIEvent* e)
{
	if (will_close) return false;

	/* Put the event to controls first */
	if (ctrls->PutEvent(e)) return true;

	switch (e->t) {
	case GUIEV_KEYPRESS:
		switch (e->k) {
		case GUI_DEFCLOSE: will_close = true; return true;
		case '\t': ctrls->Rotate(); break;
		}
		return false;

	case GUIEV_RESIZE:
		ResizeWnd();
		UpdateSize();
		return false; //don't consume resize event!

	default: break;
	}
	return false;
}
