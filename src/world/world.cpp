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
		errout("Unable to initialize data pipe. Possibly invalid root directory.\n");
		result = 1;
		return;
	}
	alloc_gb = (float)(data->GetAllocatedRAM()) / 1024.f / 1024.f / 1024.f;
	printf("Size of voxel = %lu bytes\n",sizeof(voxel));
	printf("Allocated data pipe memory: %llu bytes (%.3f GiB)\n",data->GetAllocatedRAM(),alloc_gb);

	/* Create LVR */
	lvr = new LVR(data);

	/* Create Player */
	PC = new Player(sets->PCData,data);

	//FIXME: debugging stuff
	scale = 0.33;
	lvr->SetScale(0.33);
	far = DEFFARPLANE;
	fog = DEFFOGPLANE;
	fov = vector3d(DEFFOVX,DEFFOVY,1);
	data->SetGP(vector3dulli(0));
	test = data->LoadModel("testmodel.dat",vector3di(128,100,135));
	if (!test) abort();
	lvr->SetPosition(vector3d(128,90,135));
	PC->SetPos(vector3di(128,90,135));
	lvr->SetFogStart(fog);
	lvr->SetFogColor(vector3di(100));
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
	PlasticActor* npc = new PlasticActor(PCLS_COMMONER,data);
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

void PlasticWorld::ProcessEvents(SGUIEvent* e)
{
	//DEBUG:
	vector3d tr = test->GetRot();
	CurseGUIButton* btn;

	result = 0;
	switch (e->t) {
	case GUIEV_KEYPRESS:
		/* User pressed a key */
		switch (e->k) { //FIXME: use binder
		/* DEBUGging stuff */
		case '`': dbg_toggle(); break;
		case '[': scale -= 0.01; break;
		case ']': scale += 0.01; break;
		case ',': fov.X -= 0.1; break;
		case '.': fov.X += 0.1; break;
		case 'n': fov.Y -= 0.1; break;
		case 'm': fov.Y += 0.1; break;
		case '<': fov.X -= 1; break;
		case '>': fov.X += 1; break;
		case 'N': fov.Y -= 1; break;
		case 'M': fov.Y += 1; break;
		case ';': far--; break;
		case '\'': far++; break;
		case 't': fog--; break;
		case 'y': fog++; break;
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

		lvr->SetScale(scale);
		lvr->SetFOV(fov);
		lvr->SetFarDist(far);
		lvr->SetFogStart(fog);

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
		//DEBUG:
		if (e->b.t == GUIFB_SWITCHED) {
			btn = (CurseGUIButton*)e->b.ctl;
			if (btn) {
				btn->SetCaption("OK");
			}
		}
		break;

	default:
		errout("Warning: unknown event type pumped. Possibly memory corruption.\n");
		result = 1;
	}
}
