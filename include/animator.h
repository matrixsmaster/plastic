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

#ifndef ANIMATOR_H_
#define ANIMATOR_H_

#include "voxel.h"
#include "datapipe.h"
#include "vmodel.h"
#include "pltime.h"


struct SDAFrame {
	int state;
	int wait_ms;
	float last;
};

/* Discrete animator */
class DAnimator {
private:
	DataPipe* pipe;
	VModel* model;
	const PlasticTime* gtime;
	char* mdname;
	int frames;
	int loop_b, loop_e;
	SDAFrame* anim;
	int cframe;
	char next_anim[MAXPATHLEN];

	void Frame();

public:
	DAnimator(DataPipe* pipeptr, const PlasticTime* gtptr, VModel* modptr, const char* modnm);
	virtual ~DAnimator();

	bool LoadAnim(const char* name);

	void Update();
};

#endif /* ANIMATOR_H_ */
