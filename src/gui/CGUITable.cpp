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

/* Implementation file of Table control class */

#include "CGUIControls.h"


#define WIDTH 5

using namespace std;

CurseGUITable::CurseGUITable(CurseGUICtrlHolder* p, int x, int y, int w, int rows, int col, int wdth) :
		CurseGUIControl(p,x,y)
{
	showt = false;
	g_col = col;
	g_rows = rows;
	auto_height = 1;
	g_wdth = wdth;
	g_hght = 1;

	//temporarily? like a deer? )
	tbl.resize(g_col);
	for(size_t i = 0; i < tbl.size(); ++i) {
		for(int j = 0; j < g_rows; ++j) {
			tbl[i].push_back("");
		}
	}
}

void CurseGUITable::SetData(string data, int r, int c)
{
	tbl[c][r] = data;
}

void CurseGUITable::SetWidth(int width)
{
	//TODO
}

void CurseGUITable::DrawCell(WINDOW* wd, int r, int c)
{
	char ch = ' ';
	bool border = true;
	int w = g_wdth+2;
	int h = g_hght + 2;
	int x = c*(w-1);
	int y = r*(h-1);

	//Calculate height cell
//	g_hght = data.size()/g_wdth;


	string str = tbl.at(c).at(r);

	//Draw table like a deer
	for(int i = y; i < y+h; ++i) {
		(i == y || i == y+h-1) ? border = true : border = false;

		for(int j = x; j < x+w; ++j) {

			if(border) {
				if(j == x || j == x+w-1) ch = '+';
				else ch = '-';

			} else {
				if((i == y+h-2 && j == x+w-1) || j == x) {
					ch = '|';
				} else {
					//TODO print text
					ch = ' ';
					if(!str.empty()) {
						if(j-1 < (int)str.size())
							ch = str.at(j-1);
					}
				}
			}
			mvwaddch(wd, i, j, ch);
			ch = ' ';
		}
	}
}

void CurseGUITable::Update()
{
	WINDOW* wd = wnd->GetWindow();

	//draw rows and columns
	for(int i = 0; i < g_rows; ++i) {
		for(int j = 0; j < g_col; ++j) {
			DrawCell(wd, i, j);
		}
	}
}

bool CurseGUITable::PutEvent(SGUIEvent* e)
{
	switch (e->t) {
	case GUIEV_KEYPRESS:
		if (!selected) return false;

		switch (e->k) {
		case 120:
			//TODO
			return true;
		default: break;
		}
		break;

		case GUIEV_MOUSE:
			break;

		default: break;
	}
	return false;
}
