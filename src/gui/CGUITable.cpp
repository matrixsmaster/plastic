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


CurseGUITable::CurseGUITable(CurseGUICtrlHolder* p, int x, int y, int rows, int col, int wcell, int htable) :
		CurseGUIControl(p,x,y)
{
	g_col = col;
	g_rows = rows;
	g_wcell = wcell;
	g_hcell = 1;
	g_htable = htable;
	g_wtable = (g_wcell + BORDER_WIDTH/2) * g_col + 2;
	cur_x = cur_y = 0;
	scrolly = scrollx = 0;
	g_ly = 0;

	tbl.resize(g_rows);
	for(size_t i = 0; i < tbl.size(); ++i) {
		for(int j = 0; j < g_col; ++j) {
			tbl[i].push_back("");
		}
	}
}

void CurseGUITable::SetData(string data, int r, int c)
{
	if((r >= 0) && (r < g_rows) && (c >= 0) && (c < g_col))
		tbl[r][c] = data;
}

void CurseGUITable::SetWidth(int width)
{
	//TODO
}

int CurseGUITable::GetTableHeight()
{
	int tmp,i;

	tmp = 0;
	for(i = 0; i < g_rows; ++i) {
		tmp += GetRowHeight(i);
	}
	return tmp;
}

int CurseGUITable::GetRowHeight(int r)
{
	size_t h,tmp,i;
	h = 0;
	for(i = 0; i < tbl.at(r).size(); ++i) {
		tmp = (int)ceil((float)(tbl.at(r).at(i).size()) / (float)g_wcell);
		if (h < tmp) h = tmp;
	}

	return (int)h;
}

int CurseGUITable::DrawCell(WINDOW* wd, int r, int c)
{
	char ch = ' ';
	bool borderx,bordery;
	int w, h, x, y;
	int lx;

	string str = tbl.at(r).at(c);
	//Calculate cell height
	g_hcell = GetRowHeight(r);

	w = g_wcell + BORDER_WIDTH;
	h = g_hcell + BORDER_HEIGHT;
	x = g_x + cur_x;
	y = g_y + cur_y - scrolly;

	lx = g_ly*(g_wcell - (BORDER_WIDTH / 2));

	for(int i = y; i < (y + h); ++i) {
		bordery = ((i == y) || (i == y + h - 1));

		//cut off invisible parts
		if(i < g_y) {
			if(!bordery)
				lx += g_wcell;
			continue;
		}
		if(i > (g_y + g_htable)) continue;

		for(int j = x; j < (x + w); ++j) {
			borderx = ((j == x) || (j == x + w - 1));

			if(bordery) {
				ch = (borderx)? '+':'-';
			} else {
				if (borderx) ch = '|';
				else {
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
	return 0;
}


void CurseGUITable::Update()
{
	int i,j,th,step;
	int h;
	WINDOW* wd = wnd->GetWindow();


	if (scrolly < 0) scrolly = 0;

	th = GetTableHeight();
	if (th < g_htable) {
		scrolly = 0;
		h = g_htable + 3;
	}
	else h = th - g_htable + 3;

	if (scrolly > h) scrolly = h;

	//draw table
	for(i = 0, cur_y = 0; i < g_rows; ++i) {
		for(j = 0, cur_x = 0; j < g_col; ++j) {
			DrawCell(wd, i, j);
			cur_x += g_wcell + BORDER_WIDTH / 2;
		}
		cur_y += GetRowHeight(i) + BORDER_HEIGHT / 2;
	}

	//rows scrollbar
	step = th / g_htable;
	if(step > 0)
		mvwaddch(wd, (g_y+(scrolly/step)), g_wtable, '|');

	//columns scrollbar


}

bool CurseGUITable::PutEvent(SGUIEvent* e)
{
	switch (e->t) {
	case GUIEV_KEYPRESS:
		if (!selected) return false;

		switch (e->k) {
		case KEY_UP:
			scrolly--;
			return true;
		case KEY_DOWN:
			scrolly++;
			return true;
		case KEY_LEFT:
			scrollx++;
			return true;
		case KEY_RIGHT:
			scrollx--;
			return true;
		case 'x':
			scrolly--;
			break;
		case 'c':
			scrolly++;
			break;
		default: break;
		}
		break;

		case GUIEV_MOUSE:
			break;

		default: break;
	}
	return false;
}
