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

/* CurseGUI is a set of wrapper classes to make easier to use ncurses in object-oriented environment */

#include "CurseGUI.h"


CurseGUIBase::CurseGUIBase()
{
	wnd = NULL;
	backgr = NULL;
	backgr_size = 0;
	result = 0;
	g_w = g_h = 0;
	will_close = false;
	cmanager = NULL;
}

bool CurseGUIBase::UpdateSize()
{
	int w,h;
	bool r = false;
	getmaxyx(wnd,h,w);
	if ((h != g_h) || (w != g_w)) r = true;
	g_h = h;
	g_w = w;
	return r;
}

void CurseGUIBase::SetBackgroundData(SGUIPixel* ptr, int size)
{
	backgr = ptr;
	backgr_size = size;
	if (!ptr) backgr_size = 0;
}

void CurseGUIBase::UpdateBack()
{
	int i,j,l,lin;
	short lc,clc;
	char* lbuf;

	werase(wnd);
	if ((!cmanager) || (!backgr) || (backgr_size < g_w*g_h)) return;

	lbuf = (char*)malloc(g_w);
	if (!lbuf) return;

	l = 0;
	wcolor_set(wnd,1,NULL);
	lc = -1;
	for (i = 0; i < g_h; i++) {
		lin = i * g_w;
		for (j = 0; j < g_w; j++,lin++) {
			clc = cmanager->CheckPair(&backgr[lin]);
			if (lc != clc) {
				if (l > 0) {
					wcolor_set(wnd,lc,NULL);
					mvwaddnstr(wnd,i,j-l,lbuf,l);
					l = 0;
				}
			}
			lbuf[l++] = backgr[lin].sym;
			lc = clc;
		}
		if (l > 0) {
			wcolor_set(wnd,lc,NULL);
			mvwaddnstr(wnd,i,j-l,lbuf,l);
			l = 0;
		}
	}

	wcolor_set(wnd,1,NULL);
	free(lbuf);
}

CurseGUI::CurseGUI() : CurseGUIBase()
{
	wnd = initscr();
	if (!wnd) {
		result = 1;
		endwin();
		return;
	}

	if (has_colors() == FALSE) {
		result = 2;
		endwin();
		return;
	}
	start_color();

	if (can_change_color() == FALSE) {
		result = 3;
		endwin();
		return;
	}

	cmanager = new CGUIColorManager();

	noecho();
	cbreak();
	keypad(wnd,TRUE);
	nodelay(wnd,TRUE);

	refresh();
	UpdateSize();
	result = 0;
}

CurseGUI::~CurseGUI()
{
	RmAllWindows();
	if (wnd) delwin(wnd);
	if (cmanager) delete cmanager;
	endwin();
	refresh();
}

//void CurseGUI::SetColortable(const SGUIWCol* table, int count)
//{
//	int i;
//	for (i = 0; i < count; i++)
//		init_pair(i+1,table[i].f,table[i].b);
//}

void CurseGUI::Update(bool refr)
{
	std::vector<CurseGUIWnd*>::iterator it;
	UpdateBack();
	/*touchwin(wnd); //ncurses' man page suggests this, but this causes too much flicker*/
	for (it = windows.begin(); it != windows.end(); it++)
		(*it)->Update(false);
	cmanager->Apply();
	if (refr) wrefresh(wnd);
}

void CurseGUI::SoftReset()
{
	RmAllWindows();
	clear();
	refresh();
	Update(true);
}

CurseGUIWnd* CurseGUI::MkWindow(int x, int y, int w, int h)
{
	CurseGUIWnd* nwd;
	if ((x<0) || (y<0) || (w<1) || (h<1)) return NULL;
	nwd = new CurseGUIWnd(this,x,y,w,h);
	windows.push_back(nwd);
	return nwd;
}

void CurseGUI::AddWindow(CurseGUIWnd* n_wnd)
{
	if (n_wnd) windows.push_back(n_wnd);
}

bool CurseGUI::RmWindow(CurseGUIWnd* ptr)
{
	std::vector<CurseGUIWnd*>::iterator it;
	for (it = windows.begin(); it != windows.end(); it++)
		if (*it == ptr) {
			delete ptr;
			windows.erase(it);
			return true;
		}
	return false;
}

bool CurseGUI::RmWindow(int no)
{
	if ((no < 0) || (no >= (int)windows.size()))
		/* we don't want more than two billion windows, though */
		return false;
	return (RmWindow(windows[no]));
}

void CurseGUI::RmAllWindows()
{
	while (windows.size()) {
		delete (*(windows.end()-1));
		windows.pop_back();
	}
}

CurseGUIWnd* CurseGUI::GetWindowN(int no)
{
	if ((no < 0) || (no >= (int)windows.size()))
		return NULL;
	return (windows[no]);
}

bool CurseGUI::PumpEvents(CGUIEvent* e)
{
	int i;
	bool consumed = false;
	result = 1;

	if (will_close) return true; //to not process event furthermore

	/* Get the resize event */
	if (UpdateSize()) {
		e->t = GUIEV_RESIZE;
	} else {
		/* Or, get the keypress event */
		e->t = GUIEV_KEYPRESS;
		e->k = getch(); //FIXME: use something more robust than that
		if ((!e->k) || (e->k == ERR)) return true; //Event consumed 'cause there's no event!
	}

	for (i = windows.size() - 1; i >= 0; i--) {
		if (windows[i]->PutEvent(e)) {
			consumed = true;
			if (windows[i]->WillClose()) RmWindow(i);
			break;
		}
	}

	if (!consumed) {
		switch (e->t) {
		case GUIEV_KEYPRESS:
			switch (e->k) {
			case GUI_DEFCLOSE:
				will_close = true;
				consumed = true;
				break;
			}
			break;

		case GUIEV_RESIZE:
			//do nothing (at least for now)
			break;

		default: break;
		}
	}

	result = 0;
	return consumed;
}

CurseGUIWnd::CurseGUIWnd(CurseGUI* scrn, int x, int y, int w, int h)
{
	parent = scrn;
	wnd = subwin(scrn->GetWindow(),h,w,y,x);
	if (wnd) {
		g_w = w;
		g_h = h;
		g_x = x;
		g_y = y;
	}
	focused = true;
	boxed = true;
}

CurseGUIWnd::~CurseGUIWnd()
{
	if (wnd) delwin(wnd);
}

void CurseGUIWnd::Update(bool refr)
{
	werase(wnd);
	UpdateBack();
	wcolor_set(wnd,0,NULL);
	if (boxed) box(wnd,0,0);
	if (refr) wrefresh(wnd);
}

void CurseGUIWnd::Move(int x, int y)
{
	if ((x < 0) || (y < 0)) return;
	mvderwin(wnd,y,x);
	g_x = x;
	g_y = y;
}

void CurseGUIWnd::Resize(int w, int h)
{
	if ((w < 1) && (h < 1)) return;
	wresize(wnd,h,w);
	UpdateSize();
}

bool CurseGUIWnd::PutEvent(CGUIEvent* e)
{
	if (will_close || (!focused)) return false;

	switch (e->t) {
	case GUIEV_KEYPRESS:
		switch (e->k) {
		case GUI_DEFCLOSE: will_close = true; return true;
		}
		return false;

	case GUIEV_RESIZE:
		UpdateSize();
		return false; //don't consume resize event!

	default: break;
	}
	return false;
}
