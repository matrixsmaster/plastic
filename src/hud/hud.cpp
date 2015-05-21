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
	gui = guiptr;

	stats.fps = 0;
	stats.gp.x = 0; stats.gp.y = 0; stats.gp.z = 0;
	stats.lp.x = 0; stats.lp.y = 0; stats.lp.z = 0;

	//add some overlays
//	Spawn(0,0, gui->GetWidth()/4, 1, false, "fps = 0"); //top
	Spawn(0,gui->GetHeight()-gui->GetHeight()/4, gui->GetWidth()/4, gui->GetHeight()/4, true, "Testing bottom overlay"); //bottom
	Spawn(gui->GetWidth()/5, 0, gui->GetWidth()/5, STAT_OVRL_HEIGHT, false, "fps = 0");
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

void HUD::UpdateFPS(uli fps)
{
	if(overlays.empty()) return;

	stats.fps = fps;
	UpdateStatusOvrl();
}

void HUD::UpdateStatusOvrl()
{
	//TODO
	string str;

	if(overlays.size() > STAT_OVRL-1) {
		overlays[STAT_OVRL]->ClearLog();
		str = "fps = ";
		str += intToString(stats.fps);
		overlays[STAT_OVRL]->PutString(str);
		str = "GPos [";
		str += intToString(stats.gp.x);
		str += " ";
		str += intToString(stats.gp.y);
		str += " ";
		str += intToString(stats.gp.z);
		str += "]";
		overlays[STAT_OVRL]->PutString(str);
		str = "LPos [";
		str += intToString(stats.lp.x);
		str += " ";
		str += intToString(stats.lp.y);
		str += " ";
		str += intToString(stats.lp.z);
		str += "]";
		overlays[STAT_OVRL]->PutString(str);
	}
}

void HUD::PutStrBottom(const char* str)
{
	if(overlays.size() > 1) {
		overlays[BTM_OVRL]->PutString(str);
	}
}

bool HUD::GetTransparent()
{
	//TODO check focus
	if(overlays.size() >  0)
		return overlays[BTM_OVRL]->GetTransparent();
	return false;
}

void HUD::SetTransparent(bool t)
{
	//TODO check focus
	if(!overlays.empty())
		overlays[BTM_OVRL]->SetTransparent(t);
}

void HUD::SetBckgrMask(SGUIPixel* pxl)
{
	//TODO
}
