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

#include "debug.h"
#include "support.h"

CurseGUIDebugWnd* debug_ui = NULL;

void dbg_init(CurseGUI* gui)
{
	debug_ui = new CurseGUIDebugWnd(gui);
	gui->AddWindow(debug_ui);
}

void dbg_finalize()
{
	debug_ui = NULL;
}

void dbg_toggle()
{
	if (debug_ui) debug_ui->ToggleShow();
}

void dbg_logstr(const char* str)
{
	if (!str) return;
	if (debug_ui) debug_ui->PutString(str);
	else errout("[DEBUG] %s\n",str);
}

void dbg_print(const char* fmt, ...)
{
	char str[DBGUIMAXLEN];
	va_list vl;

	if (!fmt) return;

	va_start(vl,fmt);
	vsnprintf(str,DBGUIMAXLEN,fmt,vl);
	va_end(vl);

	if (debug_ui) debug_ui->PutString(str);
	else errout("[DEBUG] %s\n",str);
}
