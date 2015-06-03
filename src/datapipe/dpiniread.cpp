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

/* Part of DataPipe class. INI file facilities. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datapipe.h"
#include "support.h"

#ifdef DPDEBUG
#include "debug.h"
#endif


bool DataPipe::LoadIni(const std::string name)
{
	FILE* f;
	int r;
	char nm[MAXPATHLEN];
	char key[MAXINISTRLEN],fld[MAXINISTRLEN];
	IniData cur;

	//make filepath and try to open file
	snprintf(nm,MAXPATHLEN,"%s/%s.ini",root,name.c_str());
	f = fopen(nm,"r");
	if (!f) {
		errout("Unable to open INI file '%s'\n",nm);
		return false;
	}

	//loading
	while (!feof(f)) {
		r = fscanf(f,FMTINISTRING,key,fld);
		if (r == 2)
			cur.insert(make_pair(std::string(key),std::string(fld)));
	}
	fclose(f);

	//append to ini map
	ini.insert(make_pair(name,cur));

	return true;
}

void DataPipe::GetIniDataC(const char* ininame, const char* inifield, char* dest, int maxlen)
{
	/* for use in old-styled code */
	std::string nm(ininame), fl(inifield);
	if ((!dest) || (maxlen < 2)) return;
	strncpy(dest,GetIniDataS(nm,fl).c_str(),maxlen);
}

std::string DataPipe::GetIniDataS(const std::string ininame, const std::string inifield)
{
	/* for use in new-styled code */
	std::string res;
	IniData* fnd;

	//search for file in known files map
	if (!ini.count(ininame)) {
		//not found, try to load up
		if (!LoadIni(ininame)) return res;
	}

	//search for field
	fnd = &(ini[ininame]);
	if (!fnd->count(inifield)) return res;
	res = (*fnd)[inifield];

	return res;
}
