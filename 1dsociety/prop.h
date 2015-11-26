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

#ifndef PROP_H_
#define PROP_H_

#include <map>
#include "resrc.h"
#include "actor.h"

struct SBBox {
	int start,len;
};

struct STerritoty {
	SBBox terr;
	int val;
};

enum EProperty {
	PRP_TERRITORY,
	PRP_OBJECT,
	PRP_RESOURCE,
	PRP_BUILDING,
	PRP_SLAVE,
};

struct SProperty {
	EProperty t;
	union {
		STerritoty ter;
		SDynObj* obj;
		SResource res;
		SBuilding* bld;
		SActor* npc;
	} d;
};

typedef std::map<unsigned,SProperty> PropertyMap;

#endif /* PROP_H_ */
