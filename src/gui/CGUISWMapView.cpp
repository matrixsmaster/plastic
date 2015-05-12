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
#include "support.h"


static const SGUIPixel empty_tile = {
		' ', { 1000, 1000, 1000 }, { 0, 0, 0 }
};

CurseGUIMapViewWnd::CurseGUIMapViewWnd(CurseGUI* scrn, DataPipe* pdat) :
		CurseGUIWnd(scrn,0,0,2,2)
{
	pipe = pdat;
	scale = 1;
	showelev = false;
	m_w = m_h = 0;

	name = "Map View";
	showname = true;

	ResizeWnd();
	SetAutoAlloc(true);
	DrawMap();
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
	int x,y,l,mx,my,ml,rt,rl;
	unsigned k,u;
	char vert[MAPVIEWRULSTR],horz[MAPVIEWRULSTR];
	const SWGCell* map = pipe->GetGlobalSurfaceMap();
	vector2di mbs,msz = pipe->GetGlobalSurfaceSize();
	vector3d col;

	//store dimensions
	m_w = g_w - 3; //reserve space for a border and a ruler
	m_h = g_h - 3;

	//calculate shift
	mbs.X = pos.X + msz.X / 2;
	mbs.Y = pos.Y + msz.Y / 2;
	mbs += base;
	if (scale > 0) mbs *= scale;
	else mbs /= scale * -1;
	mbs -= vector2di(m_w/2,m_h/2);

	//prepare rulers
	rt = (g_w-3) / MAPVIEWRULX;
	rl = (g_h-3) / MAPVIEWRULY;
	vert[0] = 0;

	/* Draw the map */
	for (y = 0; y < m_h; y++) {
		l = (y + 2) * g_w + 2; //make a room for a window border and a ruler
		if (l >= backgr_size) break; //just in case, check buffer bounds

		//apply scale
		if (scale > 0) //magnification
			my = (y + mbs.Y) / scale;
		else //shrinking
			my = (y + mbs.Y) * -scale;

		//draw the vertical ruler
		if (y % rl == 0) {
			snprintf(vert,sizeof(vert),">%d",(my-msz.Y/2));
			u = 0;
		}
		if (u < strlen(vert)) {
			backgr[l-1] = empty_tile;
			backgr[l-1].sym = vert[u++];
		} else
			backgr[l-1].sym = ' ';

		//reset horizontal ruler string
		horz[0] = 0;

		for (x = 0; x < m_w; x++, l++) {
			if (l >= backgr_size) break; //just in case, check buffer bounds

			//apply scale
			if (scale > 0) //magnification
				mx = (x + mbs.X) / scale;
			else //shrinking
				mx = (x + mbs.X) * -scale;

			//draw the horizontal ruler
			if (y == 0) {
				if (x % rt == 0) {
					snprintf(horz,sizeof(horz),"|%d",(mx-msz.X/2));
					k = 0;
				}
				if (k < strlen(horz)) {
					backgr[l-g_w] = empty_tile;
					backgr[l-g_w].sym = horz[k++];
				} else
					backgr[l-g_w].sym = ' ';
			}

			//calculate map cell linear address
			ml = (msz.Y - my - 1) * msz.X + mx; //invert Y axis

			if (	(mx < 0) || (mx >= msz.X) ||
					(my < 0) || (my >= msz.Y) ||
					(ml >= (msz.X*msz.Y)) )

				backgr[l] = empty_tile;
			else {
				backgr[l] = wrld_tiles[map[ml].t];

				//Elevation
				if (!showelev) continue;
				col = tripletovecf(backgr[l].bg);
				switch (map[ml].elev) {
				case 0: col *= MAPVIEWLOWM; break;
				case 1: break;
				case 2: col *= MAPVIEWHIGHM; break;
				}
				if (col.X > 1000) col.X = 1000;
				if (col.Y > 1000) col.Y = 1000;
				if (col.Z > 1000) col.Z = 1000;
				backgr[l].bg = vecftotriple(col);
			}
		}
	}
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

		//standard controls
		case GUI_DEFCLOSE: will_close = true; return true;
		case '\t': ctrls->Rotate(); return true;
		default: return true; //consume ALL keyboard events

		//elevation showing
		case 'e':
		case 'E':
			showelev ^= true;
			DrawMap();
			break;

		//navigation
		case KEY_UP: base.Y--; d = true; break;
		case KEY_DOWN: base.Y++; d = true; break;
		case KEY_LEFT: base.X--; d = true; break;
		case KEY_RIGHT: base.X++; d = true; break;

		//scale
		case '-':
			scale--;
			if (scale == 0) scale = -2;
			d = true;
			break;
		case '=':
			scale++;
			if (scale == 0) scale = 2;
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
