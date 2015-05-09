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
#include <string.h>
#include "actorhelpers.h"

using namespace std;


bool FillActorBasicStats(SPAStats* stats, DataPipe* pipe)
{
	if (!pipe) return false;

	//TODO

//	return false;
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
