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

#ifndef CURSEGUI_H_
#define CURSEGUI_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <ncurses.h>
#include <vector>
#include <string>
#include "CGUIEvents.h"
#include "CGUIColorManager.h"


#define CGMOUSE_LEFT (BUTTON1_PRESSED | BUTTON1_RELEASED)
#define CGMOUSE_RGHT (BUTTON2_PRESSED | BUTTON2_RELEASED)


/* ********************************** GUI BASE ********************************** */

class CurseGUIBase {
protected:
	WINDOW* wnd;					//ncurses window struct
	int result;						//last result
	int g_w, g_h;					//graphic width and height
	SGUIPixel* backgr;				//ptr to background image
	int backgr_size;				//size of image (to keep track on changing)
	bool will_close;				//'window is now closing' flag
	CGUIColorManager* cmanager;		//main colormanager instance ptr
	bool autoalloc;					//automatic memory allocation flag

	///Internal background updater.
	void UpdateBack();

	///Internal auto-allocated background resizer.
	void ResizeBack();

public:
	CurseGUIBase();
	virtual ~CurseGUIBase();

	///Returns result of last operation (0 if everything is OK).
	int GetLastResult()					{ return result; }

	///Returns ncurses' WINDOW struct pointer.
	WINDOW* GetWindow()					{ return wnd; }

	///Returns width (overall) of the window.
	int GetWidth()						{ return g_w; }

	///Returns height (overall) of the window.
	int GetHeight()						{ return g_h; }

	///Updates size variables and returns true if they are had changed.
	bool UpdateSize();

	///Specify background data buffer.
	void SetBackgroundData(SGUIPixel* ptr, int size);

	///Returns pointer to background data buffer.
	SGUIPixel* GetBackgroundData()		{ return backgr; }

	///Fills the background with the pixel provided.
	void FillBackgroundData(const SGUIPixel p);

	//TODO:write comments
	bool SetAutoAlloc(bool a);
	bool GetAutoAlloc()					{ return autoalloc; }

	///Returns if the window or frame is going to close.
	bool WillClose()		{ return will_close; }

	///Returns a pointer to current ColorManager.
	CGUIColorManager* GetColorManager()	{ return cmanager; }

	virtual void Update(bool) = 0;
};

/* ********************************** GUI MAIN ********************************** */

class CurseGUIWnd;

/* Curse GUI Main class */
class CurseGUI : public CurseGUIBase {
private:
	std::vector<CurseGUIWnd*> windows;	//windows holder
	std::vector<SGUIEvent> eventFIFO;	//internal events buffer
	char* backmask;						//background mask (used to estimate space occupied by windows)
	mmask_t oldmouse;					//original terminal mouse driver state
	int oldmouseint;					//original mouse click interval
	int c_x,c_y;						//cursor position
	SGUIPixel activew;					//active window border style
	SGUIPixel backgrw;					//inactive (background) window border style
	bool wndmove_flag;					//window movement flag
	int wndmove_ox,wndmove_oy;			//window movement original mouse position
	CurseGUIWnd* wndmove_wnd;			//window movement pointer holder

	void Reorder(int by);

public:
	CurseGUI();
	virtual ~CurseGUI();

	///Update all GUI (calling refresh automatically if refr == true).
	void Update(bool refr);

	///Reset GUI softly (in runtime).
	void SoftReset();

	///Create new CurseGUI window.
	CurseGUIWnd* MkWindow(int x, int y, int w, int h, const char* name);

	///Register CurseGUI window created outside.
	void AddWindow(CurseGUIWnd* n_wnd);

	///Remove CurseGUI window by its pointer.
	bool RmWindow(CurseGUIWnd* ptr);

	///Remove CurseGUI window by number.
	bool RmWindow(const int no);

	///Remove all CurseGUI windows with a given name.
	bool RmWindow(const char* name);

	///Destroy all active CurseGUI windows.
	void RmAllWindows();

	///Returns number of registered CurseGUI windows.
	int GetNumWindows()							{ return windows.size(); }

	///Returns CurseGUI window pointer by window number.
	CurseGUIWnd* GetWindowN(const int no);

	///Returns CurseGUI window pointer by window name.
	CurseGUIWnd* GetWindowN(const char* name);

	///Set window focus by pointer.
	void SetFocus(CurseGUIWnd* ptr);

	///Set window focus by number.
	void SetFocus(const int no);

	///Set window focus by name.
	void SetFocus(const char* name);

	///Push events through all windows and controls.
	///Returns whatever event was consumed or not.
	bool PumpEvents(SGUIEvent* e);

	///Append new event into current event queue.
	///Pointer may be freed after this call.
	void AddEvent(SGUIEvent* e);

	///Update background masking by opened windows.
	void UpdateBackmask();

	///Returns background mask.
	char* GetBackmask()							{ return backmask; }

	///Moves cursor to position at the end of each frame.
	void SetCursorPos(const int x, const int y) { c_x = x; c_y = y; }

	///Set the active window border style.
	void SetActiveWndBorder(const SGUIPixel p)	{ activew = p; }

	///Returns the active window border style.
	const SGUIPixel* GetActiveWndBorder()		{ return &activew; }

	///Set the inactive (background) window border style.
	void SetBackgrWndBorder(const SGUIPixel p)	{ backgrw = p; }

	///Returns the inactive (background) window border style.
	const SGUIPixel* GetBackgrWndBorder()		{ return &backgrw; }
};

/* ********************************** GUI WINDOWS ********************************** */

/* Window types enumeration to make windows distinguishable by its purpose */
enum ECGUIWindowType {
	GUIWT_BASIC,
	GUIWT_OVERLAY,
	GUIWT_DEBUGUI,
	GUIWT_OTHER
};

class CurseGUICtrlHolder;

/* CurseGUI Window Base Class */
class CurseGUIWnd : public CurseGUIBase {
protected:
	CurseGUI* parent;		//TODO: comment
	ECGUIWindowType type;
	std::string name;
	bool focused;
	bool boxed;
	bool stayontop;
	bool showname;
	int g_x, g_y;
	CurseGUICtrlHolder* ctrls;

	void DrawDecoration();

public:
	CurseGUIWnd(CurseGUI* scrn, int x, int y, int w, int h);
	virtual ~CurseGUIWnd();

	///Returns parent (main GUI instance) of the window.
	CurseGUI* GetParent()				{ return parent; }

	///Sets drawing window border on or off.
	virtual void SetBoxed(bool b)		{ boxed = b; }

	///Returns true if focus has gained.
	virtual bool GainFocus()			{ focused = true; return true; }

	///Returns true if focus has loosed.
	virtual bool LooseFocus()			{ focused = false; return true; }

	///Returns whether window is focused.
	bool IsFocused()					{ return focused; }

	///Lets a window to stay on top (in most cases).
	void StayOnTop(bool s)				{ stayontop = s; }

	///Returns if this window wants to stay on top.
	bool IsStayOnTop()					{ return stayontop; }

	///Lets a window to show its name if border is enabled.
	void ShowName(bool s)				{ showname = s; }

	///Returns if this window is showing its name.
	bool IsShowName()					{ return showname; }

	///Sets a name for a window.
	virtual void SetName(const char* nm);

	///Returns window name.
	std::string GetName()				{ return name; }

	///Returns window type.
	ECGUIWindowType GetType()			{ return type; }

	///Update window content on the screen.
	virtual void Update(bool refr);

	///Move window on the screen.
	virtual void Move(int x, int y);

	///Resize window to a given columns and rows.
	virtual void Resize(int w, int h);

	///Window' event processor.
	virtual bool PutEvent(SGUIEvent* e);

	///Returns window controls holder.
	CurseGUICtrlHolder* GetControls()	{ return ctrls; }

	int GetPosX()						{ return g_x; }
	int GetPosY()						{ return g_y; }
};

#endif /* CURSEGUI_H_ */
