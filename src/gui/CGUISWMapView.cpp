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
#include "misconsts.h"


static const SGUIPixel empty_tile = {
		' ', { 1000, 1000, 1000 }, { 0, 0, 0 }
};

static const SGUIPixel curpos_tile = {
		'X', { 1000, 1000, 1000 }, { 900, 0, 0 }
};

static const SGUIPixel bottom_line = {
		' ', { 0, 900, 0 }, { 0, 0, 0 }
};

CurseGUIMapViewWnd::CurseGUIMapViewWnd(CurseGUI* scrn, DataPipe* pdat) :
		CurseGUIWnd(scrn,0,0,2,2)
{
	type = GUIWT_OTHER;
	name = WNDNAM_MAPVIEW;
	showname = true;

	pipe = pdat;
	scale = 1;
	showelev = false;
	m_w = m_h = 0;

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
	int x,y,l,mx,my,myi,ml,rt,rl;
	unsigned k,u;
	char vert[MAPVIEWRULSTR],horz[MAPVIEWRULSTR],botl[MAPVIEWBOTLINSTR];
	const SWGCell* map = pipe->GetGlobalSurfaceMap();
	vector2di mbs,msz = pipe->GetGlobalSurfaceSize();
	vector3d col;

	//store dimensions
	m_w = g_w - 3; //reserve space for a border and a ruler
	m_h = g_h - 4; //reserve space for: border, ruler, bottom line

	//calculate shift
	mbs.X = gpos.X;
	mbs.Y = msz.Y - gpos.Y - 1;
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

		myi = msz.Y - my - 1; //invert Y axis

		//draw the vertical ruler
		if (y % rl == 0) {
			snprintf(vert,sizeof(vert),">%d",myi);
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
					snprintf(horz,sizeof(horz),"|%d",mx);
					k = 0;
				}
				if (k < strlen(horz)) {
					backgr[l-g_w] = empty_tile;
					backgr[l-g_w].sym = horz[k++];
				} else
					backgr[l-g_w].sym = ' ';
			}

			//calculate map cell linear address
			ml = myi * msz.X + mx;

			if (	(mx < 0) || (mx >= msz.X) ||
					(my < 0) || (my >= msz.Y) ||
					(ml >= (msz.X*msz.Y)) )

				backgr[l] = empty_tile;
			else if ((mx == gpos.X) && (my == (msz.Y - gpos.Y - 1)))
				backgr[l] = curpos_tile;
			else
				backgr[l] = wrld_tiles[map[ml].t];

			//Elevation
			if ((!showelev) || (backgr[l].sym == ' ')) continue;

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
			backgr[l].sym = map[ml].elev + '1';
		}
	}

	/* Draw bottom line */
	snprintf(botl,sizeof(botl),"GPos = [%d %d %d];  LPos = [%d %d %d]",
			gpos.X,gpos.Y,gpos.Z,lpos.X,lpos.Y,lpos.Z);

	for (k = 2; k < (unsigned)m_w; k++) {
		l = (g_h-2) * g_w + k;
		u = k - 2;
		backgr[l] = bottom_line;
		if ((u >= strlen(botl)) || (u >= sizeof(botl))) break;
		backgr[l].sym = botl[u];
	}
}

void CurseGUIMapViewWnd::SetPos(const vector3di glob, const vector3di loc)
{
	gpos = glob;
	lpos = loc;
	DrawMap();
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
		case KEY_UP:	base.Y -= (scale < 0)? -scale:1; d = true; break;
		case KEY_DOWN:	base.Y += (scale < 0)? -scale:1; d = true; break;
		case KEY_LEFT:	base.X -= (scale < 0)? -scale:1; d = true; break;
		case KEY_RIGHT:	base.X += (scale < 0)? -scale:1; d = true; break;

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
