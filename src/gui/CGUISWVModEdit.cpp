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
	OBJEDIT_WORK,
	OBJEDIT_WNDXP,
	OBJEDIT_WNDXN,
	OBJEDIT_WNDYP,
	OBJEDIT_WNDYN,
	OBJEDIT_ZOOMP,
	OBJEDIT_ZOOMM,
	OBJEDIT_MROTXP,
	OBJEDIT_MROTXN,
	OBJEDIT_MROTYP,
	OBJEDIT_MROTYN,
	OBJEDIT_MROTZP,
	OBJEDIT_MROTZN,
	OBJEDIT_MROTRST,
};

using namespace std;

CurseGUIVModEditWnd::CurseGUIVModEditWnd(CurseGUI* scrn, const char* modfn, SGameSettings* setts, SVoxelTab* vtab, bool rw) :
		CurseGUIWnd(scrn,0,0,4,4)
{
	type = GUIWT_OTHER;
	name = (rw)? WNDNAM_VMODEDIT:WNDNAM_VMODVIEW;
	showname = true;
	readonly = !rw;

	pipe = new DataPipeDummy(setts);
	pipe->SetVoxTab(vtab);
	model = (modfn)? (pipe->LoadModel(modfn,vector3di(),vector3di())):NULL;
	fname = modfn;

	lvr = new LVR(pipe);
	if (model)
		campos = model->GetPos().ToReal();
	lvr->SetPosition(campos);

	binder = new KeyBinder(pipe);
	binder->RegKeyByName("OBJEDIT_WORK",OBJEDIT_WORK);
	binder->RegKeyByName("OBJEDIT_WNDXP",OBJEDIT_WNDXP);
	binder->RegKeyByName("OBJEDIT_WNDXN",OBJEDIT_WNDXN);
	binder->RegKeyByName("OBJEDIT_WNDYP",OBJEDIT_WNDYP);
	binder->RegKeyByName("OBJEDIT_WNDYN",OBJEDIT_WNDYN);
	binder->RegKeyByName("OBJEDIT_ZOOMP",OBJEDIT_ZOOMP);
	binder->RegKeyByName("OBJEDIT_ZOOMM",OBJEDIT_ZOOMM);
	binder->RegKeyByName("OBJEDIT_MROTXP",OBJEDIT_MROTXP);
	binder->RegKeyByName("OBJEDIT_MROTXN",OBJEDIT_MROTXN);
	binder->RegKeyByName("OBJEDIT_MROTYP",OBJEDIT_MROTYP);
	binder->RegKeyByName("OBJEDIT_MROTYN",OBJEDIT_MROTYN);
	binder->RegKeyByName("OBJEDIT_MROTZP",OBJEDIT_MROTZP);
	binder->RegKeyByName("OBJEDIT_MROTZN",OBJEDIT_MROTZN);
	binder->RegKeyByName("OBJEDIT_MROTRST",OBJEDIT_MROTRST);

	SetAutoAlloc(true);
	ResizeWnd(scrn->GetWidth()/2,scrn->GetHeight()/2);
}

CurseGUIVModEditWnd::~CurseGUIVModEditWnd()
{
	delete binder;
	delete lvr;

	if (model) pipe->UnloadModel(model);
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

	//create filename editbox
	e_fname = new CurseGUIEditBox(ctrls,1,1,g_w-2,string(fname));

	//create working surface
	w = g_w - 2 - ((readonly)? 0:VMODEDITRPAN);
	h = g_h - 2 - ((readonly)? 0:VMODEDITBPAN);
	surf = new CurseGUIPicture(ctrls,1,2,w,h);
	lvr->Resize(w,h);

	Retrace();
}

void CurseGUIVModEditWnd::Retrace()
{
	lvr->Frame();
	lvr->Postprocess();
#ifdef LVRDOUBLEBUFFERED
	lvr->SwapBuffers();
#endif
	surf->SetPicture(lvr->GetRender());
}

bool CurseGUIVModEditWnd::PutEvent(SGUIEvent* e)
{
	vector3di mr;

	if (will_close) return false;

	/* Check for 'return to work' mode key */
	if ((e->t == GUIEV_KEYPRESS) && (binder->DecodeKey(e->k) == OBJEDIT_WORK)) {
		ctrls->Select(NULL);
		return true;
	}

	/* Put the event to controls first */
	if (ctrls->PutEvent(e)) return true;

	/* Get the model rotation */
	if (model) mr = model->GetRot();

	/* Window-wide event */
	switch (e->t) {
	case GUIEV_KEYPRESS:
		/* Standard key decoding */
		switch (e->k) {
		/* Standard GUI shortcuts */
		case GUI_DEFCLOSE: will_close = true; return true;
		case '\t': ctrls->Rotate(); break;
		default:
			/* Window-specific controls */
			switch (binder->DecodeKey(e->k)) {
			case OBJEDIT_WNDXP: ResizeWnd(g_w+1,g_h); break;
			case OBJEDIT_WNDXN: ResizeWnd(g_w-1,g_h); break;
			case OBJEDIT_WNDYP: ResizeWnd(g_w,g_h+1); break;
			case OBJEDIT_WNDYN: ResizeWnd(g_w,g_h-1); break;

			case OBJEDIT_ZOOMP:
				campos.Y += 1;
				lvr->SetPosition(campos);
				break;

			case OBJEDIT_ZOOMM:
				campos.Y -= 1;
				lvr->SetPosition(campos);
				break;

			case OBJEDIT_MROTXP: mr.X += 2; break;
			case OBJEDIT_MROTXN: mr.X -= 2; break;
			case OBJEDIT_MROTYP: mr.Y += 2; break;
			case OBJEDIT_MROTYN: mr.Y -= 2; break;
			case OBJEDIT_MROTZP: mr.Z += 2; break;
			case OBJEDIT_MROTZN: mr.Z -= 2; break;
			case OBJEDIT_MROTRST: mr = vector3di(); break;
			}
		}
		if (model) model->SetRot(mr);
		Retrace();
		return true;

	case GUIEV_CTLBACK:
		switch (e->b.t) {
		case GUIFB_EDITOK:
			if (e->b.ctl == e_fname) {
				//file name changed
				if (model)
					pipe->UnloadModel(model);
				if (e_fname->GetText().empty()) {
					fname = NULL;
					model = NULL;
				} else {
					fname = e_fname->GetText().c_str();
					model = pipe->LoadModel(fname,vector3di(),vector3di());
					if (model) {
						campos = model->GetPos().ToReal();
						Retrace();
					}
					dbg_print("[MODEDIT]: model '%s' loaded to %p",fname,model);
				}
			}
			break;

		default: break;
		}
		break;

	case GUIEV_RESIZE:
		UpdateSize();
		ResizeWnd(g_w,g_h);
		return false; //don't consume resize event!

	default: break;
	}

	/* That's not our event, pass thru */
	return false;
}
