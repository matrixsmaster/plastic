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

/*
 * ported from TDWS (dynworld) project
 * and adapted for stl
 */

#ifndef CURSEGUI_H_
#define CURSEGUI_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <ncurses.h>
#include <vector>
#include "CGUIEvents.h"


struct SGUIWCol {
//	char sym;
	short f,b;
};

struct SGUIPixel {
	char sym;
	short col;
};

class CurseGUIBase {
protected:
	WINDOW* wnd;
	int result;
	int g_w, g_h;
	SGUIPixel* backgr;
	int backgr_size;
	bool will_close;

	///Internal background updater.
	void UpdateBack();

public:
	CurseGUIBase();
	virtual ~CurseGUIBase();

	///Returns result of last operation (0 if everything is OK).
	int GetLastResult()		{ return result; }

	///Returns ncurses' WINDOW struct pointer.
	WINDOW* GetWindow()		{ return wnd; }

	///Returns width (overall) of the window.
	int GetWidth()			{ return g_w; }

	///Returns height (overall) of the window.
	int GetHeight()			{ return g_h; }

	///Updates size variables and returns true if they are had changed.
	bool UpdateSize();

	///Specify background data buffer.
	void SetBackgroundData(SGUIPixel* ptr, int size);

	///Returns if the window or frame is going to close.
	bool WillClose()		{ return will_close; }

	virtual void Update(bool) = 0;
};

class CurseGUIWnd;
class CurseGUI : public CurseGUIBase {
private:
	std::vector<CurseGUIWnd*> windows;

public:
	CurseGUI();
	virtual ~CurseGUI();

	///Specify the color pairs to use.
	void SetColortable(const SGUIWCol* table, int count);

	///Update all GUI (calling refresh automatically if refr == true).
	void Update(bool refr);

	///Reset GUI softly (in runtime).
	void SoftReset();

	///Create new CurseGUI window.
	CurseGUIWnd* MkWindow(int x, int y, int w, int h);

	///Register CurseGUI window created outside.
	void AddWindow(CurseGUIWnd* n_wnd);

	///Remove CurseGUI window by its' pointer.
	bool RmWindow(CurseGUIWnd* ptr);

	///Remove CurseGUI window by number.
	bool RmWindow(int no);

	///Destroy all active CurseGUI windows.
	void RmAllWindows();

	///Returns number of registered CurseGUI windows.
	int GetNumWindows()		{ return windows.size(); }

	///Returns CurseGUI window pointer by window number.
	CurseGUIWnd* GetWindowN(int no);

	///Push events through all windows and controls.
	///Returns whatever event was consumed or not.
	bool PumpEvents(CGUIEvent* e);
};

class CurseGUIWnd : public CurseGUIBase {
public:
	CurseGUIWnd(CurseGUI* scrn, int x, int y, int w, int h);
	virtual ~CurseGUIWnd();
	void Update(bool refr);
	void Move(int x, int y);
	void Resize(int w, int h);
	bool PutEvent(CGUIEvent e);
};

#endif /* CURSEGUI_H_ */
