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
}

PlasticPhysics::~PlasticPhysics()
{
	//TODO
}


bool PlasticPhysics::GetSurroundingVox(const SPPModelRec* mod, vector3di p)
{
	voxel sv,ov;
	vector3di cp = p;

	//FIXME
	cp.Z -= 1;
	if (pipe->GetVoxel(&cp, true)) return true;

	cp.Z += 1;
	cp.X -= 1;
	if (pipe->GetVoxel(&cp, true)) return true;

	cp.X += 1;
	cp.Z += 1;
	if (pipe->GetVoxel(&cp, true)) return true;

	cp.X += 1;
	cp.Z += 1;
	if (pipe->GetVoxel(&cp, true)) return true;

	cp.Z -= 1;
	cp.X += 1;
	if (pipe->GetVoxel(&cp, true)) return true;

	cp.X -= 1;
	cp.Y -= 1;
	if (pipe->GetVoxel(&cp, true)) return true;

	cp.Y += 2;
	if (pipe->GetVoxel(&cp, true)) return true;

	return false;
}

const SPPCollision PlasticPhysics::Collision(const SPPModelRec* mod)
{
	SPPCollision res;
	int b;				//bound side
	voxel sv,ov;		//scene voxel, object voxel
	vector3di sp,p, cp;	//scene center point, object point, contact point
	bool contact;

	res.no_collision = true; //FIXME
	res.depth = 0;
	contact = false;

	b = mod->modptr->GetBoundSide();
	sp = mod->modptr->GetSPos();

	//Search one-dimensional collision
	//FIXME search on the border of the cube
	for (p.X = (sp.X - b/2); p.X < (sp.X + b/2); ++(p.X)) {
		for (p.Y = (sp.Y - b/2); p.Y < (sp.Y + b/2); ++(p.Y)) {
			for (p.Z = (sp.Z - b/2); p.Z < (sp.Z + b/2); ++(p.Z)) {

				sv = pipe->GetVoxel(&p, true);
				ov = mod->modptr->GetVoxelAt(&p);

				if (sv && ov && (res.no_collision)) {
					res.no_collision = true; //false;
					res.start = p;
					res.depth = 1;
					break;
				}

				//Get surrounding voxels
//				if (!sv && ov && !contact)
//					if (!contact) {
//						contact = GetSurroundingVox(mod, p);
//						cp = p;
//					}
			}
			if (!res.no_collision) break;
		}
		if (!res.no_collision) break; // && contact
	}

	if (!res.no_collision)
		dbg_print("[PHY] Last collision: %d, start [ %d %d %d ], depth = %d, (%p)", res.no_collision, res.start.X, res.start.Y, res.start.Z, res.depth, mod->modptr);
	if (contact)
		dbg_print("[PHY] Contact on [ %d %d %d ] voxel", cp.X, cp.Y, cp.Z);
	return res;
}


void PlasticPhysics::Quantum()
{
	PPModVec::iterator im;
	VModVec::iterator iv;
	SPPModelRec cur;
	SPPCollision ccol;
	VModVec* fmod = pipe->GetModels();
	bool sys_changed = false;

	/* Check and update models presence */
	pipe->ReadLock();
	im = mods.begin();
	for (iv = fmod->begin(); iv < fmod->end(); ++iv) {
		//if a foreign model vector is larger than ours, add new model
		if (im >= mods.end()) {
			cur.modptr = *iv;
			cur.oldspos = (*iv)->GetPos();
			cur.moved = false;
			cur.changed = false;
			cur.contact = false;
			mods.push_back(cur);
			im = mods.end();
			dbg_print("[PHY] Added model %p",(im-1)->modptr);
			continue;
		}

		//check model movement
		if ((*iv) == im->modptr) {
			im->changed = false; //reset flag
			if ((*iv)->GetPos() != im->oldspos) {
				im->oldspos = (*iv)->GetPos();
				im->moved = true;
			}
			++im;
		} else {
			//model vector is out of sync
			dbg_print("[PHY] Removing model %p",im->modptr);
			im = mods.erase(im);
			--iv;
		}
	}

	/* Calculate collisions */
	for (im = mods.begin(); im < mods.end(); ++im) {
		//skip contacting objects, which isn't moved
		if ((!im->changed) && (!im->moved) && (im->contact)) continue;

		ccol = Collision(&(*im));
		if (ccol.no_collision) {
			im->changed = false;
			continue;
		}

		//TODO: gravity

		//TODO: move object


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
			dbg_print("[PHY] Early return due to model vectors is out of sync");
			pipe->ReadUnlock();
			return;
		}

		//check model needs updating
		if (!im->changed) continue;

		//actually update the model position
		im->modptr->SetPos(im->oldspos);
	}

	pipe->ReadUnlock();
}

