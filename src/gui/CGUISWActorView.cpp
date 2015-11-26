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
		CurseGUIWnd(scrn,1,1,ACTRVIEWW,4)
{
	type = GUIWT_OTHER;
	name = WNDNAM_ACTVIEW;
	showname = true;

	actor = actr;

	SetAutoAlloc(true);
	ResizeWnd();
}

#define AV_ADDLABELMACRO(Name,Val,Max,Line,Shift) { \
		snprintf(buf,sizeof(buf),"%s: %d (%d)",Name,Val,Max); \
		lbl = new CurseGUILabel(ctrls,ACTPORTRAITW+Shift,Line,30,1,buf); \
		if (Val < Max) { \
			cfmt.fg = low; \
		} else if (Val > Max) { \
			cfmt.fg = hig; \
		} else { \
			cfmt.fg = nor; \
		} \
		lbl->SetFormat(cfmt); }

void CurseGUIActorViewWnd::ResizeWnd()
{
	int w,h,x,y;
	char buf[ACTRVIEWW*3];
	CurseGUILabel* lbl;
	SPAAttrib atrs = actor->GetAttributes();
	SPABase bs = actor->GetStats(false);
	SPABase cr = actor->GetStats(true);
	SCTriple low = ACTRVIEWLACOL;
	SCTriple nor = ACTRVIEWNACOL;
	SCTriple hig = ACTRVIEWHACOL;
	SGUIPixel cfmt;
	memset(&cfmt,0,sizeof(cfmt));

	w = ACTRVIEWW;
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
	if (actor->GetPortrait())
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
	AV_ADDLABELMACRO("HP",cr.HP,bs.HP,1,4);
	AV_ADDLABELMACRO("Qual",cr.Qual,bs.Qual,2,2);

	//stats (phys)
	AV_ADDLABELMACRO("CC",cr.CC,bs.CC,3,4);
	AV_ADDLABELMACRO("Spd",cr.Spd,bs.Spd,4,3);
	AV_ADDLABELMACRO("Str",cr.Str,bs.Str,5,3);
	AV_ADDLABELMACRO("Eff",cr.Eff,bs.Eff,6,3);
	AV_ADDLABELMACRO("RS",cr.RS,bs.RS,7,4);
	AV_ADDLABELMACRO("Acc",cr.Acc,bs.Acc,8,3);

	//stats (brain)
	AV_ADDLABELMACRO("Eng",cr.MR.Eng,bs.MR.Eng,10,3);
	AV_ADDLABELMACRO("Spch",cr.MR.Spch,bs.MR.Spch,11,2);
	AV_ADDLABELMACRO("Brv",cr.MR.Brv,bs.MR.Brv,12,3);
	if (atrs.female)
		AV_ADDLABELMACRO("Bety",cr.Chr,bs.Chr,13,2)
	else
		AV_ADDLABELMACRO("Char",cr.Chr,bs.Chr,13,2)
	AV_ADDLABELMACRO("Trd",cr.MR.Trd,bs.MR.Trd,14,3);

	//stats (battle)
	AV_ADDLABELMACRO("AP",cr.AP,bs.AP,16,4);
	AV_ADDLABELMACRO("DT",cr.DT,bs.DT,17,4);
	AV_ADDLABELMACRO("DM",cr.DM,bs.DM,18,4);
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
