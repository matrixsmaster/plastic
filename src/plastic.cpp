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
/* Note: all threads are gathered here to maintain them simultaneously. */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include "plastic.h"
#include "support.h"
#include "debug.h"
#include "CurseGUI.h"
#include "world.h"
#include "LVR.h"
#include "datapipe.h"


static SGameSettings	g_set = DEFAULT_SETTINGS;
static PlasticWorld*	g_wrld = NULL;
static CurseGUI*		g_gui = NULL;
static pthread_t		t_event = 0;
static pthread_t		t_render = 0;
static pthread_t		t_loader = 0;
static pthread_mutex_t	m_render,m_resize;
static bool				g_quit = false;
static uli				g_fps = 0;


/* *********************************************************** */

static void* plastic_eventhread(void* ptr)
{
	SGUIEvent my_e;
	bool resz;

	while ((g_gui) && (!g_gui->WillClose())) {

		/* Stop rendering any screen data while processing event */
		pthread_mutex_lock(&m_render);

		/* Events pump */
		if (!g_gui->PumpEvents(&my_e)) {
			/* Resize event will interfere with LVR frame processing, so lock it */
			resz = (my_e.t == GUIEV_RESIZE);
			if (resz) pthread_mutex_lock(&m_resize);

			/* No one consumed event, need to be processed inside the core */
			g_wrld->ProcessEvents(&my_e);

			/* Unlock resize mutex if needed */
			if (resz) pthread_mutex_unlock(&m_resize);
		}

		g_wrld->GetHUD()->UpdateFPS(g_fps); //FIXME

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
		pthread_mutex_lock(&m_resize);
		lvr->Frame();
		pthread_mutex_unlock(&m_resize);

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

static void* plastic_chunksthread(void* ptr)
{
	DataPipe* pipe = reinterpret_cast<DataPipe*> (ptr);

	while (!g_quit) {

		pipe->ChunkQueue();

		usleep(CHUNKUSLEEP);
	}

	return NULL;
}

/* *********************************************************** */

static void plastic_start()
{
	int r;

	printf("Starting...\n\n");

	/* Create a world! */
	g_wrld = new PlasticWorld(&g_set);
	r = g_wrld->GetLastResult();
	if (r) {
		errout("Unable to create the world (error #%d)\n",r);
		abort();
	}

	/* Make a CurseGUI */
	g_gui = new CurseGUI();
	r = g_gui->GetLastResult();
	if (r) {
		errout("Unable to create CurseGUI (error #%d)\n",r);
		if (r == 3)
			errout("Note: setting 'TERM=xterm-256color' helps in most Linux systems.\n");
		abort();
	}

	/* Connect world to GUI */
	g_wrld->ConnectGUI(g_gui);

	/* Init debug UI */
	dbg_init(g_gui);

	/* Create mutexes */
	pthread_mutex_init(&m_render,NULL);
	pthread_mutex_init(&m_resize,NULL);

	/* Start chunks loading queue */
	pthread_create(&t_loader,NULL,plastic_chunksthread,g_wrld->GetDataPipe());

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
	if (t_loader) {
		errout("Closing chunks queue... ");
		pthread_join(t_loader,NULL);
		errout("OK\n");
	}

	/* Destroy mutexes */
	pthread_mutex_destroy(&m_render);
	pthread_mutex_destroy(&m_resize);

	/* Destroy debugging UI */
	dbg_finalize();

	/* Destroy all main classes instances */
	if (g_wrld) delete g_wrld;
	if (g_gui) delete g_gui;

	printf("\nCleanup complete.\n");
}

/* *********************************************************** */

int main(int argc, char* argv[])
{
	srand(time(NULL));

	printf(HELLOSTR,PRODNAME,VERMAJOR,VERMINOR,BUILDNO,PRODNAME);

	/* Parse and print current settings. */
	if (!argparser(argc,argv,&g_set)) {
		/* Print out some helpful information */
		arghelp(argv[0]);
		return 1;
	}
	printsettings(&g_set);

	/* Show interactive startup shell if needed. */
	if (g_set.use_shell) {
		if (!interactive_shell(&g_set)) return 0;
	}

	/* Prepare and start the game. */
	plastic_start();

	/* Keep updating world until quit event is fired. */
	while (!g_quit) {
		g_wrld->Quantum();
		usleep(WORLDUSLEEP);
	}

	/* Free resources. */
	plastic_cleanup();

	printf("\n\nGood exit.\n");
	return 0;
}
