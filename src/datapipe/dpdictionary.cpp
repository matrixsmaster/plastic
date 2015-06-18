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
#include "datapipe.h"
#include "support.h"

using namespace std;


bool DataPipe::LoadDict(const std::string name)
{
	FILE* f;
	char nm[MAXPATHLEN];
	char buf[MAXDICTSTRLEN];
	vector<string> cur;

	//make filepath and try to open file
	snprintf(nm,MAXPATHLEN,"%s/dct/%s.dat",root,name.c_str());
	f = fopen(nm,"r");
	if (!f) {
		errout("Unable to open dictionary file '%s'\n",nm);
		return false;
	}

	//load dictionary
	while (!feof(f)) {
		fgets(buf,MAXDICTSTRLEN,f);
		if (strlen(buf) > 1) {
			//trim trailing newline
			if (buf[strlen(buf)-1] == '\n')
				buf[strlen(buf)-1] = 0;
			cur.push_back(string(buf));
		}
	}
	fclose(f);

	//append to map
	if (cur.empty()) return false;
	dicts.insert(make_pair(name,cur));

	return true;
}

DPDict* DataPipe::GetDictionary(const char* dct_name)
{
	if (dct_name == NULL) return NULL;
	return (GetDictionary(string(dct_name)));
}

DPDict* DataPipe::GetDictionary(const std::string dct_name)
{
	if (dct_name.empty()) return NULL;

	if (!dicts.count(dct_name)) {
		//try to load new dictionary
		if (!LoadDict(dct_name)) return NULL;
	}

	//return a pointer to the whole string vector
	return (&(dicts[dct_name]));
}
