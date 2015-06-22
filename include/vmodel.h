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

#ifndef VMODEL_H_
#define VMODEL_H_

#include "voxel.h"
#include "vecmath.h"
#include "mtx3d.h"
#include "misconsts.h"
#include "vobject.h"

#define VOXMARKERLEN 64
#define VOXMARKERFMT "%c = %63[^\n]"

struct SVoxCharPair {
	char c;
	voxel v;
};

struct SVoxHide {
	voxel v;
	bool hidden;
};

class VModel : public VSceneObject {
private:
	int s_x,s_y,s_z;		//extent
	voxel** dat;			//original data
	ulli datlen;			//linear length of single state buffer
	int nstates;			//number of states available
	voxel* buf;				//modified (working) buffer
	ulli buflen;			//linear length of working buf
	int bufside;			//side length of working buf
	bool changed;			//optimization flag of rotations
	vector3di oldrres;		//old rotational result
	vector3di dcenter;		//original data center point
	vector3di center;		//working buf center point
	int state;				//current state
	SVoxelTab* vtab;		//voxel table
	SVoxHide* hdtab;		//voxel hide table

	bool AllocBuf();
	void HideEm();
	voxel FindVoxel(EVoxelType tp);
	voxel FindVoxel(const char* mrk);

public:
	VModel(SVoxelTab* tabptr);
	virtual ~VModel();

	///Returns an original (not rotated) model dimensions.
	vector3di GetOrgSize()			{ return vector3di(s_x,s_y,s_z); }

	///Returns a modified (rotated) model dimensions.
	vector3di GetModSize()			{ return vector3di(bufside); }

	///Returns a side of bounding box of rotated model (working buffer).
	int GetBoundSide()				{ return bufside; }

	///Returns a length (volume of the cube) of original model's data buffer.
	ulli GetOrgLen()				{ return datlen; }

	///Returns a length (volume of the cube) of model's working buffer.
	ulli GetModLen()				{ return buflen; }

	///Returns number of model 3D states.
	int GetNumStates()				{ return nstates; }

	///Returns current model 3D state.
	int GetState()					{ return state; }

	///Sets up a model 3D state and updates working buffer.
	void SetState(int s);

	///Loads a model from local file.
	bool LoadFromFile(const char* fn);

	///Returns amount of RAM allocated by a VModel.
	ulli GetAllocatedRAM();

	///Sets up a new model rotation and updates working buffer.
	void SetRot(const vector3di r);

	///Applies rotation to working buffer. No need to call this if you're using SetRot().
	void ApplyRot();

	///Hides all voxels equal to 'id'.
	void HideVoxels(voxel id, bool hide);

	///Hides all voxels of type 'tp'.
	void HideVoxels(EVoxelType tp, bool hide);

	///Hides all voxels with mark string equal to 'mrk'.
	void HideVoxels(const char* mrk, bool hide);

	///Replaces all voxels equal to 'old_id' with a voxels of 'new_id'.
	void ReplaceVoxel(voxel old_id, voxel new_id);

	///Replaces all voxels with mark string equal to 'mrk' with a voxels of 'new_id'.
	void ReplaceVoxel(const char* mrk, voxel new_id);

	///Returns a voxel in working buffer at specified scene coordinates.
	voxel GetVoxelAt(const vector3di* p);
};

#endif /* VMODEL_H_ */
