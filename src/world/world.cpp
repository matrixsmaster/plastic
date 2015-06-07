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


enum {
	PWKEY_CONSOLE,
	PWKEY_CHARTAB,
	PWKEY_MAPVIEW,
	PWKEY_INVENTORY,
	PWKEY_RENDCFG,
};

PlasticWorld::PlasticWorld(SGameSettings* settings)
{
	float alloc_gb;

	/* Init internal variables */
	result = -1;
	sets = settings;
	data = NULL;
	render = NULL;
	gui = NULL;
	PC = NULL;
	hud = NULL;
	radar = NULL;
	binder = NULL;
	once = false;
	g_w = g_h = 0;
	rtime = epoch = passed = 0;
	clkres = NULL;
	fps = 0;
	lock_update = true;

	/* Create and set up DataPipe */
	data = new DataPipe(sets);
	if (data->GetStatus() == DPIPE_ERROR) {
		errout("Unable to initialize data pipe.\n");
		result = 1;
		return;
	}

	/* Create Player */
	PC = new Player(sets->PCData,data);

	/* Create and init the world generator */
	if (sets->world_r < WGMINRADIUS) {
		errout("Impossibly small world radius.\n");
		result = 2;
		return;
	}
	wgen = new WorldGen(sets->world_r,data->GetVoxTable());

	/* Connect wgen to datapipe */
	data->ConnectWorldGen(wgen);

	/* Create or Load saved game data */
	if (sets->new_game) {
		if (!NewGame()) return;
	} else if (!LoadGame()) {
		result = 3;
		return;
	}

	/* Get used RAM amount */
	alloc_gb = (float)(data->GetAllocatedRAM()) / 1024.f / 1024.f / 1024.f;
	printf("Size of voxel = %lu bytes\n",sizeof(voxel));
	printf("Chunks buffer capacity = %d * (%d ^ 3) voxels\n",HOLDCHUNKS,CHUNKBOX);
	printf("Initially allocated memory: %llu bytes (%.3f GiB)\n",data->GetAllocatedRAM(),alloc_gb);

	/* Bind keyboard */
	BindKeys();

	/* Create render pool */
	render = new RenderPool(data);
	render->GetSkies()->SetTime(&gtime);

	/* Everything is OK */
	result = 0;
}

PlasticWorld::~PlasticWorld()
{
	//Game data
	SaveGame();
	if (PC) delete PC;
	RemoveAllActors();
	if (clkres) delete clkres;
	data->ConnectWorldGen(NULL); //disconnect WG
	if (wgen) delete wgen;

	//UI parts
	if (binder) delete binder;
	if (hud) delete hud;
	if (radar) delete radar;
	if (render) delete render;

	//The last one: DataPipe
	if (data) delete data;
}

void PlasticWorld::Quantum()
{
//	VModVec* objs = data->GetModels();

	//Check all instances needed
	if ((!PC) || (!hud) || (!radar)) return;

	//Check if the world is on pause
	if (lock_update) return;

	if (!once) {
		once = true;

		//Update Player, HUD etc
		PlayerMoved();

		//Show greeting string
		hud->PutStrToLog("Welcome to the world!"); //FIXME: Write something here

		//FIXME: debugging stuff
		test = data->LoadModel("testmodel.dat",PC->GetPos()+vector3di(0,0,20),data->GetGP());
		if (!test) abort();
	}

	UpdateTime();
	radar->Update();
	hud->SetHP(PC->GetStats(true).HP,PC->GetStats(false).HP);
	hud->SetCharge(PC->GetStats(true).CC,PC->GetStats(false).CC);

	data->WriteLock();
	//FIXME: debug
	if (test->GetState() == 0)
		test->SetState(test->GetNumStates()-1);
	else
		test->SetState(test->GetState()-1);
	data->WriteUnlock();
}

void PlasticWorld::Frame()
{
	if ((!gui) || (!render)) return;

	gui->SetBackgroundData(render->GetRender(),render->GetRenderLen());
	render->SetMask(gui->GetBackmask(),g_w,g_h);

	gtime.fr++;

	hud->SetFPS(fps);
	hud->UpdateClock();
}

void PlasticWorld::ConnectGUI(CurseGUI* guiptr)
{
	gui = guiptr;
	ConnectGUI();
}

void PlasticWorld::ConnectGUI()
{
	lock_update = true;

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
	hud->SetPTime(&gtime);

	//Reset radar
	if (radar) delete radar;
	radar = new PlasticRadar(data);
	radar->SetWH(hud->GetMapWidth(),hud->GetMapHeight());
	radar->SetFOV(render->GetFOV());
	radar->Update();
	hud->SetMap(radar->GetImage(),radar->GetImageLen());

	//OK
	lock_update = false;
	result = 0;
}

bool PlasticWorld::CreateActor()
{
	//FIXME: debug only
	PlasticActor* npc = new PlasticActor(PCLS_COMMONER,PBOD_PNEUMO,data);
	actors.push_back(npc);
	npc->SetPos(PC->GetPos());
	npc->SetGPos(PC->GetGPos());
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

	binder->RegKeyByName("CONSOLE",PWKEY_CONSOLE);
	binder->RegKeyByName("CHAR_TAB",PWKEY_CHARTAB);
	binder->RegKeyByName("MAP_VIEW",PWKEY_MAPVIEW);
	binder->RegKeyByName("INVENTORY",PWKEY_INVENTORY);
	binder->RegKeyByName("RENDER_CFG",PWKEY_RENDCFG);
}

void PlasticWorld::UpdateTime()
{
	ulli now;
	timespec cur;

	//get current time
	result = 0;
	clock_gettime(CLOCK_MONOTONIC_RAW,&cur);
	now = cur.tv_sec * PLTIMEMS + (cur.tv_nsec / PLTIMEUS);

	if (!clkres) {
		/* Check resolution on first call */
		clkres = new timespec;
		clock_getres(CLOCK_MONOTONIC_RAW,clkres);
		if (clkres->tv_nsec > PLTIMEMINRES) result = 1;
		epoch = now;
	}

	//calculate relative time and time gap
	now -= epoch;
	passed = now - rtime;
	rtime = now;

	//update real-time part of game time
	gtime.rms += passed;

	//deal with frame-time and game seconds
	if (gtime.rms >= PLTIMEMS) {
		gtime.rms = 0;
		fps = gtime.fr;
		gtime.fr = 0;
		gtime.sc += PLTIMESCALE;
	}

	//calculate game time
	gtime.mn += gtime.sc / 60;
	gtime.sc = gtime.sc % 60;
	gtime.hr += gtime.mn / 60;
	gtime.mn = gtime.mn % 60;
	gtime.day += gtime.hr / PLTIMEDAYLEN;
	gtime.hr = gtime.hr % PLTIMEDAYLEN;
	gtime.month += gtime.day / (PLTIMENUMWEEKS * PLTIMENUMDAYS);
	gtime.day = gtime.day % (PLTIMENUMWEEKS * PLTIMENUMDAYS);
	gtime.year += gtime.month / PLTIMENUMMONTHS;
	gtime.month = gtime.month % PLTIMENUMMONTHS;
	gtime.dow = (EPlDayOfWeek)(gtime.day % PLTIMENUMDAYS);
}

SWRayObjIntersect* PlasticWorld::ScreenRay(const vector2di p)
{
	std::vector<PlasticActor*>::iterator oi;

	cinters.pnt = render->GetProjection(p);
	cinters.model = NULL;
	cinters.actor = NULL;
	if (cinters.pnt != vector3di(-1)) {
		data->IntersectModel(&(cinters.pnt),&(cinters.model),true);
		if (cinters.model) {
			for (oi = actors.begin(); oi != actors.end(); ++oi) {
				if ((*oi)->GetModel() == cinters.model) {
					cinters.actor = *oi;
					break;
				}
			}
		}
	}

	return &cinters;
}

void PlasticWorld::PlayerMoved()
{
	if (PC->GetGMov() != vector3di()) {
		if (data->Move(PC->GetGMov())) dbg_logstr("Move success");
		else dbg_logstr("Move failed");
		PC->GMove();
		PC->SetGPos(data->GetGP());
		PC->SetScenePos(data->GetGP());
	}

	render->SetRot(PC->GetRot());
	render->SetPos(PC->GetPos());

	hud->SetGPos(PC->GetGPos());
	hud->SetLPos(PC->GetPos());
	hud->SetState(PC->GetStateStr());

	radar->SetCenter(PC->GetPos());
	radar->SetRotation(PC->GetRot());
}

#define SPAWNWNDMACRO(Name,Create) { \
							wptr = gui->GetWindowN(Name); \
							if (!wptr) { \
								wptr = Create; \
								gui->AddWindow(wptr); } \
							gui->SetFocus(wptr); }

void PlasticWorld::ProcessEvents(SGUIEvent* e)
{
	CurseGUIWnd* wptr;

	//FIXME: DEBUG:
	vector3di tr = test->GetRot();
	vector3di x;
	char s[128];

	result = 1;

	//Check all instances needed
	if ((!PC) || (!hud) || (!radar)) return;

	result = 0;

	//Try to pass event directly to Player
	if (PC->ProcessEvent(e)) {
		/* Player movement */
		PlayerMoved();
		return;
	}

	//The event is more global
	switch (e->t) {
	case GUIEV_KEYPRESS:
		/* User pressed a key */
		switch (binder->DecodeKey(e->k)) {
		case PWKEY_CONSOLE: /*console*/
			dbg_toggle();
			break;

		case PWKEY_CHARTAB: /*PC/NPC stats tab*/
			if (cinters.actor && cinters.model)
				SPAWNWNDMACRO(WNDNAM_ACTVIEW,new CurseGUIActorViewWnd(gui,cinters.actor))
			else
				SPAWNWNDMACRO(WNDNAM_ACTVIEW,new CurseGUIActorViewWnd(gui,PC));
			break;

		case PWKEY_MAPVIEW: /*map view*/
			SPAWNWNDMACRO(WNDNAM_MAPVIEW,new CurseGUIMapViewWnd(gui,data));
			(reinterpret_cast<CurseGUIMapViewWnd*>(wptr))->SetPos(PC->GetGPos(),PC->GetPos());
			break;

		case PWKEY_INVENTORY: /*inventory*/
			SPAWNWNDMACRO(WNDNAM_INVENTORY,new CurseGUIInventoryWnd(gui,PC->GetInventory()));
			break;

		case PWKEY_RENDCFG: /*LVR config*/
			SPAWNWNDMACRO(WNDNAM_LVRCONF,new CurseGUIRenderConfWnd(gui,render));
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
			case 'z':
				if (hud) hud->SetAlpha(OVRL_LOG, 0);
				break;
			case 'x':
				if (hud) hud->SetAlpha(OVRL_LOG, 0.5);
				break;
			case 'c':
				if (hud) hud->ToggleMisc();
				break;
			case 'v':
				SPAWNWNDMACRO("Test One",new CurseGUIMessageBox(gui,"Test One","Testing","OK|Cancel|Something else|No|Yes"));
				break;

			case 'b':
				SPAWNWNDMACRO("Message",new CurseGUIMessageBox(gui,NULL,"Some fucking long text. I don't know what to write in here, but this string SHOULD be somewhat longer than possible to contain in 50% of ncurses window. There.",NULL));
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

			//FIXME: debug only
			ScreenRay(curso);
			x = cinters.pnt;
			if (cinters.model) {
				if (cinters.actor)
					snprintf(s,128,"%d:%d Actor: %s",curso.X,curso.Y,cinters.actor->GetAttributes().name);
				else
					snprintf(s,128,"%d:%d->%d:%d:%d (%p)",curso.X,curso.Y,x.X,x.Y,x.Z,cinters.model);
			} else
				snprintf(s,128,"%d:%d->%d:%d:%d",curso.X,curso.Y,x.X,x.Y,x.Z);
			hud->PutStrToLog(s);
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

	//FIXME: debug
	wptr = gui->GetWindowN("Message");
	if (wptr)
		dbg_print("'Message' window returned %d",((CurseGUIMessageBox*)wptr)->GetButtonPressed());
	wptr = gui->GetWindowN("Test One");
	if (wptr)
		dbg_print("'Test One' window returned %d",((CurseGUIMessageBox*)wptr)->GetButtonPressed());
}
