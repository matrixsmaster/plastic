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
#include "support.h"
#include "debug.h"


PlasticActor::PlasticActor(SPAAttrib a, DataPipe* pptr) :
		VSceneObject(), IGData()
{
	pipe = pptr;
	InitVars();

	attrib = a;
	AutoInitStats();
}

PlasticActor::PlasticActor(EPAClass c, bool fem, NameGen* names, DataPipe* pptr) :
		VSceneObject(), IGData()
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

PlasticActor::PlasticActor()
{
	pipe = NULL;
	InitVars();
}

PlasticActor::~PlasticActor()
{
	Delete();
	RmPortrait();
}

void PlasticActor::InitVars()
{
	/* Do NOT use anything related to DataPipe here,
	 * static initializations only! */
	isnpc = true;
	memset(&attrib,0,sizeof(attrib));
	memset(&base,0,sizeof(base));
	curr = base;
	model = NULL;
	portrait = NULL;
	portrait_reg = false;
	anim = NULL;
	world = NULL;
	headtxd = 0;
	memset(limbs,0,sizeof(limbs));
}

void PlasticActor::RmPortrait()
{
	if (!portrait) return;
	if (portrait_reg)
		pipe->UnloadSprite(portrait);
	else
		delete portrait;
	portrait = NULL;
	portrait_reg = false;
}

bool PlasticActor::SerializeToFile(FILE* f)
{
	SPAFileHeader hdr;

	//Fill in header information
	hdr.gpx = gpos.X;
	hdr.gpy = gpos.Y;
	hdr.gpz = gpos.Z;
	hdr.lpx = pos.X;
	hdr.lpy = pos.Y;
	hdr.lpz = pos.Z;
	hdr.have_portrait = (portrait != NULL);
	if (portrait) {
		hdr.port_w = portrait->GetWidth();
		hdr.port_h = portrait->GetHeight();
	}

	//Write header
	fwrite(&hdr,sizeof(hdr),1,f);

	//Write attributes and stats
	fwrite(&attrib,sizeof(attrib),1,f);
	fwrite(&base,sizeof(base),1,f);
	fwrite(&curr,sizeof(curr),1,f);

	//Write limbs state
	fwrite(limbs,sizeof(limbs),1,f);

	//Write portrait image
	if (portrait)
		fwrite(portrait->GetImage(),sizeof(SGUIPixel),hdr.port_w*hdr.port_h,f);

	return true;
}

bool PlasticActor::DeserializeFromFile(FILE* f)
{
	SPAFileHeader hdr;
	SGUIPixel* portr;
	size_t sz;

	//reset actor information (just in case)
	Delete();
	RmPortrait();

	//read and apply header data
	if (fread(&hdr,sizeof(hdr),1,f) < 1) return false;
	gpos = vector3di(hdr.gpx,hdr.gpy,hdr.gpz);
	pos = vector3di(hdr.lpx,hdr.lpy,hdr.lpz);

	//read and apply attribs and stats
	if (fread(&attrib,sizeof(attrib),1,f) < 1) return false;
	if (fread(&base,sizeof(base),1,f) < 1) return false;
	if (fread(&curr,sizeof(curr),1,f) < 1) return false;

	//read limbs state
	if (fread(limbs,sizeof(limbs),1,f) < 1) return false;

	//get actor's portrait
	if (hdr.have_portrait) {
		//allocate temporary picture buffer
		sz = hdr.port_w * hdr.port_h * sizeof(SGUIPixel);
		portr = (SGUIPixel*)malloc(sz);
		if (!portr) return false;

		//load picture from file
		if (fread(portr,1,sz,f) < 1) return false;

		//create portrait sprite
		portrait = new VSprite();
		portrait->Assign(portr,hdr.port_w,hdr.port_h);
		portrait_reg = false;

		//free temp buf
		free(portr);
	}

	return true;
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
		portrait_reg = true;
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
	anim->LoadAnim("walking"); //FIXME: debug

	//Register portrait (if wasn't done before)
	if ((portrait) && (!portrait_reg)) {
		pipe->AddSprite(portrait);
		portrait_reg = true;
	}

	//Register face voxel
	nvi = *(pipe->GetVInfo("face")); //copy 'face' voxel
	nvi.mark = mstrnacpy(NULL,nvi.mark,0); //copy the mark
	nvi.not_used = false; //just in case
	nvi.texture = portrait; //apply face texture
	headtxd = pipe->AppendVoxel(&nvi);

	//Replace old face voxel
	model->ReplaceVoxel("face",headtxd);

	//Update limbs state
	UpdateLimbs();

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
	if (headtxd) pipe->RemoveVoxel(headtxd);
	headtxd = 0;

	//We don't need world instance anymore
	world = NULL;
}

void PlasticActor::Animate()
{
	if (model && anim) anim->Update();
}

void PlasticActor::Damage(const vector3di* pnt)
{
	voxel vc;
	const SVoxelInf* vci;
	int i,j;
	EPABodyPartType bp = PBP_INVALID;
	int dam;

	//TODO: calc resistance
	dam = 10;

	if (model && pnt) {
		//commit 3d-visible damage
		vc = model->GetVoxelAt(pnt);
		vci = pipe->GetVInfo(vc);
		if (vci->mark) {
			dbg_print("PA::Damage(): vci mark = '%s'",vci->mark);
			//find the corresponding body part
			for (i = 0; i < NUMBODPART; i++) {
				for (j = 0; j < PABPNUMALIASES; j++) {
					if (!pabtype_to_str[i].aka[j]) break;
					if (!strcmp(pabtype_to_str[i].aka[j],vci->mark)) {
						bp = pabtype_to_str[i].bt;
						break;
					}
				}
			}
			if (bp != PBP_INVALID) {
				dbg_print("PA::Damage(): BP = '%s'",pabtype_to_str[bp].aka[0]);
				//FIXME: debug
				limbs[bp] += dam;
				UpdateLimbs();
			}
		}
	}

	//TODO: calc actual damage
	curr.HP -= dam;
}

void PlasticActor::UpdateLimbs()
{
	int i,j;

	if (!model) return;
	for (i = 0; i < NUMBODPART; i++) {
		if (limbs[i] >= PAMAXLIMBDAM) {
			for (j = 0; j < PABPNUMALIASES; j++)
				model->HideVoxels(pabtype_to_str[i].aka[j],true);
		}
	}
}

SPABase PlasticActor::GetStats(bool current)
{
	if (current) return curr;
	else return base;
}
