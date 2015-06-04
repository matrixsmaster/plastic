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

#include "vobject.h"


VSceneObject::VSceneObject()
{
	rotm = GenOMatrix();
}

void VSceneObject::SetRotI()
{
	vector3d rotf = rot.ToReal();

	//Prepare all variables of new rotation
	RotNormDegF(&rotf);
	rotm = GenMtxRotX(rotf.X * M_PI / 180.f);
	rotm = Mtx3Mul(rotm,GenMtxRotY(rotf.Y * M_PI / 180.f));
	rotm = Mtx3Mul(rotm,GenMtxRotZ(rotf.Z * M_PI / 180.f));
}

void VSceneObject::SetPosI()
{
	//Calculate scene position
	spos = pos + ((gpos - scenter) * CHUNKBOX);

	//Check move out of chunk
	gmov = vector3di();
	if (pos.X >= CHUNKBOX) gmov.X += 1;
	else if (pos.X < 0) gmov.X -= 1;
	if (pos.Y >= CHUNKBOX) gmov.Y += 1;
	else if (pos.Y < 0) gmov.Y -= 1;
	if (pos.Z >= CHUNKBOX) gmov.Z += 1;
	else if (pos.Z < 0) gmov.Z -= 1;
}

void VSceneObject::GMove()
{
	pos -= (gmov * CHUNKBOX);
	SetPosI();
}

void VSceneObject::SetScenePos(const vector3di* p)
{
	scenter = *p;
	SetPosI();
}

void VSceneObject::SetScenePos(const vector3di p)
{
	scenter = p;
	SetPosI();
}

void VSceneObject::SetPos(const vector3di p)
{
	pos = p;
	SetPosI();
}

void VSceneObject::SetRot(const vector3di r)
{
	rot = r;
	SetRotI();
}

void VSceneObject::SetGPos(const vector3di p)
{
	gpos = p;
	SetPosI();
}
