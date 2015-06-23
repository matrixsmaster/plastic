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


VModel::VModel(SVoxelTab* tabptr) :
		VSceneObject()
{
	s_x = s_y = s_z = 0;
	dat = NULL;
	datlen = 0;
	nstates = 0;
	buf = NULL;
	buflen = 0;
	bufside = 0;
	changed = false;
	state = 0;
	vtab = tabptr;
	extab = NULL;
}

VModel::~VModel()
{
	int i;
	if (buf) free(buf);
	if (extab) free(extab);
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
	EVoxelType tp;
	SVoxCharPair* tab = NULL;

	if ((dat) || (buf) || (!fn)) return false;

	mf = fopen(fn,"r");
	if (!mf) return false;

	//read dimensions, number of states and number of voxel types used
	if (	(fscanf(mf,"%d %d %d %d %d\n",
					&s_x,&s_y,&s_z,&nstates,&vtc) != 5) ||
			(nstates < 1) )
		goto bad_exit;

	//make a char to voxel conversion table and reset it
	j = vtc * sizeof(SVoxCharPair);
	tab = (SVoxCharPair*)malloc(j);
	if (!tab) goto bad_exit; //this would happen if table dimension is invalid
	memset(tab,0,j);

	//reserve memory for a voxel mark string
	s = (char*)malloc(VOXMARKERLEN);
	if (!s) goto bad_exit;

	//read voxel types table (skips empty lines and comments)
	for (i = 0; ((i < vtc) && (!feof(mf)));) {

		char tmp = fgetc(mf);
		switch (tmp) {
		case '!':
			//read the direct voxel number
			if (fscanf(mf,"%c = %hu\n",&(tab[i].c),&(tab[i].v)) == 2) i++;
			break;

		case '?':
			//read the type
			if ( (fscanf(mf,"%c = %d\n",&(tab[i].c),((int*)&tp)) == 2) &&
					(tp >= 0) && (tp < NUMVOXTYPES) ) {
				//search for a first voxel with that type
				tab[i].v = 0;
				for (j = 0; j < vtab->len; j++)
					if (vtab->tab[j].type == tp) {
						tab[i].v = (voxel)j;
						break;
					}
				i++;
			}
			break;

		case '@':
			//read the mark
			if (fscanf(mf,VOXMARKERFMT,&(tab[i].c),s) == 2) {
				//search for a first voxel with that mark
				tab[i].v = 0;
				for (j = 0; j < vtab->len; j++)
					if ((vtab->tab[j].mark) && (!strcmp(vtab->tab[j].mark,s))) {
						tab[i].v = (voxel)j;
						break;
					}
				i++;
			}
			break;

		case '\n':
			//just skip
			break;

		default:
			//skip everything till next line or eof
			while ((fgetc(mf) != '\n') && (!feof(mf))) ;
		}
	}

	if (feof(mf)) goto bad_exit; //shouldn't be at the end of table
	free(s); //free temporary string memory

	//create and initialize voxel extra data table
	j = (vtc + 1) * sizeof(SVoxExtData); //reserve space for a stopper
	extab = (SVoxExtData*)malloc(j);
	if (!extab) goto bad_exit;
	memset(extab,0,j);
	for (i = 0, j = 0; i < vtc; i++) {
		//copy non-zero voxels
		if (tab[i].v) extab[j++].v = tab[i].v;
	}

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

void VModel::SetRot(const vector3di r)
{
	vector3di nres;
	vector3d test(bufside);

	//Set new rotation
	rot = r;
	SetRotI();

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

	HideEm();
	ReplaceEm();
	changed = false;
}

void VModel::HideEm()
{
	int i;
	ulli l;

	//seek though ext tab
	for (i = 0; (extab[i].v); i++) {
		//don't touch not-hidden voxels
		if (!extab[i].hidden) continue;

		//remove hidden voxel from working buf
		for (l = 0; l < buflen; l++)
			if (buf[l] == extab[i].v)
				buf[l] = 0;
	}
}

void VModel::ReplaceEm()
{
	int i;
	ulli l;

	//seek though ext tab
	for (i = 0; (extab[i].v); i++) {
		//don't touch not-changed voxels
		if (!extab[i].nid) continue;

		//replace voxel in working buffer
		for (l = 0; l < buflen; l++)
			if (buf[l] == extab[i].v)
				buf[l] = extab[i].nid;
	}
}

voxel VModel::FindVoxel(EVoxelType tp)
{
	int i;
	voxel v;
	if ((!vtab) || (tp == VOXT_EMPTY)) return 0;

	//seek though ext tab
	for (i = 0; (extab[i].v); i++) {
		v = extab[i].v;
		if (v < vtab->rlen) {
			//voxel is in the vox table, check it
			if (vtab->tab[v].type == tp)
				return v;
		}
	}

	return 0;
}

voxel VModel::FindVoxel(const char* mrk)
{
	int i;
	voxel v;
	if ((!vtab) || (!mrk)) return 0;

	//seek though ext tab
	for (i = 0; (extab[i].v); i++) {
		v = extab[i].v;
		if (v < vtab->rlen) {
			//voxel is in the vox table, check it
			if (	(vtab->tab[v].mark) &&
					(!strcmp(vtab->tab[v].mark,mrk)) )
				return v;
		}
	}

	return 0;
}

void VModel::HideVoxels(voxel id, bool hide)
{
	int i;

	for (i = 0; (extab[i].v); i++) {
		if (extab[i].v == id)
			extab[i].hidden = hide;
	}

	if (hide) HideEm(); //just remove voxels from working buf
	else ApplyRot();	//regenerate working buf
}

void VModel::HideVoxels(EVoxelType tp, bool hide)
{
	HideVoxels(FindVoxel(tp),hide);
}

void VModel::HideVoxels(const char* mrk, bool hide)
{
	HideVoxels(FindVoxel(mrk),hide);
}

void VModel::ReplaceVoxel(voxel old_id, voxel new_id)
{
	int i;

	for (i = 0; (extab[i].v); i++) {
		if (extab[i].v == old_id)
			extab[i].nid = new_id;
	}

	ReplaceEm();
}

void VModel::ReplaceVoxel(const char* mrk, voxel new_id)
{
	ReplaceVoxel(FindVoxel(mrk),new_id);
}

voxel VModel::GetVoxelAt(const vector3di* p)
{
	ulli l;

	//move it to positive side
	vector3di x = *p - spos + center;
	//get offset
	l = x.Z * bufside * bufside + x.Y * bufside + x.X;
	if (l >= buflen) return 0;
	return buf[l]; //got the voxel
}
