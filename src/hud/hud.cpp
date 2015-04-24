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

#include "hud.h"

using namespace std;

HUD::HUD(CurseGUI* guiptr)
{
	gui = guiptr;

	//add some overlays
	Spawn(0,0,"FPS = 0"); //top
	Spawn(0,gui->GetHeight()-2,"Testing bottom overlay"); //bottom
}

HUD::~HUD()
{
	vector<CurseGUIOverlay*>::iterator it;

	//remove overlays (in case we reset HUD or something)
	for (it = overlays.begin(); it != overlays.end(); ++it)
		gui->RmWindow((*it));
	overlays.clear();
}

void HUD::Spawn(int x, int y, const char* txt)
{
	CurseGUIOverlay* ptr;
	ptr = new CurseGUIOverlay(gui,x,y);
	overlays.push_back(ptr);
	gui->AddWindow(ptr);
	ptr->PutString(txt);
}

void HUD::UpdateFPS(uli fps)
{
	/* Begemot, do something here!
	 * we need to show just one number, so need something like Flush() method,
	 * which will eventually clear overlay' log (prevent creating memory leak)
	 * Then, print() method will be great to pass some formatted information.
	 * Use dbg_print() as example of very simple implementation.
	 * */
	//overlays[0]->PutString( bla -bla-bla
}
