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

#include <stdlib.h>
#include <string>
#include "CGUISpecWnd.h"
#include "renderpool.h"

using namespace std;


CurseGUIRenderConfWnd::CurseGUIRenderConfWnd(CurseGUI* scrn, RenderPool* ppool) :
		CurseGUIWnd(scrn,8,8,26,9)
{
	type = GUIWT_OTHER;
	name = "LVR config";
	showname = true;

	pool = ppool;
	scale = pool->GetScale();
	fov = pool->GetFOV();
	far = pool->GetFarDist();
	ppset = pool->GetPostprocess();

	//Create labels
	new CurseGUILabel(ctrls,1,1,5,1,"Scale");
	new CurseGUILabel(ctrls,1,2,5,1,"FOV H");
	new CurseGUILabel(ctrls,1,3,5,1,"FOV V");
	new CurseGUILabel(ctrls,1,4,5,1,"Far");
	new CurseGUILabel(ctrls,14,1,5,1,"Fog S");
	new CurseGUILabel(ctrls,14,2,5,1,"Fog C");
	new CurseGUILabel(ctrls,24,2,1,1,"R");
	new CurseGUILabel(ctrls,24,3,1,1,"G");
	new CurseGUILabel(ctrls,24,4,1,1,"B");
	new CurseGUILabel(ctrls,14,5,5,1,"Noise");

	//Create edits
	e_scale = new CurseGUIEditBox(ctrls,6,1,6,"0");
	e_fovx = new CurseGUIEditBox(ctrls,6,2,6,"0");
	e_fovy = new CurseGUIEditBox(ctrls,6,3,6,"0");
	e_far = new CurseGUIEditBox(ctrls,6,4,6,"0");
	e_fog = new CurseGUIEditBox(ctrls,19,1,6,"0");
	e_fogr = new CurseGUIEditBox(ctrls,19,2,5,"0");
	e_fogg = new CurseGUIEditBox(ctrls,19,3,5,"0");
	e_fogb = new CurseGUIEditBox(ctrls,19,4,5,"0");
	e_noise = new CurseGUIEditBox(ctrls,19,5,6,"0");

	//Create buttons
	b_apply = new CurseGUIButton(ctrls,1,7,9,"Apply");
	b_reset = new CurseGUIButton(ctrls,13,7,9,"Reset");

	//Fill them
	Fill();
}

void CurseGUIRenderConfWnd::Fill()
{
	char buf[10];

	snprintf(buf,sizeof(buf),"%.2f",scale);
	e_scale->SetText(string(buf));

	snprintf(buf,sizeof(buf),"%d",(int)fov.X);
	e_fovx->SetText(string(buf));
	snprintf(buf,sizeof(buf),"%d",(int)fov.Y);
	e_fovy->SetText(string(buf));

	snprintf(buf,sizeof(buf),"%d",far);
	e_far->SetText(string(buf));

	snprintf(buf,sizeof(buf),"%d",ppset.fog_dist);
	e_fog->SetText(string(buf));

	snprintf(buf,sizeof(buf),"%hd",ppset.fog_col.r);
	e_fogr->SetText(string(buf));
	snprintf(buf,sizeof(buf),"%hd",ppset.fog_col.g);
	e_fogg->SetText(string(buf));
	snprintf(buf,sizeof(buf),"%hd",ppset.fog_col.b);
	e_fogb->SetText(string(buf));

	snprintf(buf,sizeof(buf),"%d",ppset.noise);
	e_noise->SetText(string(buf));
}

void CurseGUIRenderConfWnd::Scan()
{
	int tmp;

	sscanf((e_scale->GetText().c_str()),"%f",&scale);

	sscanf((e_fovx->GetText().c_str()),"%d",&tmp);
	fov.X = tmp;
	sscanf((e_fovy->GetText().c_str()),"%d",&tmp);
	fov.Y = tmp;

	sscanf((e_far->GetText().c_str()),"%d",&far);
	sscanf((e_fog->GetText().c_str()),"%d",&ppset.fog_dist);
	sscanf((e_fogr->GetText().c_str()),"%hd",&(ppset.fog_col.r));
	sscanf((e_fogg->GetText().c_str()),"%hd",&(ppset.fog_col.g));
	sscanf((e_fogb->GetText().c_str()),"%hd",&(ppset.fog_col.b));
	sscanf((e_noise->GetText().c_str()),"%d",&(ppset.noise));
}

void CurseGUIRenderConfWnd::Apply()
{
	pool->SetScale(scale);
	pool->SetFOV(fov);
	pool->SetFarDist(far);
	pool->SetPostprocess(ppset);
}

void CurseGUIRenderConfWnd::Reset()
{
	SLVRPostProcess temp = DEFPOSTPROC;

	scale = DEFSCALE;
	fov.X = DEFFOVX;
	fov.Y = DEFFOVY;
	far = DEFFARPLANE;
	ppset = temp;
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
		case GUI_DEFCLOSE: will_close = true; break;
		case '\t': ctrls->Rotate(); break;
		}
		return true;

	case GUIEV_RESIZE:
		UpdateSize();
		return false; //don't consume resize event!

	case GUIEV_CTLBACK:
		switch (e->b.t) {
		case GUIFB_SWITCHED:
			//check button
			if (e->b.ctl == b_apply) {
				Scan();
				Apply();
			} else if (e->b.ctl == b_reset) {
				Reset();
				Apply();
			}
			break;

		case GUIFB_EDITOK:
			Scan();
			Apply();
			break;

		default: break;
		}
		break;

	default: break;
	}

	/* That's not our event, pass thru */
	return false;
}
