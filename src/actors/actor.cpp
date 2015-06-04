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
#include "actorhelpers.h"


PlasticActor::PlasticActor(SPAAttrib a, DataPipe* pptr) :
		VSceneObject()
{
	pipe = pptr;
	InitVars();

	attrib = a;
	AutoInitStats();
}

PlasticActor::PlasticActor(EPAClass c, EPABodyType b, DataPipe* pptr) :
		VSceneObject()
{
	pipe = pptr;
	InitVars();

	strcpy(attrib.name,"Auto"); //FIXME: namegen
	attrib.female = (rand() > RAND_MAX / 3); //kekeke
	attrib.cls = c;
	attrib.body = b;

	AutoInitStats();
}

PlasticActor::~PlasticActor()
{
	Delete();
}

void PlasticActor::InitVars()
{
	isnpc = true;
	model = NULL;
}

void PlasticActor::AutoInitStats()
{
	SPAAttrib na = attrib;
	SPABase ns;
	if (FillActorBasicStats(&na,&ns,pipe)) {
		attrib = na;
		base = ns;
		curr = ns;
	}

	//FIXME: debug only!
	portrait = (SGUIPixel*)malloc(ACTPORTRAITH*ACTPORTRAITW*sizeof(SGUIPixel));
	VSprite* s = pipe->LoadSprite("spr/testspr.dat");
	memcpy(portrait,s->GetImage(),ACTPORTRAITH*ACTPORTRAITW*sizeof(SGUIPixel));
	pipe->PurgeSprites();
}

void PlasticActor::UpdateModelPos()
{
	if (!model) return;
	model->SetPos(pos);
	model->SetScenePos(scenter);
	model->SetGPos(gpos);
	model->SetRot(rot);
//	pos = model->GetPos();
//	gpos = model->GetGPos();
//	rot = model->GetRot();
//	SetRotI();
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
	v = MtxPntMul(&rotm,&v);

	pos.X += (int)round(v.X);
	pos.Y += (int)round(v.Y);
	pos.Z -= (int)round(v.Z); //to conform rotation/movement of renderer (flipped Y axis)

	SetPosI();
	UpdateModelPos();
}

bool PlasticActor::Spawn()
{
	model = pipe->LoadModel(attrib.model,pos,gpos);
	return (model != NULL);
}

void PlasticActor::Delete()
{
	if (model) pipe->UnloadModel(model);
	model = NULL;
	if (portrait) free(portrait);
	portrait = NULL;
}

SPABase PlasticActor::GetStats(bool current)
{
	if (current) return curr;
	else return base;
}
