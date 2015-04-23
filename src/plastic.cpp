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
#include <pthread.h>
#include <unistd.h>
#include "plastic.h"
#include "visual.h"
#include "support.h"
#include "datapipe.h"
#include "LVR.h"
#include "CurseGUI.h"
#include "debug.h"


static SGameSettings	g_set = DEFAULT_SETTINGS;
static CurseGUI*		g_gui = NULL;
static DataPipe*		g_data = NULL;
static LVR*				g_lvr = NULL;
static pthread_t		t_event = 0;


/* *********************************************************** */

static void plastic_shell()
{
	//TODO
}

static void* plastic_eventhread(void* ptr)
{
	CGUIEvent my_e;
	//DEBUG:
	vector3d r;

	while ((g_gui) && (!g_gui->WillClose())) {

		/* Events */
		if (!g_gui->PumpEvents(&my_e)) {
			/* No one consumed event, need to be processed */
			switch (my_e.t) {
			case GUIEV_KEYPRESS:
				switch (my_e.k) {
				case 't': /* test */
					dbg_logstr("Testing OK Alice in Wonderland bla bla bla. Test if string is too long for add to one string");
					break;
					/* DEBUG */
				case KEY_UP: r.X += 1; break;
				case KEY_DOWN: r.X -= 1; break;
				case KEY_LEFT: r.Y += 1; break;
				case KEY_RIGHT: r.Y -= 1; break;
				}
				g_lvr->SetEulerRotation(r);
				break;

			case GUIEV_RESIZE:
				//TODO: should we do anything here? Yes, we should!
				/* Size of terminal has changed */
				if (!g_lvr->Resize(g_gui->GetWidth(),g_gui->GetHeight())) {
					errout("Can't resize LVR frame!");
					abort();
				}
				g_gui->SetBackgroundData(g_lvr->GetRender(),g_lvr->GetRenderLen());
				break;

			default:
				errout("Events: WTF?!"); //TODO: write something more intelligent
			}
		}

		//temporarily there
		g_lvr->Frame();
		g_gui->Update(true);

		/* To keep CPU load low(er) */
		usleep(EVENTUSLEEP);
	}
	return NULL;
}

static void plastic_start()
{
	float alloc_gb;

	/* Create and set up DataPipe */
	g_data = new DataPipe(g_set.root);
	if (g_data->GetStatus() == DPIPE_ERROR) {
		errout("Unable to initialize data pipe. Possibly invalid root directory.\n");
		abort();
	}
	alloc_gb = (float)(g_data->GetAllocatedRAM()) / 1024.f / 1024.f / 1024.f;
	printf("Size of voxel = %lu bytes\n",sizeof(voxel));
	printf("Allocated data pipe memory: %llu bytes (%.3f GiB)\n",g_data->GetAllocatedRAM(),alloc_gb);

	/* Make a CurseGUI */
	g_gui = new CurseGUI();
	if (g_gui->GetLastResult()) {
		errout("Unable to create CurseGUI!\n");
		abort();
	}
	g_gui->SetColortable(gui_coltable,COLPAIRS);

	/* Init LVR */
	g_lvr = new LVR(g_data);
	if (!g_lvr->Resize(g_gui->GetWidth(),g_gui->GetHeight())) {
		errout("Unable to resize LVR render window!\n");
		abort();
	}

	/* Connect lvr output to CurseGUI main background */
	g_gui->SetBackgroundData(g_lvr->GetRender(),g_lvr->GetRenderLen());

	//DEBUG:
	dbg_init(g_gui);
	g_data->SetGP(vector3dulli(0));
	g_lvr->SetPosition(vector3d(128));

	/* Start events processing thread */
	pthread_create(&t_event,NULL,plastic_eventhread,NULL);
}

static void plastic_cleanup()
{
	/* Join all active threads */
	if (t_event) {
		errout("Closing events thread... ");
		pthread_join(t_event,NULL);
		errout("OK\n");
	}

	/* Destroy debugging UI */
	dbg_finalize();

	/* Destroy all main classes instances */
	if (g_gui) delete g_gui;
	if (g_lvr) delete g_lvr;
	if (g_data) delete g_data;
}

/* *********************************************************** */

int main(int argc, char* argv[])
{
	printf(HELLOSTR,PRODNAME,VERMAJOR,VERMINOR,BUILDNO,PRODNAME);

	if (!argparser(argc,argv)) return 1;

	printsettings(&g_set);
	if (g_set.use_shell) plastic_shell();

	plastic_start();

	while (!g_gui->WillClose()) usleep(100000); //FIXME: stub

	plastic_cleanup();

	printf("\n\n\nGood exit.\n");
	return 0;
}
