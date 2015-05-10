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

#include <math.h>
#include "CGUIControls.h"

using namespace std;

#define BORDER_WIDTH 2
#define BORDER_HEIGHT 2


CurseGUITable::CurseGUITable(CurseGUICtrlHolder* p, int x, int y, int w, int rows, int col, int wdth) :
		CurseGUIControl(p,x,y)
{
	showt = false;
	g_col = col;
	g_rows = rows;
	auto_height = 1;
	g_wdth = wdth;
	g_hght = 1;
	cur_x = cur_y = 0;


	tbl.resize(g_rows);
	for(size_t i = 0; i < tbl.size(); ++i) {
		for(int j = 0; j < g_col; ++j) {
			tbl[i].push_back("");
		}
	}
}

void CurseGUITable::SetData(string data, int r, int c)
{
	//TODO: check bounds of 'r' and 'c' !
//	if((r > 0) && (r < g_rows) && (c > 0) && (c < g_col))
	tbl[r][c] = data;
}

void CurseGUITable::SetWidth(int width)
{
	//TODO
}

int CurseGUITable::GetHeightRow(int r)
{
	//TODO
	size_t h,tmp;
	h = 0;
	for(size_t i = 0; i < tbl.at(r).size(); ++i) {
		tmp = (int)ceil((float)(tbl.at(r).at(i).size()) / (float)g_wdth);
//		printf("%u  ",tmp);
		if (h < tmp) h = tmp;
	}

	return (int)h;
}

void CurseGUITable::DrawCell(WINDOW* wd, int r, int c)
{
	char ch = ' ';
	bool borderx,bordery;
	int w, h, x, y;

	string str = tbl.at(r).at(c);
	//Calculate height cell
	g_hght = GetHeightRow(r);

	w = g_wdth + BORDER_WIDTH;
	h = g_hght + BORDER_HEIGHT;
	x = g_x + cur_x;
	y = g_y + cur_y;

	int lx = 0;

	for(int i = y; i < y+h; ++i) {
		bordery = ((i == y) || (i == y+h-1));

		for(int j = x; j < x+w; ++j) {
			borderx = (j == x || j == x+w-1);

			if(bordery) {
				ch = (borderx)? '+':'-';
			} else {
				if (borderx) ch = '|';
				else {
					//TODO print text
					ch = ' ';
					if(!str.empty()) {
						if(lx < (int)str.size())
							ch = str.at(lx++);
					}
				}
			}

			//Make table outline bold, if the table is selected
			if ((borderx || bordery) && selected) wattrset(wd,A_BOLD);

			mvwaddch(wd, i, j, ch);
			ch = ' ';

			//restore attribute
			wattrset(wd,A_NORMAL);
		}
	}
}

void CurseGUITable::Update()
{
	int i,j;
	WINDOW* wd = wnd->GetWindow();

	//draw rows and columns
	for(i = 0, cur_y = 0; i < g_rows; ++i) {
		for(j = 0, cur_x = 0; j < g_col; ++j) {
			DrawCell(wd, i, j);
			cur_x += g_wdth + BORDER_WIDTH / 2;
		}
		cur_y += GetHeightRow(i) + BORDER_HEIGHT / 2;
	}
}

bool CurseGUITable::PutEvent(SGUIEvent* e)
{
	switch (e->t) {
	case GUIEV_KEYPRESS:
		if (!selected) return false;

		switch (e->k) {
		case KEY_UP:
		case KEY_DOWN:
		case KEY_LEFT:
		case KEY_RIGHT:
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
