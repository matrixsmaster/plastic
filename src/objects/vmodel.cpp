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
#include <stdio.h>
#include <string.h>
#include "vmodel.h"
#include "vecmisc.h"


VModel::VModel()
{
	s_x = s_y = s_z = 0;
	dat = NULL;
	datlen = 0;
	nstates = 0;
	buf = NULL;
	buflen = 0;
	bufside = 0;
	rotm = GenOMatrix();
	changed = false;
	state = 0;
}

VModel::~VModel()
{
	int i;
	if (buf) free(buf);
	if (dat) {
		for (i = 0; i < nstates; i++) free(dat[i]);
		free(dat);
	}
}

bool VModel::LoadFromFile(const char* fn)
{
	FILE* mf;
	char* s = NULL;
	voxel v;
	int i,k,l,x,y,z,sd,vtc;
	ulli j;
	vector3d rv;
	SMatrix3d rm;
	SVoxCharPair* tab = NULL;

	if ((dat) || (buf) || (!fn)) return false;

	mf = fopen(fn,"r");
	if (!mf) return false;

	//read dimensions, number of states, and number of voxel types used
	if ( (fscanf(mf,"%d %d %d %d %d\n",&s_x,&s_y,&s_z,&nstates,&vtc) != 5) ||
			(nstates < 1) )
		goto bad_exit;

	//make a char to voxel table
	j = vtc * sizeof(SVoxCharPair);
	tab = (SVoxCharPair*)malloc(j);
	if (!tab) goto bad_exit; //this would happen if table dimension is invalid
	memset(tab,0,j);

	//read voxel types table (skips empty lines and comments)
	for (i = 0; ((i < vtc) && (!feof(mf)));) {
		if (fscanf(mf,"%c = %hu\n",&(tab[i].c),&(tab[i].v)) == 2) i++;
	}
	if (feof(mf)) goto bad_exit; //shouldn't be at the end of file

	//allocate memory: original data states map
	datlen = s_x * s_y * s_z;
	j = nstates * sizeof(voxel*);
	dat = (voxel**)malloc(j);
	if (!dat) goto bad_exit; //Unable to allocate memory
	memset(dat,0,j);

	//allocate memory: original data buffers
	j = datlen * sizeof(voxel);
	for (i = 0; i < nstates; i++) {
		dat[i] = (voxel*)malloc(j);
		if (!dat[i]) goto bad_exit; //destructor will free memory just OK
		memset(dat[i],0,j);
	}

	//make a string
	l = (s_x + 1) * nstates + 2; //reserve two chars for newline and zero
	s = (char*)malloc(l);
	if (!s) goto bad_exit;

	//read data
	y = z = 0;
	while (!feof(mf)) {
		if (fgets(s,l,mf) == NULL) break;
		if ((s[0] == ';') || (s[0] == 0)) continue;

		//for each symbol place
		for (i = 0, sd = 0, x = 0; i < (int)strlen(s); i++,x++) {
			//state delimiter
			if (s[i] == '|') {
				x = 0;
				if (++sd >= nstates) break;
				else continue;
			}

			//find voxel type
			for (k = 0, v = 0; k < vtc; k++)
				if (tab[k].c == s[i]) {
					v = tab[k].v;
					break;
				}
			//calculate linear offset
			j = z * s_x * s_y + y * s_x + x;
			if (j < datlen)
				dat[sd][j] = v;
		}

		//move on to next row or layer
		if (++y >= s_y) {
			y = 0;
			if (++z >= s_z)
				break;
		}
	}
	free(tab);
	free(s);
	fclose(mf);

	//calculate the model center and rotated extent
	dcenter = vector3di((s_x/2),(s_y/2),(s_z/2));
	rv = vector3d(dcenter.Max());
	rm = Mtx3Mul(GenMtxRotX(45 * M_PI / 180.f),GenMtxRotY(45 * M_PI / 180.f));
	rv = MtxPntMul(&rm,&rv);

	//calculate rotation buffer side len
	bufside = (int)round(rv.Z * 2);
	center = vector3di(bufside/2);

	state = 0;
	changed = true;
	return AllocBuf();

bad_exit:
	fclose(mf);
	if (tab) free(tab);
	if (s) free(s);
	return false;
}

bool VModel::AllocBuf()
{
	if (buf) free(buf);

	buflen = (ulli)bufside * (ulli)bufside * (ulli)bufside;

	buf = (voxel*)malloc((size_t)buflen*sizeof(voxel));
	if (!buf) {
		buflen = 0;
		return false;
	}

	ApplyRot(); //effectively copies dat into buf

	return true;
}

ulli VModel::GetAllocatedRAM()
{
	return ((datlen * nstates + buflen) * sizeof(voxel));
}

void VModel::SetRot(const vector3d r)
{
	vector3di nres;
	vector3d test(bufside);

	//Prepare all variables of new rotation
	rot = r;
	RotNormDegF(&rot);
	rotm = GenMtxRotX(rot.X * M_PI / 180.f);
	rotm = Mtx3Mul(rotm,GenMtxRotY(rot.Y * M_PI / 180.f));
	rotm = Mtx3Mul(rotm,GenMtxRotZ(rot.Z * M_PI / 180.f));

	//Check if new rotation makes any sense relatively to old
	if (!changed) {
		test = MtxPntMul(&rotm,&test);
		nres.X = (int)round(test.X);
		nres.Y = (int)round(test.Y);
		nres.Z = (int)round(test.Z);
		if (nres != oldrres) changed = true;
		oldrres = nres;
	}

	//If it does, recalculate buffer
	if (changed) ApplyRot();
}

void VModel::SetState(int s)
{
	if (s < 0) state = 0;
	else if (s >= nstates) state = nstates - 1;
	else state = s;

	ApplyRot(); //refresh working buffer
}

void VModel::ApplyRot()
{
	int x,y,z;
	ulli l,nl;
	vector3di iv;
	vector3d v,ctrd,ctrb;

	if ((!dat) || (!buf)) return;

	//reset current buffer
	memset(buf,0,buflen*sizeof(voxel));

	//prepare centers (just to make is slightly faster)
	ctrd = dcenter.ToReal();
	ctrb = center.ToReal();

	//rotate!
	for (z = 0, l = 0; z < s_z; z++) {
		iv.Z = z;
		for (y = 0; y < s_y; y++) {
			iv.Y = y;
			for (x = 0; x < s_x; x++,l++) {
				if (dat[state][l] == 0) continue; //skip empty voxels
				iv.X = x;

				//calculate new voxel position at a given co-ords
				v = iv.ToReal() - ctrd;
				v = MtxPntMul(&rotm,&v);
				v += ctrb;

				//calculate linear offset of new position
				nl = (int)round(v.Z) * bufside * bufside +
						(int)round(v.Y) * bufside +
						(int)round(v.X);

				//apply the result
				if (nl < buflen)
					buf[nl] = dat[state][l];
			}
		}
	}

	changed = false;
}

voxel VModel::GetVoxelAt(const vector3di* p)
{
	ulli l;

	//move it to positive side
	vector3di x = *p - pos + center;
	//get offset
	l = x.Z * bufside * bufside + x.Y * bufside + x.X;
	if (l >= buflen) return 0;
	else return buf[l]; //got the voxel
}
