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
	int w,h,i,b;
	char* tok, * hld;

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

	w = strlen(text);							//text width
	i = MSGBOXSIZEX * parent->GetWidth() / 100;	//max width
	b = (int)name.size() + 4;					//title width
	if (i < b) i = b;
	if (w < b) w = b;
	if (w > i) {
		w = i;
		h = strlen(text) / i + 1;
	} else
		h = 1;

	new CurseGUILabel(ctrls,1,1,w,h,string(text));

	h += 2; //make room for buttons

	b = 0;
	hld = NULL;
	if (butns == NULL) {
		new CurseGUIButton(ctrls,1,h,6,"OK");
		b++;
	} else {
		//TODO
	}

	for (i = 0; i < b; i++) {
//		ctrls-
	}

	w += 2;
	h += 2;

	Resize(w,h);

	MoveToCenter();
}

void CurseGUIMessageBox::MoveToCenter()
{
	//TODO
}

bool CurseGUIMessageBox::PutEvent(SGUIEvent* e)
{
	//TODO
	return false;
}
