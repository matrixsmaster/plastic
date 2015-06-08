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

#include <sstream>
#include "hud.h"
#include "CGUIControls.h"

using namespace std;

HUD::HUD(CurseGUI* guiptr)
{
	int w, h;
	gui = guiptr;

	memset(&plt, 0, sizeof(plt));

	w = gui->GetWidth();
	h = gui->GetHeight();

	pcname = "";

	/*add some overlays */

	//Fps overlay
	Spawn(0,0, 8, 1, true);

	//Bottom log overlay
	Spawn(0,(h-(h/5)), (w/3), (h/5), true);

	//Status overlay on top
	Spawn((w/5), 0, (w/5), STAT_OVRL_HEIGHT, false);
	SetAlpha(OVRL_STATE, 0);

	//Map overlay
	Spawn((w-(w/4)), 0, (w/4), (h/4), false);
	SetAlpha(OVRL_MAP, 0);

	//Status bar overlay
	Spawn((w/3), (h-1), w-(w/3)-10, 1, true);
	SetAlpha(OVRL_STBTM, 0);

	//Time overlay
	Spawn(0,1, 8, 3, true);

	//Charge and HP overlay
	Spawn((w-10),(h-2), 10, 2, false);
	SetAlpha(OVRL_CHRG_HP, 0);

	InitControls();
}

HUD::~HUD()
{
	vector<CurseGUIOverlay*>::iterator it;

	//remove overlays (in case we reset HUD or something)
	for (it = overlays.begin(); it != overlays.end(); ++it)
		gui->RmWindow((*it));
	overlays.clear();
}

void HUD::Spawn(int x, int y, int w, int h, bool logging)
{
	CurseGUIOverlay* ptr;
	ptr = new CurseGUIOverlay(gui,x,y, w, h, logging);
	overlays.push_back(ptr);
	gui->AddWindow(ptr);
}

void HUD::InitControls()
{
	CurseGUIProgrBar* tmp;
	SGUIPixel fmt;

	//Create Charge meter
	memset(&fmt,0,sizeof(SGUIPixel));
	fmt.fg.r = 100; fmt.fg.g = 100; fmt.fg.b = 900; //light blue (initially as foreground)
	tmp = new CurseGUIProgrBar(overlays[OVRL_CHRG_HP]->GetControls(), 0, 0, 10, 0, 100); //charge
	tmp->SetShowPercent(true); //show percent of charge
	tmp->SetFormat(fmt);
	fmt.bg = fmt.fg; //swap bg/fg
	memset(&(fmt.fg),0,sizeof(SCTriple));
	tmp->SetForegrFormat(fmt);

	//Create health meter
	memset(&fmt,0,sizeof(SGUIPixel));
	fmt.fg.r = 1000; fmt.fg.g = 100; fmt.fg.b = 100; //soft red (initially as foreground)
	tmp = new CurseGUIProgrBar(overlays[OVRL_CHRG_HP]->GetControls(), 0, 1, 10, 0, 100); //HP
	tmp->SetFormat(fmt);
	fmt.bg = fmt.fg; //swap bg/fg
	memset(&(fmt.fg),0,sizeof(SCTriple));
	tmp->SetForegrFormat(fmt);

	//Create temporary control widget
	new CurseGUILabel(overlays[OVRL_STATE]->GetControls(), 0, 0, gui->GetWidth()/5, 1, "GPos [ 0 0 0 ]");
	new CurseGUILabel(overlays[OVRL_STATE]->GetControls(), 0, 1, gui->GetWidth()/5, 1, "LPos [ 0 0 0 ]");

	//TODO add all controls
}

string HUD::intToString(int n)
{
	ostringstream ss;
	ss << n;
	return ss.str();
}

void HUD::ClearLog(HUDOverlayType t)
{
	if(overlays.empty()) return;
	overlays[t]->ClearLog();
}

void HUD::PutString(HUDOverlayType t, string str)
{
	if(overlays.empty()) return;
	overlays[t]->PutString(str);
}

int HUD::GetMapWidth()
{
	if(overlays.empty()) return 0;

	return overlays[OVRL_MAP]->GetWidth();
}

int HUD::GetMapHeight()
{
	if(overlays.empty()) return 0;

	return overlays[OVRL_MAP]->GetHeight();
}

void HUD::SetMap(SGUIPixel *pxl, int l)
{
	if(overlays.empty()) return;

	overlays[OVRL_MAP]->SetBackgroundData(pxl, l);
}

void HUD::SetFPS(uli fps)
{
	string str;

	ClearLog(OVRL_FPS);
	str = "FPS=";
	str += intToString(fps);
	PutString(OVRL_FPS, str);
}

void HUD::PutStrToLog(const char* str)
{
	if (!overlays.empty()) PutString(OVRL_LOG, str);
}

void HUD::SetCharge(int v, int m)
{
	int tmp;
	CurseGUIProgrBar* bar;

	if(overlays.empty()) return;

	bar = (CurseGUIProgrBar*)overlays[OVRL_CHRG_HP]->GetControls()->GetControl(0, GUICL_PROGRBAR);
	if (!bar) return;
	tmp = v * 100 / m;
	bar->SetValue(tmp);
}

void HUD::SetHP(int v, int m)
{
	int tmp;
	CurseGUIProgrBar* bar;

	if(overlays.empty()) return;

	bar = (CurseGUIProgrBar*)overlays[OVRL_CHRG_HP]->GetControls()->GetControl(1, GUICL_PROGRBAR);
	if (!bar) return;
	tmp = v * 100 / m;
	bar->SetValue(tmp);
}

void HUD::SetState(string str)
{
	string s = pcname + str;
	PutString(OVRL_STBTM, s);
}

void HUD::SetState(const char* str)
{
	SetState(string(str));
}

void HUD::UpdateClock()
{
	char tmp[9];

	ClearLog(OVRL_CLOCK);
	//name of day
	snprintf(tmp, 8, "%s", day_to_string[plt->dow].s);
	PutString(OVRL_CLOCK, string(tmp));
	//date
	snprintf(tmp, 9, "%02d/%01d/%03d", (plt->day+1), (plt->month+1), plt->year);
	PutString(OVRL_CLOCK, string(tmp));
	//time
	snprintf(tmp, 9, "%02d:%02d:%02d", plt->hr, plt->mn, plt->sc);
	PutString(OVRL_CLOCK, string(tmp));
}

void HUD::SetAlpha(HUDOverlayType t, float a)
{
	if(overlays.empty()) return;
	overlays[t]->SetAlpha(a);
}

void HUD::SetPTime(PlasticTime* t)
{
	plt = t;
}

void HUD::SetLabelCaption(char *buf, int s, int l, int x, int y, int z)
{
	CurseGUILabel* lbl;

	if(overlays.empty()) return;

	lbl = (CurseGUILabel*)overlays[OVRL_STATE]->GetControls()->GetControl(l, GUICL_LABEL);
	if (!lbl) return;
	snprintf(buf+1, s, "Pos [ %d %d %d ]", x, y, z);
	lbl->SetCaption(string(buf));
}

void HUD::SetGPos(vector3di gp)
{
	char tmp[20];
	tmp[0] = 'G';
	SetLabelCaption(tmp, 20, 0, gp.X, gp.Y, gp.Z);
}

void HUD::SetLPos(vector3di lp)
{
	char tmp[20];
	tmp[0] = 'L';
	SetLabelCaption(tmp, 20, 1, lp.X, lp.Y, lp.Z);
}

void HUD::ShowMisc(bool s)
{
	if (!overlays.empty())
		overlays[OVRL_STATE]->SetHidden(s);
}

void HUD::ToggleMisc()
{
	if (!overlays.empty())
		overlays[OVRL_STATE]->SetHidden(!overlays[OVRL_STATE]->IsHidden());
}

void HUD::SetPCName(const char* n)
{
	pcname = string(n) + ": ";
}
