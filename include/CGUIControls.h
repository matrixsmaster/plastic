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


#define BW 2	//Border width
#define BH 2	//Border Height


enum ECGUIControlType {
	GUICL_EMPTY,
	GUICL_PICTURE,
	GUICL_BUTTON,
	GUICL_EDITBOX,
	GUICL_CHECKBOX,
	GUICL_PROGRBAR,
	GUICL_TABLE,
	GUICL_LABEL
};

/* ******************************************************************** */

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

	//FIXME: comment
	int GetNumControls()					{ return ((int)controls.size()); }
	int GetNumControls(ECGUIControlType t);
	CurseGUIControl* GetControl(int num);
	CurseGUIControl* GetControl(int num, ECGUIControlType t);
};

/* ******************************************************************** */

/* Base class for Curse GUI controls and other UI elements */
class CurseGUIControl
{
protected:
	CurseGUICtrlHolder* holder;
	ECGUIControlType typ;
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

	///Returns the type of the control element.
	ECGUIControlType GetType()				{ return typ; }

	///Moves the control over the owning window.
	virtual void Move(int x, int y)			{ g_x = x; g_y = y; }

	///Returns X position of control.
	int GetPosX()							{ return g_x; }

	///Returns Y position of control.
	int GetPosY()							{ return g_y; }

	///Returns overall width of control element.
	virtual int GetWidth()					{ return g_w; }

	///Returns overall height of control element.
	virtual int GetHeight()					{ return g_h; }

	///Updates graphical representation of control.
	virtual void Update() = 0;

	///Events pump endpoint. Returns True if event is consumed.
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
	void SetValue(int v);

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
	std::vector<int> clw;	//array of column widths

	int g_col;				//quantity of columns
	int g_rows;				//quantity of rows
	int cur_x, cur_y;		//the current position of cells
	int scrolly, scrollx;	//offset count table
	bool g_header;

	int DrawCell(WINDOW* wd, int r, int c);
	int GetTableHeight();
	int GetSumTableWidth(int c);

public:
	/* CurseGUITable
	 *
	 * x      - x position
	 * y      - y position
	 * rows	  - quantity of rows
	 * col    - quantity of columns
	 * wcell  - cell width
	 * htable - visible height of the table
	 * wtable - visible width of the table
	 * header - if true header in table is static
	 */
	CurseGUITable(CurseGUICtrlHolder* p, int x, int y, int rows, int col, int wcell, int htable, int wtable, bool header);
	virtual ~CurseGUITable()	{}

	void Update();
	bool PutEvent(SGUIEvent* e);

	///Returns header static or not.
	bool IsStatic()								{ return g_header; }

	///Returns all columns width and width of borders.
	int GetTableWidth();

	///Returns row height.
	int GetRowHeight(int r);

	///Write data into a cell.
	void SetData(std::string data, int r, int c);

	///Set column width.
	void SetColumnWidth(int c, int w);

	///Apply scrolling position.
	void SetScrolly(int s)						{ scrolly = s; }

	///Clear data from all table.
	void EraseTableData();

	///Clear data from a cell.
	void EraseData(int r, int c);

	///Add a row to the end of the table.
	void AddRow();

	///Delete a row from the end of the table.
	void DelRow();

	///Add a column to the end of the table.
	void AddColumn(int width);

	///Delete a column from the end of the table.
	void DelColumn();
};

/* ******************************************************************** */

class CurseGUILabel : public CurseGUIControl
{
public:
	CurseGUILabel(CurseGUICtrlHolder* p, int x, int y, int w, int h, std::string capt);
	virtual ~CurseGUILabel()				{}

	void SetCaption(std::string capt)		{ text = capt; }

	//Label is a static element, cannot be selected.
	bool Select(bool s)						{ return false; }
	bool IsSelected()						{ return false; }

	void Update();
	bool PutEvent(SGUIEvent* e)				{ return false; }
};

#endif /* CGUICONTROLS_H_ */
