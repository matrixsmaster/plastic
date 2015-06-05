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
#include "misconsts.h"


CurseGUIActorViewWnd::CurseGUIActorViewWnd(CurseGUI* scrn, PlasticActor* actr) :
		CurseGUIWnd(scrn,1,1,2,2)
{
	type = GUIWT_OTHER;
	name = WNDNAM_ACTVIEW;
	showname = true;

	actor = actr;

	SetAutoAlloc(true);
	ResizeWnd();
}

void CurseGUIActorViewWnd::ResizeWnd()
{
	int w,h,x,y;
	char buf[ACTRVIEWMINW*3];
	SPAAttrib atrs = actor->GetAttributes();
	SPABase bs = actor->GetStats(false);
	SPABase cr = actor->GetStats(true);

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

	//main attributes
	snprintf(buf,sizeof(buf),"  Type: %s\n  Name: %s\nGender: %s\n Class: %s\n  Body: %s",
			((actor->IsNPC())? "NPC":"Player"),
			atrs.name,
			((atrs.female)? "Female":"Male"),
			paclass_to_str[atrs.cls].s,
			BodyTypeToStr(atrs.body));
	new CurseGUILabel(ctrls,1,ACTPORTRAITH+1,ACTPORTRAITW,5,buf);

	//stats (main)
	snprintf(buf,sizeof(buf)," HP: %d (%d)",cr.HP,bs.HP);
	new CurseGUILabel(ctrls,ACTPORTRAITW+3,1,30,1,buf);

	snprintf(buf,sizeof(buf),"Qual: %d (%d)",cr.Qual,bs.Qual);
	new CurseGUILabel(ctrls,ACTPORTRAITW+2,2,30,1,buf);

	//stats (phys)
	snprintf(buf,sizeof(buf)," CC: %d (%d)",cr.CC,bs.CC);
	new CurseGUILabel(ctrls,ACTPORTRAITW+3,3,30,1,buf);

	snprintf(buf,sizeof(buf),"Spd: %d (%d)",cr.Spd,bs.Spd);
	new CurseGUILabel(ctrls,ACTPORTRAITW+3,4,30,1,buf);

	snprintf(buf,sizeof(buf),"Str: %d (%d)",cr.Str,bs.Str);
	new CurseGUILabel(ctrls,ACTPORTRAITW+3,5,30,1,buf);

	snprintf(buf,sizeof(buf),"Eff: %d (%d)",cr.Eff,bs.Eff);
	new CurseGUILabel(ctrls,ACTPORTRAITW+3,6,30,1,buf);

	snprintf(buf,sizeof(buf)," RS: %d (%d)",cr.RS,bs.RS);
	new CurseGUILabel(ctrls,ACTPORTRAITW+3,7,30,1,buf);

	snprintf(buf,sizeof(buf),"Acc: %d (%d)",cr.Acc,bs.Acc);
	new CurseGUILabel(ctrls,ACTPORTRAITW+3,8,30,1,buf);

	//stats (brain)
	snprintf(buf,sizeof(buf),"Eng: %d (%d)",cr.Eng,bs.Eng);
	new CurseGUILabel(ctrls,ACTPORTRAITW+3,10,30,1,buf);

	snprintf(buf,sizeof(buf),"Spch: %d (%d)",cr.Spch,bs.Spch);
	new CurseGUILabel(ctrls,ACTPORTRAITW+2,11,30,1,buf);

	snprintf(buf,sizeof(buf),"Brv: %d (%d)",cr.Brv,bs.Brv);
	new CurseGUILabel(ctrls,ACTPORTRAITW+3,12,30,1,buf);

	if (atrs.female)
		snprintf(buf,sizeof(buf),"Bety: %d (%d)",cr.Chr,bs.Chr);
	else
		snprintf(buf,sizeof(buf),"Char: %d (%d)",cr.Chr,bs.Chr);
	new CurseGUILabel(ctrls,ACTPORTRAITW+2,13,30,1,buf);

	snprintf(buf,sizeof(buf),"Trd: %d (%d)",cr.Trd,bs.Trd);
	new CurseGUILabel(ctrls,ACTPORTRAITW+3,14,30,1,buf);

	//stats (battle)
	snprintf(buf,sizeof(buf),"AP: %d (%d)",cr.AP,bs.AP);
	new CurseGUILabel(ctrls,ACTPORTRAITW+4,16,30,1,buf);

	snprintf(buf,sizeof(buf),"DT: %d (%d)",cr.DT,bs.DT);
	new CurseGUILabel(ctrls,ACTPORTRAITW+4,17,30,1,buf);

	snprintf(buf,sizeof(buf),"DM: %d (%d)",cr.DM,bs.DM);
	new CurseGUILabel(ctrls,ACTPORTRAITW+4,18,30,1,buf);
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
