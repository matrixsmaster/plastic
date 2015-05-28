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

using namespace std;

HUD::HUD(CurseGUI* guiptr)
{
	int w, h;
	gui = guiptr;

	st_gp.X = 0; st_gp.Y = 0; st_gp.Z = 0;
	st_lp.X = 0; st_lp.Y = 0; st_lp.Z = 0;

	w = gui->GetWidth();
	h = gui->GetHeight();

	/*add some overlays */

	//fps overlay
	Spawn(0,0, 10, 1, false, "fps = 0");

	//bottom log overlay
	Spawn(0,(h-(h/5)), (w/3), (h/5), true,
			"Testing ovaerlay bla bla bla yeah! Alice in wonderland. Cynic.");

	//status overlay on top
	Spawn((w/5), 0, (w/5), STAT_OVRL_HEIGHT, false, "");

	//map overlay
	Spawn((w-(w/4)), 0, (w/4), (h/4), false, "     !!MAAAP!!");

	//bottom state overlay
	Spawn((w/3), (h-1), w/*-(w/3))*/, 1, false, " STATE string");
	SetAlpha(OVRL_STBTM, 0);
}

HUD::~HUD()
{
	vector<CurseGUIOverlay*>::iterator it;

	//remove overlays (in case we reset HUD or something)
	for (it = overlays.begin(); it != overlays.end(); ++it)
		gui->RmWindow((*it));
	overlays.clear();
}

void HUD::Spawn(int x, int y, int w, int h, bool logging, const char* txt)
{
	CurseGUIOverlay* ptr;
	ptr = new CurseGUIOverlay(gui,x,y, w, h, logging);
	overlays.push_back(ptr);
	gui->AddWindow(ptr);
	ptr->PutString(txt);
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

void HUD::UpdateFPS(uli fps)
{
	string str;

	if(overlays.empty()) return;
	ClearLog(OVRL_FPS);
	str = "fps = ";
	str += intToString(fps);
	PutString(OVRL_FPS, str);
}


void HUD::PutStrToLog(const char* str)
{
	PutString(OVRL_LOG, str);
}

void HUD::UpdateStatusOvrl()
{
	string str;

	ClearLog(OVRL_ST);
	str = "GPos [";
	str += intToString(st_gp.X);
	str += " ";
	str += intToString(st_gp.Y);
	str += " ";
	str += intToString(st_gp.Z);
	str += "]";
	PutString(OVRL_ST, str);
	str = "LPos [";
	str += intToString(st_lp.X);
	str += " ";
	str += intToString(st_lp.Y);
	str += " ";
	str += intToString(st_lp.Z);
	str += "]";
	PutString(OVRL_ST, str);
	str = "";
	PutString(OVRL_ST, str);
}

void HUD::DrawMap()
{
	//TODO
}

void HUD::UpdateState(string str)
{
	string s = "STATE: ";
	s += str;
	PutString(OVRL_STBTM, s);
}

void HUD::SetAlpha(HUDOverlayType t, float a)
{
	if(overlays.empty()) return;
	overlays[t]->SetAlpha(a);
}

void HUD::SetBckgrMask(SGUIPixel* pxl)
{
	//TODO
}
