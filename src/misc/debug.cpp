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

#include <pthread.h>
#include "debug.h"

CurseGUIDebugWnd* debug_ui = NULL;
static pthread_mutex_t debug_mutex;

void dbg_init(CurseGUI* gui)
{
	pthread_mutex_init(&debug_mutex,NULL);
	debug_ui = new CurseGUIDebugWnd(gui);
	gui->AddWindow(debug_ui);
}

void dbg_finalize()
{
	pthread_mutex_destroy(&debug_mutex);
}

void dbg_toggle()
{
	if (debug_ui) debug_ui->ToggleShow();
}

void dbg_logstr(const char* str)
{
	if (!debug_ui) return;

	pthread_mutex_lock(&debug_mutex);
	debug_ui->PutString(str);
	pthread_mutex_unlock(&debug_mutex);
}

void dbg_print(const char* fmt, ...)
{
	if (!debug_ui) return;
	char str[DBGUIMAXLEN];
	va_list vl;
	va_start(vl,fmt);
	vsnprintf(str,DBGUIMAXLEN,fmt,vl);
	va_end(vl);

	pthread_mutex_lock(&debug_mutex);
	debug_ui->PutString(str);
	pthread_mutex_unlock(&debug_mutex);
}
