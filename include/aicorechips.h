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

#ifndef AICORECHIPS_H_
#define AICORECHIPS_H_

#include "aichip.h"
#include "actortypes.h"

/* AI ROM */
class AIROMChip : public AIChip {
protected:

public:
	AIROMChip() : AIChip()	{ kind = AICHIP_ROM; }
	virtual ~AIROMChip()	{}
};

/* AI Braincard */
class AIBraincard : public AIChip {
protected:
	unsigned hash;

public:
	AIBraincard() : AIChip()	{ kind = AICHIP_BCARD; }
	virtual ~AIBraincard()		{}
};

/* AI Memory */
class AIMEMChip : public AIChip {
protected:
	unsigned hash;
	SPAPsyBase psy;

public:
	AIMEMChip() : AIChip()	{ kind = AICHIP_MEMORY; }
	virtual ~AIMEMChip()	{}
};

#endif /* AICORECHIPS_H_ */
