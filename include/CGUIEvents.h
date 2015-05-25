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
	GUIEV_NONE,			//Nothing or error
	GUIEV_KEYPRESS,		//Simple keyboard event
	GUIEV_RESIZE,		//Terminal window resized
	GUIEV_MOUSE,		//Mouse event
	GUIEV_CTLBACK,		//GUI Controls feedback
	GUIEV_COMMAND		//Command pipe event
};

enum EGUIFeedbackType {
	GUIFB_NONE,			//Nothing or error
	GUIFB_SELECTED,		//Control selected (remove this?)
	GUIFB_SWITCHED,		//Button switched
	GUIFB_CHECKON,		//CheckBox checked
	GUIFB_CHECKOFF,		//CheckBox cleared
	GUIFB_EDITOK		//EditBox Enter key hit
};

class CurseGUIWnd;
class CurseGUIControl;
struct SGUICtlFeedback {
	EGUIFeedbackType t;		//feedback type
	CurseGUIWnd* 	 wnd;	//owner of the control element
	CurseGUIControl* ctl;	//control element, which generated this event
};

enum EGUICommandType {
	GUICT_NONE,			//Nothing or error
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

#define GUISCRL_UP BUTTON4_PRESSED
#define GUISCRL_DW (BUTTON2_PRESSED | REPORT_MOUSE_POSITION)


#endif /* CGUIEVENTS_H_ */
