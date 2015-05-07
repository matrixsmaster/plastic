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
#include "CGUIControls.h"


static SGameSettings	g_set = DEFAULT_SETTINGS;
static PlasticWorld*	g_wrld = NULL;
static CurseGUI*		g_gui = NULL;
static pthread_t		t_event = 0;
static pthread_t		t_render = 0;
static pthread_mutex_t	m_render;
static bool				g_quit = false;
static uli				g_fps = 0;


/* *********************************************************** */

static void* plastic_eventhread(void* ptr)
{
	CGUIEvent my_e;

	//projection testing:
	vector2di curso;
	char s[128];
	vector3di x;
	bool d;
	//other debug
	CurseGUIWnd* wnd;
	CurseGUIControl* ctl;

	while ((g_gui) && (!g_gui->WillClose())) {

		pthread_mutex_lock(&m_render);

		/* Events pump */
		if (!g_gui->PumpEvents(&my_e)) {
			/* No one consumed event, need to be processed inside the core */
			g_wrld->ProcessEvents(&my_e);
		}

		g_wrld->GetHUD()->UpdateFPS(g_fps);

		//debug:
		d = false;
		if (my_e.t == GUIEV_MOUSE) {
			if (my_e.m.bstate & BUTTON1_CLICKED) {
				curso.X = my_e.m.x;
				curso.Y = my_e.m.y;
				d = true;
			}
		} else if (my_e.t == GUIEV_KEYPRESS) {
			switch (my_e.k) {
			case KEY_F(1): g_gui->MkWindow(curso.X,curso.Y,10,5,"Test"); break;
			case '0':
				//testing window
				g_gui->MkWindow(g_gui->GetWidth()/2,g_gui->GetHeight()/2,20,10,"SomeWin");
				wnd = g_gui->GetWindowN("SomeWin");
				wnd->SetAutoAlloc(true);
				ctl = new CurseGUIPicture(wnd->GetControls(),1,1,10,5); //auto-registering
				break;
			case '9':
				ctl->Delete();
				break;
			}
		}
		if (d) {
			x = g_wrld->GetRenderer()->GetProjection(curso);
			snprintf(s,128,"%d:%d->%d:%d:%d",curso.X,curso.Y,x.X,x.Y,x.Z);
			g_wrld->GetHUD()->PutStrBottom(s);
			g_gui->SetCursorPos(curso.X,curso.Y);
		}

		pthread_mutex_unlock(&m_render);

		/* To keep CPU load low(er) */
		usleep(EVENTUSLEEP);
	}
	g_quit = true;

	return NULL;
}

static void* plastic_renderthread(void* ptr)
{
	LVR* lvr;
	time_t beg;
	uli fps = 0;

	lvr = g_wrld->GetRenderer();
	beg = clock();

	while (!g_quit) {
		lvr->Frame();
		fps++;
		if ((clock() - beg) >= CLOCKS_PER_SEC) {
			g_fps = fps;
			fps = 0;
			beg = clock();
		}

		pthread_mutex_lock(&m_render);
		g_gui->Update(true);
		pthread_mutex_unlock(&m_render);
	}

	return NULL;
}

/* *********************************************************** */

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

	/* Create mutex */
	pthread_mutex_init(&m_render,NULL);

	/* Start events processing thread */
	pthread_create(&t_event,NULL,plastic_eventhread,NULL);

	/* Start rendering thread */
	pthread_create(&t_render,NULL,plastic_renderthread,NULL);
}

static void plastic_cleanup()
{
	/* Join all active threads */
	if (t_render) {
		errout("Closing render thread... ");
		pthread_join(t_render,NULL);
		errout("OK\n");
	}
	if (t_event) {
		errout("Closing events thread... ");
		pthread_join(t_event,NULL);
		errout("OK\n");
	}

	/* Destroy mutex */
	pthread_mutex_destroy(&m_render);

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
	if (g_set.use_shell) interactive_shell(&g_set);

	/* Prepare and start the game. */
	plastic_start();

	/* Keep updating world until quit event is fired. */
	while (!g_quit) {
		g_wrld->Quantum();
		usleep(WORLDUSLEEP);
	}

	/* Free resources. */
	plastic_cleanup();

	printf("\n\n\nGood exit.\n");
	return 0;
}
