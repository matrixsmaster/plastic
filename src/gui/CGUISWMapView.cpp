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


static const SGUIPixel empty_tile = {
		' ', { 1000, 1000, 1000 }, { 0, 0, 0 }
};

CurseGUIMapViewWnd::CurseGUIMapViewWnd(CurseGUI* scrn, DataPipe* pdat) :
		CurseGUIWnd(scrn,0,0,2,2)
{
	pipe = pdat;
	basex = basey = 0;
	scale = 1;

	name = "Map View";
	showname = true;

	ResizeWnd();
	SetAutoAlloc(true);
	DrawMap();
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

void CurseGUIMapViewWnd::DrawMap()
{
	int x,y,l,mx,my,ml;
	const SWGCell* map = pipe->GetGlobalSurfaceMap();
	vector2di msz = pipe->GetGlobalSurfaceSize();

	/* Draw the map */
	for (y = 0; y < g_h-3; y++) {
		l = (y + 2) * g_w + 2; //make a room for a window border and a ruler
		//apply scale
		if (scale > 0) //magnification
			my = (y + basey) / scale;
		else //shrinking
			my = (y + basey) * -scale;

		for (x = 0; x < g_w-3; x++, l++) {
			//apply scale
			if (scale > 0) //magnification
				mx = (x + basex) / scale;
			else //shrinking
				mx = (x + basex) * -scale;

			//calculate map cell linear address
			ml = (msz.Y - my - 1) * msz.X + mx; //invert Y axis

			if (	(mx < 0) || (mx >= msz.X) ||
					(my < 0) || (my >= msz.Y) ||
					(ml >= (msz.X*msz.Y)) )

				backgr[l] = empty_tile;
			else {
				backgr[l] = wrld_tiles[map[ml].t];
				//TODO: elev
			}
		}
	}

	/* Draw the rulers */
}

bool CurseGUIMapViewWnd::PutEvent(SGUIEvent* e)
{
	bool d = false;

	if (will_close) return false;

	/* Put the event to controls first */
	if (ctrls->PutEvent(e)) return true;

	/* Window-wide event */
	switch (e->t) {
	case GUIEV_KEYPRESS:
		switch (e->k) {
		case GUI_DEFCLOSE: will_close = true; return true;
		case '\t': ctrls->Rotate(); return true;
		case KEY_UP: basey--; d = true; break;
		case KEY_DOWN: basey++; d = true; break;
		case KEY_LEFT: basex--; d = true; break;
		case KEY_RIGHT: basex++; d = true; break;
		case '-':
			scale--;
			if (scale == 0) scale = -1;
			d = true;
			break;
		case '=':
			scale++;
			if (scale == 0) scale = 1;
			d = true;
			break;
		}
		break;

	case GUIEV_RESIZE:
		ResizeWnd();
		DrawMap();
		return false; //don't consume resize event!

	default: break;
	}

	if (d) {
		//update map and consume event
		DrawMap();
		return true;
	}

	/* That's not our event, pass thru */
	return false;
}
