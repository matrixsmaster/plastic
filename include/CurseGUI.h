/*
 * CurseGUI.h
 *
 *  Created on: Apr 7, 2015
 *      Author: matrixsmaster
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
};

class CurseGUIWnd : public CurseGUIBase {
public:
	CurseGUIWnd(CurseGUI* scrn, int x, int y, int w, int h);
	virtual ~CurseGUIWnd();
	void Update(bool refr);
	void Move(int x, int y);
	void Resize(int w, int h);
};

#endif /* CURSEGUI_H_ */
