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

/* Special kinds of CurseGUI windows are defined here */

#ifndef INCLUDE_CGUISPECWND_H_
#define INCLUDE_CGUISPECWND_H_

#include <string>
#include <vector>
#include "CurseGUI.h"


class CurseGUIDebugWnd : public CurseGUIWnd {
private:
	std::vector<std::string> log;
	int cnt; //tmp
	bool hidden;
	int key;
	bool edit;
	std::string edit_line;
	void ResizeWnd();

public:
	CurseGUIDebugWnd(CurseGUI* scrn, int x, int y);
	virtual ~CurseGUIDebugWnd();

	void Update(bool refr);
	bool PutEvent(CGUIEvent* e);
	void PutString(char* str);
	void PutString(std::string str);
};

#endif /* INCLUDE_CGUISPECWND_H_ */
