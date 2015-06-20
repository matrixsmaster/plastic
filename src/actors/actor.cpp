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
#include "world.h"


PlasticActor::PlasticActor(SPAAttrib a, DataPipe* pptr) :
		VSceneObject()
{
	pipe = pptr;
	InitVars();

	attrib = a;
	AutoInitStats();
}

PlasticActor::PlasticActor(EPAClass c, bool fem, NameGen* names, DataPipe* pptr) :
		VSceneObject()
{
	pipe = pptr;
	InitVars();

	//Set gender and name
	attrib.female = fem;
	strcpy(attrib.name,names->GetHumanName(fem).c_str());

	//Set class
	attrib.cls = c;

	//Generate body type
	base.Body = 0;
	attrib.body = PBOD_INVALID;
	while (!(attrib.body & base.Body)) {
		attrib.body = (EPABodyType)(1 << (pipe->GetRNG()->RangedNumber(NUMBODTYPE)));
		AutoInitStats();
	}
}

PlasticActor::~PlasticActor()
{
	Delete();
	if (portrait) free(portrait);
}

void PlasticActor::InitVars()
{
	isnpc = true;
	model = NULL;
	portrait = NULL;
	anim = NULL;
	world = NULL;
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
	if (!portrait) {
		portrait = (SGUIPixel*)malloc(ACTPORTRAITH*ACTPORTRAITW*sizeof(SGUIPixel));
		VSprite* s = pipe->LoadSprite("spr/testspr.dat");
		memcpy(portrait,s->GetImage(),ACTPORTRAITH*ACTPORTRAITW*sizeof(SGUIPixel));
		pipe->PurgeSprites();
	}
}

#define PA_MODSTATMACRO(Stat) \
		if (ns.Stat) curr.Stat += ns.Stat; \
		if (curr.Stat < 0) curr.Stat = 0; \
		if (curr.Stat > ACTORATTRIBMAX) curr.Stat = ACTORATTRIBMAX;

void PlasticActor::ModCurStats(const SPABase ns)
{
	PA_MODSTATMACRO(HP);
	PA_MODSTATMACRO(Qual);
	PA_MODSTATMACRO(CC);
	PA_MODSTATMACRO(Spd);
	PA_MODSTATMACRO(Str);
	PA_MODSTATMACRO(Eff);
	PA_MODSTATMACRO(RS);
	PA_MODSTATMACRO(Acc);
	PA_MODSTATMACRO(Eng);
	PA_MODSTATMACRO(Spch);
	PA_MODSTATMACRO(Brv);
	PA_MODSTATMACRO(Chr);
	PA_MODSTATMACRO(Trd);
	PA_MODSTATMACRO(AP);
	PA_MODSTATMACRO(DT);
	PA_MODSTATMACRO(DM);
}

void PlasticActor::UpdateModelPos()
{
	if (!model) return;
	model->SetPos(pos);
	model->SetScenePos(scenter);
	model->SetGPos(gpos);
	model->SetRot(rot);
}

void PlasticActor::ReadModelPos()
{
	if (!model) return;
	pos = model->GetPos();
	scenter = model->GetScenePos();
	gpos = model->GetGPos();
	rot = model->GetRot();
	SetRotI();
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

	ReadModelPos(); //update our position out of model position in scene

	pos.X += (int)round(v.X);
	pos.Y += (int)round(v.Y);
	pos.Z -= (int)round(v.Z); //to conform rotation/movement of renderer (flipped Y axis)

	SetPosI(); //internal settings has to be done
	UpdateModelPos(); //now update model position back
}

bool PlasticActor::UseObject(InventoryObject* obj)
{
	//TODO
	return false;
}

bool PlasticActor::WearObject(InventoryObject* obj)
{
	//TODO
	return false;
}

bool PlasticActor::Spawn(PlasticWorld* wrld)
{
	model = pipe->LoadModel(attrib.model,pos,gpos);
	if (model == NULL) return false;
	world = wrld;
	anim = new DAnimator(pipe,world->GetGameTimePtr(),model,attrib.model);
	anim->LoadAnim("walking"); //FIXME: debug
	return true;
}

void PlasticActor::Delete()
{
	if (model) pipe->UnloadModel(model);
	if (anim) delete anim;
	anim = NULL;
	model = NULL;
	world = NULL;
}

void PlasticActor::Animate()
{
	if (model && anim) anim->Update();
}

SPABase PlasticActor::GetStats(bool current)
{
	if (current) return curr;
	else return base;
}
