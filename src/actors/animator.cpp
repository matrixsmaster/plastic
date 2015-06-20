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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include "animator.h"


DAnimator::DAnimator(DataPipe* pipeptr, const PlasticTime* gtptr, VModel* modptr, const char* modnm)
{
	pipe = pipeptr;
	gtime = gtptr;
	model = modptr;
	mdname = NULL;
	frames = 0;
	loop_b = loop_e = 0;
	anim = NULL;
	cframe = 0;

	//extract the model's name
	mdname = (char*)malloc(strlen(modnm));
	if (mdname) {
		strcpy(mdname,modnm); //copy full original string, including path and extension
		memmove(mdname,basename(mdname),strlen(basename(mdname))); //hopefully thread-safe
		*(strchrnul(mdname,'.')) = 0; //locate an extension and break a string there
	}
}

DAnimator::~DAnimator()
{
	if (mdname) free(mdname);
	if (anim) free(anim);
}

bool DAnimator::LoadAnim(const char* name)
{
	int i;
	size_t l;
	char ini[MAXPATHLEN], fld[MAXINISTRLEN], res[MAXINISTRLEN];

	cframe = 0;
	if ((!mdname) || (!name)) return false;

	//create animation file path (relative to VFS)
	snprintf(ini,sizeof(ini),"anm/%s_%s",mdname,name);

	//read frames count
	pipe->GetIniDataC(ini,"Frames",res,sizeof(res));
	frames = atoi(res);
	if (!frames) return false;

	//allocate and setup animation memory
	l = (size_t)frames * sizeof(SDAFrame);
	anim = (SDAFrame*)realloc(anim,l);
	if (!anim) return false;
	memset(anim,0,l);

	//get looping data
	pipe->GetIniDataC(ini,"LoopStart",res,sizeof(res));
	loop_b = atoi(res);
	pipe->GetIniDataC(ini,"LoopEnd",res,sizeof(res));
	loop_e = atoi(res);

	//load frames data
	for (i = 0; i < frames; i++) {
		//state number
		snprintf(fld,sizeof(fld),"Frame%d",i);
		pipe->GetIniDataC(ini,fld,res,sizeof(res));
		anim[i].state = atoi(res);
		//timeout
		snprintf(fld,sizeof(fld),"Frame%dTime",i);
		pipe->GetIniDataC(ini,fld,res,sizeof(res));
		anim[i].wait_ms = atoi(res);
	}

	return true;
}

void DAnimator::Update()
{
	//failsafe
	if ((cframe >= frames) || (cframe < 0))
		return;

	//should we wait some more?
	if (round(gtime->sms - anim[cframe].last) < anim[cframe].wait_ms)
		return;

	//switch to next frame
	if (cframe == loop_e) cframe = loop_b;
	else cframe++;
	if (cframe >= frames) return; //no loop in animation, and animation has played
	anim[cframe].last = gtime->sms;
	model->SetState(anim[cframe].state);
}
