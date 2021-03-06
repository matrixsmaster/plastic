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
#include "debug.h"

using namespace std;


CurseGUIInventoryWnd::CurseGUIInventoryWnd(CurseGUI* scrn, Inventory* iptr) :
	CurseGUIWnd(scrn,0,0,2,2)
{
	type = GUIWT_OTHER;
	sorttype = INV_SDEFAULT;
	name = WNDNAM_INVENTORY;
	showname = true;

	invent = iptr;
	sitem = prev = 1;
	cso = 0;
	srch = 1;

	ResizeWnd();
	SetSelectedItem();
}

void CurseGUIInventoryWnd::ResizeWnd()
{
	int w,h,x,y;

	int x1,y1,wt,ht,wwt,nr,nc,wb;
	int fc,hd;
	SGUIPixel cfmt;
	memset(&cfmt,0,sizeof(cfmt));


	hd = 5; //description height
	wb = 20; //right buttons an label width
	wwt = 6; nr = 7; nc = 6;
	x1 = 1; y1 = 1;
	ht = 15;

	w = INVENTSIZEX * parent->GetWidth() / 100;
	h = INVENTSIZEY * parent->GetHeight() / 100;

	//visible width of table
	wt = w - wb;
	//visible height of table
	ht = h - hd - 5;

	//lower-right corner
	x = parent->GetWidth() - w;
	y = parent->GetHeight() - h;

	Move(x,y);
	Resize(w,h);

	//reset holder
	if (ctrls) delete ctrls;
	ctrls = new CurseGUICtrlHolder(this);

	//set the number of rows
	nr = invent->GetNumItems() + 1;

	//create table
	table = new CurseGUITable(ctrls,x1,y1,nr,nc,wwt,ht,wt, true);
	table->SetColumnWidth(0, 4);
	table->SetColumnWidth(3, 5);

	fc = table->GetTableWidth() - wwt;
	table->SetColumnWidth(1, wt - fc);

	cfmt.fg.r = 10; cfmt.fg.g = 400; cfmt.fg.b = 10;
	description_lbl = new CurseGUILabel(ctrls, x1, y1+ht+2, wt, 1, "Description:");
	description_lbl->SetFormat(cfmt);

	cfmt.fg.r = 10; cfmt.fg.g = 700; cfmt.fg.b = 10;
	description_lbl = new CurseGUILabel(ctrls, x1, y1+ht+3, wt, hd, "");
	description_lbl->SetFormat(cfmt);

	destr_btn = new CurseGUIButton(ctrls, wt+3, y1+1, 11, "Destroy");
	drop_btn = new CurseGUIButton(ctrls, wt+3, y1+2, 11, "Drop");
	wear_btn = new CurseGUIButton(ctrls, wt+3, y1+3, 11, "Wear");
	use_btn = new CurseGUIButton(ctrls, wt+3, y1+4, 11, "Use");
	repair_btn = new CurseGUIButton(ctrls, wt+3, y1+5, 11, "Repair");

	new CurseGUILabel(ctrls, wt+5, y1+7, 7, 1, "Search");
	search_edit = new CurseGUIEditBox(ctrls, wt+3, y1+8, 12, "");
	searchname = new CurseGUICheckBox(ctrls, wt+3, y1+9, 13, "name");
	searchname->SetChecked(true);
	searchdesc = new CurseGUICheckBox(ctrls, wt+3, y1+10, 13, "descr");

	sort_btn  = new CurseGUIButton(ctrls, wt+3, y1+12, 8, "Sort");
	sortname = new CurseGUICheckBox(ctrls, wt+3, y1+13, 13, "by name");
	sortwght = new CurseGUICheckBox(ctrls, wt+3, y1+14, 13, "by weight");
	sortcnt = new CurseGUICheckBox(ctrls, wt+3, y1+15, 13, "by count");
	sortcost = new CurseGUICheckBox(ctrls, wt+3, y1+16, 13, "by cost");

	FillTableHeader();
	FillInventoryTable();

}

void CurseGUIInventoryWnd::FillTableHeader()
{
	table->SetData("N", 0, 0);
	table->SetData("Name", 0, 1);
	table->SetData("Count", 0, 2);
	table->SetData("Weight", 0, 3);
	table->SetData("Cond.", 0, 4);
	table->SetData("Cost", 0, 5);

}

void CurseGUIInventoryWnd::FillInventoryTable()
{
	char tmp[5];

	for (int i = 1; i < invent->GetNumItems()+1; ++i) {
		sprintf(tmp, "%d", i);
		table->SetData(string(tmp), i, 0);
		table->SetData(invent->GetInventoryObject(i-1)->GetName(), i, 1);
		table->SetData(IntToString(invent->GetInventoryObject(i-1)->GetCount()), i, 2);
		table->SetData(IntToString(invent->GetInventoryObject(i-1)->GetWeight()), i, 3);
		table->SetData(IntToString(invent->GetInventoryObject(i-1)->GetCondition()), i, 4);
		table->SetData(IntToString(invent->GetInventoryObject(i-1)->GetCost()), i, 5);
	}
}

void CurseGUIInventoryWnd::SetSelectedItem()
{
	char tmp[5];
	int n;

	if (sitem < 1) sitem = 1;
	else if (sitem > invent->GetNumItems()) sitem = invent->GetNumItems();

	//previous item
	sprintf(tmp, "%d", prev);
	table->SetData(string(tmp), prev, 0);

	memset(&tmp, 0, 5);

	//highlight current item
	if (invent->GetNumItems()) {
		sprintf(tmp, ">%d", sitem);
		table->SetData(string(tmp), sitem, 0);
	}

	//show description for item from inventory
	ShowDescription();

	//items scrolling
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
	if (invent->GetNumItems() > 0)
		description_lbl->SetCaption(invent->GetInventoryObject(sitem-1)->GetDesc());
	else
		description_lbl->SetCaption("You have no items.");
}

int CurseGUIInventoryWnd::Search(int n)
{
	int i;
	string nm, pt;

	pt = search_edit->GetText();

	for (i = n; i < invent->GetNumItems(); i++) {
		switch(srch) {
		case 2: //search by description
			nm = invent->GetInventoryObject(i)->GetDesc();
			break;
		case 3: //search by name and description
			nm = invent->GetInventoryObject(i)->GetName() + " " + invent->GetInventoryObject(i)->GetDesc();;
			break;
		default: //search by name
			nm = invent->GetInventoryObject(i)->GetName();
			break;
		}
		if (strcasestr(nm.c_str(),pt.c_str()))
			return i;
	}

	return -1;
}

void CurseGUIInventoryWnd::SearchObject()
{
	int n = -1;

	//FIXME comment it!
	if (Search(cso + 1) == -1)
		cso = -1;
	n = Search(cso+1);
	cso = n;

	if (n > -1) {
		prev = sitem;
		sitem = n+1;
		SetSelectedItem();
	}
}

void CurseGUIInventoryWnd::DestroyObject()
{
	InventoryObject *obj;
	if (!invent->GetNumItems()) return;

	obj = invent->GetInventoryObject(sitem-1);

	if (!invent->DestroyObject(obj)) {
		dbg_print("Error destroy \"%s\" ", obj->GetName().c_str());

		return;
	}
	//TODO send message to overlay
	dbg_print("Destroy \"%s\" ", obj->GetName().c_str());

	//Write count of object to table
	table->SetData(IntToString(obj->GetCount()), sitem, 2);

	if (obj->GetCount() > 0) return;

	table->EraseTableData();
	table->DelRow();

	//fill table and select first item
	FillTableHeader();

	if(invent->GetNumItems() > 0) {
		FillInventoryTable();
		SetSelectedItem();
	} else description_lbl->SetCaption("You have no items.");

}

void CurseGUIInventoryWnd::DropObject()
{
	//TODO button "drop object"
}

void CurseGUIInventoryWnd::WearObject()
{
	//TODO button "wear object"
}

void CurseGUIInventoryWnd::UseObject()
{
	//TODO button "use object"
}

void CurseGUIInventoryWnd::RepairObject()
{
	//TODO button "repair object"
}

void CurseGUIInventoryWnd::Sort()
{
	if (invent->GetNumItems() < 1) return;

	//clear table
	table->EraseTableData();

	//set default sorting type if checkboxes are not checked
	if (!sortname->GetChecked() && !sortwght->GetChecked() && !sortcnt->GetChecked() && !sortcost->GetChecked())
			sorttype = INV_SDEFAULT;

	//choose sorting type
	switch(sorttype) {
	case INV_SNAME:
		invent->SortByName();
		break;
	case INV_SWEIGHT:
		invent->SortByWeight();
		break;
	case INV_SCOUNT:
		invent->SortByCount();
		break;
	case INV_SCOST:
		invent->SortByCost();
		break;
	default:
		invent->SortDefault();
		break;
	}

	//fill table and select first item
	FillTableHeader();
	FillInventoryTable();
	prev = sitem = 1;
	SetSelectedItem();
}

void CurseGUIInventoryWnd::CheckCbox(CurseGUIControl* ctl)
{
	int i;
	CurseGUICheckBox* ptr;

	//Set searching variable
	if (ctl == searchname) {
		if (searchname->GetChecked())
			srch++;
		else srch--;
		return;
	} else if (ctl == searchdesc) {
		if (searchdesc->GetChecked()) srch += 2;
		else srch -= 2;
		return;
	}

	//set sorting type
	if (ctl == sortname)		sorttype = INV_SNAME;
	else if (ctl == sortwght)	sorttype = INV_SWEIGHT;
	else if (ctl == sortcnt)	sorttype = INV_SCOUNT;
	else if (ctl == sortcost)	sorttype = INV_SCOST;

	//disable other checkboxes (ignores all checkboxes above the position of the 'sort' button)
	for (i = 0; i < ctrls->GetNumControls(GUICL_CHECKBOX); i++) {
		ptr = (CurseGUICheckBox*)ctrls->GetControl(i,GUICL_CHECKBOX);
		if (ptr->GetPosY() <= sort_btn->GetPosY()) continue;
		if (ptr != ctl) ptr->SetChecked(false);
	}
}

void CurseGUIInventoryWnd::CheckButtons(CurseGUIControl* ctl)
{
	if (ctl == destr_btn) {
		DestroyObject();
	}  else if (ctl == drop_btn) {
		//TODO
	} else if (ctl == wear_btn) {
		//TODO
	} else if (ctl == use_btn) {
		//TODO
	} else if (ctl == repair_btn) {
		//TODO
	} else if (ctl == sort_btn) {
		Sort();
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
		case KEY_HOME:
			prev = sitem;
			sitem = 0;
			SetSelectedItem();
			return true;
		case KEY_END:
			prev = sitem;
			sitem = invent->GetNumItems();
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
			//search edit
			SearchObject();
			break;

		case GUIFB_CHECKON:
		case GUIFB_CHECKOFF:
			//checkboxes
			CheckCbox(e->b.ctl);
			break;

		case GUIFB_SWITCHED:
			//check buttons
			CheckButtons(e->b.ctl);
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


