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
#include "datapipe.h"
#include "vmodel.h"
#include "keybinder.h"
#include "LVR.h"
#include "support.h"
#include "debug.h"


enum {
	OBJEDIT_ZOOMP,
	OBJEDIT_ZOOMM,
};

CurseGUIVModEditWnd::CurseGUIVModEditWnd(CurseGUI* scrn, VModel* mod, SGameSettings* setts, SVoxelTab* vtab, bool rw) :
		CurseGUIWnd(scrn,0,0,4,4)
{
	type = GUIWT_OTHER;
	name = (rw)? WNDNAM_VMODEDIT:WNDNAM_VMODVIEW;
	showname = true;
	readonly = !rw;

	pipe = new DataPipeDummy(setts);
	pipe->SetVoxTab(vtab);
	model = mod;
	pipe->AddModel(model);

	lvr = new LVR(pipe);
	campos = model->GetPos().ToReal();
	lvr->SetPosition(campos);

	binder = new KeyBinder(pipe);
	binder->RegKeyByName("OBJEDIT_ZOOMP",OBJEDIT_ZOOMP);
	binder->RegKeyByName("OBJEDIT_ZOOMM",OBJEDIT_ZOOMM);

	SetAutoAlloc(true);
	ResizeWnd(scrn->GetWidth()/2,scrn->GetHeight()/2);
}

CurseGUIVModEditWnd::~CurseGUIVModEditWnd()
{
	delete binder;
	delete lvr;

	pipe->RemoveModel(model);
	delete pipe;
}

void CurseGUIVModEditWnd::ResizeWnd(int w, int h)
{
	if (w < VMODEDITMINW) {
		w = VMODEDITMINW;
		if (w >= parent->GetWidth()) {
			will_close = true; //FIXME: close self immediately
			return;
		}
	}
	if (h < VMODEDITMINH) {
		h = VMODEDITMINH;
		if (h >= parent->GetHeight()) {
			will_close = true;
			return;
		}
	}

	//reset holder
	if (ctrls) delete ctrls;
	ctrls = new CurseGUICtrlHolder(this);

	//resize and move
	Resize(w,h);
	w = (parent->GetWidth() - w) / 2;
	h = (parent->GetHeight() - h) / 2;
	Move(w,h);

	//create working surface
	w = g_w - 2 - ((readonly)? 0:VMODEDITRPAN);
	h = g_h - 2 - ((readonly)? 0:VMODEDITBPAN);
	surf = new CurseGUIPicture(ctrls,1,1,w,h);
	lvr->Resize(w,h);

	Retrace();
}

void CurseGUIVModEditWnd::Retrace()
{
	lvr->Frame();
#ifdef LVRDOUBLEBUFFERED
	lvr->SwapBuffers();
#endif
	surf->SetPicture(lvr->GetRender());
}

bool CurseGUIVModEditWnd::PutEvent(SGUIEvent* e)
{
	if (will_close) return false;

	/* Put the event to controls first */
	if (ctrls->PutEvent(e)) return true;

	/* Window-wide event */
	switch (e->t) {
	case GUIEV_KEYPRESS:
		switch (e->k) {
		case GUI_DEFCLOSE: will_close = true; return true;
		case '\t': ctrls->Rotate(); break;
		default:
			switch (binder->DecodeKey(e->k)) {
			case OBJEDIT_ZOOMP:
				campos.Y += 1;
				lvr->SetPosition(campos);
				break;

			case OBJEDIT_ZOOMM:
				campos.Y -= 1;
				lvr->SetPosition(campos);
				break;
			}
		}
		Retrace();
		return true;

//	case GUIEV_MOUSE:
//		//TODO
//		return true;

	case GUIEV_RESIZE:
		UpdateSize();
		ResizeWnd(g_w,g_h);
		return false; //don't consume resize event!

	default: break;
	}

	/* That's not our event, pass thru */
	return false;
}
