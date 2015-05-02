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

/* Defines the entry point of the application. Please keep this file as clean as possible. */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "plastic.h"
#include "support.h"
#include "debug.h"
#include "CurseGUI.h"
#include "world.h"
#include "LVR.h"


static SGameSettings	g_set = DEFAULT_SETTINGS;
static PlasticWorld*	g_wrld = NULL;
static CurseGUI*		g_gui = NULL;
static pthread_t		t_event = 0;


/* *********************************************************** */

static void plastic_shell()
{
	//TODO: interactive startup shell
}

static void* plastic_eventhread(void* ptr)
{
	CGUIEvent my_e;
	//temporary FPS counter:
	time_t beg;
	ulli cnt = 0;
	beg = clock();
	//projection testing:
	vector2di curso;
	char s[128];
	vector3di x;
	bool d;

	while ((g_gui) && (!g_gui->WillClose())) {

		/* Events pump */
		if (!g_gui->PumpEvents(&my_e)) {
			/* No one consumed event, need to be processed inside the core */
			g_wrld->ProcessEvents(&my_e);
		}

		//FIXME: create separate rendering thread
		g_wrld->GetRenderer()->Frame();
		cnt++;
		if ((clock() - beg) >= CLOCKS_PER_SEC) {
			g_wrld->GetHUD()->UpdateFPS(cnt);
//			dbg_print("fps = %llu",cnt);
			cnt = 0;
			beg = clock();
		}
		g_gui->Update(true);

		//debug:
		d = false;
		if (my_e.t == GUIEV_MOUSE) {
//			switch (my_e.k) {
//			case 'i': curso.Y--; d = true; break;
//			case 'k': curso.Y++; d = true; break;
//			case 'j': curso.X--; d = true; break;
//			case 'l': curso.X++; d = true; break;
//			}
			if (my_e.m.bstate & BUTTON1_CLICKED) {
				curso.X = my_e.m.x;
				curso.Y = my_e.m.y;
				d = true;
			}
		}
		if (d) {
			x = g_wrld->GetRenderer()->GetProjection(curso);
			snprintf(s,128,"%d:%d->%d:%d:%d",curso.X,curso.Y,x.X,x.Y,x.Z);
			g_wrld->GetHUD()->PutStrBottom(s);
			g_gui->SetCursorPos(curso.X,curso.Y);
		}

		/* To keep CPU load low(er) */
		usleep(EVENTUSLEEP);
	}
	return NULL;
}

static void plastic_start()
{
	int r;

	/* Create a world! */
	g_wrld = new PlasticWorld(&g_set);
	r = g_wrld->GetLastResult();
	if (r) {
		errout("Unable to create world (error #%d)\n",r);
		abort();
	}

	/* Make a CurseGUI */
	g_gui = new CurseGUI();
	r = g_gui->GetLastResult();
	if (r) {
		errout("Unable to create CurseGUI (error #%d)\n",r);
		abort();
	}

	/* Connect world to GUI */
	g_wrld->ConnectGUI(g_gui);

	/* Init debug UI */
	dbg_init(g_gui);

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
	if (g_wrld) delete g_wrld;
	if (g_gui) delete g_gui;
}

/* *********************************************************** */

int main(int argc, char* argv[])
{
	printf(HELLOSTR,PRODNAME,VERMAJOR,VERMINOR,BUILDNO,PRODNAME);

	/* Parse and print current settings. */
	if (!argparser(argc,argv,&g_set)) {
		/* Print out some helpful information */
		arghelp(argv[0]);
		return 1;
	}
	printsettings(&g_set);

	/* Show interactive startup shell if needed. */
	if (g_set.use_shell) plastic_shell();

	/* Prepare and start the game. */
	plastic_start();

	/* Keep updating world until quit event is fired. */
	while ((g_gui) && (!g_gui->WillClose())) {
		g_wrld->Quantum();
		usleep(WORLDUSLEEP);
	}

	/* Free resources. */
	plastic_cleanup();

	printf("\n\n\nGood exit.\n");
	return 0;
}
