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

#include "pphysic.h"
#include "debug.h"

using namespace std;


PlasticPhysics::PlasticPhysics(DataPipe* pipeptr)
{
	pipe = pipeptr;
	locked = false;
}

PlasticPhysics::~PlasticPhysics()
{
	//TODO
}


bool PlasticPhysics::CheckUnderlineVox(const VModel* ptr, const vector3di p)
{
	vector3di cp = p;
	VModel* misc;

	//set underline coord
	cp.Z -= 1;

	//Search contact with world
	if (pipe->GetVoxel(&cp, true)) return true;

	//Search contact with actors
	if (pipe->IntersectModel(&cp,&misc,ptr,false)) return true;

	return false;
}

bool PlasticPhysics::Contact(const SPPModelRec* mod)
{
	int b;				//bound side
	voxel ov;			//scene voxel, object voxel
	vector3di sp,p, cp;	//scene center point, object point, contact point
	int sx,sy,sz,ex,ey,ez;
	bool contact = false;

	//Getting bound side and central position of model
	b = mod->modptr->GetBoundSide();
	sp = mod->modptr->GetSPos();

	//Initial and final positions model in scene
	sx = sp.X - b/2;
	sy = sp.Y - b/2;
	sz = sp.Z - b/2;
	ex = sp.X + b/2;
	ey = sp.Y + b/2;
	ez = sp.Z + b/2;

	//Search contact
	for (p.Z = sz; p.Z < ez; ++(p.Z)) {
		for (p.X = sx; p.X < ex; ++(p.X)) {
			for (p.Y = sy; p.Y < ey; ++(p.Y)) {
				//Get voxel from model
				ov = mod->modptr->GetVoxelAt(&p);
				if (ov)
					//Check surrounding voxels
					if (CheckUnderlineVox(mod->modptr,p)) {
						contact = true;
						break;
					}
			}
			if (contact) break;
		}
		if (contact) break;
	}
#ifdef PHYDEBUG
	if (c.contact)
		dbg_print("[PHY] Contact on [ %d %d %d ] voxel (%p)", p.X, p.Y, p.Z, mod->modptr);
#endif

	return contact;
}

const SPPCollision PlasticPhysics::Collision(const SPPModelRec* mod)
{
	SPPCollision res;
	int b;				//bound side
	voxel sv,ov;		//scene voxel, object voxel
	vector3di sp,p;		//scene center point, voxel point
	int sx,sy,sz,ex,ey,ez;
	VModel* misc;

	res.no_collision = true;
	res.depth = 0;

	//Getting bound side and central position of model
	b = mod->modptr->GetBoundSide();
	sp = mod->modptr->GetSPos();

	//Initial and final positions model in scene
	sx = sp.X - b/2;
	sy = sp.Y - b/2;
	sz = sp.Z - b/2;
	ex = sp.X + b/2;
	ey = sp.Y + b/2;
	ez = sp.Z + b/2;

	//Search one-dimensional collision and contact
	for (p.X = sx; p.X < ex; ++(p.X)) {
		for (p.Y = sy; p.Y < ey; ++(p.Y)) {
			for (p.Z = sz; p.Z < ez; ++(p.Z)) {

				//Get voxel from scene (static)
				sv = pipe->GetVoxel(&p, true);
				//Get voxel from actors (dynamic)
				if (!sv)
					sv = pipe->IntersectModel(&p,&misc,mod->modptr,false);

				if (!sv) continue;

				//Get voxel from model
				ov = mod->modptr->GetVoxelAt(&p);

				if (sv && ov && res.no_collision) {
					//Collision detected.
					res.no_collision = false;
					res.start = p;
					res.depth = 1;
					break;
				}
			}
			if (!res.no_collision) break;
		}
		if (!res.no_collision) break;
	}

#ifdef PHYDEBUG
	if (!res.no_collision)
		dbg_print("[PHY] Collision: %d, start [ %d %d %d ], depth = %d, boundS = %d, (%p)", res.no_collision,
				res.start.X, res.start.Y, res.start.Z, res.depth, b, mod->modptr);
#endif

	return res;
}

vector3di PlasticPhysics::ResolveCollision(const SPPCollision ccol, const vector3di v)
{
	//vector indicating collision
	vector3di b_s = ccol.start - v;

	//normalization of the vector
	vector3di b_ss = b_s.Normalize(PHY_COLLISION_EPSILON);

	//take the offset
	vector3di c_s = b_ss * ccol.depth;

	//change the direction of the vector
	c_s *= -1;

	//displace vector and returns it
	return (vector3di(c_s) + v);
}


void PlasticPhysics::Quantum()
{
	PPModVec::iterator im;
	VModVec::iterator iv;
	SPPModelRec cur;
	SPPCollision ccol;
	VModVec* fmod = pipe->GetModels();
	bool sys_changed = false;

	/* Check update lock */
	if (locked) return;

	/* Check and update models presence */
	pipe->ReadLock();

	im = mods.begin();
	for (iv = fmod->begin(); iv < fmod->end(); ++iv) {
		//if a foreign model vector is larger than ours, add new model
		if (im >= mods.end()) {
			cur.modptr = *iv;
			cur.oldspos = (*iv)->GetPos();
			cur.newpos = cur.oldspos;
			cur.moved = false;
			cur.changed = false;
			cur.contact = false;
			mods.push_back(cur);
			im = mods.end();
#ifdef PHYDEBUG
			dbg_print("[PHY] Added model %p",(im-1)->modptr);
#endif
			continue;
		}

		//check model movement
		if ((*iv) == im->modptr) {
			im->changed = false; //reset flag
			if ((*iv)->GetPos() != im->oldspos) {
				im->oldspos = (*iv)->GetPos();
				im->newpos = im->oldspos;
				im->moved = true;
			} else im->newpos = im->oldspos;
			++im;
		} else {
			//model vector is out of sync
#ifdef PHYDEBUG
			dbg_print("[PHY] Removing model %p",im->modptr);
#endif
			im = mods.erase(im);
			--iv;
		}
	}
	//delete remainder of the models vector (if it exists)
	if (im < mods.end()) mods.erase(im,mods.end());

	if (fmod->size() != mods.size()) abort(); //FIXME: debug only

	/* Calculate collisions */
	iv = fmod->begin();
	for (im = mods.begin(); im < mods.end(); ++im) {
		//skip contacting objects, which isn't moved
		if ((!im->changed) && (!im->moved) && (im->contact)) continue;

		//Search collision with surface and actors
		ccol = Collision(&(*im));

		/* Resolve collision with a depth of one voxel. */
		if (!ccol.no_collision) {
			im->newpos = ResolveCollision(ccol, im->oldspos);
		}

		//Search contact with surface and actors
		im->contact = Contact(&(*im));

		if (ccol.no_collision && im->contact) {
			im->changed = false;
			continue;
		}

		/* Gravity */
		if (!im->contact) {
			vector3di g(0,0,-1);
			if (!ccol.no_collision)
				g = vector3di(0,0,1);
			im->newpos += g;
		}

		im->changed = true;
		sys_changed = true;
	}

	/* Change system */
	if (!sys_changed) {
		pipe->ReadUnlock();
		return;
	}

	im = mods.begin();
	for (iv = fmod->begin(); iv != fmod->end(); ++iv, ++im) {
		//don't forget to make sure we're still in sync
		if ((*iv) != im->modptr) {
			dbg_print("[PHY] Early return due to models vector is out of sync");
			pipe->ReadUnlock();
			return;
		}

		//check model needs updating
		if (!im->changed) continue;
//#if PHYDEBUG
		dbg_print("oldpos|newpos [ %d %d %d ] [ %d %d %d ] (%p)", im->oldspos.X, im->oldspos.Y, im->oldspos.Z,
				im->newpos.X, im->newpos.Y, im->newpos.Z, im->modptr);
//#endif
		//actually update the model position
		im->modptr->SetPos(im->newpos);
	}

	pipe->ReadUnlock();
}

