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

#ifndef CGUICONTROLS_H_
#define CGUICONTROLS_H_

#include <vector>
#include "CurseGUI.h"


class CurseGUIControl;
/* Automated storage container for window controls */
class CurseGUICtrlHolder
{
private:
	CurseGUIWnd* owner;
	std::vector<CurseGUIControl*> controls;

public:
	CurseGUICtrlHolder(CurseGUIWnd* parent)	{ owner = parent; }
	virtual ~CurseGUICtrlHolder();

	CurseGUIWnd* GetWindow()				{ return owner; }

	void Append(CurseGUIControl* ctl);
	void Delete(CurseGUIControl* ctl);

	void Update();
	bool PutEvent(SGUIEvent* e);
};

/* Base class for Curse GUI controls and other UI elements */
class CurseGUIControl
{
protected:
	CurseGUICtrlHolder* holder;
	CurseGUIWnd* wnd;
	int g_x, g_y;
	SCTriple back;

public:
	CurseGUIControl(CurseGUICtrlHolder* p, int x, int y);
	virtual ~CurseGUIControl()				{}

	virtual void SetBackColor(SCTriple c)	{ back = c; }

	virtual void Update() = 0;

	virtual bool PutEvent(SGUIEvent* e) = 0;

	virtual void Delete();
};

class CurseGUIPicture : public CurseGUIControl
{
private:
	SGUIPixel* pict;
	int g_w, g_h;
	bool autoalloc;
	unsigned length;

public:
	CurseGUIPicture(CurseGUICtrlHolder* p, int x, int y, int w, int h);
	virtual ~CurseGUIPicture();

	bool SetAutoAlloc(bool a);
	bool GetAutoAlloc()						{ return autoalloc; }

	void SetPicture(SGUIPixel* p);
	SGUIPixel* GetPicture()					{ return pict; }
	void ColorFill(SCTriple col);

	void Update();
	bool PutEvent(SGUIEvent* e)				{ return false; }
};

class CurseGUIButton : public CurseGUIControl
{
private:
	SGUIPixel fmt;
	int g_w;
	std::string text;

	void Click();

public:
	CurseGUIButton(CurseGUICtrlHolder* p, int x, int y, int w, std::string capt);
	virtual ~CurseGUIButton()				{}

	void SetCaption(std::string capt)		{ text = capt; }

	void Update();
	bool PutEvent(SGUIEvent* e);
};

//TODO CurseGuiTable
//class CurseGUITable : public CurseGUIControl
//{
//private:
//public:
//	CurseGUITable() {}
//	virtual ~CurseGUITable() {}
//};

/*
 * TODO:
 * V CurseGUIPicture - blit picture to window backbuffer
 *   CurseGUIButton - renders a simple button like [BUTTON]
 *   CurseGUIEditBox - an underline (_) fillable with some text or user input.
 * 					Example: EditBox(6 chars):	(______)
 * 							after some input:	(Test__)
 *   CurseGUICheckBox - (V) or (X) with switchable state. Use (O) for disabled.
 *   CurseGUIProgrBar - basic progress bar like {#### 25%     }
 *   CurseGUITable - table utilizes ncurses' line drawings. Should be possible to navigate with keys.
 * 					Ex.:	+------+-------*--+ (see curs_addch (3X) section Line graphics)
 * 							|item  |  text | 1|
 * 							+------+-------+--+
 * 							|item 2|some lo| 2|
 * 							|      |nger te|  |
 * 							|      |xt     |  |
 * 							+------+-------+--+
 */

#endif /* CGUICONTROLS_H_ */
