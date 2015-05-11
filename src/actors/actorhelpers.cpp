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

#include <string>
#include <stdlib.h>
#include <string.h>
#include "actorhelpers.h"

using namespace std;


#define GETSTATFIELD(X,Y)	fld = pipe->GetIniDataS(CLASNFONAME,cls + Y); \
							if (fld.empty()) return false; \
							stat->X = atoi(fld.c_str());

#define GETBODYFIELD(X,Y)	fld = pipe->GetIniDataS(CLASNFONAME,bod + Y); \
							if (fld.empty()) return false; \
							stat->X = atoi(fld.c_str());

bool FillActorBasicStats(SPAAttrib* attr, SPABase* stat, DataPipe* pipe)
{
	int i;
	string fld,cls,bod;

	if (!pipe) return false;

	//get class and body type string representation
	cls = paclass_to_str[attr->cls].s;
	for (i = 0; i < NUMBODTYPE; i++)
		if (pabody_to_str[i].b == attr->body) {
			bod = pabody_to_str[i].s;
			break;
		}

	//get applicable body types mask
	GETSTATFIELD(Body,"_BMask");

	//get body-related data
	if (attr->body & stat->Body) {
		GETBODYFIELD(Str,"_Str");
		GETBODYFIELD(Spd,"_Spd");
		GETBODYFIELD(Eff,"_Eff");
		GETBODYFIELD(RS,"_RS");
		GETBODYFIELD(Acc,"_Acc");
	} else {
		stat->Str = -3;
		stat->Spd = -3;
		stat->Eff = -3;
		stat->RS = -3;
		stat->Acc = -3;
	}

	GETSTATFIELD(CC,"_CC");
	GETSTATFIELD(Eng,"_Eng");
	GETSTATFIELD(Spch,"_Spch");
	GETSTATFIELD(Brv,"_Brv");
	GETSTATFIELD(Chr,"_Chr");
	GETSTATFIELD(Trd,"_Trd");
	GETSTATFIELD(AP,"_AP");
	GETSTATFIELD(DT,"_DT");
	GETSTATFIELD(DM,"_DM");

	//determine opposition class
	fld = pipe->GetIniDataS(CLASNFONAME,cls + "_Oppos");
	if (fld.empty()) return false;
	stat->Oppos = PCLS_NONE;
	for (i = 0; i < NUMCLASSES; i++)
		if (!strcmp(fld.c_str(),paclass_to_str[i].s)) {
			stat->Oppos = paclass_to_str[i].c;
			break;
		}

	return true;
}

void GetActorClassDescr(EPAClass c, char* str, unsigned len, DataPipe* pipe)
{
	string fld;
	if (!pipe) return;

	fld = paclass_to_str[c].s;
	fld += "_Descr";

	pipe->GetIniDataC(CLASNFONAME,fld.c_str(),str,len);
}
