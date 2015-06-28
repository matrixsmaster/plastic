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
/* Note: almost all the threads are gathered here to maintain them easier. */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include "plastic.h"
#include "misconsts.h"
#include "support.h"
#include "utils.h"
#include "debug.h"
#include "CurseGUI.h"
#include "world.h"
#include "datapipe.h"
#include "pltime.h"
#include "pphysic.h"


static SGameSettings	g_set = DEFAULT_SETTINGS;	/* The game settings holder */
static PlasticWorld*	g_wrld = NULL;				/* The world instance */
static CurseGUI*		g_gui = NULL;				/* The ncurses GUI instance */
static pthread_t		t_event = 0;				/* Events thread identifier */
static pthread_t		t_render = 0;				/* Screen rendering thread id */
static pthread_t		t_loader = 0;				/* Background chunks loader thread id */
static pthread_t		t_physic = 0;				/* Physics engine thread id */
static pthread_mutex_t	m_render;					/* Screen rendering / events processing mutex */
static int				g_frame = 0;				/* Screen frames counter for redrawing failsafe */
static int				g_wres = 0;					/* The last operation result code of the World instance*/
volatile bool			g_quit = false;				/* Global quit event flag */


/* *********************************************************** */

static void* plastic_eventhread(void* ptr)
{
	SGUIEvent my_e;
	CurseGUIMessageBox* mbox;
	int btn;

	while ((g_gui) && (!g_quit)) {

		/* Stop rendering any screen data while processing event */
		pthread_mutex_lock(&m_render);

		/* Events pump */
		if (!g_gui->PumpEvents(&my_e)) {
			/* No one consumed event, need to be processed inside the core */
			g_wrld->ProcessEvents(&my_e);
		}

		/* Deal with Quit event (Soft Reset) */
		if (g_gui->WillClose()) {
			/* Pause the world */
			g_wrld->StopUpdating();
			/* Show confirmation message box */
			mbox = new CurseGUIMessageBox(g_gui,"Quit","Are you sure you want to quit?","YES|NO");
			g_gui->AddWindow(mbox);
			while (mbox && (((btn = mbox->GetButtonPressed())) < 0)) {
				/* GUI idle cycle */
				usleep(EVENTUSLEEP);
				g_gui->PumpEvents(&my_e);
				g_gui->Update(true);
			}
			if (btn) {
				/* Soft reset */
				g_wrld->StopRendering();	//stop rendering
				dbg_finalize();				//disconnect debug UI from GUI
				g_wrld->ConnectGUI(NULL);	//disconnect world from GUI
				g_gui->SoftReset();			//soft-reset the GUI (all windows will be removed)
				g_gui->Update(false);		//dummy screen frame rendering
				dbg_init(g_gui);			//re-connect debug UI
				g_wrld->ConnectGUI(g_gui);	//re-connect world to GUI
				g_wrld->StartUpdating();	//restart world updates
			} else
				/* Just set the quit event flag */
				g_quit = true;
		}

		pthread_mutex_unlock(&m_render);

		/* Check DataPipe status */
		if (g_wrld->GetDataPipe()->GetStatus() == DPIPE_ERROR) {
			errout("DataPipe error detected. Program will be terminated.");
			g_quit = true;
		}

		/* To keep CPU load low(er) */
		usleep(EVENTUSLEEP);
	}

	pthread_exit(NULL);
}

static void* plastic_renderthread(void* ptr)
{
	while (!g_quit) {
		pthread_mutex_lock(&m_render);

		g_wrld->Frame();		//fast call, update HUD and select next frame
		g_gui->Update(true);	//slow call, draw everything to terminal

		/* Hack around ncurses optimization bug */
		if (++g_frame > OPTIMFRAMES) {
			redrawwin(g_gui->GetWindow());
			g_frame = 0;
		}

		pthread_mutex_unlock(&m_render);

		/* Make some room between two frames */
		usleep(UPDATUSLEEP);
	}

	pthread_exit(NULL);
}

static void* plastic_chunksthread(void* ptr)
{
	DataPipe* pipe = reinterpret_cast<DataPipe*> (ptr);

	while (!g_quit) {
		/* Move chunks loading queue */
		pipe->ChunkQueue();
		usleep(CHUNKUSLEEP);
	}

	pthread_exit(NULL);
}

static void* plastic_physicsthread(void* ptr)
{
	PlasticPhysics* phy = reinterpret_cast<PlasticPhysics*> (ptr);

	while (!g_quit) {
		/* Update world's physics */
		phy->Quantum();
		usleep(PHYSSUSLEEP);
	}

	pthread_exit(NULL);
}

/* *********************************************************** */

static void plastic_preresize(void)
{
	/* This function stops rendering to not interfere with resizing,
	 * which can (and should) re-allocate many buffers in memory,
	 * effectively resulting in SIGSEGVs. So we decided to make a
	 * simple kludge to prevent such event.
	 */
	g_wrld->StopRendering();
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
	g_wrld->SetDestRet(&g_wres);

	/* Check main clock */
	g_wrld->UpdateTime();
	if (g_wrld->GetLastResult()) {
		errout("Unacceptably low system clock resolution!\n");
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

	/* Init debug UI */
	dbg_init(g_gui);

	/* Connect the World to the GUI */
	g_wrld->ConnectGUI(g_gui);

	/* Register pre-resize callback (it's a kludge, I know) */
	g_gui->SetPreResizeCallback(plastic_preresize);

	/* Create rendering lock */
	pthread_mutex_init(&m_render,NULL);

	/* Start chunks loading queue */
	pthread_create(&t_loader,NULL,plastic_chunksthread,g_wrld->GetDataPipe());

	/* Start events processing thread */
	pthread_create(&t_event,NULL,plastic_eventhread,NULL);

	/* Start physics engine thread */
	pthread_create(&t_physic,NULL,plastic_physicsthread,g_wrld->GetPhyEngine());

	/* Start rendering thread */
	pthread_create(&t_render,NULL,plastic_renderthread,NULL);
}

#define PLA_CLOSETHREAD(Name,Hdl) \
		if (Hdl) { \
			errout("Closing %s thread... ",Name); \
			pthread_join(Hdl,NULL); \
			errout("OK\n"); \
		}

static void plastic_cleanup()
{
	ulli gamelength = g_wrld->GetTime();

	/* Join all active threads */
	PLA_CLOSETHREAD("render",t_render);
	PLA_CLOSETHREAD("physics",t_physic);
	PLA_CLOSETHREAD("events",t_event);
	PLA_CLOSETHREAD("chunks",t_loader);

	/* Destroy rendering lock */
	pthread_mutex_destroy(&m_render);

	/* Destroy debugging UI */
	dbg_finalize();

	/* Destroy all main classes instances */
	/* Note: there's a debug sequence, to be able to review
	 * game saving process. So we should destroy GUI before
	 * destroying the world, to release ncurses windows and
	 * return terminal to its original state.
	 */
	if (g_wrld) g_wrld->ConnectGUI(NULL); //disconnect GUI
	if (g_gui) delete g_gui;
	errout("\nGUI destroyed\nDestroying world... ");
	fflush(stderr); //to make sure output order is right
	if (g_wrld) delete g_wrld;
	errout("OK\n");

	/* Print out the length of game session */
	printf("\nGame session time = %llu sec.\n",gamelength / PLTIMEMS);

	printf("\nCleanup complete.\n");
}

/* *********************************************************** */

int main(int argc, char* argv[])
{
	/* Init default pseudo-random numbers generator */
	srand(time(NULL));

	/* Show the banner */
	printf(HELLOSTR,PRODNAME,VERMAJOR,VERMINOR,VERSUBVR,BUILDNO,PRODNAME);

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

	/* Print exit statement and exit */
	if (g_wres) {
		printf("\n\nBad exit (code = 0x%x)\n",g_wres);
		return 1;
	} else {
		printf("\n\nGood exit.\n");
		return 0;
	}
}
