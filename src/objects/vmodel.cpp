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
	buf = NULL;
	buflen = 0;
	bufside = 0;
	rotm = GenOMatrix();
	changed = false;
}

VModel::~VModel()
{
	if (buf) free(buf);
	if (dat) free(dat);
}

bool VModel::LoadFromFile(const char* fn)
{
	FILE* mf;
	char* s;
	voxel v;
	int i,y,z,l,fr;
	ulli j;
	vector3d rv;
	SMatrix3d rm;

	if ((dat) || (buf) || (!fn)) return false;

	mf = fopen(fn,"r");
	if (!mf) return false;

	//read dimensions
	if (fscanf(mf,"%d %d %d %d\n",&s_x,&s_y,&s_z,&fr) != 4) {
		fclose(mf);
		return false;
	}

	//allocate memory
	l = s_x + 2;
	s = (char*)malloc(l);
	datlen = s_x * s_y * s_z;
	dat = (voxel*)malloc(datlen*sizeof(voxel));
	if ((!dat) || (!s)) {
		//Unable to allocate memory
		if (s) free(s);
		if (dat) free(dat);
		dat = NULL;
		fclose(mf);
		return false;
	}

	//read data
	y = z = 0;
	while (!feof(mf)) {
		if (fgets(s,l,mf) == NULL) break;
		if ((s[0] == ';') || (s[0] == 0)) continue;

		for (i = 0; i < s_x; i++) {
			switch (s[i]) {			//FIXME
			case '.': v = 0; break;
			case 'A': v = 1; break;
			case 'B': v = 2; break;
			case 'C': v = 3; break;
			case 'D': v = 4; break;
			default: v = 0;
			}
			j = z * s_x * s_y + y * s_x + i;
			if (j < datlen)
				dat[j] = v;
		}
		if (++y >= s_y) {
			y = 0;
			if (++z >= s_z)
				break;
		}
	}
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

	changed = true;
	return AllocBuf();
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

	ApplyRot();

	return true;
}

void VModel::SetRot(const vector3d r)
{
	vector3di nres;
	vector3d test(bufside);

	rot = r;
	RotNormDegF(&rot);
	rotm = GenMtxRotX(rot.X * M_PI / 180.f);
	rotm = Mtx3Mul(rotm,GenMtxRotY(rot.Y * M_PI / 180.f));
	rotm = Mtx3Mul(rotm,GenMtxRotZ(rot.Z * M_PI / 180.f));

	if (!changed) {
		test = MtxPntMul(&rotm,&test);
		nres.X = (int)round(test.X);
		nres.Y = (int)round(test.Y);
		nres.Z = (int)round(test.Z);
		if (nres != oldrres) changed = true;
		oldrres = nres;
	}

	if (changed) ApplyRot();
}

void VModel::ApplyRot()
{
	int x,y,z;
	ulli l,nl;
	vector3di iv;
	vector3d v,ctrd,ctrb;

	if ((!dat) || (!buf)) return;

	memset(buf,0,buflen*sizeof(voxel));
	ctrd = dcenter.ToReal();
	ctrb = center.ToReal();

	for (z = 0, l = 0; z < s_z; z++) {
		iv.Z = z;
		for (y = 0; y < s_y; y++) {
			iv.Y = y;
			for (x = 0; x < s_x; x++,l++) {
				if (dat[l] == 0) continue;

				iv.X = x;
				v = iv.ToReal() - ctrd;
				v = MtxPntMul(&rotm,&v);
				v += ctrb;
				nl = (int)round(v.Z) * bufside * bufside +
						(int)round(v.Y) * bufside +
						(int)round(v.X);
				if (nl < buflen)
					buf[nl] = dat[l];
			}
		}
	}

	changed = false;
}

voxel VModel::GetVoxelAt(const vector3di* p)
{
	ulli l;
	vector3di x = *p - pos + center;
	l = x.Z * bufside * bufside + x.Y * bufside + x.X;
	if (l >= buflen) return 0;
	else return buf[l];
}
