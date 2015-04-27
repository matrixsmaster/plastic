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

#ifndef ACTOR_H_
#define ACTOR_H_

#include <string>
#include "vecmath.h"


class PlasticActor {
protected:
	vector3di pos,rot;
	std::string name;

public:
	PlasticActor() {}
	virtual ~PlasticActor() {}

	void SetPos(const vector3di p)		{ pos = p; }
	void SetRot(const vector3di r)		{ rot = r; }
	vector3di GetPos()					{ return pos; }
	vector3di GetRot()					{ return rot; }
};

class Player : public PlasticActor {
public:
	Player() : PlasticActor() {}
	virtual ~Player() {}
};

#endif /* ACTOR_H_ */
