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


class PlasticWorld {
private:
	//core variables and instances
	int result;					//FIXME: comment
	SGameSettings* sets;
	DataPipe* data;
	RenderPool* render;
	CurseGUI* gui;
	Player* PC;
	HUD* hud;
	KeyBinder* binder;
	bool once;
	int g_w,g_h;
	vector2di curso;
	PlasticTime gtime;
	uli fps;
	volatile uli frames;

	//holders
	std::vector<PlasticActor*> actors;

	//FIXME: DEBUG
	VModel* test;

	//internal facilities
	void BindKeys();
	bool CreateActor();
	void RemoveAllActors();
	void UpdateTime();

public:
	PlasticWorld(SGameSettings* settings);
	virtual ~PlasticWorld();

	///Realtime clock.
	ulli GetTime();

	///Game clock.
	PlasticTime GetGameTime()				{ return gtime; }

	///Game clock.
	const PlasticTime* GetGameTimePtr()		{ return &gtime; }

	///Main method for incremental updating world state.
	void Quantum();

	///Generate new rendered frame and blit it to main GUI background.
	void Frame();

	///Returns result of latest operation (mainly for outsiders).
	int GetLastResult()						{ return result; }

	///Connects the game world to CurseGUI.
	void ConnectGUI(CurseGUI* guiptr);

	///Restore or update CurseGUI connection.
	void ConnectGUI();

	///Returns a pointer to renderer (for outside use).
	RenderPool* GetRenderer()				{ return render; }

	///Returns HUD pointer.
	HUD* GetHUD()							{ return hud; }

	///Returns DataPipe instance currently in use.
	DataPipe* GetDataPipe()					{ return data; }

	///Main events processing facility.
	void ProcessEvents(SGUIEvent* e);
};


#endif /* WORLD_H_ */
