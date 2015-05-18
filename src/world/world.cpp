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
#include "CGUIControls.h"


PlasticWorld::PlasticWorld(SGameSettings* settings)
{
	float alloc_gb;

	/* Init internal variables */
	result = 0;
	sets = settings;
	data = NULL;
	lvr = NULL;
	gui = NULL;
	PC = NULL;
	hud = NULL;
	binder = NULL;

	/* Create and set up DataPipe */
	data = new DataPipe(sets);
	if (data->GetStatus() == DPIPE_ERROR) {
		errout("Unable to initialize data pipe.\n");
		result = 1;
		return;
	}
	alloc_gb = (float)(data->GetAllocatedRAM()) / 1024.f / 1024.f / 1024.f;
	printf("Size of voxel = %lu bytes\n",sizeof(voxel));
	printf("Core allocated memory: %llu bytes (%.3f GiB)\n",data->GetAllocatedRAM(),alloc_gb);

	/* Bind keyboard */
	BindKeys();

	/* Create LVR */
	lvr = new LVR(data);

	/* Create Player */
	PC = new Player(sets->PCData,data);

	//FIXME: debugging stuff
//	PC->SetGPos(vector3di(data->GetGlobalSurfaceSize().X/2,
//			data->GetGlobalSurfaceSize().Y/2,
//			sets->world_r - WGAIRCHUNKS - 1));
	PC->SetGPos(data->GetInitialPCGPos());
	PC->SetPos(vector3di(128,90,135));

	//TODO: move this to the first quantum of the world update cycle
	puts("\nGenerating the first parts of the world...");
	data->SetGP(PC->GetGPos());

	test = data->LoadModel("testmodel.dat",vector3di(128,100,135));
	if (!test) abort();
	lvr->SetPosition(vector3d(128,90,135));
}

PlasticWorld::~PlasticWorld()
{
	//Game data
	if (PC) delete PC;
	RemoveAllActors();

	//UI parts
	if (binder) delete binder;
	if (hud) delete hud;
	if (lvr) delete lvr;

	//The last one: DataPipe
	if (data) delete data;
}

void PlasticWorld::Quantum()
{
	//TODO: world update (!)

	//DEBUG:
//	if (data->TryLock()) return;
	data->Lock();
	if (test->GetState() == 0)
		test->SetState(test->GetNumStates()-1);
	else
		test->SetState(test->GetState()-1);
	data->Unlock();
}

void PlasticWorld::ConnectGUI(CurseGUI* guiptr)
{
	gui = guiptr;
	ConnectGUI();
}

void PlasticWorld::ConnectGUI()
{
	int nw,nh;

	if ((!gui) || (!lvr)) {
		result = 1;
		return;
	}

	//resize LVR frame
	nw = gui->GetWidth();
	nh = gui->GetHeight();
	if (!lvr->Resize(nw,nh)) {
		errout("Unable to resize LVR frame!\n");
		result = 2;
		return;
	}

	//Connect lvr output to CurseGUI main background
	gui->SetBackgroundData(lvr->GetRender(),lvr->GetRenderLen());
	lvr->SetMask(gui->GetBackmask(),nw,nh);

	//Update HUD sizes, positions etc (reset)
	if (hud) delete hud;
	hud = new HUD(gui);

	//OK
	result = 0;
}

bool PlasticWorld::CreateActor()
{
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

	result = 0;
	switch (e->t) {
	case GUIEV_KEYPRESS:
		/* User pressed a key */
		switch (binder->DecodeKey(e->k)) {
		case 0: /*console*/
			dbg_toggle();
			break;

		case 1: /*PC stats tab*/
			//TODO
			break;

		case 2: /*map view*/
			SPAWNWNDMACRO("Map View",new CurseGUIMapViewWnd(gui,data));
			(reinterpret_cast<CurseGUIMapViewWnd*>(wptr))->SetPos(PC->GetGPos(),PC->GetPos());
			break;

		case 3: /*inventory*/
			SPAWNWNDMACRO("Inventory",new CurseGUIInventoryWnd(gui,PC->GetInventory()));
			break;

		case 4: /*LVR config*/
			SPAWNWNDMACRO("LVR config",new CurseGUIRenderConfWnd(gui,lvr));
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
			case 'x':
				break;
			}
		}
		test->SetRot(tr);

		PC->ProcessEvent(e);
		lvr->SetEulerRotation(PC->GetRot().ToReal());
		lvr->SetPosition(PC->GetPos().ToReal());
		break;

	case GUIEV_RESIZE:
		/* Size of terminal has changed */
		ConnectGUI();
		break;

	case GUIEV_MOUSE:
		/* Nothing to do now */
		break;

	case GUIEV_CTLBACK:
		/* Nothing to do now */
		break;

	default:
		errout("Warning: unknown event type pumped. Possibly memory corruption.\n");
		result = 1;
	}
}
