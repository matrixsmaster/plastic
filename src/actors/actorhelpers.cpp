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
#include "gameeqs.h"

using namespace std;


#define GETSTATFIELD(X,Y)	fld = pipe->GetIniDataS(CLASNFONAME,cls + Y); \
							if (fld.empty()) return false; \
							stat->X = atoi(fld.c_str());

#define GETBODYFIELD(X,Y)	fld = pipe->GetIniDataS(CLASNFONAME,bod + Y); \
							if (fld.empty()) return false; \
							stat->X = atoi(fld.c_str());

const char* BodyTypeToStr(EPABodyType t)
{
	int i;
	for (i = 0; i < NUMBODTYPE; i++)
		if (pabody_to_str[i].b == t)
			return (pabody_to_str[i].s);
	return NULL;
}

bool FillActorBasicStats(SPAAttrib* attr, SPABase* stat, DataPipe* pipe)
{
	int i;
	string fld,cls,bod;

	if (!pipe) return false;

	//get class and body type string representation
	cls = paclass_to_str[attr->cls].s;
	bod = string(BodyTypeToStr(attr->body));

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
	GETSTATFIELD(MR.Eng,"_Eng");
	GETSTATFIELD(MR.Spch,"_Spch");
	GETSTATFIELD(MR.Brv,"_Brv");
	GETSTATFIELD(Chr,"_Chr");
	GETSTATFIELD(MR.Trd,"_Trd");
	GETSTATFIELD(AP,"_AP");
	GETSTATFIELD(DT,"_DT");
	GETSTATFIELD(DM,"_DM");

	//TODO: apply bias to biased values (-1/0/2)

	//determine opposition class
	fld = pipe->GetIniDataS(CLASNFONAME,cls + "_Oppos");
	if (fld.empty()) return false;
	stat->MR.Oppos = PCLS_NONE;
	for (i = 0; i < NUMCLASSES; i++)
		if (!strcmp(fld.c_str(),paclass_to_str[i].s)) {
			stat->MR.Oppos = paclass_to_str[i].c;
			break;
		}

	//generate quality and calculate HP
	EQ_RAND_QUAL(stat->Qual,pipe->GetRNG());
	EQ_HP_FROM_QUAL(stat->HP,stat->Qual);

	//determine body model file name
	//FIXME
	strcpy(attr->model,"mod/actors/alice.dat");

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

bool IsPABaseEqual(const SPABase a, const SPABase b)
{
	//TODO
	return true;
}
