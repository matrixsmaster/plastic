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

#include <stdlib.h>
#include "actor.h"
#include "support.h"


enum {
	PAKEY_WALK_FORW,
	PAKEY_WALK_BACK,
	PAKEY_WALK_LEFT,
	PAKEY_WALK_RGHT,
	PAKEY_WALK_UP,
	PAKEY_WALK_DW,
	PAKEY_RUN_FORW,
	PAKEY_RUN_BACK,
	PAKEY_RUN_LEFT,
	PAKEY_RUN_RGHT,
	PAKEY_RUN_UP,
	PAKEY_RUN_DW,
	PAKEY_TURN_LEFT,
	PAKEY_TURN_RGHT,
	PAKEY_TURN_UP,
	PAKEY_TURN_DW,
	PAKEY_TOG_STATE,
};

/* Player state to string conversion data */
static const char* pcstate_to_str[PCHARNUMSTATES] = {
		"Exploring",
		"Interacting",
		"Combat",
		"In vehicle"
};

Player::Player(SPAAttrib s, DataPipe* pptr) :
		PlasticActor(s,pptr)
{
	char tmp[MAXINISTRLEN];

	isnpc = false;
	state = PCS_EXPLORING;

	binder = new KeyBinder(pptr);

	binder->RegKeyByName("WALK_FORW",PAKEY_WALK_FORW);
	binder->RegKeyByName("WALK_BACK",PAKEY_WALK_BACK);
	binder->RegKeyByName("WALK_LEFT",PAKEY_WALK_LEFT);
	binder->RegKeyByName("WALK_RGHT",PAKEY_WALK_RGHT);

	binder->RegKeyByName("RUN_FORW",PAKEY_RUN_FORW);
	binder->RegKeyByName("RUN_BACK",PAKEY_RUN_BACK);
	binder->RegKeyByName("RUN_LEFT",PAKEY_RUN_LEFT);
	binder->RegKeyByName("RUN_RGHT",PAKEY_RUN_RGHT);

	binder->RegKeyByName("TURN_LEFT",PAKEY_TURN_LEFT);
	binder->RegKeyByName("TURN_RGHT",PAKEY_TURN_RGHT);
	binder->RegKeyByName("TURN_UP",PAKEY_TURN_UP);
	binder->RegKeyByName("TURN_DW",PAKEY_TURN_DW);

	binder->RegKeyByName("TOGGLE_STATE",PAKEY_TOG_STATE);

	maxrspd = atoi(pptr->GetIniDataS(KEYBINDNAME,"TURN_MAX").c_str());
	pptr->GetIniDataC(KEYBINDNAME,"WHEEL_HORZ",tmp,sizeof(tmp));
	rot_hor = mmask_by_str(tmp);
	pptr->GetIniDataC(KEYBINDNAME,"WHEEL_VERT",tmp,sizeof(tmp));
	rot_ver = mmask_by_str(tmp);

	rspd = 4; //FIXME: debug
}

Player::~Player()
{
	delete binder;
}

const char* Player::GetStateStr()
{
	if (state < PCS_EXPLORING) state = PCS_EXPLORING;
	if (state > PCS_VEHICLE) state = PCS_VEHICLE;
	return pcstate_to_str[state];
}

bool Player::ProcessEvent(const SGUIEvent* e)
{
	bool rc = false; //rotation change flag
	mmask_t modb = BUTTON_CTRL | BUTTON_SHIFT | BUTTON_ALT;

	if (e->t == GUIEV_MOUSE) {
		//Check mouse wheel event
		if (e->m.bstate & (GUISCRL_UP | GUISCRL_DW)) {

			if ((e->m.bstate & modb) == rot_ver)
				rot.X += (e->m.bstate & GUISCRL_UP)? rspd:-rspd;

			else if ((e->m.bstate & modb) == rot_hor)
				rot.Z -= (e->m.bstate & GUISCRL_UP)? rspd:-rspd;

			else return false;

			rc = true;

		} else
			return false;

	} else if (e->t == GUIEV_KEYPRESS) {

		switch (binder->DecodeKey(e->k)) {
		default: return false;

		case PAKEY_WALK_FORW: Move(LMOVE_FORW,1.2f); break; //FIXME: use speed value
		case PAKEY_WALK_BACK: Move(LMOVE_BACK,1.2f); break;
		case PAKEY_WALK_LEFT: Move(LMOVE_LEFT,1.2f); break;
		case PAKEY_WALK_RGHT: Move(LMOVE_RGHT,1.2f); break;

		case PAKEY_RUN_FORW: Move(LMOVE_FORW,2.2f); break; //FIXME: use speed value
		case PAKEY_RUN_BACK: Move(LMOVE_BACK,2.2f); break;
		case PAKEY_RUN_LEFT: Move(LMOVE_LEFT,2.2f); break;
		case PAKEY_RUN_RGHT: Move(LMOVE_RGHT,2.2f); break;

		case PAKEY_TURN_LEFT: rot.Z += rspd; rc = true; break;
		case PAKEY_TURN_RGHT: rot.Z -= rspd; rc = true; break;
		case PAKEY_TURN_DW: rot.X += rspd; rc = true; break;
		case PAKEY_TURN_UP: rot.X -= rspd; rc = true; break;

		case PAKEY_TOG_STATE:
			//TODO: check conditions, apply some changes etc
			switch (state) {
			case PCS_EXPLORING:
				state = PCS_INTERACTING;
				break;
			case PCS_INTERACTING:
				state = PCS_COMBAT;
				break;
			case PCS_COMBAT:
				state = PCS_EXPLORING;
				break;
			case PCS_VEHICLE: break;
			}

		}

	} else
		return false;

	if (rc) SetRotI(); //Update rotation

	return true;
}
