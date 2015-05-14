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

	///Returns a pointer to a window owning this controls holder.
	CurseGUIWnd* GetWindow()				{ return owner; }

	///Appends control to holder.
	void Append(CurseGUIControl* ctl);

	///Removes control from holder and destroys it.
	void Delete(CurseGUIControl* ctl);

	///Updates visual representation of all controls in this holder.
	void Update();

	///Pumps event through all controls.
	bool PutEvent(SGUIEvent* e);

	///Rotates selection through all controls in holder.
	void Rotate();

	///Selects particular control, deselects others.
	void Select(CurseGUIControl* ctl);
};

/* ******************************************************************** */

/* Base class for Curse GUI controls and other UI elements */
class CurseGUIControl
{
protected:
	CurseGUICtrlHolder* holder;
	CurseGUIWnd* wnd;
	int g_x, g_y, g_w, g_h;
	SGUIPixel fmt;
	bool selected;
	std::string text;

public:
	///Control constructor will automatically register control in controls holder.
	CurseGUIControl(CurseGUICtrlHolder* p, int x, int y);

	///Don't call destructor directly in the code outside of CurseGUI!
	virtual ~CurseGUIControl()				{}

	///Set visible format (color pair) of control element.
	virtual void SetFormat(SGUIPixel f)		{ fmt = f; }

	///Returns current format.
	SGUIPixel GetFormat()					{ return fmt; }

	///Fills in format info to given address.
	void GetFormat(SGUIPixel* p)			{ if (p) *p = fmt; }

	///Method will return true if the control can be slected.
	virtual bool Select(bool s)				{ selected = s; return true; }

	///Returns selection state of control.
	virtual bool IsSelected()				{ return selected; }

	///Updates graphical representation of control.
	virtual void Update() = 0;

	///Events pump endpoint.
	virtual bool PutEvent(SGUIEvent*) = 0;

	///Call this method to destroy and unregister control from its holder.
	virtual void Delete();
};

/* ******************************************************************** */

class CurseGUIPicture : public CurseGUIControl
{
private:
	SGUIPixel* pict;
	bool autoalloc;
	unsigned length;

public:
	CurseGUIPicture(CurseGUICtrlHolder* p, int x, int y, int w, int h);
	virtual ~CurseGUIPicture();

	bool SetAutoAlloc(bool a);
	bool GetAutoAlloc()						{ return autoalloc; }

	//Picture is a static element, cannot be selected.
	bool Select(bool s)						{ return false; }
	bool IsSelected()						{ return false; }

	void SetPicture(SGUIPixel* p);
	SGUIPixel* GetPicture()					{ return pict; }
	void ColorFill(SCTriple col);

	void Update();
	bool PutEvent(SGUIEvent* e)				{ return false; }
};

/* ******************************************************************** */

class CurseGUIButton : public CurseGUIControl
{
private:
	void Click();

public:
	CurseGUIButton(CurseGUICtrlHolder* p, int x, int y, int w, std::string capt);
	virtual ~CurseGUIButton()				{}

	void SetCaption(std::string capt)		{ text = capt; }

	void Update();
	bool PutEvent(SGUIEvent* e);
};

/* ******************************************************************** */

class CurseGUIEditBox : public CurseGUIControl
{
private:
	void Enter();

public:
	CurseGUIEditBox(CurseGUICtrlHolder* p, int x, int y, int w, std::string txt);
	virtual ~CurseGUIEditBox()				{}

	void SetText(std::string txt)			{ text = txt; }
	std::string GetText()					{ return text; }

	void Update();
	bool PutEvent(SGUIEvent* e);
};

/* ******************************************************************** */

class CurseGUICheckBox : public CurseGUIControl
{
private:
	bool checked,disabled;

	void Check();

public:
	CurseGUICheckBox(CurseGUICtrlHolder* p, int x, int y, int w, std::string capt);
	virtual ~CurseGUICheckBox()				{}

	void SetCaption(std::string capt)		{ text = capt; }
	void SetChecked(bool c)					{ checked = c; }
	bool GetChecked()						{ return checked; }
	void SetDisabled(bool d)				{ disabled = d; }
	bool GetDisabled()						{ return disabled; }

	bool Select(bool s);

	void Update();
	bool PutEvent(SGUIEvent* e);
};

/* ******************************************************************** */

class CurseGUIProgrBar : public CurseGUIControl
{
private:
	int g_min, g_max;
	int step, pos;
	bool showprc;			//show percent as text
	SGUIPixel foregr;		//foreground colors template

public:
	CurseGUIProgrBar(CurseGUICtrlHolder* p, int x, int y, int w, int min, int max);
	virtual ~CurseGUIProgrBar()				{}

	void SetNumSteps(int n);
	void Step();

	void SetForegrFormat(SGUIPixel f)		{ foregr = f; }

	void SetShowPercent(bool s)				{ showprc = s; }
	bool GetShowPercent()					{ return showprc; }

	//Progress bar is a static element, cannot be selected.
	bool Select(bool s)						{ return false; }
	bool IsSelected()						{ return false; }

	void Update();
	bool PutEvent(SGUIEvent* e)				{ return false; }
};

/* ******************************************************************** */

class CurseGUITable : public CurseGUIControl
{
private:
	std::vector<std::vector<std::string> > tbl;

	int g_col;			//quantity columns
	int g_rows;			//quantity rows
	int g_wcell;		//cell width
	int g_hcell;		//cell height
	int g_htable;		//the visible height of the table
	int g_wtable;		//the visible width of the table
	int g_tw;			//table width
	int cur_x, cur_y;
	int scrolly, scrollx;

	int DrawCell(WINDOW* wd, int x, int y);
	int GetRowHeight(int r);
	int GetTableHeight();

public:
	CurseGUITable(CurseGUICtrlHolder* p, int x, int y, int col, int rows, int wcell, int htable, int wtable);
	virtual ~CurseGUITable()	{}

	void SetData(std::string data, int r, int c);
	void SetWidth(int width);

	void Update();
	bool PutEvent(SGUIEvent* e);

	void AddRow();
	void DelRow();
	void AddColumn();
	void DelColumn();
};

/* ******************************************************************** */

class CurseGUILabel : public CurseGUIControl
{
public:
	CurseGUILabel(CurseGUICtrlHolder* p, int x, int y, int w, std::string capt);
	virtual ~CurseGUILabel()				{}

	void SetCaption(std::string capt)		{ text = capt; }

	//Label is a static element, cannot be selected.
	bool Select(bool s)						{ return false; }
	bool IsSelected()						{ return false; }

	void Update();
	bool PutEvent(SGUIEvent* e)				{ return false; }
};

/* ******************************************************************** */

/*
 * TODO:
 * V CurseGUIPicture - blit picture to window backbuffer
 * V CurseGUIButton - renders a simple button like [BUTTON]
 * V CurseGUIEditBox - an underline (_) fillable with some text or user input.
 * 					Example: EditBox(6 chars):	(______)
 * 							after some input:	(Test__)
 * V CurseGUICheckBox - (V) or (X) with switchable state. Use (O) for disabled.
 * V CurseGUIProgrBar - basic progress bar like {#### 25%     }
 *   CurseGUITable - table utilizes ncurses' line drawings. Should be possible to navigate with keys.
 * 					Ex.:	+------+-------*--+ (see curs_addch (3X) section Line graphics)
 * 							|item  |  text | 1|
 * 							+------+-------+--+
 * 							|item 2|some lo| 2|
 * 							|      |nger te|  |
 * 							|      |xt     |  |
 * 							+------+-------+--+
 * 	 CurseGUILabel - just a label, nothing more :)
 *
 */

#endif /* CGUICONTROLS_H_ */
