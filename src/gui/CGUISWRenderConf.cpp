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
#include "LVR.h"


CurseGUIRenderConfWnd::CurseGUIRenderConfWnd(CurseGUI* scrn, LVR* plvr) :
		CurseGUIWnd(scrn,0,0,1,1)
{
	CurseGUIControl* ctl;

	lvr = plvr;

	//Create controls
}

bool CurseGUIRenderConfWnd::PutEvent(SGUIEvent* e)
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
		UpdateSize();
		return false; //don't consume resize event!

	case GUIEV_CTLBACK:
		//TODO
		break;

	default: break;
	}

	/* That's not our event, pass thru */
	return false;
}
