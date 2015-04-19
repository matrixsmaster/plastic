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
	void UpdateBack();
public:
	CurseGUIBase();
	virtual ~CurseGUIBase();
	int GetLastResult()		{ return result; }
	WINDOW* GetWindow()		{ return wnd; }
	int GetWidth()			{ return g_w; }
	int GetHeight()			{ return g_h; }
	bool UpdateSize();
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
	void SetColortable(const SGUIWCol* table, int count);
	void Update(bool refr);
	void SoftReset();
	CurseGUIWnd* MkWindow(int x, int y, int w, int h);
	bool RmWindow(CurseGUIWnd* ptr);
	bool RmWindow(int no);
	void RmAllWindows();
	int GetNumWindows()		{ return windows.size(); }
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
