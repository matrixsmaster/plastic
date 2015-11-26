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

#ifndef RESRC_H_
#define RESRC_H_

//#include <vector>
#include "objs.h"

#define MAXINPS 10
//#define RESTYPES 5

enum EResType {
	RT_INVALID,
	RT_IRON,
	RT_OIL,
	RT_COPPER,
	RT_PLASTIC,
	RT_STEEL,
};

static const char resrc_names[][128] = {
		"Invalid","Iron","Oil","Copper","Plastic","Steel"
};

static const EResType mining_tab[] = {
		RT_IRON, RT_OIL, RT_COPPER,
		RT_INVALID /* terminator */
};

struct SResource {
	EResType typ;
	int point,cond;
};

struct SProduction {
	EResType in_res[MAXINPS];
	EDynObj in_obj[MAXINPS];
	EResType out_res;
	EDynObj out_obj;
};

#define PRODLINEL 4

static const SProduction prod_line[PRODLINEL] = {
		{ { RT_IRON, RT_INVALID }, { DO_INVALID }, RT_STEEL, DO_INVALID }, //iron to steel
		{ { RT_OIL, RT_COPPER, RT_INVALID }, { DO_INVALID }, RT_PLASTIC, DO_INVALID }, //oil+copper to plastic
		{ { RT_COPPER, RT_STEEL, RT_PLASTIC, RT_INVALID }, { DO_INVALID }, RT_INVALID, DO_BODYPRT }, //plastic+steel+copper to BP
		{ { RT_OIL, RT_INVALID }, { DO_BODYPRT, DO_BODYPRT, DO_INVALID }, RT_INVALID, DO_BODY }, //BPs to NPC
};

#endif /* RESRC_H_ */
