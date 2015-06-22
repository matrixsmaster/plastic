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
	if (portrait) pipe->UnloadSprite(portrait);
}

void PlasticActor::InitVars()
{
	isnpc = true;
	model = NULL;
	portrait = NULL;
	anim = NULL;
	world = NULL;
	headtxd = 0;
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

	if (!portrait) {
		//FIXME: debug only!
		portrait = pipe->LoadSprite("spr/testspr.dat");
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

void PlasticActor::SetPos(const vector3di p)
{
	pos = p;
	SetPosI();
	UpdateModelPos();
}

void PlasticActor::SetRot(const vector3di r)
{
	rot = r;
	SetRotI();
	UpdateModelPos();
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

SGUIPixel* PlasticActor::GetPortrait()
{
	if (!portrait) return NULL;
	return (portrait->GetImage());
}

bool PlasticActor::Spawn(PlasticWorld* wrld)
{
	SVoxelInf nvi;

	//Load up actor's model
	model = pipe->LoadModel(attrib.model,pos,gpos);
	if (model == NULL) return false;

	//Set up world instance pointer for call-backs
	world = wrld;

	//Initialize discrete animator
	anim = new DAnimator(pipe,world->GetGameTimePtr(),model,attrib.model);
//	anim->LoadAnim("walking"); //FIXME: debug

	//Register face voxel
	nvi = *(pipe->GetVInfo("face")); //copy 'face' voxel
	nvi.mark = NULL; //don't use the same mark
	nvi.not_used = false; //just in case
	nvi.texture = portrait; //apply face texture
	headtxd = pipe->AppendVoxel(&nvi);

	//Replace old face voxel
	model->ReplaceVoxel("face",headtxd);

	//Good to go
	return true;
}

void PlasticActor::Delete()
{
	//Destroy animation system
	if (anim) delete anim;
	anim = NULL;

	//Destroy actor's model
	if (model) pipe->UnloadModel(model);
	model = NULL;

	//To free face voxel
	pipe->RemoveVoxel(headtxd);
	headtxd = 0;

	//We don't need world instance anymore
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
