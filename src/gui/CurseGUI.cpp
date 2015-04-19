/*
 * CurseGUI.cpp
 *
 *  Created on: Apr 7, 2015
 *      Author: matrixsmaster
 */
/*
 * ported from TDWS (dynworld) project
 * and adapted for stl
 */

#include "CurseGUI.h"


CurseGUIBase::CurseGUIBase()
{
	wnd = NULL;
	backgr = NULL;
	backgr_size = 0;
	result = 0;
	g_w = g_h = 0;
}

CurseGUIBase::~CurseGUIBase()
{
	//
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
	short lc;
	char* lbuf;

	werase(wnd);
	if ((!backgr) || (backgr_size < g_w*g_h)) return;

	lbuf = (char*)malloc(g_w);
	if (!lbuf) return;

	l = 0;
	wcolor_set(wnd,1,NULL);
	lc = -1;
	for (i = 0; i < g_h; i++) {
		for (j = 0; j < g_w; j++) {
			lin = i * g_w + j;
			if (lc != backgr[lin].col) {
				if (l > 0) {
					wcolor_set(wnd,lc,NULL);
					mvwaddnstr(wnd,i,j-l,lbuf,l);
					l = 0;
				}
			}
			lbuf[l++] = backgr[lin].sym;
			lc = backgr[lin].col;
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

CurseGUI::CurseGUI()
{
	wnd = initscr();
	if (!wnd) {
		result = 1;
		return;
	}

	if (has_colors() == FALSE) {
		result = 2;
		return;
	}

	noecho();
	cbreak();
	keypad(wnd,TRUE);
//	leaveok(wnd,TRUE);
	start_color();

	refresh();
	UpdateSize();
	result = 0;
}

CurseGUI::~CurseGUI()
{
	RmAllWindows();
	if (wnd) delwin(wnd);
	endwin();
	refresh();
}

void CurseGUI::SetColortable(const SGUIWCol* table, int count)
{
	int i;
	for (i = 0; i < count; i++)
		init_pair(i+1,table[i].f,table[i].b);
}

void CurseGUI::Update(bool refr)
{
	std::vector<CurseGUIWnd*>::iterator it;
	UpdateBack();
//	touchwin(wnd); //ncurses' man page suggests this, but this causes too much flicker
	for (it = windows.begin(); it != windows.end(); it++)
		(*it)->Update(false);
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

CurseGUIWnd::CurseGUIWnd(CurseGUI* scrn, int x, int y, int w, int h)
{
	wnd = subwin(scrn->GetWindow(),h,w,y,x);
//	wnd = newwin(h,w,y,x);
	if (wnd) {
		g_w = w;
		g_h = h;
	}
}

CurseGUIWnd::~CurseGUIWnd()
{
	if (wnd) delwin(wnd);
}

void CurseGUIWnd::Update(bool refr)
{
	UpdateBack();
	box(wnd,0,0);
	if (refr) wrefresh(wnd);
}

void CurseGUIWnd::Move(int x, int y)
{
	if ((x < 0) || (y < 0)) return;
	mvderwin(wnd,y,x);
//	touchwin(wnd);
}

void CurseGUIWnd::Resize(int w, int h)
{
	if ((w < 1) && (h < 1)) return;
	wresize(wnd,h,w);
}