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


PlasticActor::PlasticActor(SPAStats s, DataPipe* pptr)
{
	pipe = pptr;
	InitVars();

	stats = s;
	if (s.base.autoinit) AutoInitStats();
	curr = s.base;
	curr.autoinit = false;
}

PlasticActor::PlasticActor(EPAClass c, DataPipe* pptr)
{
	pipe = pptr;
	InitVars();

	strcpy(stats.name,"Auto"); //FIXME: namegen
	stats.female = (rand() > RAND_MAX / 3); //kekeke
	stats.base.Cls = c;
	stats.base.autoinit = true;
	AutoInitStats();
	curr = stats.base;
	curr.autoinit = false;
}

PlasticActor::~PlasticActor()
{
	Delete();
}

void PlasticActor::InitVars()
{
	rotmat = GenOMatrix();
	model = NULL;
}

void PlasticActor::AutoInitStats()
{
	//TODO: use the tab!
	strcpy(stats.model,"alice.dat"); //debug
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

bool PlasticActor::Spawn()
{
	model = pipe->LoadModel(stats.model,pos);
	return (model != NULL);
}

void PlasticActor::Delete()
{
	pipe->UnloadModel(model);
	model = NULL;
}
