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
#include "datapipe.h"
#include "LVR.h"


static SGameSettings	g_set = DEFAULT_SETTINGS;
static CurseGUI*		g_gui = NULL;
static DataPipe*		g_data = NULL;
static LVR*				g_lvr = NULL;


/* *********************************************************** */

static void plastic_shell()
{
	//TODO
}

static void plastic_start()
{
	float alloc_gb;

	g_data = new DataPipe(g_set.root);
	if (g_data->GetStatus() == DPIPE_ERROR) {
		errout("Unable to initialize data pipe. Possibly invalid root directory.\n");
		abort();
	}
	alloc_gb = (float)(g_data->GetAllocatedRAM()) / 1024.f / 1024.f / 1024.f;
	printf("Size of voxel = %lu bytes\n",sizeof(voxel));
	printf("Allocated chunks memory: %llu bytes (%.3f GiB)\n",g_data->GetAllocatedRAM(),alloc_gb);

	g_gui = new CurseGUI();
	if (g_gui->GetLastResult()) {
		errout("Unable to create CurseGUI!\n");
		abort();
	}

	g_lvr = new LVR(g_data);
	if (!g_lvr->Resize(g_gui->GetWidth(),g_gui->GetHeight())) {
		errout("Unable to resize LVR render window!\n");
		abort();
	}

	g_gui->SetBackgroundData(g_lvr->GetRender(),g_lvr->GetRenderLen());

	//TODO
}

static void plastic_cleanup()
{
	delete g_gui;
	delete g_lvr;
	delete g_data;
}

/* *********************************************************** */

int main(int argc, char* argv[])
{
	printf(HELLOSTR,PRODNAME,VERMAJOR,VERMINOR,BUILDNO,PRODNAME);

	if (!argparser(argc,argv)) return 1;

	printsettings(&g_set);
	if (g_set.use_shell) plastic_shell();

	plastic_start();
	//TODO: main loop
	plastic_cleanup();

	printf("\n\n\nGood exit.\n");
	return 0;
}
