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

/* PlasticWorld is the core game class, which implements most of world/user interactions,
 * as well as holding crucial game facilities, like renderer, datapipe etc.
 */

#ifndef WORLD_H_
#define WORLD_H_

#include <vector>
#include <time.h>
//#include <pthread.h>
#include "plastic.h"
#include "vecmath.h"
#include "CurseGUI.h"
#include "misconsts.h"
#include "datapipe.h"
#include "LVR.h"
#include "renderpool.h"
#include "actor.h"
#include "vmodel.h"
#include "hud.h"
#include "keybinder.h"
#include "pltime.h"
#include "radar.h"
#include "wrldgen.h"
#include "society.h"
#include "pphysic.h"
#include "gamemsg.h"


/* Ray-Object intersection data holding structure */
struct SWRayObjIntersect {
	vector3di pnt;
	VModel* model;
	PlasticActor* actor;
};

/* This class is more or less the game itself. It connects all other instances, like
 * DataPipe, LVR, GUI etc to make the World.
 */
class PlasticWorld {
private:
	//core variables and instances
	int result;					//FIXME: comment
	SGameSettings* sets;
	DataPipe* data;
	RenderPool* render;
	CurseGUI* gui;
	WorldGen* wgen;
	Player* PC;
	HUD* hud;
	PlasticRadar* radar;
	KeyBinder* binder;
	bool once;
	int g_w,g_h;
	vector2di curso;
	PlasticTime gtime;
	ulli rtime,epoch,passed;
	timespec* clkres;
	uli fps;
	SWRayObjIntersect cinters;
	volatile bool lock_update;
	SSavedGameHeader gamesave;
	PlasticSociety* society;
	PlasticPhysics* physics;
	GameMessages* msgsys;
	float timescale;
//	pthread_mutex_t wrldmtx;

	//FIXME: DEBUG
	VModel* test;

	//internal facilities
	void BindKeys();
	void UpdateActorsPresence();
	void PlayerMoved();

	bool NewGame();
	bool LoadGame();
	bool SaveGame();
	void PushRenderConf();
	void PullRenderConf();

public:
	PlasticWorld(SGameSettings* settings);
	virtual ~PlasticWorld();

	///Returns result of latest operation.
	int GetLastResult()						{ return result; }

	///Realtime clock.
	ulli GetTime()							{ return rtime; }

	///Game clock.
	PlasticTime GetGameTime()				{ return gtime; }

	///Game clock.
	const PlasticTime* GetGameTimePtr()		{ return &gtime; }

	///Main clock facility.
	void UpdateTime();

	///Main method for incremental updating world state.
	void Quantum();

	///Generate new rendered frame and blit it to main GUI background.
	void Frame();

	///Stops all the rendering processes.
	void StopRendering();

	///Starts rendering the world.
	void StartRendering();

	///Connects the game world to CurseGUI.
	void ConnectGUI(CurseGUI* guiptr);

	///Restore or update CurseGUI connection.
	void ConnectGUI();

	///Returns a renderer instance.
	RenderPool* GetRenderer()				{ return render; }

	///Returns HUD instance.
	HUD* GetHUD()							{ return hud; }

	///Returns main DataPipe instance.
	DataPipe* GetDataPipe()					{ return data; }

	///Returns active physics engine instance.
	PlasticPhysics* GetPhyEngine()			{ return physics; }

	///Returns a game text messages system instance.
	GameMessages* GetMsgSys()				{ return msgsys; }

	///Screen space - world intersection test.
	SWRayObjIntersect* ScreenRay(const vector2di p);

	///Main events processing facility.
	void ProcessEvents(SGUIEvent* e);
};


#endif /* WORLD_H_ */
