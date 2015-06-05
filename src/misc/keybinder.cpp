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

#include <stdlib.h>
#include "keybinder.h"

using namespace std;


int KeyBinder::RegKeyByName(const char* name, int descriptor)
{
	int x;
	if ((!name) || (!pipe)) return -1; //just in case

	//search INI
	string dat = pipe->GetIniDataS(string(KEYBINDNAME),string(name));
	//if not found, nothing can be registered
	if (dat.empty()) return -1;

	//try to interpret information
	if (dat.size() == 1) /*char*/ {
		x = (int)dat[0];
	} else {
		x = strtol(dat.c_str(),NULL,0);
	}
	if (!x) return -1; //something wrong, or trying to register zero key code

	//append new pair
	keymap.insert(make_pair(x,descriptor));
	return descriptor;
}

int KeyBinder::DecodeKey(int key)
{
	const map<int,int>::iterator i = keymap.find(key);
	if (i == keymap.end()) return -1;
	else return (i->second);
}
