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

/* Base class for Curse GUI controls and other UI elements */
class CurseGUIControl
{
private:
	CurseGUIWnd* owner;
	int g_x, g_y;
	SCTriple back;

public:
	CurseGUIControl(CurseGUIWnd* p, int x, int y);
	virtual ~CurseGUIControl()				{}

	virtual void SetBackColor(SCTriple c)	{ back = c; }

	virtual void Update() = 0;
};

/* Automated storage container for window controls */
class CurseGUICtrlHolder
{
private:
	CurseGUIWnd* owner;
	std::vector<CurseGUIControl*> controls;

public:
	CurseGUICtrlHolder(CurseGUIWnd* parent)	{ owner = parent; }
	virtual ~CurseGUICtrlHolder();

	void Update();
};

/*
 * TODO:
 * CurseGUIPicture - blit picture to window backbuffer
 * CurseGUIButton - renders a simple button like [BUTTON]
 * CurseGUIEditBox - an underline (_) fillable with some text or user input.
 * 					Example: EditBox(6 chars):	(______)
 * 							after some input:	(Test__)
 * CurseGUICheckBox - (V) or (X) with switchable state. Use (O) for disabled.
 * CurseGUIProgrBar - basic progress bar like {#### 25%     }
 */

#endif /* CGUICONTROLS_H_ */
