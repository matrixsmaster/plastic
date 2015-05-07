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

#ifndef CGUIEVENTS_H_
#define CGUIEVENTS_H_

#include <string>
#include <ncurses.h>


enum EGUIEventType {
	GUIEV_NONE,
	GUIEV_KEYPRESS,
	GUIEV_RESIZE,
	GUIEV_MOUSE,
	GUIEV_CTLBACK,
	GUIEV_COMMAND
};

enum EGUIFeedbackType {
	GUIFB_NONE,
	GUIFB_SELECTED,
	GUIFB_SWITCHED
};

class CurseGUIControl;
struct SGUICtlFeedback {
	EGUIFeedbackType t;
	CurseGUIControl* ctl;
};

enum EGUICommandType {
	GUICT_NONE,
	GUICT_GETRAM,		//TODO: not implemented
	GUICT_LSTWINDOWS	//TODO: not implemented
};

struct SGUICommand {
	EGUICommandType t;
	std::string arg_str;
	int arg_int;
};

struct SGUIEvent { /* Use one-char names to make it simpler, keep this structure small */
	EGUIEventType	t;	//event type
	int 			k;	//pressed key code
	MEVENT			m;	//mouse event data
	SGUICtlFeedback b;	//feedback from UI controls
	SGUICommand*	c;	//pointer to command structure
};


#define GUI_DEFCLOSE KEY_F(10)

#endif /* CGUIEVENTS_H_ */
