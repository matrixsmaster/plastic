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


CurseGUITable::CurseGUITable(CurseGUICtrlHolder* p, int x, int y, int rows, int col, int wcell, int htable, int wtable, bool header) :
		CurseGUIControl(p,x,y)
{
	typ = GUICL_TABLE;
	g_col = col;
	g_rows = rows;
	g_h = htable;
	g_w = wtable;
	cur_x = cur_y = 0;
	scrolly = scrollx = 0;
	g_header = header;

	//Create two-dimensional array data.
	tbl.resize(g_rows);
	for (size_t i = 0; i < tbl.size(); ++i) {
		for(int j = 0; j < g_col; ++j) {
			tbl[i].push_back("");
		}
	}

	//Fill an array of widths.
	for (int k = 0; k < g_col; ++k)
		clw.push_back(wcell);
}

void CurseGUITable::SetData(string data, int r, int c)
{
	//Check out of border.
	if ((r >= 0) && (r < g_rows) && (c >= 0) && (c < g_col))
		tbl[r][c] = data; //Set data.
}

void CurseGUITable::EraseData(int r, int c)
{
	//Check out of border.
	if ((r >= 0) && (r < g_rows) && (c >= 0) && (c < g_col))
		tbl[r][c] = ""; //Erase data.
}

void CurseGUITable::EraseTableData()
{
	//Erase all data from the table.
	for (int i = 0; i < (int)tbl.size(); ++i) {
		for (int j = 0; j < (int)tbl.at(i).size(); ++j)
			tbl.at(i).at(j) = " ";
	}
}

void CurseGUITable::SetColumnWidth(int c, int w)
{
	//Check out of border.
	if ((c < 0) || (c > (int)clw.size()-1)) return;

	clw[c] = w; //Set the column width.
}

void CurseGUITable::AddRow()
{
	vector<string> row;

	//Add columns in row.
	for (int i = 0; i < g_col; ++i) {
		row.push_back("");
	}
	//Add row.
	tbl.push_back(row);
	g_rows++;
}

void CurseGUITable::DelRow()
{
	if (tbl.empty()) return;

	//Delete last row.
	tbl.pop_back();
	g_rows--;
}

void CurseGUITable::AddColumn(int width)
{
	//Add cells.
	for (size_t i = 0; i < tbl.size(); ++i)
		tbl.at(i).push_back("");

	//Set the value of width for the column.
	clw.push_back(width);
	g_col++;
}

void CurseGUITable::DelColumn()
{
	if (tbl.empty()) return;


	for (size_t i = 0; i < tbl.size(); ++i) {
		if (!tbl.at(i).empty())
			//Delete column.
			tbl.at(i).pop_back();

		if (!clw.empty())
			//Delete the value of widths.
			clw.pop_back();
	}
	g_col--;
}

int CurseGUITable::GetTableHeight()
{
	int res,i;

	res = 1;
	//Calculation table height.
	for (i = 0; i < g_rows; ++i) {
		res += GetRowHeight(i) + 1;
	}
	return res;
}

int CurseGUITable::GetTableWidth()
{
	int res;
	vector<int>::iterator it;

	res = 0;

	//Calculation table width.
	for (it = clw.begin(); it != clw.end(); ++it)
		res += (*it) + BW / 2;

	return res + BW /2; //Returns table width and half-border.
}

int CurseGUITable::GetSumTableWidth(int c)
{
	int res, i;;

	res = 0;
	if ((int)clw.size() > c)
		//The width of the table to a given element.
		for (i = 0; i < c; ++i)
			res += clw.at(i) + BW / 2;

	return res;
}

int CurseGUITable::GetRowHeight(int r)
{
	size_t h,tmp,i;
	h = 0;

	for(i = 0; i < tbl.at(r).size(); ++i) {
		//The height of the line, depending on the amount of text and width of column.
		tmp = (int)ceil((float)(tbl.at(r).at(i).size()) / (float)clw.at(i));
		//Search maximum value of height.
		if (h < tmp) h = tmp;
	}

	return (int)h;
}

int CurseGUITable::DrawCell(WINDOW* wd, int r, int c)
{
	char ch = ' ';
	bool borderx,bordery;	//Flags borders.
	int w,h,x,y,sx;
	int lx;	//Character position in the string.
	int tmp;
	string str;

	borderx = true;

	//Text in this cell.
	str = tbl.at(r).at(c);

	w = clw.at(c) + BW;
	h = GetRowHeight(r) + BH;
	x = g_x + cur_x - scrollx;
	y = g_y + cur_y - ((g_header && (r == 0)) ? 0 : scrolly);

	lx = 0;

	//Calculation of the displacement in the visible area.
	sx = scrollx - GetSumTableWidth(c);

	for (int i = y; i < (y + h); ++i) {
		bordery = ((i == y) || (i == y + h - 1));

		//Clipping by Y (top).
		tmp = g_y + ((g_header && (r > 0)) ? (GetRowHeight(0) + BH) : 0);
		if (i < tmp) {
			if(!bordery)
				lx += clw.at(c);
			continue;
		}
		//Clipping by Y (bottom).
		if (i > (g_y + g_h)) continue;

		if (sx > 0 && (!bordery))
			lx += (c > 0) ? (sx - 1) : (sx - 1 - c);

		//Draw line.
		for (int j = x; j < (x + w); ++j) {
			borderx = ((j == x) || (j == (x + w - 1)));

			//Clipping by X (left).
			if (j < g_x) continue;

			//Clipping by X (right).
			if (j > g_x + g_w) {
				if(!bordery && !borderx) lx++;
				continue;
			}

			if(bordery) {
				//Set the border symbol.
				ch = (borderx)? ((i > y && scrolly > 0) ? '*':'+'):'-';
			} else {
				//Set right or left border symbol.
				if (borderx) ch = '|';
				else {
					ch = ' ';
					if(!str.empty()) {
						if(lx < (int)str.size()) {
							//Set character from cell text.
							ch = str.at(lx++);
						}
					}
				}
			}

			//Make table outline bold, if the table is selected
			if ((borderx || bordery) && selected) wattrset(wd,A_BOLD);

			//Put the character into the window.
			mvwaddch(wd, i, j, ch);
			ch = ' ';

			//Restore attribute.
			wattrset(wd,A_NORMAL);
		}
	}
	return 0;
}


void CurseGUITable::Update()
{
	int i,j,th,tw;
	WINDOW* wd = wnd->GetWindow();

	/* Control scrolling. */

	th = GetTableHeight();
	//Check visibility.
	if (th <= g_h) {
		scrolly = 0;
		th = g_h;
	} else th -= g_h + 1;
	//Check bottom.
	if (scrolly < 0) scrolly = 0;
	//Check top.
	if (scrolly > th) scrolly = th;

	tw = GetTableWidth();
	//Check visibility.
	if (tw <= g_w) {
		scrollx = 0;
		tw = g_w;
	} else tw -= g_w + 1;
	//Check left.
	if (scrollx < 0) scrollx = 0;
	//Check right.
	if (scrollx > tw) scrollx = tw;

	/* Draw table */
	for (i = 0, cur_y = 0; i < g_rows; ++i) {
		for (j = 0, cur_x = 0; j < g_col; ++j) {
			DrawCell(wd, i, j);
			//Move current X position.
			cur_x += clw.at(j) + BW / 2;
		}
		//Move current Y position.
		cur_y += GetRowHeight(i) + BH / 2;
	}
}

bool CurseGUITable::PutEvent(SGUIEvent* e)
{
	switch (e->t) {
	case GUIEV_KEYPRESS:
		if (!selected) return false;

		switch (e->k) {
		case KEY_PPAGE:
			scrolly--;
			return true;
		case KEY_NPAGE:
			scrolly++;
			return true;
		case KEY_LEFT:
			scrollx--;
			return true;
		case KEY_RIGHT:
			scrollx++;
			return true;
//		case KEY_HOME:
//			scrolly = 0;
//			return true;
//		case KEY_END:
//			scrolly = GetTableHeight();
//			return true;
		default: break;
		}
		break;

		case GUIEV_MOUSE:
			break;

		default: break;
	}
	return false;
}
