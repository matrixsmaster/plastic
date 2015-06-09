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

#include <cstdlib>
#include "CGUISpecWnd.h"
#include "CGUIControls.h"
#include "inventory.h"
#include "misconsts.h"

using namespace std;


CurseGUIInventoryWnd::CurseGUIInventoryWnd(CurseGUI* scrn, Inventory* iptr) :
	CurseGUIWnd(scrn,0,0,2,2)
{
	type = GUIWT_OTHER;
	name = WNDNAM_INVENTORY;
	showname = true;

	invent = iptr;
	sitem = prev = 1;
	temp = 0;

	ResizeWnd();
	SetSelectedItem();
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

	nr = invent->GetNumberItems() + 1;

	//create controls
	table = new CurseGUITable(ctrls,x1,y1,nr,nc,wwt,ht,wt, true);
	table->SetData("N", 0, 0);
	table->SetData("Name", 0, 1);
	table->SetData("Weight", 0, 2);
	table->SetData("Cond.", 0, 3);
	table->SetData("Cost", 0, 4);
	table->SetColumnWidth(0, 4);
	table->SetColumnWidth(1, 10);
	table->SetColumnWidth(3, 5);

	new CurseGUILabel(ctrls, x1, y1+ht+1, wt, 1, "Description:");
	description_lbl = new CurseGUILabel(ctrls, x1, y1+ht+2, wt, 5, "");

	destroy_btn = new CurseGUIButton(ctrls, wt+3, y1+1, 11, "Destroy");
	drop_btn = new CurseGUIButton(ctrls, wt+3, y1+2, 11, "Drop");
	wear_btn = new CurseGUIButton(ctrls, wt+3, y1+3, 11, "Wear");
	use_btn = new CurseGUIButton(ctrls, wt+3, y1+4, 11, "Use");
	repair_btn = new CurseGUIButton(ctrls, wt+3, y1+5, 11, "Repair");

	new CurseGUILabel(ctrls, wt+5, y1+7, 7, 1, "Search");
	search_edit = new CurseGUIEditBox(ctrls, wt+2, y1+8, 12, "");

	new CurseGUILabel(ctrls, wt+5, y1+9, 7, 1, "Sort");
	sortname = new CurseGUICheckBox(ctrls, wt+2, y1+10, 13, "by name");
	sortwght = new CurseGUICheckBox(ctrls, wt+2, y1+11, 13, "by weight");

	FillInventoryTable();

}

void CurseGUIInventoryWnd::FillInventoryTable()
{
	char tmp[5];

	for (int i = 1; i < invent->GetNumberItems()+1; ++i) {
		sprintf(tmp, "%d", i);
		table->SetData(string(tmp), i, 0);
		table->SetData(invent->GetInventoryObject(i-1)->GetName(), i, 1);
		table->SetData(IntToString(invent->GetInventoryObject(i-1)->GetWeight()), i, 2);
		table->SetData(IntToString(invent->GetInventoryObject(i-1)->GetCondition()), i, 3);
		table->SetData(IntToString(invent->GetInventoryObject(i-1)->GetCost()), i, 4);
	}
}

void CurseGUIInventoryWnd::SetSelectedItem()
{
	char tmp[5];
	int n;

	if (sitem < 1) sitem = 1;
	else if (sitem > invent->GetNumberItems()) sitem = invent->GetNumberItems();

	//previous item
	sprintf(tmp, "%d", prev);
	table->SetData(string(tmp), prev, 0);

	memset(&tmp, 0, 5);

	//highlight current item
	sprintf(tmp, ">%d", sitem);
	table->SetData(string(tmp), sitem, 0);

	//show description for item from inventory
	ShowDescription();

	//scrolling items
	n = 0;
	if (sitem == 1) table->SetScrolly(n);
	else {
		for (int i = 0; i < sitem; ++i)
			n += table->GetRowHeight(i);
		table->SetScrolly(n);
	}
}

void CurseGUIInventoryWnd::ShowDescription()
{
	description_lbl->SetCaption(invent->GetInventoryObject(sitem-1)->GetDesc());
}

int CurseGUIInventoryWnd::Search(int n)
{
	int i;
	string nm, pt;

	pt = search_edit->GetText();

	for (i = n; i < invent->GetNumberItems(); i++) {
		nm = invent->GetInventoryObject(i)->GetName();
		if (strcasestr(nm.c_str(),pt.c_str()))
			return i;
	}

	return -1;
}

void CurseGUIInventoryWnd::SearchObject()
{
	int n = -1;

	if (Search(temp + 1) == -1)
		temp = -1;
	n = Search(temp+1);
	temp = n;

	if (n > -1) {
		prev = sitem;
		sitem = n+1;
		SetSelectedItem();
	}
}

void CurseGUIInventoryWnd::SortByNumber()
{
	//TODO
}

void CurseGUIInventoryWnd::SortByName()
{
//	invent->SortByName();
}

void CurseGUIInventoryWnd::Sort()
{
	//TODO
	//default: by number
	//by name
	//by weight
}

void CurseGUIInventoryWnd::CheckCbox()
{
	if (sortname->IsSelected()) {
		SortByName();
	}
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
		case KEY_UP:
			prev = sitem--;
			SetSelectedItem();
			return true;
		case KEY_DOWN:
			prev = sitem++;
			SetSelectedItem();
			return true;
		}
		return false;

	case GUIEV_RESIZE:
		ResizeWnd();
		UpdateSize();
		return false; //don't consume resize event!

	case GUIEV_CTLBACK:
		switch (e->b.t) {
		case GUIFB_EDITOK:
			SearchObject();
			break;

		case GUIFB_CHECKON:
		case GUIFB_CHECKOFF:
			CheckCbox();
			break;
		default: break;
		}
		break;

	default: break;
	}
	return false;
}

string CurseGUIInventoryWnd::IntToString(int v)
{
	char tmp[5];
	snprintf(tmp, 5, "%d", v);
	return string(tmp);
}


