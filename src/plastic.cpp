/*
 * plastic.cpp
 *
 *  Created on: Apr 19, 2015
 *      Author: matrixsmaster
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
