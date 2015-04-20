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

/* Defines the entry point of the application */

#include <stdio.h>
#include "plastic.h"
#include "visual.h"
#include "support.h"

static SGameSettings g_set = DEFAULT_SETTINGS;
static CurseGUI* g_gui = NULL;

/* *********************************************************** */

static void plastic_shell()
{
	//TODO
}

static void plastic_start()
{
	g_gui = new CurseGUI();
	if (g_gui->GetLastResult()) {
		errout("Unable to create CurseGUI!\n");
		abort();
	}
	//TODO
	delete g_gui;
}

/* *********************************************************** */

int main(int argc, char* argv[])
{
	printf(HELLOSTR,PRODNAME,VERMAJOR,VERMINOR,BUILDNO,PRODNAME);

	if (!argparser(argc,argv)) return 1;

	printsettings(&g_set);
	if (g_set.use_shell) plastic_shell();

	plastic_start();

	printf("\n\n\nGood exit.\n");
	return 0;
}
