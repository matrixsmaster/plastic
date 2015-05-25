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
	table = new CurseGUITable(ctrls,1,1,7,5,6,10,25+6);
	table->SetData("N", 0, 0);
	table->SetData("Name", 0, 1);
	table->SetData("Weight", 0, 2);
	table->SetData("Condition", 0, 3);
	table->SetData("Cost", 0, 4);

	new CurseGUILabel(ctrls, 1, 11, 12, "Description:");

//	new CurseGUILabel(ctrls, 11, );
	/*CurseGUIButton* destroy_btn;
	CurseGUIButton* drop_btn;
	CurseGUIButton* wear_btn;
	CurseGUIButton* use_btn;
	CurseGUIButton* repair_btn;

	CurseGUIEditBox* search_edit;*/
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
