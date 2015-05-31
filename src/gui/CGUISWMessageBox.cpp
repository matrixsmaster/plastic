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

#include <stdlib.h>
#include <string.h>
#include "CGUISpecWnd.h"

using namespace std;


CurseGUIMessageBox::CurseGUIMessageBox(CurseGUI* scrn, const char* title, const char* text, const char* butns) :
		CurseGUIWnd(scrn,0,0,2,2)
{
	int w,h,i,b,s,x;
	char* tok, * hld, * str, * buf;
	CurseGUIButton* btn;

	//prepare core variables
	type = GUIWT_MSGBOX;
	if (title) {
		name = string(title);
		showname = true;
	} else {
		name = "Message";
		showname = false;
	}
	stayontop = true;
	focused = true;
	boxed = true;
	result = -1;

	w = strlen(text);							//text width
	i = MSGBOXSIZEX * parent->GetWidth() / 100;	//max width
	b = (int)name.size() + 4;					//title width
	if (butns && (b < (int)strlen(butns)))
		b = strlen(butns);						//buttons line width

	//select basic width and height of text part of the window
	if (i < b) i = b;
	if (w < b) w = b;
	if (w > i) {
		w = i;
		h = strlen(text) / i + 1;
	} else
		h = 1;

	//create text field
	new CurseGUILabel(ctrls,1,1,w,h,string(text));

	h += 2; //make room for buttons

	//create buttons
	b = 0;
	if (butns == NULL) {
		new CurseGUIButton(ctrls,1,h,6,"OK");
		b = 6;
	} else {
		hld = NULL;
		buf = (char*)malloc(strlen(butns)+1);
		if (buf) {
			strcpy(buf,butns);
			str = buf;
			while ((tok = strtok_r(str,"|",&hld))) {
				str = NULL;
				s = strlen(tok) + 2;
				new CurseGUIButton(ctrls,1,h,s,string(tok));
				b += s;
			}
			free(buf);
		}
	}

	//check result buttons line width
	if (b > w) {
		//no wider than parent
		if (b <= (parent->GetWidth()-2)) w = b;
	}

	//sparse buttons
	x = ctrls->GetNumControls(GUICL_BUTTON);
	s = (w - 2 - b) / ((x < 2)? 2:x);
	x = s + 1;
	for (i = 0; i < ctrls->GetNumControls(GUICL_BUTTON); i++) {
		btn = reinterpret_cast<CurseGUIButton*> (ctrls->GetControl(i,GUICL_BUTTON));
		if (!btn) break;
		btn->Move(x,h);
		x += s + btn->GetWidth();
	}

	//finally resize message box window
	w += 2;
	h += 2;
	Resize(w,h);

	MoveToCenter();
}

void CurseGUIMessageBox::MoveToCenter()
{
	Move((parent->GetWidth()-g_w)/2,(parent->GetHeight()-g_h)/2);
}

bool CurseGUIMessageBox::PutEvent(SGUIEvent* e)
{
	int i;

	//consume nothing if closing
	if (will_close) return false;

	//pump event through all controls
	if (ctrls->PutEvent(e)) return true;

	//or try to use it
	switch (e->t) {
	case GUIEV_KEYPRESS:
		if (e->k == '\t') ctrls->Rotate();
		break;

	case GUIEV_RESIZE:
		UpdateSize();
		MoveToCenter();
		break;

	case GUIEV_CTLBACK:
		if (e->b.t != GUIFB_SWITCHED) return false;
		for (i = 0; i < ctrls->GetNumControls(GUICL_BUTTON); i++)
			if (e->b.ctl == ctrls->GetControl(i,GUICL_BUTTON)) {
				result = i;
				break;
			}
		break;

	default: break;
	}
	return false;
}

int CurseGUIMessageBox::GetButtonPressed()
{
	if (result >= 0) parent->RmWindow(this); //close immediately
	return result;
}
