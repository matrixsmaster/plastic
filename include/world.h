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

#include "vecmath.h"
#include "CurseGUI.h"
#include "plastic.h"
#include "datapipe.h"
#include "LVR.h"
#include "actor.h"
#include "hud.h"


class PlasticWorld {
private:
	int result;
	SGameSettings* sets;
	DataPipe* data;
	LVR* lvr;
	CurseGUI* gui;
	Player* PC;
	HUD* hud;

	//FIXME: delete them
	float scale;
	vector3d fov;
	int far;

public:
	PlasticWorld(SGameSettings* settings);
	virtual ~PlasticWorld();

	///Main method for incremental updating world state.
	void Quantum();

	///Returns result of latest operation (mainly for outsiders).
	int GetLastResult()						{ return result; }

	///Connects CurseGUI to World.
	void ConnectGUI(CurseGUI* guiptr);

	///Restore or update CurseGUI connection.
	void ConnectGUI();

	///Returns a pointer to renderer (for outside use).
	LVR* GetRenderer()						{ return lvr; }

	///Returns HUD pointer.
	HUD* GetHUD()							{ return hud; }

	///Main events processing facility.
	void ProcessEvents(const CGUIEvent* e);
};


#endif /* WORLD_H_ */
