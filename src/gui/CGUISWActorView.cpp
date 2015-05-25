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
#include "actor.h"
#include "actorhelpers.h"


CurseGUIActorViewWnd::CurseGUIActorViewWnd(CurseGUI* scrn, PlasticActor* actr) :
		CurseGUIWnd(scrn,1,1,2,2)
{
	type = GUIWT_OTHER;
	name = "Actor View";
	showname = true;

	actor = actr;

	SetAutoAlloc(true);
	ResizeWnd();
}

void CurseGUIActorViewWnd::ResizeWnd()
{
	int w,h,x,y;
//	char buf[ACTRVIEWMINW+1];
	SPAAttrib atrs = actor->GetAttributes();
//	SPABase bs = actor->GetStats(false);
//	SPABase cr = actor->GetStats(true);

	w = parent->GetWidth() / 2;
	if (w < ACTRVIEWMINW) w = ACTRVIEWMINW;
	h = ACTPORTRAITH + 7;

	//upper-right corner
	x = parent->GetWidth() - w;
	y = 0;

	Move(x,y);
	Resize(w,h);

	//reset holder
	if (ctrls) delete ctrls;
	ctrls = new CurseGUICtrlHolder(this);

	//create controls
	portrait = new CurseGUIPicture(ctrls,1,1,ACTPORTRAITW,ACTPORTRAITH);
	portrait->SetAutoAlloc(true);
	portrait->SetPicture(actor->GetPortrait());

	new CurseGUILabel(ctrls,3,ACTPORTRAITH+1,5,"Type:");
	new CurseGUILabel(ctrls,9,ACTPORTRAITH+1,6,((actor->IsNPC())? "NPC":"Player"));
	new CurseGUILabel(ctrls,3,ACTPORTRAITH+2,5,"Name:");
	new CurseGUILabel(ctrls,9,ACTPORTRAITH+2,ACTPORTRAITW-9,atrs.name);
	new CurseGUILabel(ctrls,1,ACTPORTRAITH+3,7,"Gender:");
	new CurseGUILabel(ctrls,9,ACTPORTRAITH+3,7,((atrs.female)? "Female":"Male"));
	new CurseGUILabel(ctrls,2,ACTPORTRAITH+4,6,"Class:");
	new CurseGUILabel(ctrls,9,ACTPORTRAITH+4,ACTPORTRAITW-9,paclass_to_str[atrs.cls].s);
	new CurseGUILabel(ctrls,3,ACTPORTRAITH+5,5,"Body:");
	new CurseGUILabel(ctrls,9,ACTPORTRAITH+5,ACTPORTRAITW-9,BodyTypeToStr(atrs.body));

	new CurseGUILabel(ctrls,ACTPORTRAITW+3,1,3,"HP:");
}

bool CurseGUIActorViewWnd::PutEvent(SGUIEvent* e)
{
	if (will_close) return false;

	switch (e->t) {
	case GUIEV_KEYPRESS:
		switch (e->k) {
		case GUI_DEFCLOSE: will_close = true; return true;
		}
		return false;

	case GUIEV_RESIZE:
		ResizeWnd();
		UpdateSize();
		return false; //don't consume resize event!

	default: break;
	}
	return false;
}
