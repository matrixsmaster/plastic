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

/* Implementation file of ProgressBar control class */

#include <sstream>
#include "CGUIControls.h"

using namespace std;

CurseGUIProgrBar::CurseGUIProgrBar(CurseGUICtrlHolder* p, int x, int y, int w, int min, int max) :
		CurseGUIControl(p,x,y)
{
	typ = GUICL_PROGRBAR;
	g_w = w;
	g_min = min;
	g_max = max;
	step = 1;
	pos = min;
	showprc = false;

	//init black on white as default foreground
	foregr.bg.r = 1000; foregr.bg.g = 1000; foregr.bg.b = 1000;
	foregr.fg.r = 0; foregr.fg.g = 0; foregr.fg.b = 0;
	foregr.sym = ' ';
}

void CurseGUIProgrBar::SetNumSteps(int n)
{
	step = (g_max - g_min) / ((n == 0)? 1:n);
}

void CurseGUIProgrBar::Step()
{
	pos += step;
	if (pos > g_max) pos = g_min;
	if (pos < g_min) pos = g_max;
}

void CurseGUIProgrBar::SetValue(int v)
{
	pos = v;
	if (pos > g_max) pos = g_min;
	if (pos < g_min) pos = g_max;
}

void CurseGUIProgrBar::Update()
{
	int i,p,l,r;
	short f,b;
	WINDOW* wd = wnd->GetWindow();
	ostringstream ss;

	//get foreground and background pair
	f = wnd->GetColorManager()->CheckPair(&foregr);
	b = wnd->GetColorManager()->CheckPair(&fmt);

	//calculate percent and length
	p = (g_max - g_min) * pos / 100;
	l = (g_w - 2) * p / 100;

	//draw the bar
	wcolor_set(wd,f,NULL);
	for (i = 0; i < g_w - 2; i++) {
		if (i < l)
			mvwaddch(wd,g_y,g_x+i+1,'#');
		else {
			wcolor_set(wd,b,NULL);
			mvwaddch(wd,g_y,g_x+i+1,' ');
		}
	}
	wcolor_set(wd,b,NULL);

	//draw percent
	if (showprc) {
		ss << p;
		ss << "%";
		r = g_w / 2 - ss.str().size() / 2;
		mvwaddstr(wd,g_y,g_x+r,ss.str().c_str());
	}

	//add borders
	mvwaddch(wd,g_y,g_x,'{');
	mvwaddch(wd,g_y,g_x+g_w-1,'}');
}
