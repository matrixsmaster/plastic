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
#include <string.h>
#include <math.h>
#include "actor.h"
#include "actorctab.h"


PlasticActor::PlasticActor(SPAStats s, DataPipe* pptr)
{
	rotmat = GenOMatrix();
	pipe = pptr;

	stats = s;
	if (s.base.autoinit) AutoInitStats();
	curr = s.base;
	curr.autoinit = false;
}

PlasticActor::PlasticActor(EPAClass c, DataPipe* pptr)
{
	rotmat = GenOMatrix();
	pipe = pptr;

	strcpy(stats.name,"Auto"); //FIXME: namegen
	stats.female = (rand() > RAND_MAX / 3); //kekeke
	stats.base.Cls = c;
	stats.base.autoinit = true;
	AutoInitStats();
	curr = stats.base;
	curr.autoinit = false;
}

void PlasticActor::AutoInitStats()
{
	//TODO
}

void PlasticActor::SetRot(const vector3di r)
{
	rot = r;
	RotNormDegI(&rot);

	rotmat = Mtx3Mul(GenMtxRotX(rot.X * M_PI / 180.f),GenMtxRotZ(rot.Z * M_PI / 180.f));
}

void PlasticActor::Move(ELMoveDir d, float step)
{
	vector3d v;
	switch (d) {
	case LMOVE_UP:   v.Z =  step; break;
	case LMOVE_DW:   v.Z = -step; break;
	case LMOVE_RGHT: v.X =  step; break;
	case LMOVE_LEFT: v.X = -step; break;
	case LMOVE_FORW: v.Y =  step; break;
	case LMOVE_BACK: v.Y = -step; break;
	}
	v = MtxPntMul(&rotmat,&v);

	pos.X += (int)round(v.X);
	pos.Y += (int)round(v.Y);
	pos.Z -= (int)round(v.Z); //to conform rotation/movement of renderer (flipped Y axis)
}

Player::Player(SPAStats s, DataPipe* pptr) :
		PlasticActor(s,pptr)
{
	binder = new KeyBinder(pptr);

	binder->RegKeyByName("WALK_FORW");
	binder->RegKeyByName("WALK_BACK");
	binder->RegKeyByName("WALK_LEFT");
	binder->RegKeyByName("WALK_RGHT");

	binder->RegKeyByName("RUN_FORW");
	binder->RegKeyByName("RUN_BACK");
	binder->RegKeyByName("RUN_LEFT");
	binder->RegKeyByName("RUN_RGHT");

	binder->RegKeyByName("TURN_LEFT");
	binder->RegKeyByName("TURN_RGHT");
	binder->RegKeyByName("TURN_UP");
	binder->RegKeyByName("TURN_DW");
}

void Player::ProcessEvent(const CGUIEvent* e)
{
	bool rc = false; //rotation change flag
	if (e->t != GUIEV_KEYPRESS) return;

	switch (binder->DecodeKey(e->k)) {

	case 0: Move(LMOVE_FORW,1.2f); break; //FIXME: use speed value
	case 1: Move(LMOVE_BACK,1.2f); break;
	case 2: Move(LMOVE_LEFT,1.2f); break;
	case 3: Move(LMOVE_RGHT,1.2f); break;

	case 4: Move(LMOVE_FORW,2.2f); break; //FIXME: use speed value
	case 5: Move(LMOVE_BACK,2.2f); break;
	case 6: Move(LMOVE_LEFT,2.2f); break;
	case 7: Move(LMOVE_RGHT,2.2f); break;

	case  8: rot.Z += 1; rc = true; break;
	case  9: rot.Z -= 1; rc = true; break;
	case 10: rot.X += 1; rc = true; break;
	case 11: rot.X -= 1; rc = true; break;

//	case : Move(LMOVE_UP,1.2f); break;
//	case : Move(LMOVE_DW,1.2f); break;
	}

	if (rc) SetRot(rot); //Update rotation
}
