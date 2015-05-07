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
#include "CGUIControls.h"
#include "CGUIOverlay.h"
#include "CGUISpecWnd.h"


CurseGUIBase::CurseGUIBase()
{
	wnd = NULL;
	backgr = NULL;
	backgr_size = 0;
	result = 0;
	g_w = g_h = 0;
	will_close = false;
	cmanager = NULL;
	autoalloc = false;
}

CurseGUIBase::~CurseGUIBase()
{
	if (autoalloc && backgr) free(backgr);
}

bool CurseGUIBase::UpdateSize()
{
	int w,h;
	bool r = false;

	getmaxyx(wnd,h,w);

	if ((h != g_h) || (w != g_w)) {
		//size was changed
		ResizeBack();
		r = true;
	}
	g_h = h;
	g_w = w;

	return r;
}

void CurseGUIBase::SetBackgroundData(SGUIPixel* ptr, int size)
{
	if (autoalloc) {
		if (!backgr) return;
		memcpy(backgr,ptr,((size < backgr_size)? size:backgr_size));
	} else {
		backgr = ptr;
		backgr_size = size;
		if (!ptr) backgr_size = 0;
	}
}

void CurseGUIBase::FillBackgroundData(const SGUIPixel p)
{
	int i;
	if ((!backgr) || (backgr_size < 1)) return;

	for (i = 0; i < backgr_size; i++)
		backgr[i] = p;
}

bool CurseGUIBase::SetAutoAlloc(bool a)
{
	SGUIPixel fill;
	fill.bg.r = 0; fill.bg.g = 0; fill.bg.b = 0;
	fill.fg.r = 1000; fill.fg.g = 1000; fill.fg.b = 1000;
	fill.sym = ' ';

	if (autoalloc) {
		if (a) {
			//nothing to do, exit to not destruct current data
			return true;
		}
		//free old memory
		free(backgr);
		backgr = NULL;
	}

	autoalloc = a;

	if (a) {
		backgr_size = g_w * g_h;
		backgr = (SGUIPixel*)malloc(backgr_size*sizeof(SGUIPixel));
		if (backgr) FillBackgroundData(fill);
		else return false;
	}
	return true;
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

void CurseGUIBase::ResizeBack()
{
	SGUIPixel fill;
	fill.bg.r = 0; fill.bg.g = 0; fill.bg.b = 0;
	fill.fg.r = 1000; fill.fg.g = 1000; fill.fg.b = 1000;
	fill.sym = ' ';

	if (!autoalloc) return;

	backgr_size = g_w * g_h;
	backgr = (SGUIPixel*)realloc(backgr,backgr_size*sizeof(SGUIPixel));

	if (backgr) FillBackgroundData(fill);
	else backgr_size = 0;
}

/* ********************************** GUI MAIN ********************************** */

#define CGABRT(X) { result = X; endwin(); return; }

CurseGUI::CurseGUI() : CurseGUIBase()
{
	SGUIEvent e;

	/* Create curses screen */
	wnd = initscr();
	if (!wnd) CGABRT(1);

	/* Make it colorful */
	if (has_colors() == FALSE) CGABRT(2);
	start_color();

	/* Make it even more colorful through a full RGB range */
	if (can_change_color() == FALSE) CGABRT(3);

	/* Restore any previous changes to terminal settings */
	if (use_default_colors() == ERR) CGABRT(4);

	/* Make the mouse alive */
	if (mousemask(ALL_MOUSE_EVENTS,&oldmouse) == 0) CGABRT(5);
	if (has_mouse() == FALSE) CGABRT(6);

	/* Spawn the color manager */
	cmanager = new CGUIColorManager();

	/* Initialize mode */
	noecho();
	cbreak();
	keypad(wnd,TRUE);
	nodelay(wnd,TRUE);

	/* Deal with size */
	PumpEvents(&e);

	/* Ready to rock! */
	refresh();
	c_x = c_y = 0;
	result = 0;
}

CurseGUI::~CurseGUI()
{
	mousemask(oldmouse,NULL);

	RmAllWindows();

	if (cmanager) delete cmanager;
	if (backmask) free(backmask);

	if (wnd) delwin(wnd);
	endwin();

	use_default_colors();
	refresh();
}

void CurseGUI::Update(bool refr)
{
	std::vector<CurseGUIWnd*>::iterator it;

	//First of all, start color manager frame
	cmanager->StartFrame();

	//Update whole GUI background image
	UpdateBack();

	//Update all windows (FIFO)
	for (it = windows.begin(); it != windows.end(); ++it)
		(*it)->Update(false);

	//Stop color manager frame so it can apply changes (if any)
	cmanager->EndFrame();

	//Check and update cursor position
	if (c_x < 0) c_x = 0;
	if (c_x >= g_w) c_x = g_w - 1;
	if (c_y < 0) c_y = 0;
	if (c_y >= g_h) c_y = g_h - 1;
	move(c_y,c_x);

	if (refr) wrefresh(wnd); //refresh if necessary
}

void CurseGUI::SoftReset()
{
	/* Will look like CurseGUI was reset, but it wasn't */
	RmAllWindows();
	cmanager->Flush();
	cmanager->Apply();
	clear();
	refresh();
	Update(true);
}

CurseGUIWnd* CurseGUI::MkWindow(int x, int y, int w, int h, const char* name)
{
	CurseGUIWnd* nwd;
	if ((x<0) || (y<0) || (w<1) || (h<1)) return NULL;
	nwd = new CurseGUIWnd(this,x,y,w,h);
	nwd->SetName(name);
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
	if (!ptr) return false;

	for (it = windows.begin(); it != windows.end(); ++it)
		if (*it == ptr) {
			//TODO: set the last 'not stay-on-top' window focused
			delete ptr;
			windows.erase(it);
			return true;
		}
	return false;
}

bool CurseGUI::RmWindow(const int no)
{
	return (RmWindow(GetWindowN(no)));
}

bool CurseGUI::RmWindow(const char* name)
{
	CurseGUIWnd* p;
	bool r = false;

	while ((p = GetWindowN(name))) {
		r = true;
		RmWindow(p);
	}
	return r;
}

void CurseGUI::RmAllWindows()
{
	while (windows.size()) {
		delete (*(windows.end()-1));
		windows.pop_back();
	}
}

CurseGUIWnd* CurseGUI::GetWindowN(const int no)
{
	if ((no < 0) || (no >= (int)windows.size()))
		/* we don't want more than two billion windows, though */
		return NULL;
	return (windows[no]);
}

CurseGUIWnd* CurseGUI::GetWindowN(const char* name)
{
	std::vector<CurseGUIWnd*>::iterator it;
	std::string nm(name);
	if (nm.empty()) return NULL;

	for (it = windows.begin(); it != windows.end(); ++it)
		if ((*it)->GetName() == nm) {
			return (*it);
		}
	return NULL;
}

void CurseGUI::SetFocus(CurseGUIWnd* ptr)
{
	std::vector<CurseGUIWnd*>::iterator it;

	for (it = windows.begin(); it != windows.end(); ++it) {
		if ((*it) == ptr)
			(*it)->GainFocus();
		else
			(*it)->LooseFocus();
	}
}

void CurseGUI::SetFocus(const int no)
{
	SetFocus(GetWindowN(no));
}

void CurseGUI::SetFocus(const char* name)
{
	SetFocus(GetWindowN(name));
}

void CurseGUI::Reorder(int by)
{
	std::vector<CurseGUIWnd*>::iterator it;
	CurseGUIWnd* tmp;
	bool flg;

	for (it = windows.begin(); it != windows.end()-1; ++it) {
		tmp = (*it);
		switch (by) {
		case 0: flg = tmp->IsFocused(); break;
		case 1: flg = tmp->IsStayOnTop(); break;
		default: flg = false;
		}
		if (flg) {
			//move it to the tail
			it = windows.erase(it);
			windows.push_back(tmp);
		}
	}
}

bool CurseGUI::PumpEvents(SGUIEvent* e)
{
	std::vector<CurseGUIWnd*>::iterator it;
	std::vector<CurseGUIWnd*>::reverse_iterator rt;
	int x,y;
	bool consumed = false;
	result = 1;

	if (will_close) {
		e->t = GUIEV_NONE; //reset event type to prevent loops
		return true; //to not process event furthermore
	}

	/* Get the resize event (most priority) */
	if (UpdateSize()) {
		e->t = GUIEV_RESIZE;
	} else {

		/* Or, get the keypress event */
		e->t = GUIEV_KEYPRESS;
		e->k = getch(); //FIXME: use something more robust than that
		if ((!e->k) || (e->k == ERR)) {

			/* Or, get the mouse event (least priority) */
			e->t = GUIEV_MOUSE;
			if (getmouse(&(e->m)) == ERR) {
				e->t = GUIEV_NONE; //reset event type to prevent loops
				return true; //Event consumed 'cause there's no event!
			}
		}
	}

	if (!windows.empty()) {

		/* Pump down the events (in order of drawing) */
		for (it = windows.begin(); it != windows.end(); ++it) {
			if ((e->t == GUIEV_KEYPRESS) || (e->t == GUIEV_MOUSE)) {
				//user input events will only be passed into focused windows
				if (!(*it)->IsFocused()) continue;
			}
			//pass the event
			if ((*it)->PutEvent(e)) {
				consumed = true; //that's a 'private' event, stop pumping
				if ((*it)->WillClose()) RmWindow(*it); //destroy closing window
				break;
			}
		}

		Reorder(0); //Windows reordering by focus value
		Reorder(1); //Windows reordering by stayontop property
	}

	/* No one has consumed the event. Need to be processed in global GUI scope. */
	if (!consumed) {
		switch (e->t) {
		case GUIEV_KEYPRESS:
			switch (e->k) {
			case GUI_DEFCLOSE: /* Default 'Close' key in GUI scope -> exit */
				will_close = true;
				consumed = true;
				break;
			}
			break;

		case GUIEV_MOUSE:
			if ((windows.empty()) || (e->m.bstate != BUTTON1_CLICKED)) break;
			for (rt = windows.rbegin(); rt != windows.rend(); ++rt) {
				//check mouse pointer is inside window
				if (((*rt)->GetPosX() > e->m.x) || (((*rt)->GetPosY() > e->m.y))) continue;
				x = (*rt)->GetPosX() + (*rt)->GetWidth();
				y = (*rt)->GetPosY() + (*rt)->GetHeight();
				if ((e->m.x >= x) || (e->m.y >= y)) continue;
				//try to set focus. If it's impossible, move on.
				if (!(*rt)->GainFocus()) continue;
				SetFocus(*rt); //do this to send 'focus lost' message to others
				consumed = true;
				break; //make sure only one window will gain focus
			}
			break;

		default: break; //just to make compiler happy
		}
	}

	/* Update the background masking */
	if (e->t == GUIEV_RESIZE)
		backmask = (char*)realloc(backmask,g_w*g_h);
	UpdateBackmask();

	result = 0;
	return consumed;
}

void CurseGUI::UpdateBackmask()
{
	int i,j,k;
	std::vector<CurseGUIWnd*>::iterator it;
	bool skip;
	CurseGUIOverlay* ovrl;
	CurseGUIDebugWnd* dbug;

	if (!backmask) return;

	memset(backmask,0,g_w*g_h);

	for (it = windows.begin(); it != windows.end(); ++it) {
		skip = false;

		//Deal with special cases
		switch ((*it)->GetType()) {
		case GUIWT_OVERLAY:
			ovrl = reinterpret_cast<CurseGUIOverlay*> (*it);
			if (ovrl->GetTransparent()) skip = true; //skip transparent overlays
			break;

		case GUIWT_DEBUGUI:
			dbug = reinterpret_cast<CurseGUIDebugWnd*> (*it);
			if (dbug->LooseFocus()) skip = true; //focus can leave debugUI only if it's hidden
			break;

		default: break;
		}
		if (skip) continue;

		//Draw the mask
		j = (*it)->GetPosX();
		k = (*it)->GetHeight() + (*it)->GetPosY();
		for (i = (*it)->GetPosY(); i < k; i++)
			memset(backmask+(i*g_w+j),1,(*it)->GetWidth());
	}
}

/* ********************************** GUI WINDOWS ********************************** */

CurseGUIWnd::CurseGUIWnd(CurseGUI* scrn, int x, int y, int w, int h)
{
	type = GUIWT_BASIC;
	parent = scrn;
	cmanager = scrn->GetColorManager();
	wnd = subwin(scrn->GetWindow(),h,w,y,x);
	if (wnd) {
		g_w = w;
		g_h = h;
		g_x = x;
		g_y = y;
	}
	focused = false;
	boxed = true;
	stayontop = false;
	ctrls = new CurseGUICtrlHolder(this);
	name = "Unnamed";
}

CurseGUIWnd::~CurseGUIWnd()
{
	if (ctrls) delete ctrls;
	if (wnd) delwin(wnd);
}

void CurseGUIWnd::SetName(const char* nm)
{
	if (nm) name = std::string(nm);
}

void CurseGUIWnd::Update(bool refr)
{
	//A template of GUI Window update process.
	werase(wnd);				//make a room for window
	UpdateBack();				//draw background image
	ctrls->Update();			//update controls
	wcolor_set(wnd,0,NULL);		//set default color pair
	if (boxed) box(wnd,0,0);	//draw border
	if (refr) wrefresh(wnd);	//and refresh if necessary
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

bool CurseGUIWnd::PutEvent(SGUIEvent* e)
{
	if (will_close) return false;

	/* Put the event to controls first */
	if (ctrls->PutEvent(e)) return true;

	/* Window-wide event */
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

	/* That's not our event, pass thru */
	return false;
}
