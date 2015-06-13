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

void PlasticPhysics::Quantum()
{
	PPModVec::iterator im;
	VModVec::iterator iv;
	SPPModelRec cur;
	VModVec* fmod = pipe->GetModels();
	bool sys_changed = false;

	/* Check and update models presence */
	pipe->ReadLock();
	im = mods.begin();
	for (iv = fmod->begin(); iv < fmod->end(); ++iv, ++im) {
		//if a foreign model vector is larger than ours, add new model
		if (im >= mods.end()) {
			cur.modptr = *iv;
			cur.oldspos = (*iv)->GetPos();
			cur.moved = false;
			cur.changed = false;
			mods.push_back(cur);
			im = mods.end() - 1;
			continue;
		}

		//check model movement
		if ((*iv) == im->modptr) {
			im->changed = false; //reset flag
			if ((*iv)->GetPos() != im->oldspos) {
				im->oldspos = (*iv)->GetPos();
				im->moved = true;
			}
		} else {
			//model vector is out of sync
			im = mods.erase(im) - 1;
			--iv;
		}
	}
	pipe->ReadUnlock();

	/* Calculate movements */
	//TODO

	/* Change system */
	if (!sys_changed) return;

	pipe->WriteLock();

	im = mods.begin();
	for (iv = fmod->begin(); iv != fmod->end(); ++iv, ++im) {
		//don't forget to make sure we're still in sync
		if ((*iv) != im->modptr) {
			dbg_print("Early return due to model vectors is out of sync");
			pipe->WriteUnlock();
			return;
		}

		//check model needs updating
		if (!im->changed) continue;

		//actually update the model position
		im->modptr->SetPos(im->oldspos);
	}

	pipe->WriteUnlock();
}
