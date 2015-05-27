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

#include "world.h"
#include "support.h"
#include "debug.h"
#include "CGUIEvents.h"
#include "CGUIControls.h"


PlasticWorld::PlasticWorld(SGameSettings* settings)
{
	float alloc_gb;

	/* Init internal variables */
	result = 0;
	sets = settings;
	data = NULL;
	render = NULL;
	gui = NULL;
	PC = NULL;
	hud = NULL;
	binder = NULL;
	once = false;
	g_w = g_h = 0;

	/* Create and set up DataPipe */
	data = new DataPipe(sets);
	if (data->GetStatus() == DPIPE_ERROR) {
		errout("Unable to initialize data pipe.\n");
		result = 1;
		return;
	}
	alloc_gb = (float)(data->GetAllocatedRAM()) / 1024.f / 1024.f / 1024.f;
	printf("Size of voxel = %lu bytes\n",sizeof(voxel));
	printf("Chunks buffer capacity = %d * (%d ^ 3) voxels\n",HOLDCHUNKS,CHUNKBOX);
	printf("Initially allocated memory: %llu bytes (%.3f GiB)\n",data->GetAllocatedRAM(),alloc_gb);

	/* Bind keyboard */
	BindKeys();

	/* Create render pool */
	render = new RenderPool(data);

	/* Create Player */
	PC = new Player(sets->PCData,data);
	PC->SetGPos(data->GetInitialPCGPos());
	PC->SetPos(data->GetInitialPCLPos());
}

PlasticWorld::~PlasticWorld()
{
	//Game data
	if (PC) delete PC;
	RemoveAllActors();

	//UI parts
	if (binder) delete binder;
	if (hud) delete hud;
	if (render) delete render;

	//The last one: DataPipe
	if (data) delete data;
}

void PlasticWorld::Quantum()
{
	//TODO: world update (!)
	if (!once) {
		once = true;

		data->SetGP(PC->GetGPos());
		render->SetPos(PC->GetPos());

		//FIXME: debugging stuff
		test = data->LoadModel("testmodel.dat",vector3di(128,100,135));
		if (!test) abort();
	}

#if 0
	//DEBUG:
	data->Lock();
//	if (data->TryLock()) return;
	if (test->GetState() == 0)
		test->SetState(test->GetNumStates()-1);
	else
		test->SetState(test->GetState()-1);
	data->Unlock();
#endif
}

void PlasticWorld::Frame()
{
	if ((!gui) || (!render)) return;

	gui->SetBackgroundData(render->GetRender(),render->GetRenderLen());
	render->SetMask(gui->GetBackmask(),g_w,g_h);

	//TODO: make precision clock inside PlasticWorld (for atmo and physics). Move FPS calc here.
}

void PlasticWorld::ConnectGUI(CurseGUI* guiptr)
{
	gui = guiptr;
	ConnectGUI();
}

void PlasticWorld::ConnectGUI()
{
	if ((!gui) || (!render)) {
		result = 1;
		return;
	}

	//resize frame
	g_w = gui->GetWidth();
	g_h = gui->GetHeight();
	if (!render->Resize(g_w,g_h)) {
		errout("Unable to resize LVR frame!\n");
		result = 2;
		return;
	}

	//Update HUD sizes, positions etc (reset)
	if (hud) delete hud;
	hud = new HUD(gui);

	//OK
	result = 0;
}

bool PlasticWorld::CreateActor()
{
	//FIXME: debug only
	PlasticActor* npc = new PlasticActor(PCLS_COMMONER,PBOD_PNEUMO,data);
	npc->SetPos(PC->GetPos());
	return (npc->Spawn());
}

void PlasticWorld::RemoveAllActors()
{
	std::vector<PlasticActor*>::iterator it;
	for (it = actors.begin(); it != actors.end(); ++it)
		delete ((*it));
	actors.clear();
}

void PlasticWorld::BindKeys()
{
	if (binder) delete binder;
	binder = new KeyBinder(data);

	binder->RegKeyByName("CONSOLE");
	binder->RegKeyByName("CHAR_TAB");
	binder->RegKeyByName("MAP_VIEW");
	binder->RegKeyByName("INVENTORY");
	binder->RegKeyByName("RENDER_CFG");
}

#define SPAWNWNDMACRO(Name,Create) \
							wptr = gui->GetWindowN(Name); \
							if (!wptr) { \
								wptr = Create; \
								gui->AddWindow(wptr); } \
							gui->SetFocus(wptr);

void PlasticWorld::ProcessEvents(SGUIEvent* e)
{
	CurseGUIWnd* wptr;

	//DEBUG:
	vector3d tr = test->GetRot();
	vector3di x;
	char s[128];

	result = 0;

	if (PC->ProcessEvent(e)) {
		render->SetRot(PC->GetRot());
		render->SetPos(PC->GetPos());
		return;
	}

	switch (e->t) {
	case GUIEV_KEYPRESS:
		/* User pressed a key */
		switch (binder->DecodeKey(e->k)) {
		case 0: /*console*/
			dbg_toggle();
			break;

		case 1: /*PC stats tab*/
			SPAWNWNDMACRO("Actor View",new CurseGUIActorViewWnd(gui,PC));
			break;

		case 2: /*map view*/
			SPAWNWNDMACRO("Map View",new CurseGUIMapViewWnd(gui,data));
			(reinterpret_cast<CurseGUIMapViewWnd*>(wptr))->SetPos(PC->GetGPos(),PC->GetPos());
			break;

		case 3: /*inventory*/
			SPAWNWNDMACRO("Inventory",new CurseGUIInventoryWnd(gui,PC->GetInventory()));
			break;

		case 4: /*LVR config*/
			SPAWNWNDMACRO("LVR config",new CurseGUIRenderConfWnd(gui,render));
			break;

		default:
			/* DEBUGging stuff */
			switch (e->k) {
			case '1': tr.X -= 2; break;
			case '2': tr.X += 2; break;
			case '3': tr.Y += 2; break;
			case '4': tr.Y -= 2; break;
			case '5': tr.Z -= 2; break;
			case '6': tr.Z += 2; break;
			case KEY_F(5):
					if (CreateActor()) dbg_print("CA Success");
					else dbg_print("CA FAILED");
					return;
			case KEY_F(4):
					gui->GetColorManager()->Flush();
					printf("TESTING: YOU SHOULDN'T SEE THIS!!!");
					redrawwin(gui->GetWindow());
					return;
			case 'z': //KEY_F(2): //tmp
				if(hud) {
					if(hud->GetTransparent()) {
						hud->SetTransparent(false);
					} else hud->SetTransparent(true);
				}
				break;
			}
		}
		test->SetRot(tr); //FIXME: debug only
		break;

	case GUIEV_RESIZE:
		/* Size of terminal has changed */
		ConnectGUI();
		break;

	case GUIEV_MOUSE:
		/* Mouse */
		if (e->m.bstate & GUIMOUS_LEFT) {
			curso.X = e->m.x;
			curso.Y = e->m.y;

			x = render->GetProjection(curso);
			snprintf(s,128,"%d:%d->%d:%d:%d",curso.X,curso.Y,x.X,x.Y,x.Z);
			hud->PutStrBottom(s);
			gui->SetCursorPos(curso.X,curso.Y);
		}
		break;

	case GUIEV_CTLBACK:
		/* Nothing to do now */
		break;

	default:
		errout("Warning: unknown event type pumped. Possibly memory corruption.\n");
		result = 1;
	}
}
