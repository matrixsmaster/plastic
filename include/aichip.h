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

#ifndef AICHIP_H_
#define AICHIP_H_

#include <stdlib.h>
#include <stdio.h>

enum EAIChipType {
	AICHIP_EMPTY,
	AICHIP_ROM,
	AICHIP_AUDIO,
	AICHIP_VIDEO,
	AICHIP_BCARD,
	AICHIP_MEMORY,
	AICHIP_PATHS,
	AICHIP_STT,
	AICHIP_TTS,
	AICHIP_FACEREC,
	AICHIP_FACEMOT,
	AICHIP_BODYCON,
	AICHIP_WICOMM,
	AICHIP_INVALID /* terminator */
};

/* Base class of the A.I. chip */
class AIChip {
protected:
	EAIChipType kind;

public:
	AIChip()			{ kind = AICHIP_EMPTY; }
	virtual ~AIChip()	{}

	/* There's no need to use IGData, so we'll use this simple methods */
	virtual void PushData(FILE* f) {}
	virtual bool PullData(FILE* f) { return true; }
};

#endif /* AICHIP_H_ */
