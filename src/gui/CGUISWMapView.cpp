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
#include "datapipe.h"


CurseGUIMapViewWnd::CurseGUIMapViewWnd(CurseGUI* scrn, DataPipe* pdat) :
		CurseGUIWnd(scrn,0,0,2,2)
{
	pipe = pdat;
	name = "Map View";
	showname = true;
	ResizeWnd();
}

CurseGUIMapViewWnd::~CurseGUIMapViewWnd()
{
	//TODO
}

void CurseGUIMapViewWnd::ResizeWnd()
{
	int w,h,x,y;

	w = MAPVIEWSIZEX * parent->GetWidth() / 100;
	h = MAPVIEWSIZEY * parent->GetHeight() / 100;
	x = parent->GetWidth() / 2 - w / 2;
	y = parent->GetHeight() / 2 - h / 2;

	Move(x,y);
	Resize(w,h);
}

void CurseGUIMapViewWnd::Update(bool refr)
{
	//A template of GUI Window update process.
	werase(wnd);				//make a room for window
	UpdateBack();				//draw background image
	ctrls->Update();			//update controls
	DrawDecoration();		 	//draw border, title etc
	wcolor_set(wnd,0,NULL);		//set default color pair (just in case)
	if (refr) wrefresh(wnd);	//and refresh if necessary
}

bool CurseGUIMapViewWnd::PutEvent(SGUIEvent* e)
{
	if (will_close) return false;

	/* Put the event to controls first */
	if (ctrls->PutEvent(e)) return true;

	/* Window-wide event */
	switch (e->t) {
	case GUIEV_KEYPRESS:
		switch (e->k) {
		case GUI_DEFCLOSE: will_close = true; return true;
		case '\t': ctrls->Rotate(); return true;
		}
		return false;

	case GUIEV_RESIZE:
		ResizeWnd();
		return false; //don't consume resize event!

	default: break;
	}

	/* That's not our event, pass thru */
	return false;
}
